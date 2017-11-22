/** @file
  SoC specific Library containg functions to initialize various SoC components

  Copyright 2017-2020 NXP

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#ifdef CHASSIS2
#include <Chassis2/NxpSoc.h>
#elif CHASSIS3
#include <Chassis3/NxpSoc.h>
#endif
#include <Library/ArmSmcLib.h>
#include <Library/BaseLib.h>
#include <Library/IoAccessLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/PrintLib.h>
#include <Library/SerialPortLib.h>

#include <DramInfo.h>
#include "NxpChassis.h"

/*
 *  Structure to list available SOCs.
 *  Name, Soc Version, Number of Cores
 */
STATIC CPU_TYPE mCpuTypeList[] = {
  CPU_TYPE_ENTRY (LS1043A, LS1043A, 4),
  CPU_TYPE_ENTRY (LS1046A, LS1046A, 4),
  CPU_TYPE_ENTRY (LS2088A, LS2088A, 8),
};

UINT32
EFIAPI
GurRead (
  IN  UINTN     Address
  )
{
  if (FixedPcdGetBool (PcdGurBigEndian)) {
    return SwapMmioRead32 (Address);
  } else {
    return MmioRead32 (Address);
  }
}

/*
 * Return the type of initiator (core or hardware accelerator)
 */
UINT32
InitiatorType (
  IN UINT32 Cluster,
  IN UINTN  InitId
  )
{
  CCSR_GUR *GurBase;
  UINT32   Idx;
  UINT32   Type;

  GurBase = (VOID *)PcdGet64 (PcdGutsBaseAddr);
  Idx = (Cluster >> (InitId * 8)) & TP_CLUSTER_INIT_MASK;
  Type = GurRead ((UINTN)&GurBase->TpItyp[Idx]);

  if (Type & TP_ITYP_AV_MASK) {
    return Type;
  }

  return 0;
}

/*
 *  Return the mask for number of cores on this SOC.
 */
UINT32
CpuMask (
  VOID
  )
{
  CCSR_GUR  *GurBase;
  UINTN     ClusterIndex;
  UINTN     Count;
  UINT32    Cluster;
  UINT32    Type;
  UINT32    Mask;
  UINTN     InitiatorIndex;

  GurBase = (VOID *)PcdGet64 (PcdGutsBaseAddr);
  ClusterIndex = 0;
  Count = 0;
  Mask = 0;

  do {
    Cluster = GurRead ((UINTN)&GurBase->TpCluster[ClusterIndex].Lower);
    for (InitiatorIndex = 0; InitiatorIndex < TP_INIT_PER_CLUSTER; InitiatorIndex++) {
      Type = InitiatorType (Cluster, InitiatorIndex);
      if (Type) {
        if (TP_ITYP_TYPE_MASK (Type) == TP_ITYP_TYPE_ARM) {
          Mask |= 1 << Count;
        }
        Count++;
      }
    }
    ClusterIndex++;
  } while (CHECK_CLUSTER (Cluster));

  return Mask;
}

/*
 *  Return the number of cores on this SOC.
 */
UINTN
CpuNumCores (
  VOID
  )
{
  UINTN Count;
  UINTN Num;

  Count = 0;
  Num = CpuMask ();

  while (Num) {
    Count += Num & 1;
    Num >>= 1;
  }

  return Count;
}

/*
 *  Return core's cluster
 */
INT32
QoriqCoreToCluster (
  IN UINTN Core
  )
{
  CCSR_GUR  *GurBase;
  UINTN     ClusterIndex;
  UINTN     Count;
  UINT32    Cluster;
  UINT32    Type;
  UINTN     InitiatorIndex;

  GurBase = (VOID *)PcdGet64 (PcdGutsBaseAddr);
  ClusterIndex = 0;
  Count = 0;
  do {
    Cluster = GurRead ((UINTN)&GurBase->TpCluster[ClusterIndex].Lower);
    for (InitiatorIndex = 0; InitiatorIndex < TP_INIT_PER_CLUSTER; InitiatorIndex++) {
      Type = InitiatorType (Cluster, InitiatorIndex);
      if (Type) {
        if (Count == Core) {
          return ClusterIndex;
        }
        Count++;
      }
    }
    ClusterIndex++;
  } while (CHECK_CLUSTER (Cluster));

  return -1;      // cannot identify the cluster
}

/*
 *  Return the type of core i.e. A53, A57 etc of inputted
 *  core number.
 */
UINTN
QoriqCoreToType (
  IN UINTN Core
  )
{
  CCSR_GUR  *GurBase;
  UINTN     ClusterIndex;
  UINTN     Count;
  UINT32    Cluster;
  UINT32    Type;
  UINTN     InitiatorIndex;

  GurBase = (VOID *)PcdGet64 (PcdGutsBaseAddr);
  ClusterIndex = 0;
  Count = 0;

  do {
    Cluster = GurRead ((UINTN)&GurBase->TpCluster[ClusterIndex].Lower);
    for (InitiatorIndex = 0; InitiatorIndex < TP_INIT_PER_CLUSTER; InitiatorIndex++) {
      Type = InitiatorType (Cluster, InitiatorIndex);
      if (Type) {
        if (Count == Core) {
          return Type;
        }
        Count++;
      }
    }
    ClusterIndex++;
  } while (CHECK_CLUSTER (Cluster));

  return EFI_NOT_FOUND;      /* cannot identify the cluster */
}

STATIC
UINTN
CpuMaskNext (
  IN  UINTN  Cpu,
  IN  UINTN  Mask
  )
{
  for (Cpu++; !((1 << Cpu) & Mask); Cpu++);

  return Cpu;
}

/*
 * Print CPU information
 */
VOID
PrintCpuInfo (
  VOID
  )
{
  SYS_INFO SysInfo;
  UINTN    CoreIndex;
  UINTN    Core;
  UINT32   Type;
  UINT32   NumCpus;
  UINT32   Mask;
  CHAR8    *CoreName;

  GetSysInfo (&SysInfo);
  DEBUG ((DEBUG_INIT, "Clock Configuration:"));

  NumCpus = CpuNumCores ();
  Mask = CpuMask ();

  for (CoreIndex = 0, Core = CpuMaskNext(-1, Mask);
       CoreIndex < NumCpus;
       CoreIndex++, Core = CpuMaskNext(Core, Mask))
  {
    if (!(CoreIndex % 3)) {
      DEBUG ((DEBUG_INIT, "\n      "));
    }

    Type = TP_ITYP_VERSION (QoriqCoreToType (Core));
    switch (Type) {
      case TY_ITYP_VERSION_A7:
        CoreName = "A7";
        break;
      case TY_ITYP_VERSION_A53:
        CoreName = "A53";
        break;
      case TY_ITYP_VERSION_A57:
        CoreName = "A57";
        break;
      case TY_ITYP_VERSION_A72:
        CoreName = "A72";
        break;
      default:
        CoreName = " Unknown Core ";
    }
    DEBUG ((DEBUG_INIT, "CPU%d(%a):%-4d MHz  ",
      Core, CoreName, SysInfo.FreqProcessor[Core] / MHZ));
  }

  DEBUG ((DEBUG_INIT, "\n      Bus:      %-4d MHz  ", SysInfo.FreqSystemBus / MHZ));
  DEBUG ((DEBUG_INIT, "DDR:      %-4d MT/s", SysInfo.FreqDdrBus / MHZ));

  if (SysInfo.FreqFman[0] != 0) {
    DEBUG ((DEBUG_INIT, "\n      FMAN:     %-4d MHz  ",  SysInfo.FreqFman[0] / MHZ));
  }

  DEBUG ((DEBUG_INIT, "\n"));
}

/*
 * Return system bus frequency
 */
UINT64
GetBusFrequency (
   VOID
  )
{
  SYS_INFO SocSysInfo;

  GetSysInfo (&SocSysInfo);

  return SocSysInfo.FreqSystemBus;
}

/*
 * Return SDXC bus frequency
 */
UINT64
GetSdxcFrequency (
   VOID
  )
{
  SYS_INFO SocSysInfo;

  GetSysInfo (&SocSysInfo);

  return SocSysInfo.FreqSdhc;
}

/*
 * Print Soc information
 */
VOID
PrintSoc (
  VOID
  )
{
  CHAR8    Buf[20];
  CCSR_GUR *GurBase;
  UINTN    Count;
  //
  // Svr : System Version Register
  //
  UINTN    Svr;
  UINTN    Ver;

  GurBase = (VOID *)PcdGet64 (PcdGutsBaseAddr);

  Svr = GurRead ((UINTN)&GurBase->Svr);
  Ver = SVR_SOC_VER (Svr);

  for (Count = 0; Count < ARRAY_SIZE (mCpuTypeList); Count++) {
    if ((mCpuTypeList[Count].SocVer & SVR_WO_E) == Ver) {
      AsciiStrCpyS (Buf, sizeof (Buf), mCpuTypeList[Count].Name);

      if (IS_E_PROCESSOR (Svr)) {
        AsciiStrCatS (Buf, sizeof (Buf), "E");
      }
      break;
    }
  }

  DEBUG ((DEBUG_INFO, "SoC: %a (0x%x); Rev %d.%d\n",
          Buf, Svr, SVR_MAJOR (Svr), SVR_MINOR (Svr)));

  return;
}

/*
 * Dump RCW (Reset Control Word) on console
 */
VOID
PrintRCW (
  VOID
  )
{
  CCSR_GUR *Base;
  UINTN    Count;

  Base = (VOID *)PcdGet64 (PcdGutsBaseAddr);

  /*
   * Display the RCW, so that no one gets confused as to what RCW
   * we're actually using for this boot.
   */

  DEBUG ((DEBUG_INIT, "Reset Configuration Word (RCW):"));
  for (Count = 0; Count < ARRAY_SIZE (Base->RcwSr); Count++) {
    UINT32 Rcw = SwapMmioRead32 ((UINTN)&Base->RcwSr[Count]);

    if ((Count % 4) == 0) {
      DEBUG ((DEBUG_INIT, "\n      %08x:", Count * 4));
    }

    DEBUG ((DEBUG_INIT, " %08x", Rcw));
  }

  DEBUG ((DEBUG_INIT, "\n"));
}

/*
 * Setup SMMU in bypass mode
 * and also set its pagesize
 */
VOID
SmmuInit (
  VOID
  )
{
  UINT32 Value;

  /* set pagesize as 64K and ssmu-500 in bypass mode */
  Value = (MmioRead32 ((UINTN)SMMU_REG_SACR) | SACR_PAGESIZE_MASK);
  MmioWrite32 ((UINTN)SMMU_REG_SACR, Value);

  Value = (MmioRead32 ((UINTN)SMMU_REG_SCR0) | SCR0_CLIENTPD_MASK) & ~SCR0_USFCFG_MASK;
  MmioWrite32 ((UINTN)SMMU_REG_SCR0, Value);

  Value = (MmioRead32 ((UINTN)SMMU_REG_NSCR0) | SCR0_CLIENTPD_MASK) & ~SCR0_USFCFG_MASK;
  MmioWrite32 ((UINTN)SMMU_REG_NSCR0, Value);
}

/*
 * Return current Soc Name form mCpuTypeList
 */
CHAR8 *
GetSocName (
  VOID
  )
{
  UINT8     Count;
  UINTN     Svr;
  UINTN     Ver;
  CCSR_GUR  *GurBase;

  GurBase = (VOID *)PcdGet64 (PcdGutsBaseAddr);

  Svr = GurRead ((UINTN)&GurBase->Svr);
  Ver = SVR_SOC_VER (Svr);

  for (Count = 0; Count < ARRAY_SIZE (mCpuTypeList); Count++) {
    if ((mCpuTypeList[Count].SocVer & SVR_WO_E) == Ver) {
      return (CHAR8 *)mCpuTypeList[Count].Name;
    }
  }

  return NULL;
}

UINTN
GetDramSize (
  IN VOID
  )
{
  ARM_SMC_ARGS  ArmSmcArgs;

  ArmSmcArgs.Arg0 = SMC_DRAM_BANK_INFO;
  ArmSmcArgs.Arg1 = -1;

  ArmCallSmc (&ArmSmcArgs);

  if (ArmSmcArgs.Arg0) {
    return 0;
  } else {
    return ArmSmcArgs.Arg1;
  }
}

EFI_STATUS
GetDramBankInfo (
  IN OUT DRAM_INFO *DramInfo
  )
{
  ARM_SMC_ARGS  ArmSmcArgs;
  UINT32        I;
  UINTN         DramSize;

  DramSize = GetDramSize ();
  DEBUG ((DEBUG_INFO, "DRAM Total Size 0x%lx \n", DramSize));

  // Ensure DramSize has been set
  ASSERT (DramSize != 0);

  I = 0;

  do {
    ArmSmcArgs.Arg0 = SMC_DRAM_BANK_INFO;
    ArmSmcArgs.Arg1 = I;

    ArmCallSmc (&ArmSmcArgs);
    if (ArmSmcArgs.Arg0) {
      if (I > 0) {
        break;
      } else {
        ASSERT (ArmSmcArgs.Arg0 == 0);
      }
    }

    DramInfo->DramRegion[I].BaseAddress = ArmSmcArgs.Arg1;
    DramInfo->DramRegion[I].Size = ArmSmcArgs.Arg2;

    DramSize -= DramInfo->DramRegion[I].Size;

    DEBUG ((DEBUG_INFO, "bank[%d]: start 0x%lx, size 0x%lx\n",
      I, DramInfo->DramRegion[I].BaseAddress, DramInfo->DramRegion[I].Size));

    I++;
  } while (DramSize);

  DramInfo->NumOfDrams = I;

  DEBUG ((DEBUG_INFO, "Number Of DRAM in system %d \n", DramInfo->NumOfDrams));

  return EFI_SUCCESS;
}
