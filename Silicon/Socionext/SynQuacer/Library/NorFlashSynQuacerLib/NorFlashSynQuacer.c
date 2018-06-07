/** @file

 Copyright (c) 2011-2014, ARM Ltd. All rights reserved.<BR>

 This program and the accompanying materials
 are licensed and made available under the terms and conditions of the BSD License
 which accompanies this distribution.  The full text of the license may be found at
 http://opensource.org/licenses/bsd-license.php

 THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
 WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

 **/

#include <Base.h>
#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/NorFlashPlatformLib.h>

#include <Platform/MemoryMap.h>

#define FW_CODE_REGION_BASE SYNQUACER_SPI_NOR_BASE
#define FW_CODE_REGION_SIZE (FW_ENV_REGION_BASE - FW_CODE_REGION_BASE)

#define FW_ENV_REGION_BASE  FixedPcdGet32 (PcdFlashNvStorageVariableBase)
#define FW_ENV_REGION_SIZE  (FixedPcdGet32 (PcdFlashNvStorageVariableSize) + \
                             FixedPcdGet32 (PcdFlashNvStorageFtwWorkingSize) + \
                             FixedPcdGet32 (PcdFlashNvStorageFtwSpareSize))

STATIC NOR_FLASH_DESCRIPTION mNorFlashDevices[] = {
  {
    // UEFI code region
    SYNQUACER_SPI_NOR_BASE,                             // device base
    FW_CODE_REGION_BASE,                                // region base
    FW_CODE_REGION_SIZE,                                // region size
    SIZE_64KB,                                          // block size
    {
      0x19c118b0, 0xc423, 0x42be, { 0xb8, 0x0f, 0x70, 0x6f, 0x1f, 0xcb, 0x59, 0x9a }
    }
  },
  {
    // Environment variable region
    SYNQUACER_SPI_NOR_BASE,                             // device base
    FW_ENV_REGION_BASE,                                 // region base
    FW_ENV_REGION_SIZE,                                 // region size
    SIZE_64KB,                                          // block size
    {
      0x3105bd7a, 0x82c3, 0x486f, { 0xb1, 0x03, 0x1e, 0x09, 0x54, 0xec, 0x85, 0x75 }
    }
  },
};

EFI_STATUS
NorFlashPlatformInitialization (
  VOID
  )
{
  return EFI_SUCCESS;
}

EFI_STATUS
NorFlashPlatformGetDevices (
  OUT NOR_FLASH_DESCRIPTION   **NorFlashDevices,
  OUT UINT32                  *Count
  )
{
  if (NorFlashDevices == NULL ||
      Count == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *Count = ARRAY_SIZE (mNorFlashDevices);
  *NorFlashDevices = mNorFlashDevices;

  return EFI_SUCCESS;
}
