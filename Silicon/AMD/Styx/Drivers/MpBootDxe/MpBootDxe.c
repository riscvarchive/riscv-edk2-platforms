/** @file

  Copyright (c) 2016, AMD Inc. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Library/ArmLib.h>
#include <Library/ArmSmcLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/CacheMaintenanceLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>

#include <Guid/ArmMpCoreInfo.h>
#include <Protocol/AmdMpBoot.h>


/* These externs are used to relocate our Pen code into pre-allocated memory */
extern VOID  *SecondariesPenStart;
extern VOID  *SecondariesPenEnd;
extern UINTN *AsmParkingBase;
extern UINTN *AsmMailboxBase;


STATIC
EFI_PHYSICAL_ADDRESS
ConfigurePen (
  IN EFI_PHYSICAL_ADDRESS     MpParkingBase,
  IN UINTN                    MpParkingSize,
  IN ARM_CORE_INFO            *ArmCoreInfoTable,
  IN UINTN                    ArmCoreCount
  )
{
  EFI_PHYSICAL_ADDRESS     PenBase;
  UINTN                    PenSize;
  UINTN                    MailboxBase;
  UINTN                    CoreNum;
  UINTN                    CoreMailbox;
  UINTN                    CoreParking;

  //
  // Set Pen at the 2K-offset of the Parking area, skipping an 8-byte slot for the Core#.
  // For details, refer to the "Multi-processor Startup for ARM Platforms" document:
  // https://acpica.org/sites/acpica/files/MP%20Startup%20for%20ARM%20platforms.docx
  //
  PenBase = (EFI_PHYSICAL_ADDRESS)((UINTN)MpParkingBase + SIZE_2KB + sizeof(UINT64));
  PenSize  = (UINTN)&SecondariesPenEnd - (UINTN)&SecondariesPenStart;

  // Relocate the Pen code
  CopyMem ((VOID*)(PenBase), (VOID*)&SecondariesPenStart, PenSize);

  // Put spin-table mailboxes below the pen code so we know where they are relative to code.
  // Make sure this is 8 byte aligned.
  MailboxBase = (UINTN)PenBase + ((UINTN)&SecondariesPenEnd - (UINTN)&SecondariesPenStart);
  if (MailboxBase % sizeof(UINT64) != 0) {
    MailboxBase += sizeof(UINT64) - MailboxBase % sizeof(UINT64);
  }

  // Update variables used in the Pen code
  *(UINTN*)(PenBase + ((UINTN)&AsmMailboxBase - (UINTN)&SecondariesPenStart)) = MailboxBase;
  *(UINTN*)(PenBase + ((UINTN)&AsmParkingBase - (UINTN)&SecondariesPenStart)) = (UINTN)MpParkingBase;

  for (CoreNum = 0; CoreNum < ArmCoreCount; CoreNum++) {
    // Clear the jump address at spin-table slot
    CoreMailbox = MailboxBase + CoreNum * sizeof (UINT64);
    *((UINTN*)(CoreMailbox)) = 0x0;

    // Clear the jump address and set Core# at mp-parking slot
    CoreParking = (UINTN)MpParkingBase + CoreNum * SIZE_4KB;
    *((UINTN*)(CoreParking + sizeof (UINT64))) = 0x0;
    *((UINTN*)(CoreParking + SIZE_2KB)) = CoreNum;

    // Update table entry to be consumed by FDT parser
    ArmCoreInfoTable[CoreNum].MailboxSetAddress = CoreMailbox;
  }

  // flush the cache before launching secondary cores
  WriteBackDataCacheRange ((VOID *)MpParkingBase, MpParkingSize);

  return PenBase;
}


EFI_STATUS
EFIAPI
MpBootDxeEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS               Status;
  AMD_MP_BOOT_PROTOCOL     *MpBootProtocol;
  EFI_PHYSICAL_ADDRESS     MpParkingBase;
  UINTN                    MpParkingSize;
  ARM_CORE_INFO            *ArmCoreInfoTable;
  UINTN                    ArmCoreCount;
  UINTN                    CoreNum;
  EFI_PHYSICAL_ADDRESS     PenBase;

  DEBUG ((EFI_D_ERROR, "MpBootDxe Loaded\n"));

  MpBootProtocol = NULL;
  Status = gBS->LocateProtocol (
               &gAmdMpBootProtocolGuid,
               NULL,
               (VOID **)&MpBootProtocol
               );
  if (EFI_ERROR (Status) || MpBootProtocol == NULL) {
    DEBUG ((EFI_D_ERROR, "Warning: Failed to locate MP-Boot Protocol.\n"));
    return EFI_UNSUPPORTED;
  }

  if ((VOID *)MpBootProtocol->MpBootInfo == NULL) {
    DEBUG ((EFI_D_ERROR, "Warning: MpBootInfo not allocated.\n"));
    return EFI_UNSUPPORTED;
  }

  MpParkingBase = MpBootProtocol->MpBootInfo->MpParkingBase;
  if ((VOID *)MpParkingBase == NULL) {
    DEBUG ((EFI_D_ERROR, "Warning: MpParkingBase not allocated.\n"));
    return EFI_UNSUPPORTED;
  }
  if (((UINTN)MpParkingBase & (SIZE_4KB -1)) != 0) {
    DEBUG ((EFI_D_ERROR, "Warning: MpParkingBase not 4K aligned.\n"));
    return EFI_UNSUPPORTED;
  }

  ArmCoreInfoTable = MpBootProtocol->MpBootInfo->ArmCoreInfoTable;
  if (ArmCoreInfoTable == NULL) {
    DEBUG ((EFI_D_ERROR, "Warning: ArmCoreInfoTable not allocated.\n"));
    return EFI_UNSUPPORTED;
  }

  ArmCoreCount = MpBootProtocol->MpBootInfo->ArmCoreCount;
  if (ArmCoreCount < 2) {
    DEBUG ((EFI_D_ERROR, "Warning: Found %d cores.\n", ArmCoreCount));
    return EFI_UNSUPPORTED;
  }

  MpParkingSize = ArmCoreCount * SIZE_4KB;
  if (MpParkingSize > MpBootProtocol->MpBootInfo->MpParkingSize) {
    DEBUG ((EFI_D_ERROR, "Warning: MpParkingSize = 0x%lX, not large enough for %d cores.\n",
      MpBootProtocol->MpBootInfo->MpParkingSize, ArmCoreCount));
    return EFI_UNSUPPORTED;
  }

  if ((VOID *)MpBootProtocol->ParkSecondaryCore == NULL) {
    DEBUG ((EFI_D_ERROR, "Warning: ParkSecondaryCore() not supported.\n"));
    return EFI_UNSUPPORTED;
  }

  // Move secondary cores to our new Pen
  PenBase = ConfigurePen (MpParkingBase, MpParkingSize, ArmCoreInfoTable, ArmCoreCount);
  for (CoreNum = 0; CoreNum < ArmCoreCount; CoreNum++) {
    MpBootProtocol->ParkSecondaryCore (&ArmCoreInfoTable[CoreNum], PenBase);
  }

  return EFI_SUCCESS;
}


