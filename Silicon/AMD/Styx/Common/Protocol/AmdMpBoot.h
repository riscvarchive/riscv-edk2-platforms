/** @file

  Copyright (c) 2016, AMD Inc. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _AMD_MP_BOOT_H_
#define _AMD_MP_BOOT_H_

extern EFI_GUID gAmdMpBootProtocolGuid;

typedef
VOID
(EFIAPI *PARK_SECONDARY_CORE) (
  IN ARM_CORE_INFO        *ArmCoreInfo,
  IN EFI_PHYSICAL_ADDRESS SecondaryEntry
  );

typedef struct _AMD_MP_BOOT_INFO {
  EFI_PHYSICAL_ADDRESS  MpParkingBase;
  UINTN                 MpParkingSize;
  ARM_CORE_INFO         *ArmCoreInfoTable;
  UINTN                 ArmCoreCount;
} AMD_MP_BOOT_INFO;

typedef struct _AMD_MP_BOOT_PROTOCOL {
  PARK_SECONDARY_CORE  ParkSecondaryCore;
  AMD_MP_BOOT_INFO     *MpBootInfo;
} AMD_MP_BOOT_PROTOCOL;

#endif // _AMD_MP_BOOT_H_
