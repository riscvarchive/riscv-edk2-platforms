/** @file
  This driver installs SMBIOS information for QSP

  Copyright (c) 2011, Bei Guan <gbtju85@gmail.com>
  Copyright (c) 2011 - 2019, Intel Corporation. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include "SmbiosPlatformDxe.h"


UINT32
GetSystemMemorySizeBelow4gb(
  VOID
  )
{
  //
  // CMOS 0x34/0x35 specifies the system memory above 16 MB.
  // * CMOS(0x35) is the high byte
  // * CMOS(0x34) is the low byte
  // * The size is specified in 64kb chunks
  // * Since this is memory above 16MB, the 16MB must be added
  //   into the calculation to get the total memory size.
  //
  return (UINT32) (((UINTN) CmosRead16 (0x34) << 16) + SIZE_16MB);
}

STATIC
UINT64
GetSystemMemorySizeAbove4gb(
  VOID
)
{
  UINT32 Size;
  //
  // CMOS 0x5b-0x5d specifies the system memory above 4GB MB.
  // * CMOS(0x5d) is the most significant size byte
  // * CMOS(0x5c) is the middle size byte
  // * CMOS(0x5b) is the least significant size byte
  // * The size is specified in 64kb chunks
  //
  Size = (CmosRead16 (0x5c) << 8) + CmosRead8 (0x5b);

  return LShiftU64(Size, 16);
}

/**
  Installs SMBIOS information for SIMICS QSP

  @param ImageHandle     Module's image handle
  @param SystemTable     Pointer of EFI_SYSTEM_TABLE

  @retval EFI_SUCCESS    Smbios data successfully installed
  @retval Other          Smbios data was not installed

**/
EFI_STATUS
EFIAPI
SmbiosTablePublishEntry (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS                Status;
  EFI_SMBIOS_PROTOCOL       *Smbios;
  SMBIOS_TABLE_TYPE19       *Type19Record;
  EFI_SMBIOS_HANDLE         MemArrayMappedAddrSmbiosHandle;
  UINT8                     NumSlots;
  UINT64                    TotalMemorySize;

  //
  // Find the SMBIOS protocol
  //
  Status = gBS->LocateProtocol (
                  &gEfiSmbiosProtocolGuid,
                  NULL,
                  (VOID**)&Smbios
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Generate Memory Array Mapped Address info
  //
  NumSlots = 2;
  TotalMemorySize = 0;
  TotalMemorySize = GetSystemMemorySizeBelow4gb();
  Type19Record = AllocatePool(sizeof(SMBIOS_TABLE_TYPE19));
  ZeroMem(Type19Record, sizeof(SMBIOS_TABLE_TYPE19));
  Type19Record->Hdr.Type = EFI_SMBIOS_TYPE_MEMORY_ARRAY_MAPPED_ADDRESS;
  Type19Record->Hdr.Length = sizeof(SMBIOS_TABLE_TYPE19);
  Type19Record->Hdr.Handle = 0;
  Type19Record->StartingAddress = 0;
  Type19Record->EndingAddress = (UINT32)RShiftU64(TotalMemorySize, 10) - 1;
  Type19Record->MemoryArrayHandle = SMBIOS_HANDLE_PI_RESERVED;
  Type19Record->PartitionWidth = (UINT8)(NumSlots);

  MemArrayMappedAddrSmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  Status = Smbios->Add(Smbios, NULL, &MemArrayMappedAddrSmbiosHandle, (EFI_SMBIOS_TABLE_HEADER*)Type19Record);
  ASSERT_EFI_ERROR(Status);

  TotalMemorySize = GetSystemMemorySizeAbove4gb();
  Type19Record->StartingAddress = 0xFFFFFFFF;
  Type19Record->ExtendedStartingAddress = 0xFFFFFFFF;
  Type19Record->ExtendedEndingAddress = TotalMemorySize + 0xFFFFFFFF - 1;

  MemArrayMappedAddrSmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  Status = Smbios->Add(Smbios, NULL, &MemArrayMappedAddrSmbiosHandle, (EFI_SMBIOS_TABLE_HEADER*)Type19Record);
  FreePool(Type19Record);
  ASSERT_EFI_ERROR(Status);

  return Status;
}
