/** @file

  Copyright (c) 2020, Jeremy Linton All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

  This driver acts like a stub to set the Broadcom
  Genet MAC address, until the actual network driver
  is in place.

**/

#include <Library/ArmLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>

#include <Net/Genet.h>
#include <PiDxe.h>

STATIC
VOID
RMWRegister (
  UINT32                Offset,
  UINT32                Mask,
  UINT32                In
  )
{
  EFI_PHYSICAL_ADDRESS  Addr;
  UINT32                Data;
  UINT32                Shift;

  Addr = GENET_BASE_ADDRESS + Offset;
  Data = 0;
  Shift = 1;
  if (In) {
    while (!(Mask & Shift))
      Shift <<= 1;
    Data = (MmioRead32 (Addr) & ~Mask) | ((In * Shift) & Mask);
  } else {
    Data = MmioRead32 (Addr) & ~Mask;
  }

  MmioWrite32 (Addr, Data);

  ArmDataMemoryBarrier ();
}

STATIC
VOID
WdRegister (
  UINT32                Offset,
  UINT32                In
  )
{
  EFI_PHYSICAL_ADDRESS  Base = GENET_BASE_ADDRESS;

  MmioWrite32 (Base + Offset, In);

  ArmDataMemoryBarrier ();
}

STATIC
VOID
SetMacAddress (
  UINT8*                MacAddr
)
{
  // Bring the UMAC out of reset
  RMWRegister (GENET_SYS_RBUF_FLUSH_CTRL, 0x2, 1);
  gBS->Stall (10);
  RMWRegister (GENET_SYS_RBUF_FLUSH_CTRL, 0x2, 0);

  // Update the MAC
  DEBUG ((DEBUG_INFO, "Using MAC address %02X:%02X:%02X:%02X:%02X:%02X\n",
    MacAddr[0], MacAddr[1], MacAddr[2], MacAddr[3], MacAddr[4], MacAddr[5]));

  WdRegister (GENET_UMAC_MAC0, (MacAddr[0] << 24) | (MacAddr[1] << 16) |
    (MacAddr[2] << 8) | MacAddr[3]);
  WdRegister (GENET_UMAC_MAC1, (MacAddr[4] << 8) | MacAddr[5]);

}

/**
  The entry point of Genet UEFI Driver.

  @param  ImageHandle                The image handle of the UEFI Driver.
  @param  SystemTable                A pointer to the EFI System Table.

  @retval  EFI_SUCCESS               The Driver or UEFI Driver exited normally.
  @retval  EFI_INCOMPATIBLE_VERSION  _gUefiDriverRevision is greater than
                                     SystemTable->Hdr.Revision.

**/
EFI_STATUS
EFIAPI
GenetEntryPoint (
  IN  EFI_HANDLE          ImageHandle,
  IN  EFI_SYSTEM_TABLE    *SystemTable
  )
{
  UINT64 MacAddr;

  // Read the MAC address
  MacAddr = PcdGet64 (PcdBcmGenetMacAddress);

  if (MacAddr != 0) {
    SetMacAddress ((UINT8*)&MacAddr);
  }

  return EFI_SUCCESS;
}
