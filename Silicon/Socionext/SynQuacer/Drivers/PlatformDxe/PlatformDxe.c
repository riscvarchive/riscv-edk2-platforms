/** @file
  SynQuacer DXE platform driver.

  Copyright (c) 2017, Linaro, Ltd. All rights reserved.<BR>

  This program and the accompanying materials are licensed and made available
  under the terms and conditions of the BSD License which accompanies this
  distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
**/

#include "PlatformDxe.h"

STATIC EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR mNetsecDesc[] = {
  {
    ACPI_ADDRESS_SPACE_DESCRIPTOR,                    // Desc
    sizeof (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR) - 3,   // Len
    ACPI_ADDRESS_SPACE_TYPE_MEM,                      // ResType
    0,                                                // GenFlag
    0,                                                // SpecificFlag
    32,                                               // AddrSpaceGranularity
    SYNQUACER_NETSEC1_BASE,                           // AddrRangeMin
    SYNQUACER_NETSEC1_BASE +
    SYNQUACER_NETSEC1_BASE_SZ - 1,                    // AddrRangeMax
    0,                                                // AddrTranslationOffset
    SYNQUACER_NETSEC1_BASE_SZ,                        // AddrLen
  }, {
    ACPI_ADDRESS_SPACE_DESCRIPTOR,                    // Desc
    sizeof (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR) - 3,   // Len
    ACPI_ADDRESS_SPACE_TYPE_MEM,                      // ResType
    0,                                                // GenFlag
    0,                                                // SpecificFlag
    32,                                               // AddrSpaceGranularity
    FixedPcdGet32 (PcdNetsecEepromBase),              // AddrRangeMin
    FixedPcdGet32 (PcdNetsecEepromBase) +
    SYNQUACER_EEPROM_BASE_SZ - 1,                     // AddrRangeMax
    0,                                                // AddrTranslationOffset
    SYNQUACER_EEPROM_BASE_SZ,                         // AddrLen
  }, {
    ACPI_ADDRESS_SPACE_DESCRIPTOR,                    // Desc
    sizeof (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR) - 3,   // Len
    ACPI_ADDRESS_SPACE_TYPE_MEM,                      // ResType
    0,                                                // GenFlag
    0,                                                // SpecificFlag
    32,                                               // AddrSpaceGranularity
    FixedPcdGet32 (PcdNetsecPhyAddress),              // AddrRangeMin
    FixedPcdGet32 (PcdNetsecPhyAddress),              // AddrRangeMax
    0,                                                // AddrTranslationOffset
    1,                                                // AddrLen
  }, {
    ACPI_END_TAG_DESCRIPTOR                           // Desc
  }
};

STATIC EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR mI2c0Desc[] = {
  {
    ACPI_ADDRESS_SPACE_DESCRIPTOR,                    // Desc
    sizeof (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR) - 3,   // Len
    ACPI_ADDRESS_SPACE_TYPE_MEM,                      // ResType
    0,                                                // GenFlag
    0,                                                // SpecificFlag
    32,                                               // AddrSpaceGranularity
    SYNQUACER_I2C0_BASE,                              // AddrRangeMin
    SYNQUACER_I2C0_BASE + SYNQUACER_I2C0_SIZE - 1,    // AddrRangeMax
    0,                                                // AddrTranslationOffset
    SYNQUACER_I2C0_SIZE,                              // AddrLen
  }, {
    ACPI_END_TAG_DESCRIPTOR                           // Desc
  }
};

STATIC EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR mI2c1Desc[] = {
  {
    ACPI_ADDRESS_SPACE_DESCRIPTOR,                    // Desc
    sizeof (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR) - 3,   // Len
    ACPI_ADDRESS_SPACE_TYPE_MEM,                      // ResType
    0,                                                // GenFlag
    0,                                                // SpecificFlag
    32,                                               // AddrSpaceGranularity
    SYNQUACER_I2C1_BASE,                              // AddrRangeMin
    SYNQUACER_I2C1_BASE + SYNQUACER_I2C1_SIZE - 1,    // AddrRangeMax
    0,                                                // AddrTranslationOffset
    SYNQUACER_I2C1_SIZE,                              // AddrLen
  }, {
    ACPI_END_TAG_DESCRIPTOR                           // Desc
  }
};

STATIC
EFI_STATUS
RegisterDevice (
  IN  EFI_GUID                            *TypeGuid,
  IN  EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR   *Desc,
  OUT EFI_HANDLE                          *Handle
  )
{
  NON_DISCOVERABLE_DEVICE             *Device;
  EFI_STATUS                          Status;

  Device = (NON_DISCOVERABLE_DEVICE *)AllocateZeroPool (sizeof (*Device));
  if (Device == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Device->Type = TypeGuid;
  Device->DmaType = NonDiscoverableDeviceDmaTypeNonCoherent;
  Device->Resources = Desc;

  Status = gBS->InstallMultipleProtocolInterfaces (Handle,
                  &gEdkiiNonDiscoverableDeviceProtocolGuid, Device,
                  NULL);
  if (EFI_ERROR (Status)) {
    goto FreeDevice;
  }
  return EFI_SUCCESS;

FreeDevice:
  FreePool (Device);

  return Status;
}

#define SMMU_SCR0                         0x0
#define SMMU_SCR0_SHCFG_INNER             (0x2 << 22)
#define SMMU_SCR0_MTCFG                   (0x1 << 20)
#define SMMU_SCR0_MEMATTR_INNER_OUTER_WB  (0xf << 16)

STATIC
VOID
SmmuEnableCoherentDma (
  VOID
  )
{
  //
  // The SCB SMMU (MMU-500) is shared between the NETSEC and eMMC devices, and
  // is configured in passthrough mode by default. Let's set the global memory
  // type override as well, so that all memory accesses by these devices are
  // inner shareable inner/outer writeback cacheable.
  //
  MmioOr32 (SYNQUACER_SCB_SMMU_BASE + SMMU_SCR0,
    SMMU_SCR0_SHCFG_INNER | SMMU_SCR0_MTCFG | SMMU_SCR0_MEMATTR_INNER_OUTER_WB);
}

EFI_STATUS
EFIAPI
PlatformDxeEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                      Status;
  VOID                            *Dtb;
  UINTN                           DtbSize;
  EFI_HANDLE                      Handle;

  Dtb = NULL;
  Status = DtPlatformLoadDtb (&Dtb, &DtbSize);
  if (!EFI_ERROR (Status)) {
    Status = gBS->InstallConfigurationTable (&gFdtTableGuid, Dtb);
  }
  if (EFI_ERROR (Status)) {
     DEBUG ((DEBUG_ERROR,
      "%a: failed to install FDT configuration table - %r\n", __FUNCTION__,
      Status));
  }

  Handle = NULL;
  Status = RegisterDevice (&gNetsecNonDiscoverableDeviceGuid, mNetsecDesc,
             &Handle);
  ASSERT_EFI_ERROR (Status);

  Handle = NULL;
  Status = RegisterDevice (&gSynQuacerNonDiscoverableRuntimeI2cMasterGuid,
             mI2c0Desc, &Handle);
  ASSERT_EFI_ERROR (Status);

  //
  // Install the PCF8563 I2C Master protocol on this handle so the RTC driver
  // can identify it as the I2C master it can invoke directly, rather than
  // through the I2C driver stack (which cannot be used at runtime)
  //
  Status = gBS->InstallProtocolInterface (&Handle,
                  &gPcf8563RealTimeClockLibI2cMasterProtocolGuid,
                  EFI_NATIVE_INTERFACE, NULL);
  ASSERT_EFI_ERROR (Status);

  Handle = NULL;
  Status = RegisterDevice (&gSynQuacerNonDiscoverableI2cMasterGuid, mI2c1Desc,
             &Handle);
  ASSERT_EFI_ERROR (Status);

  SmmuEnableCoherentDma ();

  Status = RegisterPcieNotifier ();
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}
