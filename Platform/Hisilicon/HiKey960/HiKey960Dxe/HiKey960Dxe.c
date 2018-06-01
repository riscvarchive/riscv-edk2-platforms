/** @file
*
*  Copyright (c) 2018, Linaro Ltd. All rights reserved.
*
*  This program and the accompanying materials
*  are licensed and made available under the terms and conditions of the BSD License
*  which accompanies this distribution.  The full text of the license may be found at
*  http://opensource.org/licenses/bsd-license.php
*
*  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
*  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
*
**/

#include <Guid/EventGroup.h>

#include <Hi3660.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/PrintLib.h>
#include <Library/SerialPortLib.h>
#include <Library/TimerLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>

#include "HiKey960Dxe.h"

STATIC
VOID
InitSdCard (
  IN VOID
  )
{
  UINT32        Data;

  //
  // LDO16
  // 000: 1.75V, 001: 1.8V, 010: 2.4V, 011: 2.6V, 100: 2.7V,
  // 101: 2.85V, 110: 2.95V, 111: 3.0V.
  //
  Data = MmioRead32 (PMIC_LDO16_VSET_REG) & LDO16_VSET_MASK;
  Data |= 6;
  MmioWrite32 (PMIC_LDO16_VSET_REG, Data);
  MmioOr32 (PMIC_LDO16_ONOFF_ECO_REG, LDO16_ONOFF_ECO_LDO16_ENABLE);
  //
  // wait regulator stable
  //
  MicroSecondDelay (100);

  //
  // LDO9
  // 000: 1.75V, 001: 1.8V, 010: 1.825V, 011: 2.8V, 100: 2.85V,
  // 101: 2.95V, 110: 3.0V, 111: 3.3V.
  //
  Data = MmioRead32 (PMIC_LDO9_VSET_REG) & LDO9_VSET_MASK;
  Data |= 5;
  MmioWrite32 (PMIC_LDO9_VSET_REG, Data);
  MmioOr32 (PMU_REG_BASE + (0x6a << 2), 2);
  //
  // wait regulator stable
  //
  MicroSecondDelay (100);

  //
  // GPIO203
  //
  MmioWrite32 (IOMG_AO_REG_BASE + (24 << 2), 0); // GPIO function

  //
  // SD pinmux
  //
  MmioWrite32 (IOMG_MMC0_000_REG, IOMG_FUNC1); // SD_CLK
  MmioWrite32 (IOMG_MMC0_001_REG, IOMG_FUNC1); // SD_CMD
  MmioWrite32 (IOMG_MMC0_002_REG, IOMG_FUNC1); // SD_DATA0
  MmioWrite32 (IOMG_MMC0_003_REG, IOMG_FUNC1); // SD_DATA1
  MmioWrite32 (IOMG_MMC0_004_REG, IOMG_FUNC1); // SD_DATA2
  MmioWrite32 (IOMG_MMC0_005_REG, IOMG_FUNC1); // SD_DATA3
  MmioWrite32 (IOCG_MMC0_000_REG, IOCG_DRIVE (15)); // SD_CLK float with 32mA
  MmioWrite32 (IOCG_MMC0_001_REG, IOCG_PULLUP | IOCG_DRIVE (8)); // SD_CMD
  MmioWrite32 (IOCG_MMC0_002_REG, IOCG_PULLUP | IOCG_DRIVE (8)); // SD_DATA0
  MmioWrite32 (IOCG_MMC0_003_REG, IOCG_PULLUP | IOCG_DRIVE (8)); // SD_DATA1
  MmioWrite32 (IOCG_MMC0_004_REG, IOCG_PULLUP | IOCG_DRIVE (8)); // SD_DATA2
  MmioWrite32 (IOCG_MMC0_005_REG, IOCG_PULLUP | IOCG_DRIVE (8)); // SD_DATA3

  //
  // SC_SEL_SD:
  //   0xx: 3.2MHz, 100: PPLL0, 101: PPLL1, 11x: PPLL2.
  // SC_DIV_SD:
  //   divider = value + 1
  //
  do {
    MmioOr32 (
      CRG_CLKDIV4,
      CLKDIV4_SC_SEL_SD (7) |
      (CLKDIV4_SC_SEL_SD_MASK << CLKDIV4_SC_MASK_SHIFT)
      );
    Data = MmioRead32 (CRG_CLKDIV4) & CLKDIV4_SC_SEL_SD_MASK;
  } while (Data != CLKDIV4_SC_SEL_SD (7));

  //
  // Unreset SD controller
  //
  MmioWrite32 (CRG_PERRSTDIS4, PERRSTEN4_SD);
  do {
    Data = MmioRead32 (CRG_PERRSTSTAT4);
  } while ((Data & PERRSTEN4_SD) == PERRSTEN4_SD);
  //
  // Enable SD controller clock
  //
  MmioOr32 (CRG_PEREN0, PEREN0_GT_HCLK_SD);
  MmioOr32 (CRG_PEREN4, PEREN4_GT_CLK_SD);
  do {
    Data = MmioRead32 (CRG_PERCLKEN4);
  } while ((Data & PEREN4_GT_CLK_SD) != PEREN4_GT_CLK_SD);
}

VOID
InitPeripherals (
  IN VOID
  )
{
  //
  // Enable FPLL0
  //
  MmioOr32 (SCTRL_SCFPLLCTRL0, SCTRL_SCFPLLCTRL0_FPLL0_EN);

  InitSdCard ();

  //
  // Enable wifi clock
  //
  MmioOr32 (PMIC_HARDWARE_CTRL0, PMIC_HARDWARE_CTRL0_WIFI_CLK);
  MmioOr32 (PMIC_OSC32K_ONOFF_CTRL, PMIC_OSC32K_ONOFF_CTRL_EN_32K);
}

/**
  Notification function of the event defined as belonging to the
  EFI_END_OF_DXE_EVENT_GROUP_GUID event group that was created in
  the entry point of the driver.

  This function is called when an event belonging to the
  EFI_END_OF_DXE_EVENT_GROUP_GUID event group is signalled. Such an
  event is signalled once at the end of the dispatching of all
  drivers (end of the so called DXE phase).

  @param[in]  Event    Event declared in the entry point of the driver whose
                       notification function is being invoked.
  @param[in]  Context  NULL
**/
STATIC
VOID
OnEndOfDxe (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  UINT32        BootMode;
  VOID          *RecoveryStr;
  VOID          *SwitchStr;

  BootMode = MmioRead32 (SCTRL_BAK_DATA0) & BOOT_MODE_MASK;
  if (BootMode == BOOT_MODE_RECOVERY) {
    RecoveryStr = "WARNING: CAN NOT BOOT KERNEL IN RECOVERY MODE!\r\n";
    SwitchStr = "Switch to normal boot mode, then reboot to boot kernel.\r\n";
    SerialPortWrite (RecoveryStr, AsciiStrLen (RecoveryStr));
    SerialPortWrite (SwitchStr, AsciiStrLen (SwitchStr));
  }
}

EFI_STATUS
EFIAPI
HiKey960EntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS            Status;
  EFI_EVENT             EndOfDxeEvent;

  InitPeripherals ();

  //
  // Create an event belonging to the "gEfiEndOfDxeEventGroupGuid" group.
  // The "OnEndOfDxe()" function is declared as the call back function.
  // It will be called at the end of the DXE phase when an event of the
  // same group is signalled to inform about the end of the DXE phase.
  // Install the INSTALL_FDT_PROTOCOL protocol.
  //
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  OnEndOfDxe,
                  NULL,
                  &gEfiEndOfDxeEventGroupGuid,
                  &EndOfDxeEvent
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  return Status;
}
