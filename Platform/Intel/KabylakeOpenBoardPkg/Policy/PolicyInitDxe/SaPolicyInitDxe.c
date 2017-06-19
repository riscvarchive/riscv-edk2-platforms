/** @file
  This file is SampleCode for SA DXE Policy initialization.

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#include "SaPolicyInitDxe.h"

//
// Function implementations
//

/**
  Get data for platform policy from setup options.

  @param[in] SaPolicy                  The pointer to get SA Policy protocol instance

  @retval EFI_SUCCESS                  Operation success.

**/
EFI_STATUS
EFIAPI
UpdateDxeSaPolicy (
  IN OUT  SA_POLICY_PROTOCOL    *SaPolicy
  );


/**
  Initialize SA DXE Policy

  @param[in] ImageHandle          Image handle of this driver.

  @retval EFI_SUCCESS             Initialization complete.
  @exception EFI_UNSUPPORTED      The chipset is unsupported by this driver.
  @retval EFI_OUT_OF_RESOURCES    Do not have enough resources to initialize the driver.
  @retval EFI_DEVICE_ERROR        Device error, driver exits abnormally.
**/
EFI_STATUS
EFIAPI
SaPolicyInitDxe (
  IN EFI_HANDLE                   ImageHandle
  )
{
  EFI_STATUS               Status;
  SA_POLICY_PROTOCOL       *SaPolicy;

  //
  // Call CreateSaDxeConfigBlocks to create & initialize platform policy structure
  // and get all Intel default policy settings.
  //
  Status = CreateSaDxeConfigBlocks(&SaPolicy);
  DEBUG((DEBUG_INFO, "SaPolicy->TableHeader.NumberOfBlocks = 0x%x\n ", SaPolicy->TableHeader.NumberOfBlocks));
  ASSERT_EFI_ERROR(Status);

    UpdateDxeSaPolicy (SaPolicy);

  //
  // Install SaInstallPolicyProtocol.
  // While installed, RC assumes the Policy is ready and finalized. So please
  // update and override any setting before calling this function.
  //
  Status = SaInstallPolicyProtocol (ImageHandle, SaPolicy);
  ASSERT_EFI_ERROR (Status);

  return Status;
}
