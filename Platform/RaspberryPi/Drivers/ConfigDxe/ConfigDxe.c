/** @file
 *
 *  Copyright (c) 2019, ARM Limited. All rights reserved.
 *  Copyright (c) 2018 - 2019, Andrei Warkentin <andrey.warkentin@gmail.com>
 *
 *  SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 **/

#include <Uefi.h>
#include <Library/AcpiLib.h>
#include <Library/HiiLib.h>
#include <Library/DebugLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/IoLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DevicePathLib.h>
#include <IndustryStandard/RpiMbox.h>
#include <IndustryStandard/Bcm2711.h>
#include <IndustryStandard/Bcm2836.h>
#include <IndustryStandard/Bcm2836Gpio.h>
#include <Library/GpioLib.h>
#include <Protocol/RpiFirmware.h>
#include "ConfigDxeFormSetGuid.h"

#define FREQ_1_MHZ 1000000

extern UINT8 ConfigDxeHiiBin[];
extern UINT8 ConfigDxeStrings[];

STATIC RASPBERRY_PI_FIRMWARE_PROTOCOL *mFwProtocol;
STATIC UINT32 mModelFamily = 0;
STATIC UINT32 mModelInstalledMB = 0;

/*
 * The GUID inside Platform/RaspberryPi/RPi3/AcpiTables/AcpiTables.inf and
 * Platform/RaspberryPi/RPi4/AcpiTables/AcpiTables.inf _must_ match below.
 */
STATIC CONST EFI_GUID mAcpiTableFile = {
  0x7E374E25, 0x8E01, 0x4FEE, { 0x87, 0xf2, 0x39, 0x0C, 0x23, 0xC6, 0x06, 0xCD }
};

typedef struct {
  VENDOR_DEVICE_PATH VendorDevicePath;
  EFI_DEVICE_PATH_PROTOCOL End;
} HII_VENDOR_DEVICE_PATH;

STATIC HII_VENDOR_DEVICE_PATH mVendorDevicePath = {
  {
    {
      HARDWARE_DEVICE_PATH,
      HW_VENDOR_DP,
      {
        (UINT8)(sizeof (VENDOR_DEVICE_PATH)),
        (UINT8)((sizeof (VENDOR_DEVICE_PATH)) >> 8)
      }
    },
    CONFIGDXE_FORM_SET_GUID
  },
  {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    {
      (UINT8)(END_DEVICE_PATH_LENGTH),
      (UINT8)((END_DEVICE_PATH_LENGTH) >> 8)
    }
  }
};


STATIC EFI_STATUS
InstallHiiPages (
  VOID
  )
{
  EFI_STATUS     Status;
  EFI_HII_HANDLE HiiHandle;
  EFI_HANDLE     DriverHandle;

  DriverHandle = NULL;
  Status = gBS->InstallMultipleProtocolInterfaces (&DriverHandle,
                  &gEfiDevicePathProtocolGuid,
                  &mVendorDevicePath,
                  NULL);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  HiiHandle = HiiAddPackages (&gConfigDxeFormSetGuid,
                DriverHandle,
                ConfigDxeStrings,
                ConfigDxeHiiBin,
                NULL);

  if (HiiHandle == NULL) {
    gBS->UninstallMultipleProtocolInterfaces (DriverHandle,
           &gEfiDevicePathProtocolGuid,
           &mVendorDevicePath,
           NULL);
    return EFI_OUT_OF_RESOURCES;
  }
  return EFI_SUCCESS;
}


STATIC EFI_STATUS
SetupVariables (
  VOID
  )
{
  UINTN Size;
  UINT8 Var8;
  UINT32 Var32;
  EFI_STATUS Status;

  /*
   * Create the vars with default value.
   * If we don't, forms won't be able to update.
   */

  Size = sizeof (UINT32);
  Status = gRT->GetVariable (L"CpuClock",
                  &gConfigDxeFormSetGuid,
                  NULL, &Size, &Var32);
  if (EFI_ERROR (Status)) {
    PcdSet32 (PcdCpuClock, PcdGet32 (PcdCpuClock));
  }

  Size = sizeof (UINT32);
  Status = gRT->GetVariable(L"CustomCpuClock",
                            &gConfigDxeFormSetGuid,
                            NULL, &Size, &Var32);
  if (EFI_ERROR (Status)) {
    PcdSet32 (PcdCustomCpuClock, PcdGet32 (PcdCustomCpuClock));
  }

  if (mModelFamily >= 4 && mModelInstalledMB > 3 * 1024) {
    /*
     * This allows changing PcdRamLimitTo3GB in forms.
     */
    PcdSet32 (PcdRamMoreThan3GB, 1);

    Size = sizeof (UINT32);
    Status = gRT->GetVariable (L"RamLimitTo3GB",
                               &gConfigDxeFormSetGuid,
                               NULL, &Size, &Var32);
    if (EFI_ERROR (Status)) {
      PcdSet32 (PcdRamLimitTo3GB, PcdGet32 (PcdRamLimitTo3GB));
    }
  } else {
    PcdSet32 (PcdRamMoreThan3GB, 0);
    PcdSet32 (PcdRamLimitTo3GB, 0);
  }

  Size = sizeof (UINT32);
  Status = gRT->GetVariable (L"OptDeviceTree",
                  &gConfigDxeFormSetGuid,
                  NULL, &Size, &Var32);
  if (EFI_ERROR (Status)) {
    PcdSet32 (PcdOptDeviceTree, PcdGet32 (PcdOptDeviceTree));
  }

  Size = sizeof (UINT32);
  Status = gRT->GetVariable (L"SdIsArasan",
                  &gConfigDxeFormSetGuid,
                  NULL, &Size, &Var32);
  if (EFI_ERROR (Status)) {
    PcdSet32 (PcdSdIsArasan, PcdGet32 (PcdSdIsArasan));
  }

  Size = sizeof (UINT32);
  Status = gRT->GetVariable (L"MmcDisableMulti",
                  &gConfigDxeFormSetGuid,
                  NULL, &Size, &Var32);
  if (EFI_ERROR (Status)) {
    PcdSet32 (PcdMmcDisableMulti, PcdGet32 (PcdMmcDisableMulti));
  }

  Size = sizeof (UINT32);
  Status = gRT->GetVariable (L"MmcForce1Bit",
                  &gConfigDxeFormSetGuid,
                  NULL, &Size, &Var32);
  if (EFI_ERROR (Status)) {
    PcdSet32 (PcdMmcForce1Bit, PcdGet32 (PcdMmcForce1Bit));
  }

  Size = sizeof (UINT32);
  Status = gRT->GetVariable (L"MmcForceDefaultSpeed",
                  &gConfigDxeFormSetGuid,
                  NULL, &Size, &Var32);
  if (EFI_ERROR (Status)) {
    PcdSet32 (PcdMmcForceDefaultSpeed, PcdGet32 (PcdMmcForceDefaultSpeed));
  }

  Size = sizeof (UINT32);
  Status = gRT->GetVariable (L"MmcSdDefaultSpeedMHz",
                  &gConfigDxeFormSetGuid,
                  NULL, &Size, &Var32);
  if (EFI_ERROR (Status)) {
    PcdSet32 (PcdMmcSdDefaultSpeedMHz, PcdGet32 (PcdMmcSdDefaultSpeedMHz));
  }

  Size = sizeof (UINT32);
  Status = gRT->GetVariable (L"MmcSdHighSpeedMHz",
                  &gConfigDxeFormSetGuid,
                  NULL, &Size, &Var32);
  if (EFI_ERROR (Status)) {
    PcdSet32 (PcdMmcSdHighSpeedMHz, PcdGet32 (PcdMmcSdHighSpeedMHz));
  }

  Size = sizeof (UINT32);
  Status = gRT->GetVariable (L"DebugEnableJTAG",
                  &gConfigDxeFormSetGuid,
                  NULL, &Size, &Var32);
  if (EFI_ERROR (Status)) {
    PcdSet32 (PcdDebugEnableJTAG, PcdGet32 (PcdDebugEnableJTAG));
  }

  Size = sizeof (UINT32);
  Status = gRT->GetVariable (L"DebugShowUEFIExit",
                  &gConfigDxeFormSetGuid,
                  NULL, &Size, &Var32);
  if (EFI_ERROR (Status)) {
    PcdSet32 (PcdDebugShowUEFIExit, PcdGet32 (PcdDebugShowUEFIExit));
  }

  Size = sizeof (UINT8);
  Status = gRT->GetVariable (L"DisplayEnableScaledVModes",
                  &gConfigDxeFormSetGuid,
                  NULL, &Size, &Var8);
  if (EFI_ERROR (Status)) {
    PcdSet8 (PcdDisplayEnableScaledVModes, PcdGet8 (PcdDisplayEnableScaledVModes));
  }

  Size = sizeof (UINT32);
  Status = gRT->GetVariable (L"DisplayEnableSShot",
                  &gConfigDxeFormSetGuid,
                  NULL, &Size, &Var32);
  if (EFI_ERROR (Status)) {
    PcdSet32 (PcdDisplayEnableSShot, PcdGet32 (PcdDisplayEnableSShot));
  }

  return EFI_SUCCESS;
}


STATIC VOID
ApplyVariables (
  VOID
  )
{
  UINTN Gpio34Group;
  UINTN Gpio48Group;
  EFI_STATUS Status;
  UINT32 CpuClock = PcdGet32 (PcdCpuClock);
  UINT32 CustomCpuClock = PcdGet32 (PcdCustomCpuClock);
  UINT32 Rate = 0;
  UINT64 SystemMemorySize;

  switch (CpuClock) {
  case 0: // Low
    Rate = FixedPcdGet32 (PcdCpuLowSpeedMHz) * FREQ_1_MHZ;
    break;
  case 1: // Default
    /*
     * What the Raspberry Pi Foundation calls "max clock rate" is really the default value
     * from: https://www.raspberrypi.org/documentation/configuration/config-txt/overclocking.md
     */
    Status = mFwProtocol->GetMaxClockRate (RPI_MBOX_CLOCK_RATE_ARM, &Rate);
    if (Status != EFI_SUCCESS) {
      DEBUG ((DEBUG_ERROR, "Couldn't read default CPU speed %r\n", Status));
    }
    break;
  case 2: // Max
    Rate = FixedPcdGet32 (PcdCpuMaxSpeedMHz) * FREQ_1_MHZ;
    break;
  case 3: // Custom
    Rate = CustomCpuClock * FREQ_1_MHZ;
    break;
  }

  if (Rate != 0) {
    DEBUG ((DEBUG_INFO, "Setting CPU speed to %u MHz\n", Rate / FREQ_1_MHZ));
    Status = mFwProtocol->SetClockRate (RPI_MBOX_CLOCK_RATE_ARM, Rate, 1);
    if (Status != EFI_SUCCESS) {
      DEBUG ((DEBUG_ERROR, "Couldn't set the CPU speed: %r\n", Status));
    } else {
      PcdSet32 (PcdCustomCpuClock, Rate / FREQ_1_MHZ);
    }
  }

  Status = mFwProtocol->GetClockRate (RPI_MBOX_CLOCK_RATE_ARM, &Rate);
  if (Status != EFI_SUCCESS) {
    DEBUG ((DEBUG_ERROR, "Couldn't get the CPU speed: %r\n", Status));
  } else {
    DEBUG ((DEBUG_INFO, "Current CPU speed is %u MHz\n", Rate / FREQ_1_MHZ));
  }

  if (mModelFamily >= 4 && PcdGet32 (PcdRamMoreThan3GB) != 0 &&
      PcdGet32 (PcdRamLimitTo3GB) == 0) {
    /*
     * Similar to how we compute the > 3 GB RAM segment's size in PlatformLib/
     * RaspberryPiMem.c, with some overlap protection for the Bcm2xxx register
     * spaces. This computation should also work for models with more than 4 GB
     * RAM, if there ever exist ones.
     */
    SystemMemorySize = (UINT64)mModelInstalledMB * SIZE_1MB;
    ASSERT (SystemMemorySize > 3UL * SIZE_1GB);
    SystemMemorySize = MIN(SystemMemorySize, BCM2836_SOC_REGISTERS);
    if (BCM2711_SOC_REGISTERS > 0) {
      SystemMemorySize = MIN(SystemMemorySize, BCM2711_SOC_REGISTERS);
    }

    Status = gDS->AddMemorySpace (EfiGcdMemoryTypeSystemMemory, 3UL * BASE_1GB,
                    SystemMemorySize - (3UL * SIZE_1GB),
                    EFI_MEMORY_UC | EFI_MEMORY_WC | EFI_MEMORY_WT | EFI_MEMORY_WB);
    ASSERT_EFI_ERROR (Status);
    Status = gDS->SetMemorySpaceAttributes (3UL * BASE_1GB,
                    SystemMemorySize - (3UL * SIZE_1GB),
                    EFI_MEMORY_WB);
    ASSERT_EFI_ERROR (Status);
  }

  if (mModelFamily == 3) {
    /*
     * Pi 3: either Arasan or SdHost goes to SD card.
     *
     * Switching two groups around, so disable both first.
     *
     * No, I've not seen a problem, but having a group be
     * routed to two sets of pins seems like asking for trouble.
     */
    GpioPinFuncSet (34, GPIO_FSEL_INPUT);
    GpioPinFuncSet (35, GPIO_FSEL_INPUT);
    GpioPinFuncSet (36, GPIO_FSEL_INPUT);
    GpioPinFuncSet (37, GPIO_FSEL_INPUT);
    GpioPinFuncSet (38, GPIO_FSEL_INPUT);
    GpioPinFuncSet (39, GPIO_FSEL_INPUT);
    GpioPinFuncSet (48, GPIO_FSEL_INPUT);
    GpioPinFuncSet (49, GPIO_FSEL_INPUT);
    GpioPinFuncSet (50, GPIO_FSEL_INPUT);
    GpioPinFuncSet (51, GPIO_FSEL_INPUT);
    GpioPinFuncSet (52, GPIO_FSEL_INPUT);
    GpioPinFuncSet (53, GPIO_FSEL_INPUT);

    if (PcdGet32 (PcdSdIsArasan)) {
      DEBUG ((DEBUG_INFO, "Routing SD to Arasan\n"));
      Gpio48Group = GPIO_FSEL_ALT3;
      /*
       * Route SDIO to SdHost.
       */
      Gpio34Group = GPIO_FSEL_ALT0;
    } else {
      DEBUG ((DEBUG_INFO, "Routing SD to SdHost\n"));
      Gpio48Group = GPIO_FSEL_ALT0;
      /*
       * Route SDIO to Arasan.
       */
      Gpio34Group = GPIO_FSEL_ALT3;
    }
    GpioPinFuncSet (34, Gpio34Group);
    GpioPinFuncSet (35, Gpio34Group);
    GpioPinFuncSet (36, Gpio34Group);
    GpioPinFuncSet (37, Gpio34Group);
    GpioPinFuncSet (38, Gpio34Group);
    GpioPinFuncSet (39, Gpio34Group);
    GpioPinFuncSet (48, Gpio48Group);
    GpioPinFuncSet (49, Gpio48Group);
    GpioPinFuncSet (50, Gpio48Group);
    GpioPinFuncSet (51, Gpio48Group);
    GpioPinFuncSet (52, Gpio48Group);
    GpioPinFuncSet (53, Gpio48Group);

  } else if (mModelFamily == 4) {
    /*
     * Pi 4: either Arasan or eMMC2 goes to SD card.
     */
    if (PcdGet32 (PcdSdIsArasan)) {
      /*
       * WiFi disabled.
       */
      GpioPinFuncSet (34, GPIO_FSEL_INPUT);
      GpioPinFuncSet (35, GPIO_FSEL_INPUT);
      GpioPinFuncSet (36, GPIO_FSEL_INPUT);
      GpioPinFuncSet (37, GPIO_FSEL_INPUT);
      GpioPinFuncSet (38, GPIO_FSEL_INPUT);
      GpioPinFuncSet (39, GPIO_FSEL_INPUT);
      /*
       * SD card pins go to Arasan.
       */
      MmioWrite32((GPIO_BASE_ADDRESS + 0xD0),
                  MmioRead32(GPIO_BASE_ADDRESS + 0xD0) | 0x2);
    } else {
      /*
       * SD card pins back to eMMC2.
       */
      MmioWrite32((GPIO_BASE_ADDRESS + 0xD0),
                  MmioRead32(GPIO_BASE_ADDRESS + 0xD0) & ~0x2);
      /*
       * WiFi back to Arasan.
       */
      GpioPinFuncSet (34, GPIO_FSEL_ALT3);
      GpioPinFuncSet (35, GPIO_FSEL_ALT3);
      GpioPinFuncSet (36, GPIO_FSEL_ALT3);
      GpioPinFuncSet (37, GPIO_FSEL_ALT3);
      GpioPinFuncSet (38, GPIO_FSEL_ALT3);
      GpioPinFuncSet (39, GPIO_FSEL_ALT3);
    }
  } else {
    DEBUG ((DEBUG_ERROR, "Model Family %d not supported...\n", mModelFamily));
  }

  /*
   * JTAG pin    JTAG sig    GPIO      Mode    Header pin
   * 1           VREF        N/A               1
   * 3           nTRST       GPIO22    ALT4    15
   * 4           GND         N/A               9
   * 5           TDI         GPIO4     ALT5    7
   * 7           TMS         GPIO27    ALT4    13
   * 9           TCK         GPIO25    ALT4    22
   * 11          RTCK        GPIO23    ALT4    16
   * 13          TDO         GPIO24    ALT4    18
   */
  if (PcdGet32 (PcdDebugEnableJTAG)) {
    GpioPinFuncSet (22, GPIO_FSEL_ALT4);
    GpioPinFuncSet (4, GPIO_FSEL_ALT5);
    GpioPinFuncSet (27, GPIO_FSEL_ALT4);
    GpioPinFuncSet (25, GPIO_FSEL_ALT4);
    GpioPinFuncSet (23, GPIO_FSEL_ALT4);
    GpioPinFuncSet (24, GPIO_FSEL_ALT4);
  } else {
    GpioPinFuncSet (22, GPIO_FSEL_INPUT);
    GpioPinFuncSet (4, GPIO_FSEL_INPUT);
    GpioPinFuncSet (27, GPIO_FSEL_INPUT);
    GpioPinFuncSet (25, GPIO_FSEL_INPUT);
    GpioPinFuncSet (23, GPIO_FSEL_INPUT);
    GpioPinFuncSet (24, GPIO_FSEL_INPUT);
  }
}


EFI_STATUS
EFIAPI
ConfigInitialize (
  IN EFI_HANDLE ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS Status;

  Status = gBS->LocateProtocol (&gRaspberryPiFirmwareProtocolGuid,
                  NULL, (VOID**)&mFwProtocol);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = mFwProtocol->GetModelFamily (&mModelFamily);
  if (Status != EFI_SUCCESS) {
    DEBUG ((DEBUG_ERROR, "Couldn't get the Raspberry Pi model family: %r\n", Status));
  } else {
    DEBUG ((DEBUG_INFO, "Current Raspberry Pi model family is %d\n", mModelFamily));
  }

  Status = mFwProtocol->GetModelInstalledMB (&mModelInstalledMB);
  if (Status != EFI_SUCCESS) {
    DEBUG ((DEBUG_ERROR, "Couldn't get the Raspberry Pi installed RAM size: %r\n", Status));
  } else {
    DEBUG ((DEBUG_INFO, "Current Raspberry Pi installed RAM size is %d MB\n", mModelInstalledMB));
  }

  Status = SetupVariables ();
  if (Status != EFI_SUCCESS) {
    DEBUG ((DEBUG_ERROR, "Couldn't not setup NV vars: %r\n", Status));
  }

  ApplyVariables ();
  Status = gBS->InstallProtocolInterface (&ImageHandle,
                  &gRaspberryPiConfigAppliedProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  NULL);
  ASSERT_EFI_ERROR (Status);

  Status = InstallHiiPages ();
  if (Status != EFI_SUCCESS) {
    DEBUG ((DEBUG_ERROR, "Couldn't install ConfigDxe configuration pages: %r\n", Status));
  }

  Status = LocateAndInstallAcpiFromFv (&mAcpiTableFile);
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}
