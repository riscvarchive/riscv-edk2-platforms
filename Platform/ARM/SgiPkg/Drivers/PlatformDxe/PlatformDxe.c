/** @file
*
*  Copyright (c) 2018, ARM Limited. All rights reserved.
*
*  This program and the accompanying materials are licensed and made available
*  under the terms and conditions of the BSD License which accompanies this
*  distribution.  The full text of the license may be found at
*  http://opensource.org/licenses/bsd-license.php
*
*  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
*  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
*
**/

#include <Library/AcpiLib.h>
#include <Library/DebugLib.h>

EFI_STATUS
InitVirtioBlockIo (
  IN EFI_HANDLE         ImageHandle
  );

EFI_STATUS
EFIAPI
ArmSgiPkgEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS              Status;

  Status = LocateAndInstallAcpiFromFv (&gSgi575AcpiTablesiFileGuid);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Failed to install ACPI tables\n", __FUNCTION__));
    return Status;
  }

  Status = EFI_REQUEST_UNLOAD_IMAGE;
  if (FeaturePcdGet (PcdVirtioSupported)) {
    Status = InitVirtioBlockIo (ImageHandle);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: Failed to install Virtio Block device\n",
        __FUNCTION__));
    }
  }

  return Status;
}
