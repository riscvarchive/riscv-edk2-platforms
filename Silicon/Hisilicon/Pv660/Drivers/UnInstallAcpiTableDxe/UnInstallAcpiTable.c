/** @file
*
*  Copyright (c) 2015, Hisilicon Limited. All rights reserved.
*  Copyright (c) 2015, Linaro Limited. All rights reserved.
*
*  This program and the accompanying materials
*  are licensed and made available under the terms and conditions of the BSD License
*  which accompanies this distribution.  The full text of the license may be found at
*  http://opensource.org/licenses/bsd-license.php
*
*  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
*  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
*
**/


#include <Uefi.h>
#include <Pi/PiDxeCis.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/PrintLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Protocol/AcpiSystemDescriptionTable.h>
#include <Protocol/AcpiTable.h>
#include <IndustryStandard/Acpi.h>
#include <Uefi/UefiSpec.h>
#include <Guid/Acpi.h>
#include "UnInstallAcpiTable.h"

#define EFI_ACPI_MAX_NUM_TABLES         20
EFI_GUID  gSataControlGuid = EFI_SATA_CONTROL_GUID;

EFI_STATUS
UnInstallSsdtTable (
 IN EFI_HANDLE         ImageHandle,
 IN EFI_SYSTEM_TABLE  *SystemTable)
{
  EFI_STATUS              Status;
  EFI_ACPI_SDT_PROTOCOL   *AcpiTableProtocol;
  EFI_ACPI_SDT_HEADER     *Table;
  EFI_ACPI_TABLE_VERSION  TableVersion;
  UINTN                   TableKey;
  UINTN                   i;
  EFI_ACPI_TABLE_PROTOCOL               *AcpiTable;
  UINT8                   DataPtr1 = 2;
  UINTN                   DataPtr1Size;
  UINT32                  SsdtName;

  DataPtr1Size = sizeof(DataPtr1);

  Status = gRT->GetVariable (
                          SATA_ENABLE_FLAG,
                          &gSataControlGuid,
                          NULL,
                          &DataPtr1Size,
                          &DataPtr1
                          );
  if (!EFI_ERROR (Status)) {
     DEBUG((EFI_D_ERROR,"Get Variable ok\n"));

  }

  if (SATAENABLE == DataPtr1) {
    Status = gBS->InstallProtocolInterface (
                   &ImageHandle,
                   &gSataEnableFlagProtocolGuid,
                   EFI_NATIVE_INTERFACE,
                   NULL
                   );
    if (!EFI_ERROR (Status)) {
      DEBUG((EFI_D_ERROR,"Install SataEnableFlag Protocol ok, %r\n",Status));

    }
    DEBUG((EFI_D_ERROR, "Current SataEnable Flag is Sata, try to uninstall Sas SSDT table\n"));
    SsdtName = EFI_SAS_SIGNATURE;
  }
  else {
    DEBUG((EFI_D_ERROR, "Current SataEnable Flag is Sas, try to uninstall Sata SSDT table\n"));
    SsdtName = EFI_SATA_SIGNATURE;
  }

  //Locate AcpiTableProtocol
  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID**)&AcpiTable);
  if (EFI_ERROR (Status)) {
     DEBUG((EFI_D_ERROR,"Unable to locate ACPI table protocol\n"));
    return EFI_ABORTED;
  }
  //
  // Find the Acpi Sdt protocol
  Status = gBS->LocateProtocol(&gEfiAcpiSdtProtocolGuid, NULL, (VOID**) &AcpiTableProtocol);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR,"Unable to locate ACPI Sdt protocol\n"));
    return EFI_ABORTED;
  }

  //
  // Search for SSDT Table and delete the matched SSDT table
  for (i = 0; i < EFI_ACPI_MAX_NUM_TABLES; i++) {
    Status = AcpiTableProtocol->GetAcpiTable(i, &Table, &TableVersion, &TableKey);
    if (EFI_ERROR(Status))
      break;
    if (Table->Signature == EFI_ACPI_3_0_SECONDARY_SYSTEM_DESCRIPTION_TABLE_SIGNATURE) {

       if(*(UINT64*)Table->OemTableId == SsdtName) {
          Status = AcpiTable->UninstallAcpiTable (AcpiTable, TableKey);
          if (!EFI_ERROR (Status)) {
            DEBUG((EFI_D_ERROR,"Successfully remove the SSDT table\n"));
            return EFI_SUCCESS;
          }
       }
    }

  }
  return EFI_SUCCESS;

}


