/** Soc.h
*  Header defining the Base addresses, sizes, flags etc for chassis 1
*
*  Copyright 2017-2020 NXP
*
*  SPDX-License-Identifier: BSD-2-Clause-Patent
*
**/

#ifndef NXP_SOC_H_
#define NXP_SOC_H_

#define CLK_FREQ                   100000000

#define CHASSIS2_DCFG_ADDRESS      0x1EE0000

/* SMMU Defintions */
#define SMMU_BASE_ADDR             0x09000000
#define SMMU_REG_SCR0              (SMMU_BASE_ADDR + 0x0)
#define SMMU_REG_SACR              (SMMU_BASE_ADDR + 0x10)
#define SMMU_REG_IDR1              (SMMU_BASE_ADDR + 0x24)
#define SMMU_REG_NSCR0             (SMMU_BASE_ADDR + 0x400)
#define SMMU_REG_NSACR             (SMMU_BASE_ADDR + 0x410)

#define SCR0_USFCFG_MASK           0x00000400
#define SCR0_CLIENTPD_MASK         0x00000001
#define SACR_PAGESIZE_MASK         0x00010000
#define IDR1_PAGESIZE_MASK         0x80000000

typedef struct {
  UINTN FreqSystemBus;
} SYS_INFO;

/* Device Configuration and Pin Control */
typedef struct {
  UINT8    Res0[0x100-0x00];
  UINT32   RcwSr[16];      /* Reset control word status */
#define CHASSIS2_RCWSR0_SYS_PLL_RAT_SHIFT  25
#define CHASSIS2_RCWSR0_SYS_PLL_RAT_MASK  0x1f
} CCSR_GUR;

VOID
GetSysInfo (
  OUT SYS_INFO *
  );

UINT32
EFIAPI
GurRead (
  IN  UINTN     Address
  );

#endif /* NXP_SOC_H_ */
