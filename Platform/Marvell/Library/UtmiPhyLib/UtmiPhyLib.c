/********************************************************************************
Copyright (C) 2016 Marvell International Ltd.

Marvell BSD License Option

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File under the following licensing terms.
Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

* Redistributions of source code must Retain the above copyright notice,
  this list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.

* Neither the name of Marvell nor the names of its contributors may be
  used to endorse or promote products derived from this software without
  specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/

#include "UtmiPhyLib.h"

typedef struct {
  EFI_PHYSICAL_ADDRESS UtmiBaseAddr;
  EFI_PHYSICAL_ADDRESS UsbCfgAddr;
  EFI_PHYSICAL_ADDRESS UtmiCfgAddr;
  UINT32 UtmiPhyPort;
} UTMI_PHY_DATA;

STATIC
VOID
RegSetSilent (
  IN EFI_PHYSICAL_ADDRESS Addr,
  IN UINT32 Data,
  IN UINT32 Mask
  )
{
  UINT32 RegData;

  RegData = MmioRead32 (Addr);
  RegData &= ~Mask;
  RegData |= Data;
  MmioWrite32 (Addr, RegData);
}

STATIC
VOID
RegSet (
  IN EFI_PHYSICAL_ADDRESS Addr,
  IN UINT32 Data,
  IN UINT32 Mask
  )
{
  DEBUG((DEBUG_INFO, "Write to address = %10x, data = %10x (mask = %10x)-\n",
    Addr, Data, Mask));
  DEBUG((DEBUG_INFO, "old value = %10x ==>\n", MmioRead32 (Addr)));
  RegSetSilent (Addr, Data, Mask);
  DEBUG((DEBUG_INFO, "new value %10x\n", MmioRead32 (Addr)));
}

STATIC
VOID
UtmiPhyPowerDown (
  IN UINT32 UtmiIndex,
  IN EFI_PHYSICAL_ADDRESS UtmiBaseAddr,
  IN EFI_PHYSICAL_ADDRESS UsbCfgAddr,
  IN EFI_PHYSICAL_ADDRESS UtmiCfgAddr,
  IN UINT32 UtmiPhyPort
  )
{
  UINT32 Mask, Data;

  DEBUG((DEBUG_INFO, "UtmiPhy: stage: UTMI %d - Power down transceiver(power down Phy)\n",
    UtmiIndex));
  DEBUG((DEBUG_INFO, "UtmiPhy: stage: Power down PLL, and SuspendDM\n"));
  /* Power down UTMI PHY */
  RegSet (UtmiCfgAddr, 0x0 << UTMI_PHY_CFG_PU_OFFSET, UTMI_PHY_CFG_PU_MASK);
  /* Config USB3 Device UTMI enable */
  Mask = UTMI_USB_CFG_DEVICE_EN_MASK;

  /*
   * Prior to PHY init, configure mux for Device
   * (Device can be connected to UTMI0 or to UTMI1)
   */
  if (UtmiPhyPort == UTMI_PHY_TO_USB_DEVICE0) {
    Data = 0x1 << UTMI_USB_CFG_DEVICE_EN_OFFSET;
    /* Config USB3 Device UTMI MUX */
    Mask |= UTMI_USB_CFG_DEVICE_MUX_MASK;
    Data |= UtmiIndex << UTMI_USB_CFG_DEVICE_MUX_OFFSET;
  } else {
    Data = 0x0 << UTMI_USB_CFG_DEVICE_EN_OFFSET;
  }

  /* Set Test suspendm mode */
  Mask = UTMI_CTRL_STATUS0_SUSPENDM_MASK;
  Data = 0x1 << UTMI_CTRL_STATUS0_SUSPENDM_OFFSET;
  /* Enable Test UTMI select */
  Mask |= UTMI_CTRL_STATUS0_TEST_SEL_MASK;
  Data |= 0x1 << UTMI_CTRL_STATUS0_TEST_SEL_OFFSET;
  RegSet (UtmiBaseAddr + UTMI_CTRL_STATUS0_REG, Data, Mask);

  /* Wait for UTMI power down */
  MicroSecondDelay (1000);
}

STATIC
VOID
UtmiPhyConfig (
  IN UINT32 UtmiIndex,
  IN EFI_PHYSICAL_ADDRESS UtmiBaseAddr,
  IN EFI_PHYSICAL_ADDRESS UsbCfgAddr,
  IN EFI_PHYSICAL_ADDRESS UtmiCfgAddr,
  IN UINT32 UtmiPhyPort
  )
{
  UINT32 Mask, Data;

  DEBUG((DEBUG_INFO, "UtmiPhy: stage: Configure UTMI PHY %d registers\n",
    UtmiIndex));
  /* Reference Clock Divider Select */
  Mask = UTMI_PLL_CTRL_REFDIV_MASK;
  Data = 0x5 << UTMI_PLL_CTRL_REFDIV_OFFSET;
  /* Feedback Clock Divider Select - 90 for 25Mhz */
  Mask |= UTMI_PLL_CTRL_FBDIV_MASK;
  Data |= 0x60 << UTMI_PLL_CTRL_FBDIV_OFFSET;
  /* Select LPFR - 0x0 for 25Mhz/5=5Mhz */
  Mask |= UTMI_PLL_CTRL_SEL_LPFR_MASK;
  Data |= 0x0 << UTMI_PLL_CTRL_SEL_LPFR_OFFSET;
  RegSet (UtmiBaseAddr + UTMI_PLL_CTRL_REG, Data, Mask);

  /* Impedance Calibration Threshold Setting */
  RegSet (UtmiBaseAddr + UTMI_CALIB_CTRL_REG,
    0x6 << UTMI_CALIB_CTRL_IMPCAL_VTH_OFFSET,
    UTMI_CALIB_CTRL_IMPCAL_VTH_MASK);

  /* Set LS TX driver strength coarse control */
  Mask = UTMI_TX_CH_CTRL_DRV_EN_LS_MASK;
  Data = 0x3 << UTMI_TX_CH_CTRL_DRV_EN_LS_OFFSET;
  /* Set LS TX driver fine adjustment */
  Mask |= UTMI_TX_CH_CTRL_IMP_SEL_LS_MASK;
  Data |= 0x3 << UTMI_TX_CH_CTRL_IMP_SEL_LS_OFFSET;
  RegSet (UtmiBaseAddr + UTMI_TX_CH_CTRL_REG, Data, Mask);

  /* Enable SQ */
  Mask = UTMI_RX_CH_CTRL0_SQ_DET_MASK;
  Data = 0x0 << UTMI_RX_CH_CTRL0_SQ_DET_OFFSET;
  /* Enable analog squelch detect */
  Mask |= UTMI_RX_CH_CTRL0_SQ_ANA_DTC_MASK;
  Data |= 0x1 << UTMI_RX_CH_CTRL0_SQ_ANA_DTC_OFFSET;
  RegSet (UtmiBaseAddr + UTMI_RX_CH_CTRL0_REG, Data, Mask);

  /* Set External squelch calibration number */
  Mask = UTMI_RX_CH_CTRL1_SQ_AMP_CAL_MASK;
  Data = 0x1 << UTMI_RX_CH_CTRL1_SQ_AMP_CAL_OFFSET;
  /* Enable the External squelch calibration */
  Mask |= UTMI_RX_CH_CTRL1_SQ_AMP_CAL_EN_MASK;
  Data |= 0x1 << UTMI_RX_CH_CTRL1_SQ_AMP_CAL_EN_OFFSET;
  RegSet (UtmiBaseAddr + UTMI_RX_CH_CTRL1_REG, Data, Mask);

  /* Set Control VDAT Reference Voltage - 0.325V */
  Mask = UTMI_CHGDTC_CTRL_VDAT_MASK;
  Data = 0x1 << UTMI_CHGDTC_CTRL_VDAT_OFFSET;
  /* Set Control VSRC Reference Voltage - 0.6V */
  Mask |= UTMI_CHGDTC_CTRL_VSRC_MASK;
  Data |= 0x1 << UTMI_CHGDTC_CTRL_VSRC_OFFSET;
  RegSet (UtmiBaseAddr + UTMI_CHGDTC_CTRL_REG, Data, Mask);
}

STATIC
UINTN
UtmiPhyPowerUp (
  IN UINT32 UtmiIndex,
  IN EFI_PHYSICAL_ADDRESS UtmiBaseAddr,
  IN EFI_PHYSICAL_ADDRESS UsbCfgAddr,
  IN EFI_PHYSICAL_ADDRESS UtmiCfgAddr,
  IN UINT32 UtmiPhyPort
  )
{
  EFI_STATUS Status;
  UINT32 Data;

  DEBUG((DEBUG_INFO, "UtmiPhy: stage: UTMI %d - Power up transceiver(Power up Phy)\n",
    UtmiIndex));
  DEBUG((DEBUG_INFO, "UtmiPhy: stage: exit SuspendDM\n"));
  /* Power up UTMI PHY */
  RegSet (UtmiCfgAddr, 0x1 << UTMI_PHY_CFG_PU_OFFSET, UTMI_PHY_CFG_PU_MASK);
  /* Disable Test UTMI select */
  RegSet (UtmiBaseAddr + UTMI_CTRL_STATUS0_REG,
    0x0 << UTMI_CTRL_STATUS0_TEST_SEL_OFFSET,
    UTMI_CTRL_STATUS0_TEST_SEL_MASK);

  DEBUG((DEBUG_INFO, "UtmiPhy: stage: Wait for PLL and impedance calibration done, and PLL ready\n"));

  /* Delay 10ms */
  MicroSecondDelay (10000);

  Data = MmioRead32 (UtmiBaseAddr + UTMI_CALIB_CTRL_REG);
  if ((Data & UTMI_CALIB_CTRL_IMPCAL_DONE_MASK) == 0) {
    DEBUG((DEBUG_ERROR, "UtmiPhy: Impedance calibration is not done\n"));
    Status = EFI_D_ERROR;
  }
  if ((Data & UTMI_CALIB_CTRL_PLLCAL_DONE_MASK) == 0) {
    DEBUG((DEBUG_ERROR, "UtmiPhy: PLL calibration is not done\n"));
    Status = EFI_D_ERROR;
  }
  Data = MmioRead32 (UtmiBaseAddr + UTMI_PLL_CTRL_REG);
  if ((Data & UTMI_PLL_CTRL_PLL_RDY_MASK) == 0) {
    DEBUG((DEBUG_ERROR, "UtmiPhy: PLL is not ready\n"));
    Status = EFI_D_ERROR;
  }

  return Status;
}

/*
 * Cp110UtmiPhyInit initializes the UTMI PHY
 * the init split in 3 parts:
 * 1. Power down transceiver and PLL
 * 2. UTMI PHY configure
 * 3. Power up transceiver and PLL
 */
STATIC
VOID
Cp110UtmiPhyInit (
  IN UINT32 UtmiPhyCount,
  IN UTMI_PHY_DATA *UtmiData
  )
{
  UINT32 i;

  for (i = 0; i < UtmiPhyCount; i++) {
    UtmiPhyPowerDown(i, UtmiData[i].UtmiBaseAddr,
      UtmiData[i].UsbCfgAddr, UtmiData[i].UtmiCfgAddr,
      UtmiData[i].UtmiPhyPort);
  }

  /* Power down PLL */
  DEBUG((DEBUG_INFO, "UtmiPhy: stage: PHY power down PLL\n"));
  RegSet (UtmiData[0].UsbCfgAddr, 0x0 << UTMI_USB_CFG_PLL_OFFSET,
    UTMI_USB_CFG_PLL_MASK);

  for (i = 0; i < UtmiPhyCount; i++) {
    UtmiPhyConfig(i, UtmiData[i].UtmiBaseAddr,
      UtmiData[i].UsbCfgAddr, UtmiData[i].UtmiCfgAddr,
      UtmiData[i].UtmiPhyPort);
  }

  for (i = 0; i < UtmiPhyCount; i++) {
    if (EFI_ERROR(UtmiPhyPowerUp(i, UtmiData[i].UtmiBaseAddr,
        UtmiData[i].UsbCfgAddr, UtmiData[i].UtmiCfgAddr,
        UtmiData[i].UtmiPhyPort))) {
      DEBUG((DEBUG_ERROR, "UtmiPhy: Failed to initialize UTMI PHY %d\n", i));
      continue;
    }
    DEBUG((DEBUG_ERROR, "UTMI PHY %d initialized to ", i));

    if (UtmiData[i].UtmiPhyPort == UTMI_PHY_TO_USB_DEVICE0)
      DEBUG((DEBUG_ERROR, "USB Device\n"));
    else
      DEBUG((DEBUG_ERROR, "USB Host%d\n", UtmiData[i].UtmiPhyPort));
  }

  /* Power up PLL */
  DEBUG((DEBUG_INFO, "UtmiPhy: stage: PHY power up PLL\n"));
  RegSet (UtmiData[0].UsbCfgAddr, 0x1 << UTMI_USB_CFG_PLL_OFFSET,
    UTMI_USB_CFG_PLL_MASK);
}

EFI_STATUS
UtmiPhyInit (
  VOID
  )
{
  EFI_STATUS Status;
  UTMI_PHY_DATA UtmiData[PcdGet32 (PcdUtmiPhyCount)];
  EFI_PHYSICAL_ADDRESS RegUtmiUnit[PcdGet32 (PcdUtmiPhyCount)];
  EFI_PHYSICAL_ADDRESS RegUsbCfg[PcdGet32 (PcdUtmiPhyCount)];
  EFI_PHYSICAL_ADDRESS RegUtmiCfg[PcdGet32 (PcdUtmiPhyCount)];
  UINTN UtmiPort[PcdGet32 (PcdUtmiPhyCount)];
  UINTN i, Count;

  Count = PcdGet32 (PcdUtmiPhyCount);
  if (Count == 0) {
    /* No UTMI PHY on platform */
    return EFI_SUCCESS;
  }

  DEBUG((DEBUG_INFO, "UtmiPhy: Initialize USB UTMI PHYs\n"));
  /* Parse UtmiPhy PCDs */
  Status = ParsePcdString ((CHAR16 *) PcdGetPtr (PcdUtmiPhyRegUtmiUnit),
    Count, RegUtmiUnit, NULL);
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, "UtmiPhy: Wrong PcdUtmiPhyRegUtmiUnit format\n"));
    return EFI_INVALID_PARAMETER;
  }

  Status = ParsePcdString ((CHAR16 *) PcdGetPtr (PcdUtmiPhyRegUsbCfg),
    Count, RegUsbCfg, NULL);
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, "UtmiPhy: Wrong PcdUtmiPhyRegUsbCfg format\n"));
    return EFI_INVALID_PARAMETER;
  }

  Status = ParsePcdString ((CHAR16 *) PcdGetPtr (PcdUtmiPhyRegUtmiCfg),
    Count, RegUtmiCfg, NULL);
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, "UtmiPhy: Wrong PcdUtmiPhyRegUtmiCfg format\n"));
    return EFI_INVALID_PARAMETER;
  }

  Status = ParsePcdString ((CHAR16 *) PcdGetPtr (PcdUtmiPhyUtmiPort),
    Count, UtmiPort, NULL);
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, "UtmiPhy: Wrong PcdUtmiPhyUtmiPort format\n"));
    return EFI_INVALID_PARAMETER;
  }

  for (i = 0 ; i < Count ; i++) {
    /* Get base address of UTMI phy */
    UtmiData[i].UtmiBaseAddr = RegUtmiUnit[i];

    /* Get usb config address */
    UtmiData[i].UsbCfgAddr = RegUsbCfg[i];

    /* Get UTMI config address */
    UtmiData[i].UtmiCfgAddr = RegUtmiCfg[i];

    /*
     * Get the usb port number, which will be used to check if
     * the utmi connected to host or device
     */
    UtmiData[i].UtmiPhyPort = UtmiPort[i];
  }

  /* Currently only Cp110 is supported */
  Cp110UtmiPhyInit (Count, UtmiData);

  return EFI_SUCCESS;
}
