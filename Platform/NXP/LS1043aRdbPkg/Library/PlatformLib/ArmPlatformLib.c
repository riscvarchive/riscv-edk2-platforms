/** ArmPlatformLib.c
*
*  Contains board initialization functions.
*
*  Based on BeagleBoardPkg/Library/BeagleBoardLib/BeagleBoard.c
*
*  Copyright (c) 2011-2012, ARM Limited. All rights reserved.
*  Copyright (c) 2016, Freescale Semiconductor, Inc. All rights reserved.
*  Copyright 2017 NXP
*  SPDX-License-Identifier: BSD-2-Clause-Patent
*
**/

#include <Library/ArmPlatformLib.h>
#include <Ppi/ArmMpCoreInfo.h>

extern VOID SocInit (VOID);

/**
  Return the current Boot Mode

  This function returns the boot reason on the platform

**/
EFI_BOOT_MODE
ArmPlatformGetBootMode (
  VOID
  )
{
  return BOOT_WITH_FULL_CONFIGURATION;
}

/**
 Placeholder for Platform Initialization
**/
EFI_STATUS
ArmPlatformInitialize (
  IN  UINTN   MpId
  )
{
 SocInit ();

 return EFI_SUCCESS;
}

ARM_CORE_INFO LS1043aMpCoreInfoCTA53x4[] = {
  {
    // Cluster 0, Core 0
    0x0, 0x0,

    // MP Core MailBox Set/Get/Clear Addresses and Clear Value
    (EFI_PHYSICAL_ADDRESS)0,
    (EFI_PHYSICAL_ADDRESS)0,
    (EFI_PHYSICAL_ADDRESS)0,
    (UINT64)0xFFFFFFFF
  },
};

EFI_STATUS
PrePeiCoreGetMpCoreInfo (
  OUT UINTN                   *CoreCount,
  OUT ARM_CORE_INFO           **ArmCoreTable
  )
{
  *CoreCount    = sizeof (LS1043aMpCoreInfoCTA53x4) / sizeof (ARM_CORE_INFO);
  *ArmCoreTable = LS1043aMpCoreInfoCTA53x4;

  return EFI_SUCCESS;
}

ARM_MP_CORE_INFO_PPI mMpCoreInfoPpi = { PrePeiCoreGetMpCoreInfo };

EFI_PEI_PPI_DESCRIPTOR      gPlatformPpiTable[] = {
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


UINTN
ArmPlatformGetCorePosition (
  IN UINTN MpId
  )
{
  return 1;
}
