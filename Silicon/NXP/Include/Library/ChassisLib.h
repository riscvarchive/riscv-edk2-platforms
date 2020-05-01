/** @file
  Chassis Lib to provide Chessis specific functionality to all SOCs in
  a Chassis.

  Copyright 2020 NXP

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef CHASSIS_LIB_H__
#define CHASSIS_LIB_H__

#include <Chassis.h>

/**
  Read Dcfg register

  @param  Address The MMIO register to read.

  @return The value read.
**/
UINT32
EFIAPI
DcfgRead32 (
  IN  UINTN     Address
  );

/**
  Write Dcfg register

  @param  Address The MMIO register to write.
  @param  Value   The value to write to the MMIO register.

  @return Value.
**/
UINT32
EFIAPI
DcfgWrite32 (
  IN      UINTN                     Address,
  IN      UINT32                    Value
  );

/**
  Function to initialize Chassis Specific functions
 **/
VOID
ChassisInit (
  VOID
  );

#endif // CHASSIS_LIB_H__
