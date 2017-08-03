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

#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiBootServicesTableLib.h>

#define MAX_ACPI_TABLES    12

EFI_ACPI_DESCRIPTION_HEADER *AcpiTableList[MAX_ACPI_TABLES];


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
  EFI_ACPI_TABLE_PROTOCOL   *AcpiTable;
  UINTN                     TableHandle;
  UINTN                     TableIndex;

  ZeroMem(AcpiTableList, sizeof(AcpiTableList));

  TableIndex = 0;
  AcpiTableList[TableIndex++] = FadtTable();
  AcpiTableList[TableIndex++] = DsdtHeader();
  AcpiTableList[TableIndex++] = MadtHeader();
  AcpiTableList[TableIndex++] = GtdtHeader();
  AcpiTableList[TableIndex++] = Dbg2Header();
  AcpiTableList[TableIndex++] = SpcrHeader();
  AcpiTableList[TableIndex++] = McfgHeader();
  AcpiTableList[TableIndex++] = CsrtHeader();
  if (PcdGetBool (PcdEnableSmmus)) {
    AcpiTableList[TableIndex++] = IortHeader();
  }
  AcpiTableList[TableIndex++] = NULL;

  DEBUG((DEBUG_INFO, "%a(): ACPI Table installer\n", __FUNCTION__));

  //
  // Find the AcpiTable protocol
  //
  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID**)&AcpiTable);
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

    Status = AcpiTable->InstallAcpiTable (
                            AcpiTable,
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

  return EFI_SUCCESS;
}

