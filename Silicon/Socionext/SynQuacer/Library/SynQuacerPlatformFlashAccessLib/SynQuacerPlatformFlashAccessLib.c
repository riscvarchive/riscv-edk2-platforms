/** @file
  Platform flash device access library for Socionext SynQuacer

  Copyright (c) 2016, Linaro, Ltd. All rights reserved.<BR>

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
#include <Library/DxeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PlatformFlashAccessLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include <Protocol/FirmwareVolumeBlock.h>

/**
  Gets firmware volume block handle by given address.

  This function gets firmware volume block handle whose
  address range contains the parameter Address.

  @param[in]  Address    Address which should be contained
                         by returned FVB handle.
  @param[out] FvbHandle  Pointer to FVB handle for output.

  @retval EFI_SUCCESS    FVB handle successfully returned.
  @retval EFI_NOT_FOUND  Failed to find FVB handle by address.

**/
STATIC
EFI_STATUS
GetFvbByAddress (
  IN  EFI_PHYSICAL_ADDRESS                Address,
  OUT EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL  **OutFvb,
  OUT EFI_PHYSICAL_ADDRESS                *FvbBaseAddress
  )
{
  EFI_STATUS                          Status;
  EFI_HANDLE                          *HandleBuffer;
  UINTN                               HandleCount;
  UINTN                               Index;
  EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL  *Fvb;
  EFI_FIRMWARE_VOLUME_HEADER          *FwVolHeader;
  EFI_FVB_ATTRIBUTES_2                Attributes;

  //
  // Locate all handles with Firmware Volume Block protocol
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareVolumeBlockProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }
  //
  // Traverse all the handles, searching for the one containing parameter Address
  //
  for (Index = 0; Index < HandleCount; Index += 1) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiFirmwareVolumeBlockProtocolGuid,
                    (VOID **) &Fvb
                    );
    if (EFI_ERROR (Status)) {
      Status = EFI_NOT_FOUND;
      break;
    }
    //
    // Checks if the address range of this handle contains parameter Address
    //
    Status = Fvb->GetPhysicalAddress (Fvb, FvbBaseAddress);
    if (EFI_ERROR (Status)) {
      continue;
    }

    //
    // Check if this FVB is writable: DXE core produces FVB protocols for
    // firmware volumes as well.
    //
    Status = Fvb->GetAttributes (Fvb, &Attributes);
    if (EFI_ERROR (Status) || !(Attributes & EFI_FVB2_WRITE_STATUS)) {
      DEBUG ((DEBUG_INFO,
        "%a: ignoring read-only FVB protocol implementation\n",
        __FUNCTION__));
      continue;
    }

    FwVolHeader = (EFI_FIRMWARE_VOLUME_HEADER *) ((UINTN) *FvbBaseAddress);
    if ((Address >= *FvbBaseAddress) && (Address <= (*FvbBaseAddress + FwVolHeader->FvLength))) {
      *OutFvb  = Fvb;
      Status   = EFI_SUCCESS;
      break;
    }

    Status = EFI_NOT_FOUND;
  }

  FreePool (HandleBuffer);
  return Status;
}

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
  EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL  *Fvb;
  EFI_STATUS                          Status;
  UINTN                               BlockSize;
  UINTN                               NumberOfBlocks;
  EFI_LBA                             Lba;
  EFI_PHYSICAL_ADDRESS                FvbBaseAddress;
  UINTN                               NumBytes;

  if (FlashAddressType != FlashAddressTypeAbsoluteAddress) {
    DEBUG ((DEBUG_ERROR, "%a: only FlashAddressTypeAbsoluteAddress supported\n",
      __FUNCTION__));

    return EFI_INVALID_PARAMETER;
  }

  if (FirmwareType != PlatformFirmwareTypeSystemFirmware) {
    DEBUG ((DEBUG_ERROR,
      "%a: only PlatformFirmwareTypeSystemFirmware supported\n",
      __FUNCTION__));

    return EFI_INVALID_PARAMETER;
  }

  //
  // Locate the gEfiFirmwareVolumeBlockProtocolGuid implementation
  // that covers the system firmware
  //
  Fvb = NULL;
  Status = GetFvbByAddress (FlashAddress, &Fvb, &FvbBaseAddress);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR,
      "%a: failed to locate FVB handle for address 0x%llx - %r\n",
      __FUNCTION__, FlashAddress, Status));
    return Status;
  }

  Status = Fvb->GetBlockSize(Fvb, 0, &BlockSize, &NumberOfBlocks);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: failed to get FVB blocksize - %r\n",
      __FUNCTION__, Status));
    return Status;
  }

  if ((Length % BlockSize) != 0) {
    DEBUG ((DEBUG_ERROR,
      "%a: Length 0x%lx is not a multiple of the blocksize 0x%lx\n",
      __FUNCTION__, Length, BlockSize));
    return EFI_INVALID_PARAMETER;
  }

  Lba = (FlashAddress - FvbBaseAddress) / BlockSize;
  if (Lba > NumberOfBlocks - 1) {
    DEBUG ((DEBUG_ERROR,
      "%a: flash device with non-uniform blocksize not supported\n",
      __FUNCTION__));
    return EFI_UNSUPPORTED;
  }

  //
  // Remap the region as device rather than uncached.
  //
  Status = gDS->AddMemorySpace (EfiGcdMemoryTypeMemoryMappedIo,
                  FlashAddress, Length, EFI_MEMORY_UC);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_WARN, "%a: gDS->AddMemorySpace () failed - %r\n",
      __FUNCTION__, Status));
  }

  Status = gDS->SetMemorySpaceAttributes (FlashAddress, Length, EFI_MEMORY_UC);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: gDS->SetMemorySpaceAttributes () failed - %r\n",
      __FUNCTION__, Status));
    return Status;
  }

  //
  // Erase the region
  //
  DEBUG ((DEBUG_INFO, "%a: erasing 0x%llx bytes at address %llx (LBA 0x%lx)\n",
    __FUNCTION__, Length, FlashAddress, Lba));

  Status = Fvb->EraseBlocks (Fvb, Lba, Length / BlockSize,
                  EFI_LBA_LIST_TERMINATOR);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Fvb->EraseBlocks () failed - %r\n",
      __FUNCTION__, Status));
    return Status;
  }

  while (Length > 0) {
    //
    // Write the new data
    //
    DEBUG ((DEBUG_INFO, "%a: writing 0x%llx bytes at LBA 0x%lx\n",
      __FUNCTION__, BlockSize, Lba));

    NumBytes = BlockSize;
    Status = Fvb->Write (Fvb, Lba, 0, &NumBytes, Buffer);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR,
        "%a: write of LBA 0x%lx failed - %r (NumBytes == 0x%lx)\n",
        __FUNCTION__, Lba, Status, NumBytes));
    }

    Buffer += BlockSize;
    Length -= BlockSize;
    Lba++;
  }

  return EFI_SUCCESS;
}
