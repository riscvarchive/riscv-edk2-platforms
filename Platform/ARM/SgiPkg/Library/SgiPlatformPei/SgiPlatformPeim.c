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

#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/PeimEntryPoint.h>
#include <Library/PeiServicesLib.h>
#include <libfdt.h>
#include <Ppi/SgiPlatformId.h>
#include <SgiPlatform.h>

/**

  This function returns the Platform ID of the platform

  This functions locates the HwConfig PPI and gets the base address of HW CONFIG
  DT from which the platform ID is obtained using FDT helper functions

  @return returns the platform ID on success else returns 0 on error

**/

STATIC
UINT32
GetSgiPlatformId (
 VOID
)
{
  CONST UINT32                  *Property;
  INT32                         Offset;
  CONST VOID                    *HwCfgDtBlob;
  SGI_HW_CONFIG_INFO_PPI        *HwConfigInfoPpi;
  EFI_STATUS                    Status;

  Status = PeiServicesLocatePpi (&gHwConfigDtInfoPpiGuid, 0, NULL,
             (VOID**)&HwConfigInfoPpi);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR,
      "PeiServicesLocatePpi failed with error %r\n", Status));
    return 0;
  }

  HwCfgDtBlob = (VOID *)(UINTN)HwConfigInfoPpi->HwConfigDtAddr;
  if (fdt_check_header (HwCfgDtBlob) != 0) {
    DEBUG ((DEBUG_ERROR, "Invalid DTB file %p passed\n", HwCfgDtBlob));
    return 0;
  }

  Offset = fdt_subnode_offset (HwCfgDtBlob, 0, "system-id");
  if (Offset == -FDT_ERR_NOTFOUND) {
    DEBUG ((DEBUG_ERROR, "Invalid DTB : system-id node not found\n"));
    return 0;
  }

  Property = fdt_getprop (HwCfgDtBlob, Offset, "platform-id", NULL);
  if (Property == NULL) {
    DEBUG ((DEBUG_ERROR, "Platform Id is NULL\n"));
    return 0;
  }

  return fdt32_to_cpu (*Property);
}

/**

 This function creates a Platform ID HOB and assigns PlatformId as the
 HobData

 @return asserts on error and returns EFI_INVALID_PARAMETER. On success
 returns EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
SgiPlatformPeim (
 IN       EFI_PEI_FILE_HANDLE  FileHandle,
 IN CONST EFI_PEI_SERVICES     **PeiServices
)
{
  SGI_PLATFORM_DESCRIPTOR       *HobData;

  // Create platform descriptor HOB
  HobData = (SGI_PLATFORM_DESCRIPTOR *)BuildGuidHob (
                                         &gArmSgiPlatformIdDescriptorGuid,
                                         sizeof (SGI_PLATFORM_DESCRIPTOR));

  // Get the platform id from the platform specific hw_config device tree
  if (HobData == NULL) {
    DEBUG ((DEBUG_ERROR, "Platform HOB is NULL\n"));
    ASSERT (FALSE);
    return EFI_OUT_OF_RESOURCES;
  }

  HobData->PlatformId = GetSgiPlatformId ();
  if (HobData->PlatformId == 0) {
    ASSERT (FALSE);
    return EFI_INVALID_PARAMETER;
  }

  return EFI_SUCCESS;
}
