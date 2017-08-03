/** @file

  C language wrapper to build DSDT generated data.

  Copyright (c) 2014 - 2016, AMD Inc. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <AmdStyxAcpiLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>

#include <Dsdt.hex>
#include <Dsdt.offset.h>


UINTN
ShiftLeftByteToUlong (
  IN UINT8 Byte,
  IN UINTN Shift
  )
{
  UINTN Data;

  Data = (UINTN)Byte;
  Data <<= Shift;
  return Data;
}

UINTN
AmlGetPkgLength (
  IN UINT8 *Buffer,
  OUT UINTN *PkgLength
  )
{
  UINTN Bytes, Length;

  Bytes = (UINTN)((Buffer[0] >> 6) & 0x3) + 1;
  switch (Bytes) {
    case 1:
      Length = (UINTN)Buffer[0];
      break;

    case 2:
      Length = ShiftLeftByteToUlong(Buffer[1], 4) +
                (UINTN)(Buffer[0] & 0x0F);
      break;

    case 3:
      Length = ShiftLeftByteToUlong(Buffer[2], 12) +
                ShiftLeftByteToUlong(Buffer[1], 4) +
                 (UINTN)(Buffer[0] & 0x0F);
      break;

    default: /* 4 bytes */
      Length = ShiftLeftByteToUlong(Buffer[3], 20) +
                ShiftLeftByteToUlong(Buffer[2], 12) +
                 ShiftLeftByteToUlong(Buffer[1], 4) +
                  (UINTN)(Buffer[0] & 0x0F);
      break;
  }

  *PkgLength = Length;
  return Bytes;
}

UINT8 *
AmlSearchStringPackage (
  IN UINT8 *Buffer,
  IN UINTN Length,
  IN CHAR8 *String
  )
{
  UINTN StrLength;

  StrLength = AsciiStrLen (String) + 1;
  if (Length > StrLength ) {
    Length -= StrLength;
    while (AsciiStrCmp((CHAR8 *)Buffer, String) != 0 && Length) {
      --Length;
      ++Buffer;
    }
    if (Length) {
      return &Buffer[StrLength];
    }
  }
  return NULL;
}

VOID
OverrideMacAddr (
  IN UINT8 *DSD_Data,
  IN UINT64 MacAddr
  )
{
  UINT8 *MacAddrPkg;
  UINTN Bytes, Length, Index = 0;

  // AML encoding: PackageOp
  if (DSD_Data[0] == 0x12) {
    // AML encoding: PkgLength
    Bytes = AmlGetPkgLength (&DSD_Data[1], &Length);

    // Search for "mac-address" property
    MacAddrPkg = AmlSearchStringPackage (&DSD_Data[Bytes + 1],
                                        Length - Bytes,
                                        "mac-address");
    if (MacAddrPkg &&
        MacAddrPkg[0] == 0x12 &&        // PackageOp
        MacAddrPkg[1] == 0x0E &&        // PkgLength
        MacAddrPkg[2] == 0x06) {        // NumElements (element must have a BytePrefix)

      MacAddrPkg += 3;
      do {
        MacAddrPkg[0] = 0x0A;           // BytePrefix
        MacAddrPkg[1] = (UINT8)(MacAddr & 0xFF);
        MacAddrPkg += 2;
        MacAddr >>= 8;
      } while (++Index < 6);
    }
  }
}

VOID
OverrideStatus (
  IN UINT8 *DSD_Data,
  IN BOOLEAN Enable
  )
{
  if (Enable) {
    // AML encoding: ReturnOp + BytePrefix
    if (DSD_Data[1] == 0xA4 && DSD_Data[2] == 0x0A) {
      DSD_Data[3] = 0x0F;
    }
  } else {
    // AML encoding: ReturnOp
    if (DSD_Data[1] == 0xA4) {
      // AML encoding: BytePrefix?
      if (DSD_Data[2] == 0x0A) {
        DSD_Data[3] = 0x00;
      } else {
        DSD_Data[2] = 0x00;
      }
    }
  }
}

EFI_ACPI_DESCRIPTION_HEADER *
DsdtHeader (
  VOID
  )
{
  AML_OFFSET_TABLE_ENTRY *Table;
  BOOLEAN                EnableOnB1;
  UINT32                 CpuId = PcdGet32 (PcdSocCpuId);

  // Enable features on Styx-B1 or later
  EnableOnB1 = (CpuId & 0xFF0) && (CpuId & 0x00F);

  Table = &DSDT_SEATTLE__OffsetTable[0];
  while (Table->Pathname) {
    if (AsciiStrCmp(Table->Pathname, "_SB_.ETH0._DSD") == 0) {
      OverrideMacAddr ((UINT8 *)&AmlCode[Table->Offset], PcdGet64 (PcdEthMacA));
    }
    else if (AsciiStrCmp(Table->Pathname, "_SB_.ETH1._DSD") == 0) {
      OverrideMacAddr ((UINT8 *)&AmlCode[Table->Offset], PcdGet64 (PcdEthMacB));
    }
    else if (AsciiStrCmp(Table->Pathname, "_SB_.AHC1._STA") == 0) {
      OverrideStatus ((UINT8 *)&AmlCode[Table->Offset],
        EnableOnB1 && FixedPcdGet8(PcdSata1PortCount) > 0);
    }
    else if (AsciiStrCmp(Table->Pathname, "_SB_.GIO2._STA") == 0) {
      OverrideStatus ((UINT8 *)&AmlCode[Table->Offset], EnableOnB1);
    }
    else if (AsciiStrCmp(Table->Pathname, "_SB_.GIO3._STA") == 0) {
      OverrideStatus ((UINT8 *)&AmlCode[Table->Offset], EnableOnB1);
    }

    ++Table;
  }

  return (EFI_ACPI_DESCRIPTION_HEADER *) &AmlCode[0];
}
