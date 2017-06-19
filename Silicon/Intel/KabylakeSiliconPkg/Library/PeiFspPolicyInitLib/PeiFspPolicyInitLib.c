/** @file
  Instance of Fsp Policy Initialization Library.

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PeiFspPolicyInitLib.h>

/**
  Performs FSP PEI Policy Pre-memory initialization.

  @param[in] FspmUpdDataPtr       Pointer to FSPM UPD data.
**/
VOID
EFIAPI
FspmPolicyInit (
  IN OUT VOID    *FspmUpd
  )
{
  FSPM_UPD              *FspmUpdDataPtr;
  EFI_STATUS            Status;

  FspmUpdDataPtr = FspmUpd;

  //
  // Initialize Intel PEI Platform Policy
  //
  PeiPolicyInitPreMem ();

  //
  // SI Pei Fsp Policy Initialization
  //
  Status = PeiFspSiPolicyInitPreMem (FspmUpdDataPtr);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_WARN, "ERROR - SI Pei Fsp Policy in Pre-Memory Initialization fail, Status = %r\n", Status));
  }

  //
  // PCH Pei Fsp Policy Initialization
  //
  Status = PeiFspPchPolicyInitPreMem (FspmUpdDataPtr);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_WARN, "ERROR - PCH Pei Fsp Policy in Pre-Memory Initialization fail, Status = %r\n", Status));
  }

  //
  // Cpu Pei Fsp Policy Initialization
  //
  Status = PeiFspCpuPolicyInitPreMem (FspmUpdDataPtr);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_WARN, "ERROR - CPU Pei Fsp Policy in Pre-Memory Initialization fail, Status = %r\n", Status));
  }


  //
  // ME Pei Fsp Policy Initialization
  //
  Status = PeiFspMePolicyInitPreMem (FspmUpdDataPtr);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_WARN, "ERROR - ME Pei Fsp Policy in Pre-Memory Initialization fail, Status = %r\n", Status));
  }

  //
  // SystemAgent Pei Fsp Policy Initialization
  //
  Status = PeiFspSaPolicyInitPreMem (FspmUpdDataPtr);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_WARN, "ERROR - SystemAgent Pei Fsp Policy in Pre-Memory Initialization fail, Status = %r\n", Status));
  }

  //
  // Other Upd Initialization
  //
  Status = PeiFspMiscUpdInitPreMem (FspmUpdDataPtr);

}

/**
  Performs FSP PEI Policy initialization.

  @param[in][out] FspsUpd  Pointer UPD data region

**/
VOID
EFIAPI
FspsPolicyInit (
  IN OUT VOID    *FspsUpd
  )
{
  FSPS_UPD              *FspsUpdDataPtr;
  EFI_STATUS            Status;

  FspsUpdDataPtr = FspsUpd;

  //
  // Initialize Intel PEI Platform Policy
  //
  PeiPolicyInit ();

  //
  // PCH Pei Fsp Policy Initialization
  //
  Status = PeiFspPchPolicyInit (FspsUpdDataPtr);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_WARN, "ERROR - PCH Pei Fsp Policy iInitialization fail, Status = %r\n", Status));
  }

  //
  // ME Pei Fsp Policy Initialization
  //
  Status = PeiFspMePolicyInit (FspsUpdDataPtr);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_WARN, "ERROR - ME Pei Fsp Policy Initialization fail, Status = %r\n", Status));
  }

  //
  // SystemAgent Pei Fsp Policy Initialization
  //
  Status = PeiFspSaPolicyInit (FspsUpdDataPtr);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_WARN, "ERROR - SystemAgent Pei Fsp Policy Initialization fail, Status = %r\n", Status));
  }


  //
  // Cpu Pei Fsp Policy Initialization
  //
  Status = PeiFspCpuPolicyInit (FspsUpdDataPtr);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_WARN, "ERROR - CPU Pei Fsp Policy Initialization fail, Status = %r\n", Status));
  }

}
