/*++

Copyright (c) 2011  - 2019, Intel Corporation. All rights reserved

  SPDX-License-Identifier: BSD-2-Clause-Patent

Module Name:

  BiosIdLib.c

Abstract:

  Boot service DXE BIOS ID library implementation.

  These functions in this file can be called during DXE and cannot be called during runtime
  or in SMM which should use a RT or SMM library.

--*/

#include <PiDxe.h>
#include <Library/BaseLib.h>

#include <Library/BiosIdLib.h>

/**
  This function returns the Version & Release Date and Time by getting and converting
  BIOS ID.

  @param BiosVersion  The Bios Version out of the conversion.
  @param BiosReleaseDate  The Bios Release Date out of the conversion.
  @param BiosReleaseTime - The Bios Release Time out of the conversion.

  @retval EFI_SUCCESS - BIOS Version & Release Date and Time have been got successfully.
  @retval EFI_NOT_FOUND - BiosId image is not found, and no parameter will be modified.
  @retval EFI_INVALID_PARAMETER - All the parameters are NULL.

**/
EFI_STATUS
GetBiosVersionDateTime (
  OUT CHAR16    *BiosVersion, OPTIONAL
  OUT CHAR16    *BiosReleaseDate, OPTIONAL
  OUT CHAR16    *BiosReleaseTime OPTIONAL
  )
{
  if ((BiosVersion == NULL) && (BiosReleaseDate == NULL) && (BiosReleaseTime == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  if (BiosVersion != NULL) {
    //
    // Fill the BiosVersion data from the BIOS ID.
    //
    StrCpy (BiosVersion, L"MNW2MAX1.X64.0084.D01");
  }

  if (BiosReleaseDate != NULL) {
    //
    // Fill the build timestamp date from the BIOS ID in the "MM/DD/YY" format.
    //
    BiosReleaseDate[0] = L'0';
    BiosReleaseDate[1] = L'1';
    BiosReleaseDate[2] = L'/';

    BiosReleaseDate[3] = L'0';
    BiosReleaseDate[4] = L'1';
    BiosReleaseDate[5] = L'/';

    //
    // Add 20 for SMBIOS table
    // Current Linux kernel will misjudge 09 as year 0, so using 2009 for SMBIOS table
    //
    BiosReleaseDate[6] = L'2';
    BiosReleaseDate[7] = L'0';
    BiosReleaseDate[8] = L'1';
    BiosReleaseDate[9] = L'9';

    BiosReleaseDate[10] = L'\0';
  }

  if (BiosReleaseTime != NULL) {

    //
    // Fill the build timestamp time from the BIOS ID in the "HH:MM" format.
    //

    BiosReleaseTime[0] = L'0';
    BiosReleaseTime[1] = L'0';
    BiosReleaseTime[2] = L':';

    BiosReleaseTime[3] = L'0';
    BiosReleaseTime[4] = L'0';

    BiosReleaseTime[5] = L'\0';
  }

  return  EFI_SUCCESS;
}

