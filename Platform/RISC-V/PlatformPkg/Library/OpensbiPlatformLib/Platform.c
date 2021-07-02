/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#include <libfdt.h>
#include <PlatformOverride.h>
#include <sbi/riscv_asm.h>
#include <sbi/sbi_domain.h>
#include <sbi/sbi_hartmask.h>
#include <sbi/sbi_platform.h>
#include <sbi/sbi_string.h>
#include <sbi/sbi_math.h>
#include <sbi_utils/fdt/fdt_domain.h>
#include <sbi_utils/fdt/fdt_fixup.h>
#include <sbi_utils/fdt/fdt_helper.h>
#include <sbi_utils/irqchip/fdt_irqchip.h>
#include <sbi_utils/serial/fdt_serial.h>
#include <sbi_utils/timer/fdt_timer.h>
#include <sbi_utils/ipi/fdt_ipi.h>
#include <sbi_utils/reset/fdt_reset.h>

extern const struct platform_override sifive_fu540;

static const struct platform_override *special_platforms[] = {
  &sifive_fu540,
};

static const struct platform_override *generic_plat = NULL;
static const struct fdt_match *generic_plat_match = NULL;

static void fw_platform_lookup_special(void *fdt, int root_offset)
{
  int pos, noff;
  const struct platform_override *plat;
  const struct fdt_match *match;

  for (pos = 0; pos < array_size(special_platforms); pos++) {
    plat = special_platforms[pos];
    if (!plat->match_table)
      continue;

    noff = fdt_find_match(fdt, -1, plat->match_table, &match);
    if (noff < 0)
      continue;

    generic_plat = plat;
    generic_plat_match = match;
    break;
  }
}

extern struct sbi_platform platform;
static u32 generic_hart_index2id[SBI_HARTMASK_MAX_BITS] = { 0 };

/*
 * The fw_platform_init() function is called very early on the boot HART
 * OpenSBI reference firmwares so that platform specific code get chance
 * to update "platform" instance before it is used.
 *
 * The arguments passed to fw_platform_init() function are boot time state
 * of A0 to A4 register. The "arg0" will be boot HART id and "arg1" will
 * be address of FDT passed by previous booting stage.
 *
 * The return value of fw_platform_init() function is the FDT location. If
 * FDT is unchanged (or FDT is modified in-place) then fw_platform_init()
 * can always return the original FDT location (i.e. 'arg1') unmodified.
 */
unsigned long fw_platform_init(unsigned long arg0, unsigned long arg1,
        unsigned long arg2, unsigned long arg3,
        unsigned long arg4)
{
  const char *model;
  void *fdt = (void *)arg1;
  u32 hartid, hart_count = 0;
  int rc, root_offset, cpus_offset, cpu_offset, len;

  root_offset = fdt_path_offset(fdt, "/");
  if (root_offset < 0)
    goto fail;

  fw_platform_lookup_special(fdt, root_offset);

  model = fdt_getprop(fdt, root_offset, "model", &len);
  if (model)
    sbi_strncpy(platform.name, model, sizeof(platform.name));

  if (generic_plat && generic_plat->features)
    platform.features = generic_plat->features(generic_plat_match);

  cpus_offset = fdt_path_offset(fdt, "/cpus");
  if (cpus_offset < 0)
    goto fail;

  fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
    rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
    if (rc)
      continue;

    if (SBI_HARTMASK_MAX_BITS <= hartid)
      continue;

    generic_hart_index2id[hart_count++] = hartid;
  }

  platform.hart_count = hart_count;

  /* Return original FDT pointer */
  return arg1;

fail:
  while (1)
    wfi();
}

static int generic_early_init(bool cold_boot)
{
  int rc;

  if (generic_plat && generic_plat->early_init) {
    rc = generic_plat->early_init(cold_boot, generic_plat_match);
    if (rc)
      return rc;
  }

  if (!cold_boot)
    return 0;

  return fdt_reset_init();
}

static int generic_final_init(bool cold_boot)
{
  void *fdt;
  int rc;

  if (generic_plat && generic_plat->final_init) {
    rc = generic_plat->final_init(cold_boot, generic_plat_match);
    if (rc)
      return rc;
  }

  if (!cold_boot)
    return 0;

  fdt = sbi_scratch_thishart_arg1_ptr();

  fdt_cpu_fixup(fdt);
  fdt_fixups(fdt);
  fdt_domain_fixup(fdt);

  if (generic_plat && generic_plat->fdt_fixup) {
    rc = generic_plat->fdt_fixup(fdt, generic_plat_match);
    if (rc)
      return rc;
  }

  return 0;
}

static void generic_early_exit(void)
{
  if (generic_plat && generic_plat->early_exit)
    generic_plat->early_exit(generic_plat_match);
}

static void generic_final_exit(void)
{
  if (generic_plat && generic_plat->final_exit)
    generic_plat->final_exit(generic_plat_match);
}

static int generic_domains_init(void)
{
  return fdt_domains_populate(sbi_scratch_thishart_arg1_ptr());
}

static u64 generic_tlbr_flush_limit(void)
{
  if (generic_plat && generic_plat->tlbr_flush_limit)
    return generic_plat->tlbr_flush_limit(generic_plat_match);
  return SBI_PLATFORM_TLB_RANGE_FLUSH_LIMIT_DEFAULT;
}

static int generic_system_reset_check(u32 reset_type, u32 reset_reason)
{
  if (generic_plat && generic_plat->system_reset_check)
    return generic_plat->system_reset_check(reset_type,
              reset_reason,
              generic_plat_match);
  return fdt_system_reset_check(reset_type, reset_reason);
}

static void generic_system_reset(u32 reset_type, u32 reset_reason)
{
  if (generic_plat && generic_plat->system_reset) {
    generic_plat->system_reset(reset_type, reset_reason,
             generic_plat_match);
    return;
  }

  fdt_system_reset(reset_type, reset_reason);
}

#define EDK2_ROOT_FW_REGION       0
#define EDK2_FW_REGION            1
#define EDK2_VARIABLE_REGION    2
#define EDK2_ALL_REGION            3
#define EDK2_END_REGION            4
static struct sbi_domain_memregion root_memregs[EDK2_END_REGION + 1] = { 0 };

struct sbi_domain_memregion *get_mem_regions(void) {
  /* EDK2 root firmware domain memory region */
  root_memregs[EDK2_ROOT_FW_REGION].order = log2roundup(FixedPcdGet32(PcdRootFirmwareDomainSize));
  root_memregs[EDK2_ROOT_FW_REGION].base = FixedPcdGet32(PcdRootFirmwareDomainBaseAddress);
  root_memregs[EDK2_ROOT_FW_REGION].flags = 0;

    /*EDK2 firmware domain memory region */
  root_memregs[EDK2_FW_REGION].order = log2roundup(FixedPcdGet32(PcdFirmwareDomainSize));
  root_memregs[EDK2_FW_REGION].base = FixedPcdGet32(PcdFirmwareDomainBaseAddress);
  root_memregs[EDK2_FW_REGION].flags = SBI_DOMAIN_MEMREGION_EXECUTABLE | SBI_DOMAIN_MEMREGION_READABLE;

    /*EDK2 firmware domain memory region */
  root_memregs[EDK2_VARIABLE_REGION].order = log2roundup(FixedPcdGet32(PcdVariableFirmwareRegionSize));
  root_memregs[EDK2_VARIABLE_REGION].base = FixedPcdGet32(PcdVariableFirmwareRegionBaseAddress);
  root_memregs[EDK2_VARIABLE_REGION].flags = SBI_DOMAIN_MEMREGION_READABLE | SBI_DOMAIN_MEMREGION_WRITEABLE;

  /* EDK2 domain allow everything memory region */
  root_memregs[EDK2_ALL_REGION].order = __riscv_xlen;
  root_memregs[EDK2_ALL_REGION].base = 0;
  root_memregs[EDK2_ALL_REGION].flags = (SBI_DOMAIN_MEMREGION_READABLE |
            SBI_DOMAIN_MEMREGION_WRITEABLE |
            SBI_DOMAIN_MEMREGION_EXECUTABLE);

  /* EDK2 domain memory region end */
  root_memregs[EDK2_END_REGION].order = 0;

  return root_memregs;
}

const struct sbi_platform_operations platform_ops = {
  .early_init            = generic_early_init,
  .final_init            = generic_final_init,
  .early_exit            = generic_early_exit,
  .final_exit            = generic_final_exit,
  .domains_root_regions  = get_mem_regions,
  .domains_init          = generic_domains_init,
  .console_init          = fdt_serial_init,
  .irqchip_init          = fdt_irqchip_init,
  .irqchip_exit          = fdt_irqchip_exit,
  .ipi_init              = fdt_ipi_init,
  .ipi_exit              = fdt_ipi_exit,
  .get_tlbr_flush_limit  = generic_tlbr_flush_limit,
  .timer_init            = fdt_timer_init,
  .timer_exit            = fdt_timer_exit,
};

#if FixedPcdGet32(PcdBootableHartNumber) == 4
#define U540_BOOTABLE_HART_COUNT FixedPcdGet32(PcdBootableHartNumber)
static u32 U540_hart_index2id[U540_BOOTABLE_HART_COUNT] = {1, 2, 3, 4};
#endif

struct sbi_platform platform = {
  .opensbi_version  = OPENSBI_VERSION,
  .platform_version  = SBI_PLATFORM_VERSION(0x0, 0x01),
  .name      = "Generic",
  .features    = SBI_PLATFORM_DEFAULT_FEATURES,
  .hart_count    = SBI_HARTMASK_MAX_BITS,
// TODO: Workaround for U540. Not sure why we need this. OpenSBI doesn't need it.
#if FixedPcdGet32(PcdBootableHartNumber) == 4
  .hart_index2id    = U540_hart_index2id,
#else
  .hart_index2id    = generic_hart_index2id,
#endif
  // TODO: Any reason why it shouldn't just be SBI_PLATFORM_DEFAULT_HART_STACK_SIZE?
  .hart_stack_size  = FixedPcdGet32(PcdOpenSbiStackSize),
  .platform_ops_addr  = (unsigned long)&platform_ops
};
