/** @file
  Source code file for a temporary build workaround.

  The purpose of this workaround is described in the module INF file.

Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <Library/BaseLib.h>

/**
  An empty entry point function.

  @param  FileHandle  Handle of the file being invoked.
  @param  PeiServices Describes the list of possible PEI Services.

  @retval  EFI_SUCCESS  This function always returns EFI_SUCCESS.

**/
EFI_STATUS
EFIAPI
TemporaryBuildWorkaroundEntry (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  return EFI_SUCCESS;
}
