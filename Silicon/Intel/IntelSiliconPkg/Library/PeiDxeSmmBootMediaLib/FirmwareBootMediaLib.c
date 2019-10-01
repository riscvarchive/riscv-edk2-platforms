/** @file
  This library identifies the firmware boot media device.

  The firmware boot media device is used to make system initialization decisions in the boot flow dependent
  upon firmware boot media. Note that the firmware boot media is the storage media that the boot firmware is stored on.
  It is not the OS storage media which may be stored upon a different non-volatile storage device.

  This file contains library implementation common to all boot phases.

Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/FirmwareBootMediaLib.h>

/**
  Determines if the platform firmware is booting from SPI.

  @retval TRUE        Platform firmware is booting from SPI
  @retval FALSE       Platform firmware is booting from a non-SPI device or the boot media is unknown
**/
BOOLEAN
EFIAPI
FirmwareBootMediaIsSpi (
  VOID
  )
{
  EFI_STATUS          Status;
  FW_BOOT_MEDIA_TYPE  BootMedia;

  Status = GetFirmwareBootMediaType (&BootMedia);
  if (EFI_ERROR (Status) || BootMedia != FwBootMediaSpi) {
    return FALSE;
  } else {
    return TRUE;
  }
}

/**
  Determines if the platform firmware is booting from UFS.

  @retval TRUE        Platform firmware is booting from UFS
  @retval FALSE       Platform firmware is booting from a non-UFS device or the boot media is unknown
**/
BOOLEAN
EFIAPI
FirmwareBootMediaIsUfs (
  VOID
  )
{
  EFI_STATUS          Status;
  FW_BOOT_MEDIA_TYPE  BootMedia;

  Status = GetFirmwareBootMediaType (&BootMedia);
  if (EFI_ERROR (Status) || BootMedia != FwBootMediaUfs) {
    return FALSE;
  } else {
    return TRUE;
  }
}

/**
  Determines if the platform firmware is booting from eMMC.

  @retval TRUE        Platform firmware is booting from eMMC
  @retval FALSE       Platform firmware is booting from a non-eMMC device or the boot media is unknown
**/
BOOLEAN
EFIAPI
FirmwareBootMediaIsEmmc (
  VOID
  )
{
  EFI_STATUS          Status;
  FW_BOOT_MEDIA_TYPE  BootMedia;

  Status = GetFirmwareBootMediaType (&BootMedia);
  if (EFI_ERROR (Status) || BootMedia != FwBootMediaEmmc) {
    return FALSE;
  } else {
    return TRUE;
  }
}

/**
  Determines if the platform firmware is booting from NVMe.

  @retval TRUE        Platform firmware is booting from NVMe.
  @retval FALSE       Platform firmware is booting from a non-NVMe device or the boot media is unknown
**/
BOOLEAN
EFIAPI
FirmwareBootMediaIsNvme (
  VOID
  )
{
  EFI_STATUS          Status;
  FW_BOOT_MEDIA_TYPE  BootMedia;

  Status = GetFirmwareBootMediaType (&BootMedia);
  if (EFI_ERROR (Status) || BootMedia != FwBootMediaNvme) {
    return FALSE;
  } else {
    return TRUE;
  }
}
