/********************************************************************************
Copyright (C) 2016 Marvell International Ltd.

Marvell BSD License Option

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File under the following licensing terms.
Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

* Redistributions of source code must Retain the above copyright notice,
  this list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.

* Neither the name of Marvell nor the names of its contributors may be
  used to endorse or promote products derived from this software without
  specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/
#define CHAR_NULL 0x0000

#include <Library/BaseLib.h>
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>

STATIC
CHAR16
CharToUpper (
  IN CHAR16 Char
  )
{

  if (Char >= L'a' && Char <= L'z') {
    return (CHAR16) (Char - (L'a' - L'A'));
  }

  return Char;
}

STATIC
BOOLEAN
IsDecimalDigitChar (
  IN CHAR16 Char
  )
{

  return (BOOLEAN) (Char >= L'0' && Char <= L'9');
}


STATIC
UINTN
HexCharToUintn (
  IN CHAR16 Char
  )
{
  if (IsDecimalDigitChar (Char)) {
    return Char - L'0';
  }

  return (UINTN) (10 + CharToUpper (Char) - L'A');
}

STATIC
BOOLEAN
IsHexDigitCharacter (
  CHAR16 Char
  )
{

  return (BOOLEAN) ((Char >= L'0' && Char <= L'9') || (Char >= L'A' &&
    Char <= L'F') || (Char >= L'a' && Char <= L'f'));
}

STATIC
UINTN
HexStrToUintn (
  CHAR16 *String
  )
{
  UINTN Result = 0;

  if (String == NULL || StrSize(String) == 0) {
    return (UINTN)(-1);
  }

  // Ignore spaces and tabs
  while ((*String == L' ') || (*String == L'\t')) {
    String++;
  }

  // Ignore leading zeros after spaces
  while (*String == L'0') {
    String++;
  }

  if (CharToUpper (*String) != L'X') {
    return (UINTN)(-1);
  }

  // Skip 'x'
  String++;

  while (IsHexDigitCharacter (*String)) {
    Result <<= 4;
    Result += HexCharToUintn (*String);
    String++;
  }

  return (UINTN) Result;
}

STATIC
UINTN
DecimalStrToUintn (
  CHAR16 *String
  )
{
  UINTN Result = 0;

  while (IsDecimalDigitChar (*String)) {
    Result = 10 * Result + (*String - L'0');
    String++;
  }

  return Result;
}

STATIC
UINTN
StrToUintn (
  CHAR16 *String
  )
{
  CHAR16 *Walker;

  // Chop off leading spaces
  for (Walker = String; Walker != NULL && *Walker != CHAR_NULL && *Walker == L' '; Walker++);

  if (StrnCmp(Walker, L"0x", 2) == 0 || StrnCmp(Walker, L"0X", 2) == 0) {
    return HexStrToUintn (Walker);
  } else {
    return DecimalStrToUintn (Walker);
  }
}

EFI_STATUS
ParsePcdString (
  IN  CHAR16 *PcdString,
  IN  UINT8  Count,
  OUT UINTN  *ValueTable,
  OUT CHAR16 **StrTable
  )
{
  BOOLEAN ValueFlag = FALSE;
  CHAR16 *Walker;
  UINTN i, Tmp = 0;

  if (ValueTable != NULL) {
    ValueFlag = TRUE;
  }

  // Set pointer at the end of PCD string
  Walker = PcdString + StrLen (PcdString);
  for (i = 0; i < Count; i++) {
    while ((--Walker) >= PcdString) {
      if (*Walker == L';') {
        // Cut off parsed chunk from PCD string by replacing ';' with
        // null-terminator
        *Walker = '\0';
        if (ValueFlag) {
          Tmp = StrToUintn ((Walker + 1));
          if ((UINTN)(-1) == Tmp) {
            return EFI_INVALID_PARAMETER;
          }
          // Entry is parsed from the end to the beginning
          // so fill table in the same manner
          ValueTable[Count - (i + 1)] = Tmp;
        } else {
          StrTable[Count - (i + 1)] = Walker + 1;
        }
        Walker--;
        break;
      }
      if (Walker == PcdString) {
        if (ValueFlag) {
          Tmp = StrToUintn ((Walker));
          if (Tmp == (UINTN)(-1)) {
            return EFI_INVALID_PARAMETER;
          }
        }
        // Last device's entry should be added to the table here.
        // If not, return error
        if (i != (Count - 1)) {
          DEBUG((DEBUG_ERROR, "ParsePcdLib: Please set PCD value for every "
            "device\n"));
          return EFI_INVALID_PARAMETER;
        }
        // We parse from the end to the beginning
        // so fill table in the same manner
        if (ValueFlag) {
          ValueTable[Count - (i + 1)] = Tmp;
        } else {
          StrTable[Count - (i + 1)] = Walker;
        }
        // End both loops
        return EFI_SUCCESS;
      }
    }
  }

  return EFI_SUCCESS;
}
