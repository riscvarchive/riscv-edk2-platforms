/** @file
  This file is a wrapper for Platform Policy driver. Get Setup
  Value to initialize Intel DXE Platform Policy.

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "PolicyInitDxe.h"

#include <KabylakeRvp3Id.h>

/**
  Initialize  DXE Platform Policy

  @param[in] ImageHandle       Image handle of this driver.
  @param[in] SystemTable       Global system service table.

  @retval EFI_SUCCESS           Initialization complete.
  @exception EFI_UNSUPPORTED       The chipset is unsupported by this driver.
  @retval EFI_OUT_OF_RESOURCES  Do not have enough resources to initialize the driver.
  @retval EFI_DEVICE_ERROR      Device error, driver exits abnormally.
**/
EFI_STATUS
EFIAPI
PolicyInitDxeEntryPoint (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS           Status;
  
  if (LibPcdGetSku () != BoardIdKabyLakeYLpddr3Rvp3) {
    return EFI_UNSUPPORTED;
  }

  //
  // SystemAgent Dxe Platform Policy Initialization
  //
  Status = SaPolicyInitDxe (ImageHandle);
  DEBUG ((DEBUG_INFO, "SystemAgent Dxe Platform Policy Initialization done\n"));
  ASSERT_EFI_ERROR (Status);

  if (PcdGetBool(PcdIntelGopEnable)) {
    //
    // GOP Dxe Policy Initialization
    //
    Status = GopPolicyInitDxe(ImageHandle);
    DEBUG((DEBUG_INFO, "GOP Dxe Policy Initialization done\n"));
    ASSERT_EFI_ERROR(Status);
  }

  return Status;

}
