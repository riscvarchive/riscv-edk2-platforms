/** @file
*
*  Copyright (c) 2011-2015, ARM Limited. All rights reserved.
*  Copyright (c) 2015-2018, Hisilicon Limited. All rights reserved.
*  Copyright (c) 2015-2016, Linaro Limited. All rights reserved.
*
*  This program and the accompanying materials
*  are licensed and made available under the terms and conditions of the BSD License
*  which accompanies this distribution.  The full text of the license may be found at
*  http://opensource.org/licenses/bsd-license.php
*
*  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
*  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
*
*  Based on the files under ArmPlatformPkg/ArmJunoPkg/AcpiTables/
*
**/


#ifndef _HI1610_PLATFORM_H_
#define _HI1610_PLATFORM_H_

#include <IndustryStandard/Acpi.h>

//
// ACPI table information used to initialize tables.
//
#define EFI_ACPI_ARM_OEM_ID           'H','I','S','I',' ',' '   // OEMID 6 bytes long
#define EFI_ACPI_ARM_OEM_TABLE_ID     SIGNATURE_64('H','I','P','0','7',' ',' ',' ') // OEM table id 8 bytes long
#define EFI_ACPI_ARM_OEM_REVISION     0x00000000
#define EFI_ACPI_ARM_CREATOR_ID       SIGNATURE_32('I','N','T','L')
#define EFI_ACPI_ARM_CREATOR_REVISION 0x20151124

// A macro to initialise the common header part of EFI ACPI tables as defined by
// EFI_ACPI_DESCRIPTION_HEADER structure.
#define ARM_ACPI_HEADER(Signature, Type, Revision) {              \
    Signature,                      /* UINT32  Signature */       \
    sizeof (Type),                  /* UINT32  Length */          \
    Revision,                       /* UINT8   Revision */        \
    0,                              /* UINT8   Checksum */        \
    { EFI_ACPI_ARM_OEM_ID },        /* UINT8   OemId[6] */        \
    EFI_ACPI_ARM_OEM_TABLE_ID,      /* UINT64  OemTableId */      \
    EFI_ACPI_ARM_OEM_REVISION,      /* UINT32  OemRevision */     \
    EFI_ACPI_ARM_CREATOR_ID,        /* UINT32  CreatorId */       \
    EFI_ACPI_ARM_CREATOR_REVISION   /* UINT32  CreatorRevision */ \
  }

#define HI1616_WATCHDOG_COUNT  2
#define HI1616_GIC_STRUCTURE_COUNT  64

#define HI1616_MPID_TA_BASE  0x10000
#define HI1616_MPID_TB_BASE  0x30000
#define HI1616_MPID_TA_2_BASE  0x50000
#define HI1616_MPID_TB_2_BASE  0x70000

// Differs from Juno, we have another affinity level beyond cluster and core
#define PLATFORM_GET_MPID_TA(ClusterId, CoreId)   (HI1616_MPID_TA_BASE | ((ClusterId) << 8) | (CoreId))
#define PLATFORM_GET_MPID_TB(ClusterId, CoreId)   (HI1616_MPID_TB_BASE | ((ClusterId) << 8) | (CoreId))
#define PLATFORM_GET_MPID_TA_2(ClusterId, CoreId)   (HI1616_MPID_TA_2_BASE | ((ClusterId) << 8) | (CoreId))
#define PLATFORM_GET_MPID_TB_2(ClusterId, CoreId)   (HI1616_MPID_TB_2_BASE | ((ClusterId) << 8) | (CoreId))

//
// Multiple APIC Description Table
//
#pragma pack (1)

typedef struct {
  EFI_ACPI_6_1_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER   Header;
  EFI_ACPI_6_1_GIC_STRUCTURE                            GicInterfaces[HI1616_GIC_STRUCTURE_COUNT];
  EFI_ACPI_6_1_GIC_DISTRIBUTOR_STRUCTURE                GicDistributor;
  EFI_ACPI_6_1_GIC_ITS_STRUCTURE                        GicITS[8];
} EFI_ACPI_6_1_MULTIPLE_APIC_DESCRIPTION_TABLE;

#pragma pack ()

#endif
