/** @file
  Platform Flash Access library.

  Copyright (c) 2018, Hisilicon Limited. All rights reserved.
  Copyright (c) 2018, Linaro Limited. All rights reserved.
  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiDxe.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/PlatformFlashAccessLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/HisiSpiFlashProtocol.h>

STATIC EFI_PHYSICAL_ADDRESS     mInternalFdAddress;
STATIC EFI_PHYSICAL_ADDRESS     mSFCMEM0BaseAddress;

STATIC HISI_SPI_FLASH_PROTOCOL         *mSpiProtocol;

/**
  Perform flash write opreation.

  @param[in] FirmwareType      The type of firmware.
  @param[in] FlashAddress      The address of flash device to be accessed.
  @param[in] FlashAddressType  The type of flash device address.
  @param[in] Buffer            The pointer to the data buffer.
  @param[in] Length            The length of data buffer in bytes.

  @retval EFI_SUCCESS           The operation returns successfully.
  @retval EFI_WRITE_PROTECTED   The flash device is read only.
  @retval EFI_UNSUPPORTED       The flash device access is unsupported.
  @retval EFI_INVALID_PARAMETER The input parameter is not valid.
**/
EFI_STATUS
EFIAPI
PerformFlashWrite (
  IN PLATFORM_FIRMWARE_TYPE       FirmwareType,
  IN EFI_PHYSICAL_ADDRESS         FlashAddress,
  IN FLASH_ADDRESS_TYPE           FlashAddressType,
  IN VOID                         *Buffer,
  IN UINTN                        Length
  )
{
  UINT32               RomAddress;
  EFI_STATUS           Status;

  DEBUG ((DEBUG_INFO,
          "PerformFlashWrite - 0x%x(%x) - 0x%x\n",
          (UINTN)FlashAddress,
          (UINTN)FlashAddressType,
          Length));

  if (FlashAddressType == FlashAddressTypeAbsoluteAddress) {
    FlashAddress = FlashAddress - mInternalFdAddress;
  }

  RomAddress = (UINT32)FlashAddress + (mInternalFdAddress - mSFCMEM0BaseAddress);

  DEBUG ((DEBUG_INFO, "Erase and Write Flash Start\n"));

  Status = mSpiProtocol->EraseWrite (
                           mSpiProtocol,
                           (UINT32) RomAddress,
                           (UINT8 *)Buffer,
                           (UINT32) Length
                           );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Erase and Write Status = %r \n", Status));
  }

  return Status;
}

/**
  Platform Flash Access Lib Constructor.

  @param[in]  ImageHandle       The firmware allocated handle for the EFI image.
  @param[in]  SystemTable       A pointer to the EFI System Table.

  @retval EFI_SUCCESS  Constructor returns successfully.
**/
EFI_STATUS
EFIAPI
PerformFlashAccessLibConstructor (
  IN EFI_HANDLE                         ImageHandle,
  IN EFI_SYSTEM_TABLE                   *SystemTable
  )
{
  EFI_STATUS  Status;

  mInternalFdAddress = (EFI_PHYSICAL_ADDRESS) PcdGet64 (PcdFdBaseAddress);

  mSFCMEM0BaseAddress = (EFI_PHYSICAL_ADDRESS) PcdGet64 (PcdSFCMEM0BaseAddress);

  DEBUG ((DEBUG_INFO,
          "PcdFlashAreaBaseAddress - 0x%x, PcdSFCMEM0BaseAddress - 0x%x \n",
          mInternalFdAddress,
          mSFCMEM0BaseAddress));

  Status = gBS->LocateProtocol (
                  &gHisiSpiFlashProtocolGuid,
                  NULL,
                  (VOID **)&mSpiProtocol);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR,
            "LocateProtocol gHisiSpiFlashProtocolGuid Status = %r \n",
            Status));
  }

  return Status;
}
