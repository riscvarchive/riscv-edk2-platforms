/** @file
*
*  Copyright (c) 2018, ARM Limited. All rights reserved.
*
*  This program and the accompanying materials are licensed and made available
*  under the terms and conditions of the BSD License which accompanies this
*  distribution.  The full text of the license may be found at
*  http://opensource.org/licenses/bsd-license.php
*
*  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
*  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
*
**/

#include <Library/ArmPlatformLib.h>
#include <Library/BaseLib.h>
#include <Ppi/ArmMpCoreInfo.h>

STATIC ARM_CORE_INFO mCoreInfoTable[] = {
  {
    // Cluster 0, Core 0
    0x0, 0x0,
  },
};

EFI_BOOT_MODE
ArmPlatformGetBootMode (
  VOID
  )
{
  return BOOT_WITH_FULL_CONFIGURATION;
}

RETURN_STATUS
ArmPlatformInitialize (
  IN  UINTN                     MpId
  )
{
  return RETURN_SUCCESS;
}

EFI_STATUS
PrePeiCoreGetMpCoreInfo (
  OUT UINTN                   *CoreCount,
  OUT ARM_CORE_INFO           **ArmCoreTable
  )
{
  *CoreCount = 1;
  *ArmCoreTable = mCoreInfoTable;
  return EFI_SUCCESS;
}

STATIC ARM_MP_CORE_INFO_PPI mMpCoreInfoPpi = {
  PrePeiCoreGetMpCoreInfo
};

EFI_PEI_PPI_DESCRIPTOR gPlatformPpiTable[] = {
  {
    EFI_PEI_PPI_DESCRIPTOR_PPI,
    &gArmMpCoreInfoPpiGuid,
    &mMpCoreInfoPpi
  }
};

VOID
ArmPlatformGetPlatformPpiList (
  OUT UINTN                   *PpiListSize,
  OUT EFI_PEI_PPI_DESCRIPTOR  **PpiList
  )
{
  *PpiListSize = sizeof (gPlatformPpiTable);
  *PpiList = gPlatformPpiTable;
}
