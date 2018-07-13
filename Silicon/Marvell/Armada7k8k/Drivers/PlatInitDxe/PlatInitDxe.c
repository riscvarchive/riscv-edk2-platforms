/** @file
  Copyright (c) 2017, Linaro Limited. All rights reserved.
  Copyright (c) 2017, Marvell International Ltd. and its affiliates

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Library/ArmadaIcuLib.h>
#include <Library/DebugLib.h>
#include <Library/MppLib.h>
#include <Library/MvComPhyLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UtmiPhyLib.h>

EFI_STATUS
EFIAPI
ArmadaPlatInitDxeEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS    Status;

  DEBUG ((DEBUG_ERROR, "\nArmada Platform Init\n\n"));

  Status = gBS->InstallProtocolInterface (&ImageHandle,
                  &gMarvellPlatformInitCompleteProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  NULL);
  ASSERT_EFI_ERROR (Status);

  MvComPhyInit ();
  UtmiPhyInit ();
  MppInitialize ();
  ArmadaIcuInitialize ();

  return EFI_SUCCESS;
}
