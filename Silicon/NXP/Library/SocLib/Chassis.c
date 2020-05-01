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

#include "NxpChassis.h"

UINT32
EFIAPI
GurRead (
  IN  UINTN     Address
  )
{
  MMIO_OPERATIONS  *GurOps;

  GurOps = GetMmioOperations (FixedPcdGetBool (PcdGurBigEndian));

  return GurOps->Read32 (Address);
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

