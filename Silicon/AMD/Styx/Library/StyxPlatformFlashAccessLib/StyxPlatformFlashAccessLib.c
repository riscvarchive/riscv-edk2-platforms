/** @file
  Platform flash device access library for AMD Styx

  Copyright (c) 2017, Linaro, Ltd. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiDxe.h>

#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/PlatformFlashAccessLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include <Protocol/AmdIscpDxeProtocol.h>

STATIC CONST UINT64 mFlashOffset = FixedPcdGet64 (PcdFvBaseAddress) -
                                   FixedPcdGet64 (PcdFdBaseAddress);
STATIC CONST UINT64 mFlashMaxSize = FixedPcdGet64 (PcdFvSize);

STATIC CONST UINTN mBlockSize = SIZE_64KB;

/**
  Perform flash write operation.

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
  EFI_STATUS                      Status;
  AMD_ISCP_DXE_PROTOCOL           *IscpDxeProtocol;

  if (FlashAddressType != FlashAddressTypeRelativeAddress) {
    DEBUG ((DEBUG_ERROR, "%a: only FlashAddressTypeRelativeAddress supported\n",
      __FUNCTION__));

    return EFI_INVALID_PARAMETER;
  }

  if (FirmwareType != PlatformFirmwareTypeSystemFirmware) {
    DEBUG ((DEBUG_ERROR,
      "%a: only PlatformFirmwareTypeSystemFirmware supported\n",
      __FUNCTION__));

    return EFI_INVALID_PARAMETER;
  }

  if ((FlashAddress % mBlockSize) != 0 || (Length % mBlockSize) != 0) {
    DEBUG ((DEBUG_ERROR,
      "%a:region [0x%lx, 0x%lx) is not a multiple of the blocksize 0x%lx\n",
      __FUNCTION__, FlashAddress, Length, mBlockSize));
    return EFI_INVALID_PARAMETER;
  }

  if (FlashAddress < mFlashOffset ||
      (FlashAddress + Length) > (mFlashOffset + mFlashMaxSize)) {
    DEBUG ((DEBUG_ERROR,
      "%a: updated region [0x%lx, 0x%lx) outside of FV region [0x%lx, 0x%lx)\n",
      __FUNCTION__, FlashAddress, FlashAddress + Length, mFlashOffset,
      mFlashOffset + mFlashMaxSize));
    return EFI_INVALID_PARAMETER;
  }

  Status = gBS->LocateProtocol (&gAmdIscpDxeProtocolGuid, NULL,
                  (VOID **)&IscpDxeProtocol);
  ASSERT_EFI_ERROR (Status);

  while (Length > 0) {
    //
    // Erase the block
    //
    DEBUG ((DEBUG_INFO, "%a: erasing 0x%llx bytes at address 0x%llx\n",
      __FUNCTION__, mBlockSize, FlashAddress));

    Status = IscpDxeProtocol->AmdExecuteEraseFvBlockDxe (IscpDxeProtocol,
                                FlashAddress, mBlockSize);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: AmdExecuteEraseFvBlockDxe () failed - %r\n",
        __FUNCTION__, Status));
    }

    //
    // Write the new data
    //
    DEBUG ((DEBUG_INFO, "%a: writing 0x%llx bytes at at address 0x%llx\\n",
      __FUNCTION__, mBlockSize, FlashAddress));

    Status = IscpDxeProtocol->AmdExecuteUpdateFvBlockDxe (IscpDxeProtocol,
                                FlashAddress, Buffer, mBlockSize);

    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR,
        "%a: write of block address 0x%lx failed - %r\n",
        __FUNCTION__, FlashAddress, Status));
    }

    FlashAddress += mBlockSize;
    Buffer += mBlockSize;
    Length -= mBlockSize;
  }

  return EFI_SUCCESS;
}
