/** @file
*
*  Copyright (c) 2018, ARM Limited. All rights reserved.
*
*  SPDX-License-Identifier: BSD-2-Clause-Patent
*
**/

#include <Library/AcpiLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <SgiPlatform.h>

typedef struct {
  SGI_PLATFORM_DESCRIPTOR SgiPlafromDescriptor;
  CONST  EFI_GUID*        AcpiTableGuid;
} SGI_PLATFORM_ACPI_TABLE_GUID_LOOKUP;

// Macro to construct the SGI_PLATFORM_ACPI_TABLE_GUID_LOOKUP structure
#define ACPI_GUID_LOOKUP(PART_NUM, CONFIG_NUM, MULTI_CHIP_MODE, GUID)          \
{                                                                              \
  {                                                                            \
    PART_NUM, CONFIG_NUM, MULTI_CHIP_MODE                                      \
  },                                                                           \
  GUID                                                                         \
}                                                                              \

STATIC SGI_PLATFORM_ACPI_TABLE_GUID_LOOKUP AcpiTableGuidLookup[] = {
  ACPI_GUID_LOOKUP (
      SGI575_PART_NUM,
      SGI575_CONF_NUM,
      MULTI_CHIP_MODE_DISABLED,
      &gSgi575AcpiTablesFileGuid),
  ACPI_GUID_LOOKUP (
      RD_N1E1_EDGE_PART_NUM,
      RD_N1_EDGE_CONF_ID,
      MULTI_CHIP_MODE_DISABLED,
      &gRdN1EdgeAcpiTablesFileGuid),
  ACPI_GUID_LOOKUP (
      RD_N1E1_EDGE_PART_NUM,
      RD_E1_EDGE_CONF_ID,
      MULTI_CHIP_MODE_DISABLED,
      &gRdE1EdgeAcpiTablesFileGuid),
};

VOID
InitVirtioDevices (
  VOID
  );

EFI_STATUS
EFIAPI
ArmSgiPkgEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS              Status;
  VOID                    *SystemIdHob;
  SGI_PLATFORM_DESCRIPTOR *HobData;
  UINTN                   Idx;
  UINT32                  ConfigId;
  UINT32                  PartNum;
  UINT32                  MultiChipMode;

  SystemIdHob = GetFirstGuidHob (&gArmSgiPlatformIdDescriptorGuid);
  if (SystemIdHob == NULL) {
    DEBUG ((DEBUG_ERROR, "System ID HOB is NULL\n"));
    return EFI_INVALID_PARAMETER;
  }

  HobData = (SGI_PLATFORM_DESCRIPTOR *)GET_GUID_HOB_DATA (SystemIdHob);

  PartNum = HobData->PlatformId;
  ConfigId = HobData->ConfigId;
  MultiChipMode = HobData->MultiChipMode;

  Status = EFI_UNSUPPORTED;

  // Walk through the AcpiTableGuidLookup lookup array
  for (Idx = 0; Idx < ARRAY_SIZE (AcpiTableGuidLookup); Idx++) {
    if ((PartNum == AcpiTableGuidLookup[Idx].SgiPlafromDescriptor.PlatformId) &&
        (ConfigId == AcpiTableGuidLookup[Idx].SgiPlafromDescriptor.ConfigId)  &&
        (MultiChipMode ==
         AcpiTableGuidLookup[Idx].SgiPlafromDescriptor.MultiChipMode)) {
      Status = LocateAndInstallAcpiFromFv (AcpiTableGuidLookup[Idx].AcpiTableGuid);
      break;
    }
  }

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Failed to install ACPI tables\n", __FUNCTION__));
    return Status;
  }

  InitVirtioDevices ();

  return Status;
}
