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

#include "IntelBdsPlatform.h"
#include <Library/UefiLib.h>
#include <Library/GenericBdsLib.h>
#include <Protocol/FirmwareVolume2.h>

VOID
EFIAPI
BdsLibBuildOptionFromApp (
  IN EFI_HANDLE                  Handle,
  IN OUT LIST_ENTRY              *BdsBootOptionList,
  IN GUID                        *FileGuid,
  IN CHAR16                      *Description
  )
{
  EFI_DEVICE_PATH_PROTOCOL          *DevicePath;
  MEDIA_FW_VOL_FILEPATH_DEVICE_PATH ShellNode;

  DevicePath = DevicePathFromHandle (Handle);

  //
  // Build the shell device path
  //
  EfiInitializeFwVolDevicepathNode (&ShellNode, FileGuid);

  DevicePath = AppendDevicePathNode (DevicePath, (EFI_DEVICE_PATH_PROTOCOL *) &ShellNode);

  //
  // Create and register the shell boot option
  //
  BdsLibRegisterNewOption (BdsBootOptionList, DevicePath, Description, L"BootOrder");

}
EFI_STATUS
EFIAPI
HwBdsLibRegisterAppBootOption (
  IN OUT LIST_ENTRY              *BdsBootOptionList,
  IN GUID                        *FileGuid,
  IN CHAR16                      *Description
  )
{
  EFI_STATUS                    Status;
  UINTN                         Index;
  UINTN                         FvHandleCount;
  EFI_HANDLE                    *FvHandleBuffer;
  EFI_FV_FILETYPE               Type;
  UINTN                         Size;
  EFI_FV_FILE_ATTRIBUTES        Attributes;
  UINT32                        AuthenticationStatus;
  EFI_FIRMWARE_VOLUME2_PROTOCOL *Fv;
  UINTN                         Count = 0;

  //
  // Check if we have on flash shell
  //
  gBS->LocateHandleBuffer (
        ByProtocol,
        &gEfiFirmwareVolume2ProtocolGuid,
        NULL,
        &FvHandleCount,
        &FvHandleBuffer
        );
  for (Index = 0; Index < FvHandleCount; Index++) {
    gBS->HandleProtocol (
          FvHandleBuffer[Index],
          &gEfiFirmwareVolume2ProtocolGuid,
          (VOID **) &Fv
          );

    Status = Fv->ReadFile (
                  Fv,
                  FileGuid,
                  NULL,
                  &Size,
                  &Type,
                  &Attributes,
                  &AuthenticationStatus
                  );
    if (EFI_ERROR (Status)) {
      //
      // Skip if no shell file in the FV
      //
      continue;
    }
    //
    // Build the shell boot option
    //
    BdsLibBuildOptionFromApp (FvHandleBuffer[Index], BdsBootOptionList,
        FileGuid, Description);

    Count++;
  }

  if (FvHandleCount != 0) {
    FreePool (FvHandleBuffer);
  }

  if (Count == 0) {
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}

