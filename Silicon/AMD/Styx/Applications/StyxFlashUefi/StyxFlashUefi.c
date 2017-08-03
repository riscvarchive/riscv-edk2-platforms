/** @file

  Copyright (c) 2017, Linaro Ltd. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Uefi.h>
#include <Library/BaseMemoryLib.h>
#include <Library/ShellCEntryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>

#include <Protocol/AmdIscpDxeProtocol.h>

#define UEFI_IMAGE_OFFSET     FixedPcdGet64 (PcdFvBaseAddress) - FixedPcdGet64 (PcdFdBaseAddress)
#define BLOCK_SIZE            SIZE_64KB

STATIC AMD_ISCP_DXE_PROTOCOL  *mIscpDxeProtocol;
STATIC UINT8                  Buffer[BLOCK_SIZE];

extern CONST UINT8            StyxFlashImageStart[];
extern CONST UINT32           StyxFlashImageSize;

/***
  Main entrypoint

  Establishes the main structure of the application.

  @retval  0         The application exited normally.
  @retval  Other     An error occurred.
***/
INTN
EFIAPI
ShellAppMain (
  IN UINTN Argc,
  IN CHAR16 **Argv
  )
{
  EFI_STATUS      Status;
  UINTN           Index;
  INTN            Remaining;

  Print (L"StyxFlashUefi: firmware updater for AMD Seattle based boards.\n");

  Status = gBS->LocateProtocol (&gAmdIscpDxeProtocolGuid, NULL,
                  (VOID **)&mIscpDxeProtocol);
  if (EFI_ERROR (Status)) {
    Print (L"Failed to locate ISCP communication protocol, terminating...\n");
    return (INTN)Status;
  }

  Index = 0;
  Remaining = StyxFlashImageSize;
  do {
    Status = mIscpDxeProtocol->AmdExecuteEraseFvBlockDxe (
                                 mIscpDxeProtocol,
                                 UEFI_IMAGE_OFFSET + Index * BLOCK_SIZE,
                                 BLOCK_SIZE);
    if (EFI_ERROR (Status)) {
      Print (L"Erase failed!\n");
      return (INTN)Status;
    }

    CopyMem (Buffer, StyxFlashImageStart + Index * BLOCK_SIZE,
      MIN (Remaining, BLOCK_SIZE));

    Status = mIscpDxeProtocol->AmdExecuteUpdateFvBlockDxe (
                                 mIscpDxeProtocol,
                                 UEFI_IMAGE_OFFSET + Index * BLOCK_SIZE,
                                 Buffer,
                                 MIN (Remaining, BLOCK_SIZE));

    if (EFI_ERROR (Status)) {
      Print (L"Update failed!\n");
      return (INTN)Status;
    }

    Remaining -= BLOCK_SIZE;
    Index++;

    Print (L"Block %d of %d updated\n", Index, StyxFlashImageSize / BLOCK_SIZE);

  } while (Remaining > 0);

  Print (L"\nDone!\n");

  return 0;
}
