/** @file
*
*  Copyright (c) 2018, Hisilicon Limited. All rights reserved.
*  Copyright (c) 2018, Linaro Limited. All rights reserved.
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
#include <Library/ArmSmcLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/OemConfigData.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include "Apei.h"
#include <PlatformArch.h>
#include "Bert.h"
#include "Einj.h"
#include "Erst.h"
#include "Hest.h"

EFI_ACPI_TABLE_PROTOCOL        *mAcpiTableProtocol = NULL;
EFI_ACPI_SDT_PROTOCOL          *mAcpiSdtProtocol = NULL;
APEI_TRUSTED_FIRMWARE_STRUCTURE      *mApeiTrustedfirmwareData;

EFI_STATUS
EFIAPI
ApeiEntryPoint (
  IN EFI_HANDLE          ImageHandle,
  IN EFI_SYSTEM_TABLE    *SystemTable
)
{
  EFI_STATUS              Status;
  ARM_SMC_ARGS            SmcRegs = {0};
  UINTN                   Size = sizeof (OEM_CONFIG_DATA);
  OEM_CONFIG_DATA         SetupData;

  Status = gRT->GetVariable (
             OEM_CONFIG_NAME,
             &gOemConfigGuid,
             NULL,
             &Size,
             &SetupData
             );
  if (EFI_ERROR (Status)) {
    SetupData.EnRasSupport = 1;
    DEBUG ((DEBUG_ERROR, "[%a]GetVariable %r.Get default variable value\n",
      __FUNCTION__, Status));
  }
  if (!SetupData.EnRasSupport) {
    return EFI_ABORTED;
  }
  if (PcdGet64 (PcdTrustedFirmwareEnable) == 0) {
    return EFI_ABORTED;
  }
  Status = gBS->LocateProtocol (
                  &gEfiAcpiTableProtocolGuid,
                  NULL,
                  (VOID**)&mAcpiTableProtocol);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  Status = gBS->LocateProtocol (
                &gEfiAcpiSdtProtocolGuid,
                NULL,
                (VOID**)&mAcpiSdtProtocol);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  Status = gBS->AllocatePool (
             EfiReservedMemoryType,
             sizeof (APEI_TRUSTED_FIRMWARE_STRUCTURE),
             (VOID**)&mApeiTrustedfirmwareData
           );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  gBS->SetMem (
    mApeiTrustedfirmwareData,
    sizeof (APEI_TRUSTED_FIRMWARE_STRUCTURE),
    0
  );
  Status = EFI_SUCCESS;
  Status |= OemInitBertTable (ImageHandle);
  Status |= OemInitHestTable (ImageHandle);
  Status |= OemInitErstTable ();
  Status |= OemInitEinjTable ();
  // smc call
  DEBUG ((DEBUG_INFO, "[%a]:[%dL]: %r\n", __FUNCTION__, __LINE__, Status));
  if (Status == EFI_SUCCESS) {
    SmcRegs.Arg0 = PRIVATE_ARM_SMC_ID_APEI;
    SmcRegs.Arg1 = (UINTN)mApeiTrustedfirmwareData;
    ArmCallSmc (&SmcRegs);
  }
  DEBUG ((DEBUG_INFO, "Acpi Apei init done.\n"));
  return EFI_SUCCESS;
}


