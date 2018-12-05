/** @file

  Sample ACPI Platform Driver

  Copyright (c) 2008 - 2011, Intel Corporation. All rights reserved.<BR>
  Copyright (c) 2014 - 2016, AMD Inc. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
/**
  Derived from:
   MdeModulePkg/Universal/Acpi/AcpiPlatformDxe/AcpiPlatform.c
**/

#include <AmdStyxAcpiLib.h>
#include <Protocol/AcpiTable.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/DxeServicesLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include <IndustryStandard/Acpi61.h>

#include <SocVersion.h>

#define MAX_ACPI_TABLES    16

EFI_ACPI_DESCRIPTION_HEADER *AcpiTableList[MAX_ACPI_TABLES];

STATIC EFI_ACPI_TABLE_PROTOCOL   *mAcpiTableProtocol;

#if DO_XGBE

STATIC CONST UINT8 mDefaultMacPackageA[] = {
  0x12, 0xe, 0x6, 0xa, 0x2, 0xa, 0xa1, 0xa, 0xa2, 0xa, 0xa3, 0xa, 0xa4, 0xa, 0xa5
};

STATIC CONST UINT8 mDefaultMacPackageB[] = {
  0x12, 0xe, 0x6, 0xa, 0x2, 0xa, 0xb1, 0xa, 0xb2, 0xa, 0xb3, 0xa, 0xb4, 0xa, 0xb5
};

#define PACKAGE_MAC_OFFSET  4
#define PACKAGE_MAC_INCR    2

STATIC
VOID
SetPackageAddress (
  UINT8         *Package,
  UINT64        MacAddress,
  UINTN         Size
  )
{
  UINTN   Index;

  for (Index = PACKAGE_MAC_OFFSET; Index < Size; Index += PACKAGE_MAC_INCR) {
    Package[Index] = (UINT8)MacAddress;
    MacAddress >>= 8;
  }
}

STATIC
VOID
PatchAmlPackage (
  CONST UINT8   *Pattern,
  CONST UINT8   *Replacement,
  UINTN         PatternLength,
  UINT8         *SsdtTable,
  UINTN         TableSize
  )
{
  UINTN         Offset;

  for (Offset = 0; Offset < (TableSize - PatternLength); Offset++) {
    if (CompareMem (SsdtTable + Offset, Pattern, PatternLength) == 0) {
      CopyMem (SsdtTable + Offset, Replacement, PatternLength);
      break;
    }
  }
}

#endif

STATIC
VOID
InstallSystemDescriptionTables (
  VOID
  )
{
  EFI_ACPI_DESCRIPTION_HEADER   *Table;
  EFI_STATUS                    Status;
  UINT32                        CpuId;
  UINTN                         Index;
  UINTN                         TableSize;
  UINTN                         TableHandle;
#if DO_XGBE
  UINT8                         MacPackage[sizeof(mDefaultMacPackageA)];
#endif

  Status = EFI_SUCCESS;
  for (Index = 0; !EFI_ERROR (Status); Index++) {
    Status = GetSectionFromFv (&gEfiCallerIdGuid, EFI_SECTION_RAW, Index,
               (VOID **) &Table, &TableSize);
    if (EFI_ERROR (Status)) {
      break;
    }

    switch (Table->OemTableId) {
    case SIGNATURE_64 ('S', 't', 'y', 'x', 'B', '1', ' ', ' '):
      CpuId = PcdGet32 (PcdSocCpuId);
      if ((CpuId & STYX_SOC_VERSION_MASK) < STYX_SOC_VERSION_B1) {
        continue;
      }
      break;

    case SIGNATURE_64 ('S', 't', 'y', 'x', 'X', 'g', 'b', 'e'):
#if DO_XGBE
      //
      // Patch the SSDT binary with the correct MAC addresses
      //
      CopyMem (MacPackage, mDefaultMacPackageA, sizeof (MacPackage));

      SetPackageAddress (MacPackage, PcdGet64 (PcdEthMacA), sizeof (MacPackage));
      PatchAmlPackage (mDefaultMacPackageA, MacPackage, sizeof (MacPackage),
        (UINT8 *)Table, TableSize);

      SetPackageAddress (MacPackage, PcdGet64 (PcdEthMacB), sizeof (MacPackage));
      PatchAmlPackage (mDefaultMacPackageB, MacPackage, sizeof (MacPackage),
        (UINT8 *)Table, TableSize);

      break;
#endif
      continue;
    }

    Status = mAcpiTableProtocol->InstallAcpiTable (mAcpiTableProtocol, Table,
                                   TableSize, &TableHandle);
    ASSERT_EFI_ERROR (Status);
    FreePool (Table);
  }
}

/**
  Entrypoint of Acpi Platform driver.

  @param  ImageHandle
  @param  SystemTable

  @return EFI_SUCCESS
  @return EFI_LOAD_ERROR
  @return EFI_OUT_OF_RESOURCES

**/
EFI_STATUS
EFIAPI
AcpiPlatformEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                Status;
  UINTN                     TableHandle;
  UINTN                     TableIndex;

  ZeroMem(AcpiTableList, sizeof(AcpiTableList));

  TableIndex = 0;
  AcpiTableList[TableIndex++] = FadtTable();
  AcpiTableList[TableIndex++] = MadtHeader();
  AcpiTableList[TableIndex++] = GtdtHeader();
  AcpiTableList[TableIndex++] = Dbg2Header();
  AcpiTableList[TableIndex++] = SpcrHeader();
  AcpiTableList[TableIndex++] = McfgHeader();
  AcpiTableList[TableIndex++] = CsrtHeader();
  if (PcdGetBool (PcdEnableSmmus)) {
    AcpiTableList[TableIndex++] = IortHeader();
  }
  AcpiTableList[TableIndex++] = PpttHeader();
  AcpiTableList[TableIndex++] = NULL;

  DEBUG((DEBUG_INFO, "%a(): ACPI Table installer\n", __FUNCTION__));

  //
  // Find the AcpiTable protocol
  //
  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL,
                  (VOID**)&mAcpiTableProtocol);
  if (EFI_ERROR (Status)) {
    DEBUG((EFI_D_ERROR, "Failed to locate AcpiTable protocol. Status = %r\n", Status));
    ASSERT_EFI_ERROR(Status);
  }

  TableIndex = 0;
  while (AcpiTableList[TableIndex] != NULL) {
    //
    // Install ACPI table
    //
    DEBUG ((EFI_D_ERROR, "Installing %c%c%c%c Table (Revision %d, Length %d) ...\n",
                          *((UINT8*)&AcpiTableList[TableIndex]->Signature),
                          *((UINT8*)&AcpiTableList[TableIndex]->Signature + 1),
                          *((UINT8*)&AcpiTableList[TableIndex]->Signature + 2),
                          *((UINT8*)&AcpiTableList[TableIndex]->Signature + 3),
                          AcpiTableList[TableIndex]->Revision,
                          AcpiTableList[TableIndex]->Length));

    Status = mAcpiTableProtocol->InstallAcpiTable (
                                   mAcpiTableProtocol,
                                   AcpiTableList[TableIndex],
                                   (AcpiTableList[TableIndex])->Length,
                                   &TableHandle
                                   );
    if (EFI_ERROR (Status)) {
      DEBUG((DEBUG_ERROR,"Error adding ACPI Table. Status = %r\n", Status));
      ASSERT_EFI_ERROR(Status);
    }
    TableIndex++;
    ASSERT( TableIndex < MAX_ACPI_TABLES );
  }

  InstallSystemDescriptionTables ();

  return EFI_SUCCESS;
}

