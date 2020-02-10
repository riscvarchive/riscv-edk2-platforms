/** @file
 *
 *  Copyright (c) 2020, Pete Batard <pete@akeo.ie>
 *
 *  SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 **/

#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol/RpiFirmware.h>

EFI_STATUS
EFIAPI
PlatformPcdLibConstructor (
  IN EFI_HANDLE ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS                       Status;
  UINT64                           MacAddr;
  RASPBERRY_PI_FIRMWARE_PROTOCOL   *mFwProtocol;

  if (PcdGet64 (PcdBcmGenetMacAddress) == 0) {
    Status = gBS->LocateProtocol (&gRaspberryPiFirmwareProtocolGuid, NULL,
                    (VOID**)&mFwProtocol);
    ASSERT_EFI_ERROR(Status);

    //
    // Get the MAC address from the firmware
    //
    Status = mFwProtocol->GetMacAddress ((UINT8*) &MacAddr);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_WARN, "%a: failed to retrieve MAC address\n", __FUNCTION__));
    } else {
      PcdSet64S (PcdBcmGenetMacAddress, MacAddr);
    }
  }

  return EFI_SUCCESS;
}
