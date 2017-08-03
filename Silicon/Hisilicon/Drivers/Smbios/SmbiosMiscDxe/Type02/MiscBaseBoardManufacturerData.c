/*++

Copyright (c) 2006 - 2009, Intel Corporation. All rights reserved.<BR>
Copyright (c) 2015, Hisilicon Limited. All rights reserved.<BR>
Copyright (c) 2015, Linaro Limited. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

Module Name:

  MiscBaseBoardManufacturerData.c

Abstract:

  This file provide OEM to define Smbios Type2 Data

Based on files under Nt32Pkg/MiscSubClassPlatformDxe/
**/

#include "SmbiosMisc.h"

//
// Static (possibly build generated) Chassis Manufacturer data.
//
MISC_SMBIOS_TABLE_DATA(SMBIOS_TABLE_TYPE2, MiscBaseBoardManufacturer) = {
    {                                                       // Hdr
        EFI_SMBIOS_TYPE_BASEBOARD_INFORMATION,                // Type,
        0,                                                    // Length,
        0                                                     // Handle
    },
    1,                                                      // BaseBoardManufacturer
    2,                                                      // BaseBoardProductName
    3,                                                      // BaseBoardVersion
    4,                                                      // BaseBoardSerialNumber
    5,                                                      // BaseBoardAssetTag
    {                                                       // FeatureFlag
        1,                                                    // Motherboard           :1
        0,                                                    // RequiresDaughterCard  :1
        0,                                                    // Removable             :1
        1,                                                    // Replaceable           :1
        0,                                                    // HotSwappable          :1
        0                                                     // Reserved              :3
    },
    6,                                                      // BaseBoardChassisLocation
    0,                                                      // ChassisHandle;
    BaseBoardTypeMotherBoard,                               // BoardType;
    0,                                                      // NumberOfContainedObjectHandles;
    {
        0
    }                                                       // ContainedObjectHandles[1];
};
