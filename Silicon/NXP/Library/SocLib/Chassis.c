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
