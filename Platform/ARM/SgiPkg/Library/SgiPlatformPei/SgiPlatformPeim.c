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

  This function returns the System ID of the platform

  This functions locates the HwConfig PPI and gets the base address of HW CONFIG
  DT from which the platform ID and config ID is obtained using FDT helper
  functions

  @param[out]      Pointer to the SGI_PLATFORM_DESCRIPTOR HoB
  @return          returns EFI_SUCCESS on success and EFI_INVALID_PARAMETER on error

**/

STATIC
EFI_STATUS
GetSgiSystemId (
  OUT SGI_PLATFORM_DESCRIPTOR   *HobData
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
    return EFI_INVALID_PARAMETER;
  }

  HwCfgDtBlob = (VOID *)(UINTN)HwConfigInfoPpi->HwConfigDtAddr;
  if (fdt_check_header (HwCfgDtBlob) != 0) {
    DEBUG ((DEBUG_ERROR, "Invalid DTB file %p passed\n", HwCfgDtBlob));
    return EFI_INVALID_PARAMETER;
  }

  Offset = fdt_subnode_offset (HwCfgDtBlob, 0, "system-id");
  if (Offset == -FDT_ERR_NOTFOUND) {
    DEBUG ((DEBUG_ERROR, "Invalid DTB : system-id node not found\n"));
    return EFI_INVALID_PARAMETER;
  }

  Property = fdt_getprop (HwCfgDtBlob, Offset, "platform-id", NULL);
  if (Property == NULL) {
    DEBUG ((DEBUG_ERROR, "Platform Id is NULL\n"));
    return EFI_INVALID_PARAMETER;
  }

  HobData->PlatformId = fdt32_to_cpu (*Property);

  Property = fdt_getprop (HwCfgDtBlob, Offset, "config-id", NULL);
  if (Property == NULL) {
    DEBUG ((DEBUG_ERROR, "Config Id is NULL\n"));
    return EFI_INVALID_PARAMETER;
  }

  HobData->ConfigId = fdt32_to_cpu (*Property);

  return EFI_SUCCESS;
}

/**

 This function creates a System ID HOB and assigns PlatformId and
 ConfigId as the HobData

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
  EFI_STATUS                    Status;

  // Create platform descriptor HOB
  HobData = (SGI_PLATFORM_DESCRIPTOR *)BuildGuidHob (
                                         &gArmSgiPlatformIdDescriptorGuid,
                                         sizeof (SGI_PLATFORM_DESCRIPTOR));

  // Get the system id from the platform specific hw_config device tree
  if (HobData == NULL) {
    DEBUG ((DEBUG_ERROR, "Platform HOB is NULL\n"));
    ASSERT (FALSE);
    return EFI_OUT_OF_RESOURCES;
  }

  Status = GetSgiSystemId (HobData);
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return EFI_INVALID_PARAMETER;
  }

  if (HobData->PlatformId == 0 || HobData->ConfigId == 0) {
    ASSERT (FALSE);
    return EFI_INVALID_PARAMETER;
  }

  return EFI_SUCCESS;
}
