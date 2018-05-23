/** @file

  Copyright (c) 2018, ARM Ltd. All rights reserved.<BR>

  This program and the accompanying materials are licensed and made available
  under the terms and conditions of the BSD License which accompanies this
  distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Library/VirtioMmioDeviceLib.h>
#include <Library/DevicePathLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <SgiPlatform.h>

#pragma pack (1)
typedef struct {
  VENDOR_DEVICE_PATH                  Vendor;
  EFI_DEVICE_PATH_PROTOCOL            End;
} VIRTIO_BLK_DEVICE_PATH;
#pragma pack ()

STATIC VIRTIO_BLK_DEVICE_PATH mVirtioBlockDevicePath =
{
  {
    {
      HARDWARE_DEVICE_PATH,
      HW_VENDOR_DP,
      {
        (UINT8)(sizeof (VENDOR_DEVICE_PATH)),
        (UINT8)((sizeof (VENDOR_DEVICE_PATH)) >> 8)
      }
    },
    EFI_CALLER_ID_GUID,
  },
  {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    {
      sizeof (EFI_DEVICE_PATH_PROTOCOL),
      0
    }
  }
};

/**
 * Entrypoint for 'VirtioBlockIo' driver
 */
EFI_STATUS
InitVirtioBlockIo (
   IN EFI_HANDLE         ImageHandle
  )
{
  EFI_STATUS Status;

  Status = gBS->InstallProtocolInterface (&ImageHandle,
                  &gEfiDevicePathProtocolGuid, EFI_NATIVE_INTERFACE,
                  &mVirtioBlockDevicePath);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Failed to install the EFI_DEVICE_PATH "
      "protocol for the virtio block device (Status == %r)\n",
      __FUNCTION__, Status));
    return Status;
  }

  Status = VirtioMmioInstallDevice (SGI_EXP_SYSPH_VIRTIO_BLOCK_BASE, ImageHandle);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR,
      "%a: Failed to install Virtio block device (Status == %r)\n",
      __FUNCTION__, Status));
    gBS->UninstallProtocolInterface (ImageHandle,
           &gEfiDevicePathProtocolGuid, &mVirtioBlockDevicePath);
  }

  return Status;
}
