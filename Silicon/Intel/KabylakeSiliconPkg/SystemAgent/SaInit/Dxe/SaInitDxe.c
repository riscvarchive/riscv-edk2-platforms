/** @file
  This is the driver that initializes the Intel System Agent.

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#include "SaInitDxe.h"
#include "SaInit.h"
#include <Library/MmPciLib.h>
#include <MemInfoHob.h>

/**
  SystemAgent Dxe Initialization.

  @param[in] ImageHandle             Handle for the image of this driver
  @param[in] SystemTable             Pointer to the EFI System Table

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_OUT_OF_RESOURCES    No enough buffer to allocate
**/
EFI_STATUS
EFIAPI
SaInitEntryPointDxe (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                Status;

  DEBUG ((DEBUG_INFO, "SaInitDxe Start\n"));


  Status = SaAcpiInit (ImageHandle);


  DEBUG ((DEBUG_INFO, "SaInitDxe End\n"));

  return EFI_SUCCESS;
}

