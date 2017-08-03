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

#include "ComPhyLib.h"
#include <Library/MvHwDescLib.h>

#define SD_LANE_ADDR_WIDTH          0x1000
#define HPIPE_ADDR_OFFSET           0x800
#define COMPHY_ADDR_LANE_WIDTH      0x28
#define SD_ADDR(base, Lane)         (base + SD_LANE_ADDR_WIDTH * Lane)
#define HPIPE_ADDR(base, Lane)      (SD_ADDR(base, Lane) + HPIPE_ADDR_OFFSET)
#define COMPHY_ADDR(base, Lane)     (base + COMPHY_ADDR_LANE_WIDTH * Lane)

DECLARE_A7K8K_NONDISCOVERABLE_TEMPLATE;

/*
 * For CP-110 we have 2 Selector registers "PHY Selectors"
 * and " PIPE Selectors".
 * PIPE selector include USB and PCIe options.
 * PHY selector include the Ethernet and SATA options, every Ethernet option
 * has different options, for example: serdes Lane2 have option Eth_port_0
 * that include (SGMII0, RXAUI0, SFI)
 */
COMPHY_MUX_DATA Cp110ComPhyMuxData[] = {
  /* Lane 0 */
  {4, {{COMPHY_TYPE_UNCONNECTED, 0x0}, {COMPHY_TYPE_SGMII1, 0x1},
       {COMPHY_TYPE_SATA1, 0x4}, {COMPHY_TYPE_SATA3, 0x4}}},
  /* Lane 1 */
  {4, {{COMPHY_TYPE_UNCONNECTED, 0x0}, {COMPHY_TYPE_SGMII2, 0x1},
       {COMPHY_TYPE_SATA0, 0x4}, {COMPHY_TYPE_SATA2, 0x4}}},
  /* Lane 2 */
  {6, {{COMPHY_TYPE_UNCONNECTED, 0x0}, {COMPHY_TYPE_SGMII0, 0x1},
       {COMPHY_TYPE_RXAUI0, 0x1}, {COMPHY_TYPE_SFI, 0x1},
       {COMPHY_TYPE_SATA0, 0x4}, {COMPHY_TYPE_SATA2, 0x4}}},
  /* Lane 3 */
  {8, {{COMPHY_TYPE_UNCONNECTED, 0x0}, {COMPHY_TYPE_RXAUI1, 0x1},
       {COMPHY_TYPE_SGMII1, 0x2}, {COMPHY_TYPE_SATA1, 0x4},
       {COMPHY_TYPE_SATA3, 0x4}}},
  /* Lane 4 */
  {7, {{COMPHY_TYPE_UNCONNECTED, 0x0}, {COMPHY_TYPE_SGMII0, 0x2},
       {COMPHY_TYPE_RXAUI0, 0x2}, {COMPHY_TYPE_SFI, 0x2},
       {COMPHY_TYPE_SGMII1, 0x1}}},
  /* Lane 5 */
  {6, {{COMPHY_TYPE_UNCONNECTED, 0x0}, {COMPHY_TYPE_SGMII2, 0x1},
       {COMPHY_TYPE_RXAUI1, 0x2}, {COMPHY_TYPE_SATA1, 0x4},
       {COMPHY_TYPE_SATA3, 0x4}}},
};

COMPHY_MUX_DATA Cp110ComPhyPipeMuxData[] = {
  /* Lane 0 */
  {2, {{COMPHY_TYPE_UNCONNECTED, 0x0}, {COMPHY_TYPE_PCIE0, 0x4}}},
  /* Lane 1 */
  {4, {{COMPHY_TYPE_UNCONNECTED, 0x0}, {COMPHY_TYPE_USB3_HOST0, 0x1},
       {COMPHY_TYPE_USB3_DEVICE, 0x2}, {COMPHY_TYPE_PCIE0, 0x4}}},
  /* Lane 2 */
  {3, {{COMPHY_TYPE_UNCONNECTED, 0x0}, {COMPHY_TYPE_USB3_HOST0, 0x1},
       {COMPHY_TYPE_PCIE0, 0x4}}},
  /* Lane 3 */
  {3, {{COMPHY_TYPE_UNCONNECTED, 0x0}, {COMPHY_TYPE_USB3_HOST1, 0x1},
       {COMPHY_TYPE_PCIE0, 0x4}}},
  /* Lane 4 */
  {4, {{COMPHY_TYPE_UNCONNECTED, 0x0}, {COMPHY_TYPE_USB3_HOST1, 0x1},
       {COMPHY_TYPE_USB3_DEVICE, 0x2}, {COMPHY_TYPE_PCIE1, 0x4}}},
  /* Lane 5 */
  {2, {{COMPHY_TYPE_UNCONNECTED, 0x0}, {COMPHY_TYPE_PCIE2, 0x4}}},
};

STATIC
VOID
ComPhyPcieRFUConfiguration (
  IN EFI_PHYSICAL_ADDRESS ComPhyAddr
)
{
  UINT32 Mask, Data;

  /* RFU configurations - hard reset ComPhy */
  Mask = COMMON_PHY_CFG1_PWR_UP_MASK;
  Data = 0x1 << COMMON_PHY_CFG1_PWR_UP_OFFSET;
  Mask |= COMMON_PHY_CFG1_PIPE_SELECT_MASK;
  Data |= 0x1 << COMMON_PHY_CFG1_PIPE_SELECT_OFFSET;
  Mask |= COMMON_PHY_CFG1_PWR_ON_RESET_MASK;
  Data |= 0x0 << COMMON_PHY_CFG1_PWR_ON_RESET_OFFSET;
  Mask |= COMMON_PHY_CFG1_CORE_RSTN_MASK;
  Data |= 0x0 << COMMON_PHY_CFG1_CORE_RSTN_OFFSET;
  Mask |= COMMON_PHY_PHY_MODE_MASK;
  Data |= 0x0 << COMMON_PHY_PHY_MODE_OFFSET;
  RegSet (ComPhyAddr + COMMON_PHY_CFG1_REG, Data, Mask);

  /* Release from hard reset */
  Mask = COMMON_PHY_CFG1_PWR_ON_RESET_MASK;
  Data = 0x1 << COMMON_PHY_CFG1_PWR_ON_RESET_OFFSET;
  Mask |= COMMON_PHY_CFG1_CORE_RSTN_MASK;
  Data |= 0x1 << COMMON_PHY_CFG1_CORE_RSTN_OFFSET;
  RegSet (ComPhyAddr + COMMON_PHY_CFG1_REG, Data, Mask);

  /* Wait 1ms - until band gap and ref clock ready */
  MicroSecondDelay (1000);
  MemoryFence ();
}

STATIC
VOID
ComPhyPciePhyConfiguration (
  IN EFI_PHYSICAL_ADDRESS ComPhyAddr,
  IN EFI_PHYSICAL_ADDRESS HpipeAddr
)
{
  UINT32 Mask, Data, PcieClk = 0;

  /* Set PIPE soft reset */
  Mask = HPIPE_RST_CLK_CTRL_PIPE_RST_MASK;
  Data = 0x1 << HPIPE_RST_CLK_CTRL_PIPE_RST_OFFSET;

  /* Set PHY Datapath width mode for V0 */
  Mask |= HPIPE_RST_CLK_CTRL_FIXED_PCLK_MASK;
  Data |= 0x1 << HPIPE_RST_CLK_CTRL_FIXED_PCLK_OFFSET;

  /* Set Data bus width USB mode for V0 */
  Mask |= HPIPE_RST_CLK_CTRL_PIPE_WIDTH_MASK;
  Data |= 0x0 << HPIPE_RST_CLK_CTRL_PIPE_WIDTH_OFFSET;

  /* Set CORE_CLK output frequency for 250Mhz */
  Mask |= HPIPE_RST_CLK_CTRL_CORE_FREQ_SEL_MASK;
  Data |= 0x0 << HPIPE_RST_CLK_CTRL_CORE_FREQ_SEL_OFFSET;
  RegSet (HpipeAddr + HPIPE_RST_CLK_CTRL_REG, Data, Mask);

  /* Set PLL ready delay for 0x2 */
  RegSet (HpipeAddr + HPIPE_CLK_SRC_LO_REG,
    0x2 << HPIPE_CLK_SRC_LO_PLL_RDY_DL_OFFSET,
    HPIPE_CLK_SRC_LO_PLL_RDY_DL_MASK);

  /* Set PIPE mode interface to PCIe3 - 0x1 */
  RegSet (HpipeAddr + HPIPE_CLK_SRC_HI_REG,
    0x1 << HPIPE_CLK_SRC_HI_MODE_PIPE_OFFSET, HPIPE_CLK_SRC_HI_MODE_PIPE_MASK);

  /* Config update polarity equalization */
  RegSet (HpipeAddr + HPIPE_LANE_EQ_CFG1_REG,
    0x1 << HPIPE_CFG_UPDATE_POLARITY_OFFSET, HPIPE_CFG_UPDATE_POLARITY_MASK);

  /* Set PIPE version 4 to mode enable */
  RegSet (HpipeAddr + HPIPE_DFE_CTRL_28_REG,
    0x1 << HPIPE_DFE_CTRL_28_PIPE4_OFFSET, HPIPE_DFE_CTRL_28_PIPE4_MASK);

  /* Enable PIN clock 100M_125M */
  Mask = HPIPE_MISC_CLK100M_125M_MASK;
  Data = 0x1 << HPIPE_MISC_CLK100M_125M_OFFSET;

  /* Set PIN_TXDCLK_2X Clock Frequency Selection for outputs 500MHz clock */
  Mask |= HPIPE_MISC_TXDCLK_2X_MASK;
  Data |= 0x0 << HPIPE_MISC_TXDCLK_2X_OFFSET;

  /* Enable 500MHz Clock */
  Mask |= HPIPE_MISC_CLK500_EN_MASK;
  Data |= 0x1 << HPIPE_MISC_CLK500_EN_OFFSET;

  if (PcieClk) {
    /* Set reference clock comes from group 1 */
    Mask |= HPIPE_MISC_REFCLK_SEL_MASK;
    Data |= 0x0 << HPIPE_MISC_REFCLK_SEL_OFFSET;
  } else {
    /* Set reference clock comes from group 2 */
    Mask |= HPIPE_MISC_REFCLK_SEL_MASK;
    Data |= 0x1 << HPIPE_MISC_REFCLK_SEL_OFFSET;
  }

  /* Force ICP */
  Mask |= HPIPE_MISC_ICP_FORCE_MASK;
  Data |= 0x1 << HPIPE_MISC_ICP_FORCE_OFFSET;
  RegSet (HpipeAddr + HPIPE_MISC_REG, Data, Mask);

  if (PcieClk) {
    /* Set reference frequcency select - 0x2 for 25MHz*/
    Mask = HPIPE_PWR_PLL_REF_FREQ_MASK;
    Data = 0x2 << HPIPE_PWR_PLL_REF_FREQ_OFFSET;
  } else {
    /* Set reference frequcency select - 0x0 for 100MHz*/
    Mask = HPIPE_PWR_PLL_REF_FREQ_MASK;
    Data = 0x0 << HPIPE_PWR_PLL_REF_FREQ_OFFSET;
  }

  /* Set PHY mode to PCIe */
  Mask |= HPIPE_PWR_PLL_PHY_MODE_MASK;
  Data |= 0x3 << HPIPE_PWR_PLL_PHY_MODE_OFFSET;
  RegSet (HpipeAddr + HPIPE_PWR_PLL_REG, Data, Mask);

  /*
   * Set the amount of time spent in the LoZ state - set
   * for 0x7 only if the PCIe clock is output
   */
  if (PcieClk)
    RegSet (HpipeAddr + HPIPE_GLOBAL_PM_CTRL,
      0x7 << HPIPE_GLOBAL_PM_RXDLOZ_WAIT_OFFSET,
      HPIPE_GLOBAL_PM_RXDLOZ_WAIT_MASK);

  /* Set Maximal PHY Generation Setting (8Gbps) */
  Mask = HPIPE_INTERFACE_GEN_MAX_MASK;
  Data = 0x2 << HPIPE_INTERFACE_GEN_MAX_OFFSET;
  /* Bypass frame detection and sync detection for RX DATA */
  Mask |= HPIPE_INTERFACE_DET_BYPASS_MASK;
  Data |= 0x1 << HPIPE_INTERFACE_DET_BYPASS_OFFSET;
  /* Set Link Train Mode (Tx training control pins are used) */
  Mask |= HPIPE_INTERFACE_LINK_TRAIN_MASK;
  Data |= 0x1 << HPIPE_INTERFACE_LINK_TRAIN_OFFSET;
  RegSet (HpipeAddr + HPIPE_INTERFACE_REG, Data, Mask);

  /* Set Idle_sync enable */
  Mask = HPIPE_PCIE_IDLE_SYNC_MASK;
  Data = 0x1 << HPIPE_PCIE_IDLE_SYNC_OFFSET;

  /* Select bits for PCIE Gen3(32bit) */
  Mask |= HPIPE_PCIE_SEL_BITS_MASK;
  Data |= 0x2 << HPIPE_PCIE_SEL_BITS_OFFSET;
  RegSet (HpipeAddr + HPIPE_PCIE_REG0, Data, Mask);

  /* Enable Tx_adapt_g1 */
  Mask = HPIPE_TX_TRAIN_CTRL_G1_MASK;
  Data = 0x1 << HPIPE_TX_TRAIN_CTRL_G1_OFFSET;

  /* Enable Tx_adapt_gn1 */
  Mask |= HPIPE_TX_TRAIN_CTRL_GN1_MASK;
  Data |= 0x1 << HPIPE_TX_TRAIN_CTRL_GN1_OFFSET;

  /* Disable Tx_adapt_g0 */
  Mask |= HPIPE_TX_TRAIN_CTRL_G0_MASK;
  Data |= 0x0 << HPIPE_TX_TRAIN_CTRL_G0_OFFSET;
  RegSet (HpipeAddr + HPIPE_TX_TRAIN_CTRL_REG, Data, Mask);

  /* Set reg_tx_train_chk_init */
  Mask = HPIPE_TX_TRAIN_CHK_INIT_MASK;
  Data = 0x0 << HPIPE_TX_TRAIN_CHK_INIT_OFFSET;

  /* Enable TX_COE_FM_PIN_PCIE3_EN */
  Mask |= HPIPE_TX_TRAIN_COE_FM_PIN_PCIE3_MASK;
  Data |= 0x1 << HPIPE_TX_TRAIN_COE_FM_PIN_PCIE3_OFFSET;
  RegSet (HpipeAddr + HPIPE_TX_TRAIN_REG, Data, Mask);
}

STATIC
VOID
ComPhyPcieSetAnalogParameters (
  IN EFI_PHYSICAL_ADDRESS HpipeAddr
)
{
  UINT32 Data, Mask;

  /* Set preset sweep configurations */
  Mask = HPIPE_TX_TX_STATUS_CHECK_MODE_MASK |
         HPIPE_TX_NUM_OF_PRESET_MASK |
         HPIPE_TX_SWEEP_PRESET_EN_MASK;
  Data = (0x1 << HPIPE_TX_STATUS_CHECK_MODE_OFFSET) |
         (0x7 << HPIPE_TX_NUM_OF_PRESET_OFFSET) |
         (0x1 << HPIPE_TX_SWEEP_PRESET_EN_OFFSET);
  MmioAndThenOr32 (HpipeAddr + HPIPE_TX_TRAIN_CTRL_11_REG, ~Mask, Data);

  /* Tx train start configuration */
  Mask = HPIPE_TX_TRAIN_START_SQ_EN_MASK |
         HPIPE_TX_TRAIN_START_FRM_DET_EN_MASK |
         HPIPE_TX_TRAIN_START_FRM_LOCK_EN_MASK |
         HPIPE_TX_TRAIN_WAIT_TIME_EN_MASK;
  Data = (0x1 << HPIPE_TX_TRAIN_START_SQ_EN_OFFSET) |
         (0x1 << HPIPE_TX_TRAIN_WAIT_TIME_EN_OFFSET);
  MmioAndThenOr32 (HpipeAddr + HPIPE_TX_TRAIN_CTRL_5_REG, ~Mask, Data);

  /* Enable Tx train P2P */
  MmioOr32 (HpipeAddr + HPIPE_TX_TRAIN_CTRL_0_REG, HPIPE_TX_TRAIN_P2P_HOLD_MASK);

  /* Configure Tx train timeout */
  MmioAndThenOr32 (
          HpipeAddr + HPIPE_TX_TRAIN_CTRL_4_REG,
          ~HPIPE_TRX_TRAIN_TIMER_MASK,
          0x17 << HPIPE_TRX_TRAIN_TIMER_OFFSET
          );

  /* Disable G0/G1/GN1 adaptation */
  MmioAnd32 (
          HpipeAddr + HPIPE_TX_TRAIN_CTRL_REG,
          ~(HPIPE_TX_TRAIN_CTRL_G1_MASK | HPIPE_TX_TRAIN_CTRL_GN1_MASK | HPIPE_TX_TRAIN_CTRL_G0_OFFSET)
          );

  /* Disable DTL frequency loop */
  MmioAnd32 (HpipeAddr + HPIPE_PWR_CTR_DTL_REG, ~HPIPE_PWR_CTR_DTL_FLOOP_EN_MASK);

  /* Configure Generation 3 DFE */
  MmioAndThenOr32 (
          HpipeAddr + HPIPE_G3_SET4_REG,
          ~HPIPE_GX_SET4_DFE_RES_MASK,
          0x3 << HPIPE_GX_SET4_DFE_RES_OFFSET
          );

  /* Use TX/RX training result for DFE */
  MmioAnd32 (HpipeAddr + HPIPE_DFE_REG0, ~HPIPE_DFE_RES_FORCE_MASK);

  /* Configure initial and final coefficient value for receiver */
  MmioAndThenOr32 (HpipeAddr + HPIPE_G3_SET1_REG,  ~Mask, Data);
  Mask = HPIPE_GX_SET1_RX_SELMUPI_MASK |
         HPIPE_GX_SET1_RX_SELMUPP_MASK |
         HPIPE_GX_SET1_SAMPLER_INPAIRX2_EN_MASK;
  Data = 0x1 | (0x1 << HPIPE_GX_SET1_RX_SELMUPP_OFFSET);
  MmioAndThenOr32 (HpipeAddr + HPIPE_G3_SET1_REG, ~Mask, Data);

  /* Trigger sampler 5us enable pulse */
  MmioAndThenOr32 (
          HpipeAddr + HPIPE_SAMPLER_N_PROC_CALIB_CTRL_REG,
          ~HPIPE_SAMPLER_MASK,
          0x1 << HPIPE_SAMPLER_OFFSET
          );
  MicroSecondDelay (5);
  MmioAnd32 (
          HpipeAddr + HPIPE_SAMPLER_N_PROC_CALIB_CTRL_REG,
          ~HPIPE_SAMPLER_MASK
          );

  /* FFE resistor tuning for different bandwidth  */
  Mask = HPIPE_GX_SET3_FFE_DEG_RES_LEVEL_MASK |
         HPIPE_GX_SET3_FFE_LOAD_RES_LEVEL_MASK;
  Data = (0x1 << HPIPE_GX_SET3_FFE_DEG_RES_LEVEL_OFFSET) |
         (0x3 << HPIPE_GX_SET3_FFE_LOAD_RES_LEVEL_OFFSET);
  MmioAndThenOr32 (HpipeAddr + HPIPE_G3_SET3_REG, ~Mask, Data);

  /* Pattern lock lost timeout disable */
  MmioAnd32 (HpipeAddr + HPIPE_FRAME_DETECT_CTRL_3_REG, ~HPIPE_PATTERN_LOCK_LOST_TIMEOUT_EN_MASK);

  /* Configure DFE adaptations */
  MmioAndThenOr32 (
          HpipeAddr + HPIPE_CDR_CONTROL_REG,
          ~(HPIPE_CDR_MAX_DFE_ADAPT_1_MASK | HPIPE_CDR_MAX_DFE_ADAPT_0_MASK | HPIPE_CDR_RX_MAX_DFE_ADAPT_1_MASK),
          0x1 << HPIPE_CDR_MAX_DFE_ADAPT_1_OFFSET
          );
  MmioAnd32 (HpipeAddr + HPIPE_DFE_CONTROL_REG, ~HPIPE_DFE_TX_MAX_DFE_ADAPT_MASK);

  /* Hpipe Generation 2 setting 1*/
  MmioAndThenOr32 (
          HpipeAddr + HPIPE_G2_SET1_REG,
          ~(HPIPE_GX_SET1_RX_SELMUPI_MASK | HPIPE_GX_SET1_RX_SELMUPP_MASK | HPIPE_GX_SET1_RX_SELMUFI_MASK),
          0x1 << HPIPE_GX_SET1_RX_SELMUPP_OFFSET
          );

  /* DFE enable */
  MmioAndThenOr32 (
          HpipeAddr + HPIPE_G2_SET4_REG,
          ~HPIPE_GX_SET4_DFE_RES_MASK,
          0x3 << HPIPE_GX_SET4_DFE_RES_OFFSET
          );

  /* Configure DFE Resolution */
  MmioAndThenOr32 (
          HpipeAddr + HPIPE_LANE_CFG4_REG,
          ~HPIPE_LANE_CFG4_DFE_EN_SEL_MASK,
          0x1 << HPIPE_LANE_CFG4_DFE_EN_SEL_OFFSET
          );

  /* VDD calibration control */
  MmioAndThenOr32 (
          HpipeAddr + HPIPE_VDD_CAL_CTRL_REG,
          ~HPIPE_EXT_SELLV_RXSAMPL_MASK,
          0x16 << HPIPE_EXT_SELLV_RXSAMPL_OFFSET
          );

  /* Set PLL Charge-pump Current Control */
  MmioAndThenOr32 (HpipeAddr + HPIPE_G3_SET5_REG, ~HPIPE_GX_SET5_ICP_MASK, 0x4);

  /* Set lane rqualization remote setting */
  Mask = HPIPE_LANE_CFG_FOM_DIRN_OVERRIDE_MASK |
         HPIPE_LANE_CFG_FOM_ONLY_MODE_MASK |
         HPIPE_LANE_CFG_FOM_PRESET_VECTOR_MASK;
  Data = (0x1 << HPIPE_LANE_CFG_FOM_DIRN_OVERRIDE_OFFSET) |
         (0x1 << HPIPE_LANE_CFG_FOM_ONLY_MODE_OFFFSET) |
         (0x2 << HPIPE_LANE_CFG_FOM_PRESET_VECTOR_OFFSET);
  MmioAndThenOr32 (HpipeAddr + HPIPE_LANE_EQ_REMOTE_SETTING_REG, ~Mask, Data);

  /* Set phy in root complex mode */
  MmioOr32 (HpipeAddr + HPIPE_LANE_EQU_CONFIG_0_REG, HPIPE_CFG_PHY_RC_EP_MASK);
}

STATIC
VOID
ComPhyPciePhyPowerUp (
  IN EFI_PHYSICAL_ADDRESS HpipeAddr
)
{
  /* Release from PIPE soft reset */
  RegSet (HpipeAddr + HPIPE_RST_CLK_CTRL_REG,
    0x0 << HPIPE_RST_CLK_CTRL_PIPE_RST_OFFSET,
    HPIPE_RST_CLK_CTRL_PIPE_RST_MASK);

  /* Wait 15ms - for ComPhy calibration done */
  MicroSecondDelay (15000);
  MemoryFence ();
}

STATIC
EFI_STATUS
ComPhyPcieCheckPll (
  IN EFI_PHYSICAL_ADDRESS HpipeAddr
)
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINT32 Data;

  /* Read Lane status */
  Data = MmioRead32 (HpipeAddr + HPIPE_LANE_STATUS0_REG);
  if ((Data & HPIPE_LANE_STATUS0_PCLK_EN_MASK) == 0) {
    DEBUG((DEBUG_INFO, "ComPhy: Read from reg = %p - value = 0x%x\n",
      HpipeAddr + HPIPE_LANE_STATUS0_REG, Data));
    DEBUG((DEBUG_INFO, "ComPhy: HPIPE_LANE_STATUS0_PCLK_EN_MASK is 0\n"));
    Status = EFI_D_ERROR;
  }

  return Status;
}

STATIC
EFI_STATUS
ComPhyPciePowerUp (
  IN UINT32 Lane,
  IN UINT32 PcieBy4,
  IN EFI_PHYSICAL_ADDRESS HpipeBase,
  IN EFI_PHYSICAL_ADDRESS ComPhyBase
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_PHYSICAL_ADDRESS HpipeAddr = HPIPE_ADDR(HpipeBase, Lane);
  EFI_PHYSICAL_ADDRESS ComPhyAddr = COMPHY_ADDR(ComPhyBase, Lane);

  DEBUG((DEBUG_INFO, "ComPhy: stage: RFU configurations - hard reset ComPhy\n"));

  ComPhyPcieRFUConfiguration (ComPhyAddr);

  DEBUG((DEBUG_INFO, "ComPhy: stage: ComPhy configuration\n"));

  ComPhyPciePhyConfiguration (ComPhyAddr, HpipeAddr);

  DEBUG((DEBUG_INFO, "ComPhy: stage: Set analog paramters\n"));

  ComPhyPcieSetAnalogParameters (HpipeAddr);

  DEBUG((DEBUG_INFO, "ComPhy: stage: ComPhy power up\n"));

  ComPhyPciePhyPowerUp (HpipeAddr);

  DEBUG((DEBUG_INFO, "ComPhy: stage: Check PLL\n"));

  Status = ComPhyPcieCheckPll (HpipeAddr);

  return Status;
}

STATIC
VOID
ComPhyUsb3RFUConfiguration (
  IN EFI_PHYSICAL_ADDRESS ComPhyAddr
)
{
  UINT32 Mask, Data;

  /* RFU configurations - hard reset ComPhy */
  Mask = COMMON_PHY_CFG1_PWR_UP_MASK;
  Data = 0x1 << COMMON_PHY_CFG1_PWR_UP_OFFSET;
  Mask |= COMMON_PHY_CFG1_PIPE_SELECT_MASK;
  Data |= 0x1 << COMMON_PHY_CFG1_PIPE_SELECT_OFFSET;
  Mask |= COMMON_PHY_CFG1_PWR_ON_RESET_MASK;
  Data |= 0x0 << COMMON_PHY_CFG1_PWR_ON_RESET_OFFSET;
  Mask |= COMMON_PHY_CFG1_CORE_RSTN_MASK;
  Data |= 0x0 << COMMON_PHY_CFG1_CORE_RSTN_OFFSET;
  Mask |= COMMON_PHY_PHY_MODE_MASK;
  Data |= 0x1 << COMMON_PHY_PHY_MODE_OFFSET;
  RegSet (ComPhyAddr + COMMON_PHY_CFG1_REG, Data, Mask);

  /* Release from hard reset */
  Mask = COMMON_PHY_CFG1_PWR_ON_RESET_MASK;
  Data = 0x1 << COMMON_PHY_CFG1_PWR_ON_RESET_OFFSET;
  Mask |= COMMON_PHY_CFG1_CORE_RSTN_MASK;
  Data |= 0x1 << COMMON_PHY_CFG1_CORE_RSTN_OFFSET;
  RegSet (ComPhyAddr + COMMON_PHY_CFG1_REG, Data, Mask);

  /* Wait 1ms - until band gap and ref clock ready */
  MicroSecondDelay (1000);
  MemoryFence ();
}

STATIC
VOID
ComPhyUsb3PhyConfiguration (
  IN EFI_PHYSICAL_ADDRESS HpipeAddr
)
{
  UINT32 Mask, Data;

  /* Set PIPE soft reset */
  Mask = HPIPE_RST_CLK_CTRL_PIPE_RST_MASK;
  Data = 0x1 << HPIPE_RST_CLK_CTRL_PIPE_RST_OFFSET;

  /* Set PHY Datapath width mode for V0 */
  Mask |= HPIPE_RST_CLK_CTRL_FIXED_PCLK_MASK;
  Data |= 0x0 << HPIPE_RST_CLK_CTRL_FIXED_PCLK_OFFSET;

  /* Set Data bus width USB mode for V0 */
  Mask |= HPIPE_RST_CLK_CTRL_PIPE_WIDTH_MASK;
  Data |= 0x0 << HPIPE_RST_CLK_CTRL_PIPE_WIDTH_OFFSET;

  /* Set CORE_CLK output frequency for 250Mhz */
  Mask |= HPIPE_RST_CLK_CTRL_CORE_FREQ_SEL_MASK;
  Data |= 0x0 << HPIPE_RST_CLK_CTRL_CORE_FREQ_SEL_OFFSET;
  RegSet (HpipeAddr + HPIPE_RST_CLK_CTRL_REG, Data, Mask);

  /* Set PLL ready delay for 0x2 */
  RegSet (HpipeAddr + HPIPE_CLK_SRC_LO_REG,
    0x2 << HPIPE_CLK_SRC_LO_PLL_RDY_DL_OFFSET,
    HPIPE_CLK_SRC_LO_PLL_RDY_DL_MASK);

  /* Set reference clock to come from group 1 - 25Mhz */
  RegSet (HpipeAddr + HPIPE_MISC_REG, 0x0 << HPIPE_MISC_REFCLK_SEL_OFFSET,
    HPIPE_MISC_REFCLK_SEL_MASK);

  /* Set reference frequcency select - 0x2 */
  Mask = HPIPE_PWR_PLL_REF_FREQ_MASK;
  Data = 0x2 << HPIPE_PWR_PLL_REF_FREQ_OFFSET;

  /* Set PHY mode to USB - 0x5 */
  Mask |= HPIPE_PWR_PLL_PHY_MODE_MASK;
  Data |= 0x5 << HPIPE_PWR_PLL_PHY_MODE_OFFSET;
  RegSet (HpipeAddr + HPIPE_PWR_PLL_REG, Data, Mask);

  /* Set the amount of time spent in the LoZ state - set for 0x7 */
  RegSet (HpipeAddr + HPIPE_GLOBAL_PM_CTRL,
    0x7 << HPIPE_GLOBAL_PM_RXDLOZ_WAIT_OFFSET,
    HPIPE_GLOBAL_PM_RXDLOZ_WAIT_MASK);

  /* Set max PHY generation setting - 5Gbps */
  RegSet (HpipeAddr + HPIPE_INTERFACE_REG,
    0x1 << HPIPE_INTERFACE_GEN_MAX_OFFSET, HPIPE_INTERFACE_GEN_MAX_MASK);

  /* Set select Data width 20Bit (SEL_BITS[2:0]) */
  RegSet (HpipeAddr + HPIPE_LOOPBACK_REG,
    0x1 << HPIPE_LOOPBACK_SEL_OFFSET, HPIPE_LOOPBACK_SEL_MASK);
}

STATIC
VOID
ComPhyUsb3SetAnalogParameters (
  IN EFI_PHYSICAL_ADDRESS HpipeAddr
)
{
  UINT32 Data, Mask;

  /* Set Pin DFE_PAT_DIS -> Bit[1]: PIN_DFE_PAT_DIS = 0x0 */
  Mask = HPIPE_LANE_CFG4_DFE_CTRL_MASK;
  Data = 0x1 << HPIPE_LANE_CFG4_DFE_CTRL_OFFSET;

  /* Set Override PHY DFE control pins for 0x1 */
  Mask |= HPIPE_LANE_CFG4_DFE_OVER_MASK;
  Data |= 0x1 << HPIPE_LANE_CFG4_DFE_OVER_OFFSET;

  /* Set Spread Spectrum Clock Enable fot 0x1 */
  Mask |= HPIPE_LANE_CFG4_SSC_CTRL_MASK;
  Data |= 0x1 << HPIPE_LANE_CFG4_SSC_CTRL_OFFSET;
  RegSet (HpipeAddr + HPIPE_LANE_CFG4_REG, Data, Mask);
}

STATIC
UINTN
ComphyUsb3PowerUp (
  UINT32 Lane,
  EFI_PHYSICAL_ADDRESS HpipeBase,
  EFI_PHYSICAL_ADDRESS ComPhyBase
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINT32 Data;
  EFI_PHYSICAL_ADDRESS HpipeAddr = HPIPE_ADDR(HpipeBase, Lane);
  EFI_PHYSICAL_ADDRESS ComPhyAddr = COMPHY_ADDR(ComPhyBase, Lane);

  DEBUG((DEBUG_INFO, "ComPhy: stage: RFU configurations - hard reset ComPhy\n"));

  ComPhyUsb3RFUConfiguration (ComPhyAddr);

  /* Start ComPhy Configuration */
  DEBUG((DEBUG_INFO, "stage: Comphy configuration\n"));

  ComPhyUsb3PhyConfiguration (HpipeAddr);

  /* Start analog paramters from ETP(HW) */
  DEBUG((DEBUG_INFO, "ComPhy: stage: Analog paramters from ETP(HW)\n"));

  ComPhyUsb3SetAnalogParameters (HpipeAddr);

  DEBUG((DEBUG_INFO, "ComPhy: stage: Comphy power up\n"));

  /* Release from PIPE soft reset */
  RegSet (HpipeAddr + HPIPE_RST_CLK_CTRL_REG,
    0x0 << HPIPE_RST_CLK_CTRL_PIPE_RST_OFFSET,
    HPIPE_RST_CLK_CTRL_PIPE_RST_MASK);

  /* Wait 15ms - for ComPhy calibration done */
  MicroSecondDelay (15000);
  MemoryFence ();

  DEBUG((DEBUG_INFO, "ComPhy: stage: Check PLL\n"));

  /* Read Lane status */
  Data = MmioRead32 (HpipeAddr + HPIPE_LANE_STATUS0_REG);
  if ((Data & HPIPE_LANE_STATUS0_PCLK_EN_MASK) == 0) {
    DEBUG((DEBUG_ERROR, "ComPhy: HPIPE_LANE_STATUS0_PCLK_EN_MASK is 0\n"));
    Status = EFI_D_ERROR;
  }

  return Status;
}

STATIC
UINT32
PollingWithTimeout (
  IN EFI_PHYSICAL_ADDRESS Addr,
  IN UINT32 Val,
  IN UINT32 Mask,
  IN UINT64 Usec_timeout
  )
{
  UINT32 Data;

  do {
    MicroSecondDelay(1);
    Data = MmioRead32(Addr) & Mask;
  } while (Data != Val  && --Usec_timeout > 0);

  if (Usec_timeout == 0)
    return Data;
  return 0;
}

STATIC
VOID
ComPhySataMacPowerDown (
  IN EFI_PHYSICAL_ADDRESS SataBase
)
{
  UINT32 Mask, Data;

  /*
   * MAC configuration - power down ComPhy
   * Use indirect address for vendor specific SATA control register
   */
  RegSet (SataBase + SATA3_VENDOR_ADDRESS,
    SATA_CONTROL_REG << SATA3_VENDOR_ADDR_OFSSET, SATA3_VENDOR_ADDR_MASK);

  /* SATA 0 power down */
  Mask = SATA3_CTRL_SATA0_PD_MASK;
  Data = 0x1 << SATA3_CTRL_SATA0_PD_OFFSET;

  /* SATA 1 power down */
  Mask |= SATA3_CTRL_SATA1_PD_MASK;
  Data |= 0x1 << SATA3_CTRL_SATA1_PD_OFFSET;

  /* SATA SSU disable */
  Mask |= SATA3_CTRL_SATA1_ENABLE_MASK;
  Data |= 0x0 << SATA3_CTRL_SATA1_ENABLE_OFFSET;

  /* SATA port 1 disable */
  Mask |= SATA3_CTRL_SATA_SSU_MASK;
  Data |= 0x0 << SATA3_CTRL_SATA_SSU_OFFSET;
  RegSet (SataBase + SATA3_VENDOR_DATA, Data, Mask);
}

STATIC
VOID
ComPhySataRFUConfiguration (
  IN EFI_PHYSICAL_ADDRESS ComPhyAddr,
  IN EFI_PHYSICAL_ADDRESS SdIpAddr
)
{
  UINT32 Mask, Data;

  /* RFU configurations - hard reset ComPhy */
  Mask = COMMON_PHY_CFG1_PWR_UP_MASK;
  Data = 0x1 << COMMON_PHY_CFG1_PWR_UP_OFFSET;
  Mask |= COMMON_PHY_CFG1_PIPE_SELECT_MASK;
  Data |= 0x0 << COMMON_PHY_CFG1_PIPE_SELECT_OFFSET;
  Mask |= COMMON_PHY_CFG1_PWR_ON_RESET_MASK;
  Data |= 0x0 << COMMON_PHY_CFG1_PWR_ON_RESET_OFFSET;
  Mask |= COMMON_PHY_CFG1_CORE_RSTN_MASK;
  Data |= 0x0 << COMMON_PHY_CFG1_CORE_RSTN_OFFSET;
  RegSet (ComPhyAddr + COMMON_PHY_CFG1_REG, Data, Mask);

  /* Set select Data  width 40Bit - SATA mode only */
  RegSet (ComPhyAddr + COMMON_PHY_CFG6_REG,
    0x1 << COMMON_PHY_CFG6_IF_40_SEL_OFFSET, COMMON_PHY_CFG6_IF_40_SEL_MASK);

  /* Release from hard reset in SD external */
  Mask = SD_EXTERNAL_CONFIG1_RESET_IN_MASK;
  Data = 0x1 << SD_EXTERNAL_CONFIG1_RESET_IN_OFFSET;
  Mask |= SD_EXTERNAL_CONFIG1_RESET_CORE_MASK;
  Data |= 0x1 << SD_EXTERNAL_CONFIG1_RESET_CORE_OFFSET;
  RegSet (SdIpAddr + SD_EXTERNAL_CONFIG1_REG, Data, Mask);

  /* Wait 1ms - until band gap and ref clock ready */
  MicroSecondDelay (1000);
  MemoryFence ();
}

STATIC
VOID
ComPhySataPhyConfiguration (
  IN EFI_PHYSICAL_ADDRESS HpipeAddr
)
{
  UINT32 Mask, Data;

  /* Set reference clock to comes from group 1 - choose 25Mhz */
  RegSet (HpipeAddr + HPIPE_MISC_REG,
    0x0 << HPIPE_MISC_REFCLK_SEL_OFFSET, HPIPE_MISC_REFCLK_SEL_MASK);

  /* Reference frequency select set 1 (for SATA = 25Mhz) */
  Mask = HPIPE_PWR_PLL_REF_FREQ_MASK;
  Data = 0x1 << HPIPE_PWR_PLL_REF_FREQ_OFFSET;

  /* PHY mode select (set SATA = 0x0 */
  Mask |= HPIPE_PWR_PLL_PHY_MODE_MASK;
  Data |= 0x0 << HPIPE_PWR_PLL_PHY_MODE_OFFSET;
  RegSet (HpipeAddr + HPIPE_PWR_PLL_REG, Data, Mask);

  /* Set max PHY generation setting - 6Gbps */
  RegSet (HpipeAddr + HPIPE_INTERFACE_REG,
    0x2 << HPIPE_INTERFACE_GEN_MAX_OFFSET, HPIPE_INTERFACE_GEN_MAX_MASK);

  /* Set select Data  width 40Bit (SEL_BITS[2:0]) */
  RegSet (HpipeAddr + HPIPE_LOOPBACK_REG,
    0x2 << HPIPE_LOOPBACK_SEL_OFFSET, HPIPE_LOOPBACK_SEL_MASK);
}

STATIC
VOID
ComPhySataSetAnalogParameters (
  IN EFI_PHYSICAL_ADDRESS HpipeAddr,
  IN EFI_PHYSICAL_ADDRESS SdIpAddr
)
{
  UINT32 Mask, Data;

  /* Hpipe Generation 1 settings 1 */
  Mask = HPIPE_GX_SET1_RX_SELMUPI_MASK |
         HPIPE_GX_SET1_RX_SELMUPP_MASK |
         HPIPE_GX_SET1_RX_SELMUFI_MASK |
         HPIPE_GX_SET1_RX_SELMUFF_MASK |
         HPIPE_GX_SET1_RX_DIGCK_DIV_MASK;
  Data = (0x1 << HPIPE_GX_SET1_RX_SELMUPP_OFFSET) |
         (0x3 << HPIPE_GX_SET1_RX_SELMUFF_OFFSET) |
         (0x1 << HPIPE_GX_SET1_RX_DIGCK_DIV_OFFSET);
  MmioAndThenOr32 (HpipeAddr + HPIPE_G1_SET1_REG, ~Mask, Data);

  /* Hpipe Generation 1 settings 3 */
  Mask = HPIPE_GX_SET3_FFE_CAP_SEL_MASK |
         HPIPE_GX_SET3_FFE_RES_SEL_MASK |
         HPIPE_GX_SET3_FFE_SETTING_FORCE_MASK |
         HPIPE_GX_SET3_FFE_DEG_RES_LEVEL_MASK |
         HPIPE_GX_SET3_FFE_LOAD_RES_LEVEL_MASK;
  Data = 0xf |
         (0x2 << HPIPE_GX_SET3_FFE_RES_SEL_OFFSET) |
         (0x1 << HPIPE_GX_SET3_FFE_SETTING_FORCE_OFFSET) |
         (0x1 << HPIPE_GX_SET3_FFE_DEG_RES_LEVEL_OFFSET) |
         (0x1 << HPIPE_GX_SET3_FFE_LOAD_RES_LEVEL_OFFSET);
  MmioAndThenOr32 (HpipeAddr + HPIPE_G1_SET3_REG, ~Mask, Data);

  /* Hpipe Generation 2 settings 1 */
  Mask = HPIPE_GX_SET1_RX_SELMUPI_MASK |
         HPIPE_GX_SET1_RX_SELMUPP_MASK |
         HPIPE_GX_SET1_RX_SELMUFI_MASK |
         HPIPE_GX_SET1_RX_SELMUFF_MASK |
         HPIPE_GX_SET1_RX_DIGCK_DIV_MASK;
  Data = (0x1 << HPIPE_GX_SET1_RX_SELMUPP_OFFSET) |
         (0x3 << HPIPE_GX_SET1_RX_SELMUFF_OFFSET) |
         (0x1 << HPIPE_GX_SET1_RX_DIGCK_DIV_OFFSET);
  MmioAndThenOr32 (HpipeAddr + HPIPE_G2_SET1_REG, ~Mask, Data);

  /* Hpipe Generation 3 settings 1 */
  Mask = HPIPE_GX_SET1_RX_SELMUPI_MASK |
         HPIPE_GX_SET1_RX_SELMUPP_MASK |
         HPIPE_GX_SET1_RX_SELMUFI_MASK |
         HPIPE_GX_SET1_RX_SELMUFF_MASK |
         HPIPE_GX_SET1_RX_DFE_EN_MASK |
         HPIPE_GX_SET1_RX_DIGCK_DIV_MASK |
         HPIPE_GX_SET1_SAMPLER_INPAIRX2_EN_MASK;
  Data = 0x2 |
         (0x2 << HPIPE_GX_SET1_RX_SELMUPP_OFFSET) |
         (0x3 << HPIPE_GX_SET1_RX_SELMUFI_OFFSET) |
         (0x3 << HPIPE_GX_SET1_RX_SELMUFF_OFFSET) |
         (0x1 << HPIPE_GX_SET1_RX_DFE_EN_OFFSET) |
         (0x2 << HPIPE_GX_SET1_RX_DIGCK_DIV_OFFSET);
  MmioAndThenOr32 (HpipeAddr + HPIPE_G3_SET1_REG, ~Mask, Data);

  /* DTL Control */
  Mask = HPIPE_PWR_CTR_DTL_SQ_DET_EN_MASK |
         HPIPE_PWR_CTR_DTL_SQ_PLOOP_EN_MASK |
         HPIPE_PWR_CTR_DTL_FLOOP_EN_MASK |
         HPIPE_PWR_CTR_DTL_CLAMPING_SEL_MASK |
         HPIPE_PWR_CTR_DTL_INTPCLK_DIV_FORCE_MASK |
         HPIPE_PWR_CTR_DTL_CLK_MODE_MASK |
         HPIPE_PWR_CTR_DTL_CLK_MODE_FORCE_MASK;
  Data = 0x1 |
         (0x1 << HPIPE_PWR_CTR_DTL_SQ_PLOOP_EN_OFFSET) |
         (0x1 << HPIPE_PWR_CTR_DTL_FLOOP_EN_OFFSET) |
         (0x1 << HPIPE_PWR_CTR_DTL_CLAMPING_SEL_OFFSET) |
         (0x1 << HPIPE_PWR_CTR_DTL_INTPCLK_DIV_FORCE_OFFSET) |
         (0x1 << HPIPE_PWR_CTR_DTL_CLK_MODE_OFFSET) |
         (0x1 << HPIPE_PWR_CTR_DTL_CLK_MODE_FORCE_OFFSET);
  MmioAndThenOr32 (HpipeAddr + HPIPE_PWR_CTR_DTL_REG, ~Mask, Data);

  /* Trigger sampler enable pulse (by toggling the bit) */
  MmioAndThenOr32 (
          HpipeAddr + HPIPE_SAMPLER_N_PROC_CALIB_CTRL_REG,
          ~HPIPE_SAMPLER_MASK,
          0x1 << HPIPE_SAMPLER_OFFSET
          );
  MmioAnd32 (
          HpipeAddr + HPIPE_SAMPLER_N_PROC_CALIB_CTRL_REG,
          ~HPIPE_SAMPLER_MASK
          );

  /* VDD Calibration Control 3 */
  MmioAndThenOr32 (
          HpipeAddr + HPIPE_VDD_CAL_CTRL_REG,
          ~HPIPE_EXT_SELLV_RXSAMPL_MASK,
          0x10 << HPIPE_EXT_SELLV_RXSAMPL_OFFSET
          );

  /* DFE Resolution Control */
  MmioOr32 (HpipeAddr + HPIPE_DFE_REG0, HPIPE_DFE_RES_FORCE_MASK);

  /* DFE F3-F5 Coefficient Control */
  MmioAnd32 (
          HpipeAddr + HPIPE_DFE_F3_F5_REG,
          ~(HPIPE_DFE_F3_F5_DFE_EN_MASK | HPIPE_DFE_F3_F5_DFE_CTRL_MASK)
          );

  /* Hpipe Generation 3 settings 3 */
  Mask = HPIPE_GX_SET3_FFE_CAP_SEL_MASK |
         HPIPE_GX_SET3_FFE_RES_SEL_MASK |
         HPIPE_GX_SET3_FFE_SETTING_FORCE_MASK |
         HPIPE_GX_SET3_FFE_DEG_RES_LEVEL_MASK |
         HPIPE_GX_SET3_FFE_LOAD_RES_LEVEL_MASK;
  Data = 0xf |
         (0x4 << HPIPE_GX_SET3_FFE_RES_SEL_OFFSET) |
         (0x1 << HPIPE_GX_SET3_FFE_SETTING_FORCE_OFFSET) |
         (0x1 << HPIPE_GX_SET3_FFE_DEG_RES_LEVEL_OFFSET) |
         (0x3 << HPIPE_GX_SET3_FFE_LOAD_RES_LEVEL_OFFSET);
  MmioAndThenOr32 (HpipeAddr + HPIPE_G3_SET3_REG, ~Mask, Data);

  /* Hpipe Generation 3 settings 4 */
  MmioAndThenOr32 (
          HpipeAddr + HPIPE_G3_SET4_REG,
          ~HPIPE_GX_SET4_DFE_RES_MASK,
          0x2 << HPIPE_GX_SET4_DFE_RES_OFFSET
          );

  /* Offset Phase Control - force offset and toggle 'valid' bit */
  MmioAndThenOr32 (
          HpipeAddr + HPIPE_PHASE_CONTROL_REG,
          ~(HPIPE_OS_PH_OFFSET_MASK | HPIPE_OS_PH_OFFSET_FORCE_MASK),
          0x5c | (0x1 << HPIPE_OS_PH_OFFSET_FORCE_OFFSET)
          );
  MmioAndThenOr32 (
          HpipeAddr + HPIPE_PHASE_CONTROL_REG,
          ~HPIPE_OS_PH_VALID_MASK,
          0x1 << HPIPE_OS_PH_VALID_OFFSET
          );
  MmioAnd32 (
          HpipeAddr + HPIPE_PHASE_CONTROL_REG,
          ~HPIPE_OS_PH_VALID_MASK
          );

  /* Set G1 TX amplitude and TX post emphasis value */
  Mask = HPIPE_GX_SET0_TX_AMP_MASK |
         HPIPE_GX_SET0_TX_AMP_ADJ_MASK |
         HPIPE_GX_SET0_TX_EMPH1_MASK |
         HPIPE_GX_SET0_TX_EMPH1_EN_MASK;
  Data = (0x8 << HPIPE_GX_SET0_TX_AMP_OFFSET) |
         (0x1 << HPIPE_GX_SET0_TX_AMP_ADJ_OFFSET) |
         (0x1 << HPIPE_GX_SET0_TX_EMPH1_OFFSET) |
         (0x1 << HPIPE_GX_SET0_TX_EMPH1_EN_OFFSET);
  MmioAndThenOr32 (HpipeAddr + HPIPE_G1_SET0_REG, ~Mask, Data);

  /* Set G2 TX amplitude and TX post emphasis value */
  Mask = HPIPE_GX_SET0_TX_AMP_MASK |
         HPIPE_GX_SET0_TX_AMP_ADJ_MASK |
         HPIPE_GX_SET0_TX_EMPH1_MASK |
         HPIPE_GX_SET0_TX_EMPH1_EN_MASK;
  Data = (0xa << HPIPE_GX_SET0_TX_AMP_OFFSET) |
         (0x1 << HPIPE_GX_SET0_TX_AMP_ADJ_OFFSET) |
         (0x2 << HPIPE_GX_SET0_TX_EMPH1_OFFSET) |
         (0x1 << HPIPE_GX_SET0_TX_EMPH1_EN_OFFSET);
  MmioAndThenOr32 (HpipeAddr + HPIPE_G2_SET0_REG, ~Mask, Data);

  /* Set G3 TX amplitude and TX post emphasis value */
  Mask = HPIPE_GX_SET0_TX_AMP_MASK |
         HPIPE_GX_SET0_TX_AMP_ADJ_MASK |
         HPIPE_GX_SET0_TX_EMPH1_MASK |
         HPIPE_GX_SET0_TX_EMPH1_EN_MASK |
         HPIPE_GX_SET0_TX_SLEW_RATE_SEL_MASK |
         HPIPE_GX_SET0_TX_SLEW_CTRL_EN_MASK;
  Data = (0xe << HPIPE_GX_SET0_TX_AMP_OFFSET) |
         (0x1 << HPIPE_GX_SET0_TX_AMP_ADJ_OFFSET) |
         (0x6 << HPIPE_GX_SET0_TX_EMPH1_OFFSET) |
         (0x1 << HPIPE_GX_SET0_TX_EMPH1_EN_OFFSET) |
         (0x4 << HPIPE_GX_SET0_TX_SLEW_RATE_SEL_OFFSET);
  MmioAndThenOr32 (HpipeAddr + HPIPE_G3_SET0_REG, ~Mask, Data);

  /* SERDES External Configuration 2 register - enable spread spectrum clock */
  MmioOr32 (SdIpAddr + SD_EXTERNAL_CONFIG2_REG, SD_EXTERNAL_CONFIG2_SSC_ENABLE_MASK);

  /* DFE reset sequence */
  MmioAndThenOr32 (
          HpipeAddr + HPIPE_PWR_CTR_REG,
          ~HPIPE_PWR_CTR_RST_DFE_MASK,
          0x1
          );
  MmioAnd32 (
          HpipeAddr + HPIPE_PWR_CTR_REG,
          ~HPIPE_PWR_CTR_RST_DFE_MASK
          );

  /* SW reset for interupt logic */
  MmioAndThenOr32 (
          HpipeAddr + HPIPE_PWR_CTR_REG,
          ~HPIPE_PWR_CTR_SFT_RST_MASK,
          0x1 << HPIPE_PWR_CTR_SFT_RST_OFFSET
          );
  MmioAnd32 (
          HpipeAddr + HPIPE_PWR_CTR_REG,
          ~HPIPE_PWR_CTR_SFT_RST_MASK
          );
}

STATIC
VOID
ComPhySataPhyPowerUp (
  IN EFI_PHYSICAL_ADDRESS SataBase
)
{
  UINT32 Data, Mask;

  /*
   * MAC configuration - power up ComPhy - power up PLL/TX/RX
   * Use indirect address for vendor specific SATA control register
   */
  RegSet (SataBase + SATA3_VENDOR_ADDRESS,
    SATA_CONTROL_REG << SATA3_VENDOR_ADDR_OFSSET, SATA3_VENDOR_ADDR_MASK);

  /* SATA 0 power up */
  Mask = SATA3_CTRL_SATA0_PD_MASK;
  Data = 0x0 << SATA3_CTRL_SATA0_PD_OFFSET;

  /* SATA 1 power up */
  Mask |= SATA3_CTRL_SATA1_PD_MASK;
  Data |= 0x0 << SATA3_CTRL_SATA1_PD_OFFSET;

  /* SATA SSU enable */
  Mask |= SATA3_CTRL_SATA1_ENABLE_MASK;
  Data |= 0x1 << SATA3_CTRL_SATA1_ENABLE_OFFSET;

  /* SATA port 1 enable */
  Mask |= SATA3_CTRL_SATA_SSU_MASK;
  Data |= 0x1 << SATA3_CTRL_SATA_SSU_OFFSET;
  RegSet (SataBase + SATA3_VENDOR_DATA, Data, Mask);

  /* MBUS request size and interface select register */
  RegSet (SataBase + SATA3_VENDOR_ADDRESS,
    SATA_MBUS_SIZE_SELECT_REG << SATA3_VENDOR_ADDR_OFSSET,
      SATA3_VENDOR_ADDR_MASK);

  /* Mbus regret enable */
  RegSet (SataBase + SATA3_VENDOR_DATA, 0x1 << SATA_MBUS_REGRET_EN_OFFSET,
    SATA_MBUS_REGRET_EN_MASK);
}

STATIC
EFI_STATUS
ComPhySataCheckPll (
  IN EFI_PHYSICAL_ADDRESS HpipeAddr,
  IN EFI_PHYSICAL_ADDRESS SdIpAddr
)
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINT32 Data,Mask;
  IN EFI_PHYSICAL_ADDRESS Addr;

  Addr = SdIpAddr + SD_EXTERNAL_STATUS0_REG;
  Data = SD_EXTERNAL_STATUS0_PLL_TX_MASK & SD_EXTERNAL_STATUS0_PLL_RX_MASK;
  Mask = Data;
  Data =  PollingWithTimeout (Addr, Data, Mask, 15000);

  if (Data != 0) {
    DEBUG((DEBUG_INFO, "ComPhy: Read from reg = %p - value = 0x%x\n",
      HpipeAddr + HPIPE_LANE_STATUS0_REG, Data));
    DEBUG((DEBUG_ERROR, "ComPhy: SD_EXTERNAL_STATUS0_PLL_TX is %d, SD_EXTERNAL_STATUS0_PLL_RX is %d\n",
      (Data & SD_EXTERNAL_STATUS0_PLL_TX_MASK),
      (Data & SD_EXTERNAL_STATUS0_PLL_RX_MASK)));
    Status = EFI_D_ERROR;
  }

  return Status;
}

STATIC
UINTN
ComPhySataPowerUp (
  IN UINT32 Lane,
  IN EFI_PHYSICAL_ADDRESS HpipeBase,
  IN EFI_PHYSICAL_ADDRESS ComPhyBase,
  IN UINT8 SataHostId
  )
{
  EFI_STATUS Status;
  UINT8 *SataDeviceTable;
  MVHW_NONDISCOVERABLE_DESC *Desc = &mA7k8kNonDiscoverableDescTemplate;
  EFI_PHYSICAL_ADDRESS HpipeAddr = HPIPE_ADDR(HpipeBase, Lane);
  EFI_PHYSICAL_ADDRESS SdIpAddr = SD_ADDR(HpipeBase, Lane);
  EFI_PHYSICAL_ADDRESS ComPhyAddr = COMPHY_ADDR(ComPhyBase, Lane);

  SataDeviceTable = (UINT8 *) PcdGetPtr (PcdPciEAhci);

  if (SataDeviceTable == NULL || SataHostId >= PcdGetSize (PcdPciEAhci)) {
    DEBUG ((DEBUG_ERROR, "ComPhySata: Sata host %d is undefined\n", SataHostId));
    return EFI_INVALID_PARAMETER;
  }

  if (!MVHW_DEV_ENABLED (Sata, SataHostId)) {
    DEBUG ((DEBUG_ERROR, "ComPhySata: Sata host %d is disabled\n", SataHostId));
    return EFI_INVALID_PARAMETER;
  }

  DEBUG ((DEBUG_INFO, "ComPhySata: Initialize SATA PHYs\n"));

  DEBUG((DEBUG_INFO, "ComPhySataPowerUp: stage: MAC configuration - power down ComPhy\n"));

  ComPhySataMacPowerDown (Desc->AhciBaseAddresses[SataHostId]);

  DEBUG((DEBUG_INFO, "ComPhy: stage: RFU configurations - hard reset ComPhy\n"));

  ComPhySataRFUConfiguration (ComPhyAddr, SdIpAddr);

  DEBUG((DEBUG_INFO, "ComPhy: stage: Comphy configuration\n"));

  ComPhySataPhyConfiguration (HpipeAddr);

  DEBUG((DEBUG_INFO, "ComPhy: stage: Analog paramters from ETP(HW)\n"));

  ComPhySataSetAnalogParameters (HpipeAddr, SdIpAddr);

  DEBUG((DEBUG_INFO, "ComPhy: stage: ComPhy power up\n"));

  ComPhySataPhyPowerUp (Desc->AhciBaseAddresses[SataHostId]);

  DEBUG((DEBUG_INFO, "ComPhy: stage: Check PLL\n"));

  Status = ComPhySataCheckPll (HpipeAddr, SdIpAddr);

  return Status;
}

STATIC
VOID
ComPhySgmiiRFUConfiguration (
  IN EFI_PHYSICAL_ADDRESS ComPhyAddr,
  IN EFI_PHYSICAL_ADDRESS SdIpAddr,
  IN UINT32 SgmiiSpeed
)
{
  UINT32 Mask, Data;

  Mask = COMMON_PHY_CFG1_PWR_UP_MASK;
  Data = 0x1 << COMMON_PHY_CFG1_PWR_UP_OFFSET;
  Mask |= COMMON_PHY_CFG1_PIPE_SELECT_MASK;
  Data |= 0x0 << COMMON_PHY_CFG1_PIPE_SELECT_OFFSET;
  RegSet (ComPhyAddr + COMMON_PHY_CFG1_REG, Data, Mask);

  /* Select Baud Rate of Comphy And PD_PLL/Tx/Rx */
  Mask = SD_EXTERNAL_CONFIG0_SD_PU_PLL_MASK;
  Data = 0x0 << SD_EXTERNAL_CONFIG0_SD_PU_PLL_OFFSET;
  Mask |= SD_EXTERNAL_CONFIG0_SD_PHY_GEN_RX_MASK;
  Mask |= SD_EXTERNAL_CONFIG0_SD_PHY_GEN_TX_MASK;
  if (SgmiiSpeed == COMPHY_SPEED_1_25G) {
    Data |= 0x6 << SD_EXTERNAL_CONFIG0_SD_PHY_GEN_RX_OFFSET;
    Data |= 0x6 << SD_EXTERNAL_CONFIG0_SD_PHY_GEN_TX_OFFSET;
  } else {
    /* 3.125G */
    Data |= 0x8 << SD_EXTERNAL_CONFIG0_SD_PHY_GEN_RX_OFFSET;
    Data |= 0x8 << SD_EXTERNAL_CONFIG0_SD_PHY_GEN_TX_OFFSET;
  }
  Mask |= SD_EXTERNAL_CONFIG0_SD_PU_RX_MASK;
  Data |= 0 << SD_EXTERNAL_CONFIG0_SD_PU_RX_OFFSET;
  Mask |= SD_EXTERNAL_CONFIG0_SD_PU_TX_MASK;
  Data |= 0 << SD_EXTERNAL_CONFIG0_SD_PU_TX_OFFSET;
  Mask |= SD_EXTERNAL_CONFIG0_HALF_BUS_MODE_MASK;
  Data |= 1 << SD_EXTERNAL_CONFIG0_HALF_BUS_MODE_OFFSET;
  RegSet (SdIpAddr + SD_EXTERNAL_CONFIG0_REG, Data, Mask);

  /* Release from hard reset */
  Mask = SD_EXTERNAL_CONFIG1_RESET_IN_MASK;
  Data = 0x0 << SD_EXTERNAL_CONFIG1_RESET_IN_OFFSET;
  Mask |= SD_EXTERNAL_CONFIG1_RESET_CORE_MASK;
  Data |= 0x0 << SD_EXTERNAL_CONFIG1_RESET_CORE_OFFSET;
  Mask |= SD_EXTERNAL_CONFIG1_RF_RESET_IN_MASK;
  Data |= 0x0 << SD_EXTERNAL_CONFIG1_RF_RESET_IN_OFFSET;
  RegSet (SdIpAddr + SD_EXTERNAL_CONFIG1_REG, Data, Mask);

  /* Release from hard reset */
  Mask = SD_EXTERNAL_CONFIG1_RESET_IN_MASK;
  Data = 0x1 << SD_EXTERNAL_CONFIG1_RESET_IN_OFFSET;
  Mask |= SD_EXTERNAL_CONFIG1_RESET_CORE_MASK;
  Data |= 0x1 << SD_EXTERNAL_CONFIG1_RESET_CORE_OFFSET;
  RegSet (SdIpAddr+ SD_EXTERNAL_CONFIG1_REG, Data, Mask);

  /* Wait 1ms - until band gap and ref clock ready */
  MicroSecondDelay (1000);
  MemoryFence ();
}

STATIC
VOID
ComPhySgmiiPhyConfiguration (
  IN EFI_PHYSICAL_ADDRESS HpipeAddr
)
{
  UINT32 Mask, Data;

  /* Set reference clock */
  Mask = HPIPE_MISC_REFCLK_SEL_MASK;
  Data = 0x0 << HPIPE_MISC_REFCLK_SEL_OFFSET;
  RegSet (HpipeAddr + HPIPE_MISC_REG, Data, Mask);

  /* Power and PLL Control */
  Mask = HPIPE_PWR_PLL_REF_FREQ_MASK;
  Data = 0x1 << HPIPE_PWR_PLL_REF_FREQ_OFFSET;
  Mask |= HPIPE_PWR_PLL_PHY_MODE_MASK;
  Data |= 0x4 << HPIPE_PWR_PLL_PHY_MODE_OFFSET;
  RegSet (HpipeAddr + HPIPE_PWR_PLL_REG, Data, Mask);

  /* Loopback register */
  Mask = HPIPE_LOOPBACK_SEL_MASK;
  Data = 0x1 << HPIPE_LOOPBACK_SEL_OFFSET;
  RegSet (HpipeAddr + HPIPE_LOOPBACK_REG, Data, Mask);

  /* Rx control 1 */
  Mask = HPIPE_RX_CONTROL_1_RXCLK2X_SEL_MASK;
  Data = 0x1 << HPIPE_RX_CONTROL_1_RXCLK2X_SEL_OFFSET;
  Mask |= HPIPE_RX_CONTROL_1_CLK8T_EN_MASK;
  Data |= 0x0 << HPIPE_RX_CONTROL_1_CLK8T_EN_OFFSET;
  RegSet (HpipeAddr + HPIPE_RX_CONTROL_1_REG, Data, Mask);

  /* DTL Control */
  Mask = HPIPE_PWR_CTR_DTL_FLOOP_EN_MASK;
  Data = 0x0 << HPIPE_PWR_CTR_DTL_FLOOP_EN_OFFSET;
  RegSet (HpipeAddr + HPIPE_PWR_CTR_DTL_REG, Data, Mask);
}

STATIC
EFI_STATUS
ComPhyEthCommonRFUPowerUp (
  IN EFI_PHYSICAL_ADDRESS SdIpAddr
)
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINT32 Mask, Data;
  EFI_PHYSICAL_ADDRESS Addr;

  /* SerDes External Configuration */
  Mask = SD_EXTERNAL_CONFIG0_SD_PU_PLL_MASK;
  Data = 0x1 << SD_EXTERNAL_CONFIG0_SD_PU_PLL_OFFSET;
  Mask |= SD_EXTERNAL_CONFIG0_SD_PU_RX_MASK;
  Data |= 0x1 << SD_EXTERNAL_CONFIG0_SD_PU_RX_OFFSET;
  Mask |= SD_EXTERNAL_CONFIG0_SD_PU_TX_MASK;
  Data |= 0x1 << SD_EXTERNAL_CONFIG0_SD_PU_TX_OFFSET;
  RegSet (SdIpAddr + SD_EXTERNAL_CONFIG0_REG, Data, Mask);

  /* Check PLL rx & tx ready */
  Addr = SdIpAddr + SD_EXTERNAL_STATUS0_REG;
  Data = SD_EXTERNAL_STATUS0_PLL_RX_MASK | SD_EXTERNAL_STATUS0_PLL_TX_MASK;
  Mask = Data;
  Data = PollingWithTimeout (Addr, Data, Mask, 15000);
  if (Data != 0) {
    DEBUG((DEBUG_ERROR, "ComPhy: Read from reg = %p - value = 0x%x\n",
      SdIpAddr + SD_EXTERNAL_STATUS0_REG, Data));
    DEBUG((DEBUG_ERROR, "ComPhy: SD_EXTERNAL_STATUS0_PLL_RX is %d, SD_EXTERNAL_STATUS0_PLL_TX is %d\n",
      (Data & SD_EXTERNAL_STATUS0_PLL_RX_MASK),
      (Data & SD_EXTERNAL_STATUS0_PLL_TX_MASK)));
    Status = EFI_D_ERROR;
  }

  /* RX init */
  Mask = SD_EXTERNAL_CONFIG1_RX_INIT_MASK;
  Data = 0x1 << SD_EXTERNAL_CONFIG1_RX_INIT_OFFSET;
  RegSet (SdIpAddr + SD_EXTERNAL_CONFIG1_REG, Data, Mask);

  /* Check that RX init done */
  Addr = SdIpAddr + SD_EXTERNAL_STATUS0_REG;
  Data = SD_EXTERNAL_STATUS0_RX_INIT_MASK;
  Mask = Data;
  Data = PollingWithTimeout (Addr, Data, Mask, 100);
  if (Data != 0) {
    DEBUG((DEBUG_ERROR, "ComPhy: Read from reg = %p - value = 0x%x\n",
      SdIpAddr + SD_EXTERNAL_STATUS0_REG, Data));
    DEBUG((DEBUG_ERROR, "ComPhy: SD_EXTERNAL_STATUS0_RX_INIT is 0\n"));
    Status = EFI_D_ERROR;
  }
  Mask =  SD_EXTERNAL_CONFIG1_RX_INIT_MASK;
  Data = 0x0 << SD_EXTERNAL_CONFIG1_RX_INIT_OFFSET;
  Mask |= SD_EXTERNAL_CONFIG1_RF_RESET_IN_MASK;
  Data |= 0x1 << SD_EXTERNAL_CONFIG1_RF_RESET_IN_OFFSET;
  RegSet (SdIpAddr + SD_EXTERNAL_CONFIG1_REG, Data, Mask);

  return Status;
}

STATIC
UINTN
ComPhySgmiiPowerUp (
  IN UINT32 Lane,
  IN UINT32 SgmiiSpeed,
  IN EFI_PHYSICAL_ADDRESS HpipeBase,
  IN EFI_PHYSICAL_ADDRESS ComPhyBase
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_PHYSICAL_ADDRESS HpipeAddr = HPIPE_ADDR(HpipeBase, Lane);
  EFI_PHYSICAL_ADDRESS SdIpAddr = SD_ADDR(HpipeBase, Lane);
  EFI_PHYSICAL_ADDRESS ComPhyAddr = COMPHY_ADDR(ComPhyBase, Lane);

  DEBUG((DEBUG_INFO, "ComPhy: stage: RFU configurations - hard reset ComPhy\n"));

  ComPhySgmiiRFUConfiguration (ComPhyAddr, SdIpAddr, SgmiiSpeed);

  DEBUG((DEBUG_INFO, "ComPhy: stage: ComPhy configuration\n"));

  ComPhySgmiiPhyConfiguration (HpipeAddr);

  /* Set analog paramters from ETP(HW) - for now use the default data */
  DEBUG((DEBUG_INFO, "ComPhy: stage: Analog paramters from ETP(HW)\n"));

  RegSet (HpipeAddr + HPIPE_G1_SET0_REG,
    0x1 << HPIPE_GX_SET0_TX_EMPH1_OFFSET, HPIPE_GX_SET0_TX_EMPH1_MASK);

  DEBUG((DEBUG_INFO, "ComPhy: stage: RFU configurations - Power Up PLL,Tx,Rx\n"));

  Status = ComPhyEthCommonRFUPowerUp (SdIpAddr);

  return Status;
}

STATIC
VOID
ComPhySfiRFUConfiguration (
  IN EFI_PHYSICAL_ADDRESS ComPhyAddr,
  IN EFI_PHYSICAL_ADDRESS SdIpAddr
)
{
  UINT32 Mask, Data;

  MmioAndThenOr32 (
          ComPhyAddr + COMMON_PHY_CFG1_REG,
          ~(COMMON_PHY_CFG1_PWR_UP_MASK | COMMON_PHY_CFG1_PIPE_SELECT_MASK),
          COMMON_PHY_CFG1_PWR_UP_MASK
          );

  /* Select Baud Rate of Comphy And PD_PLL/Tx/Rx */
  Mask = SD_EXTERNAL_CONFIG0_SD_PU_PLL_MASK |
         SD_EXTERNAL_CONFIG0_SD_PHY_GEN_RX_MASK |
         SD_EXTERNAL_CONFIG0_SD_PHY_GEN_TX_MASK |
         SD_EXTERNAL_CONFIG0_SD_PU_RX_MASK |
         SD_EXTERNAL_CONFIG0_SD_PU_TX_MASK |
         SD_EXTERNAL_CONFIG0_HALF_BUS_MODE_MASK;
  Data = (0xe << SD_EXTERNAL_CONFIG0_SD_PHY_GEN_RX_OFFSET) |
         (0xe << SD_EXTERNAL_CONFIG0_SD_PHY_GEN_TX_OFFSET);
  MmioAndThenOr32 (SdIpAddr + SD_EXTERNAL_CONFIG0_REG, ~Mask, Data);

  /* Release from hard reset */
  Mask = SD_EXTERNAL_CONFIG1_RESET_IN_MASK |
         SD_EXTERNAL_CONFIG1_RESET_CORE_MASK |
         SD_EXTERNAL_CONFIG1_RF_RESET_IN_MASK;
  Data = SD_EXTERNAL_CONFIG1_RESET_IN_MASK |
         SD_EXTERNAL_CONFIG1_RESET_CORE_MASK;
  MmioAndThenOr32 (SdIpAddr + SD_EXTERNAL_CONFIG1_REG, ~Mask, Data);

  /* Wait 1ms - until band gap and ref clock are ready */
  MicroSecondDelay (1000);
  MemoryFence ();
}

STATIC
VOID
ComPhySfiPhyConfiguration (
  IN EFI_PHYSICAL_ADDRESS HpipeAddr,
  IN UINT32 SfiSpeed
)
{
  UINT32 Mask, Data;

  /* Set reference clock */
  Mask = HPIPE_MISC_ICP_FORCE_MASK | HPIPE_MISC_REFCLK_SEL_MASK;
  Data = (SfiSpeed == COMPHY_SPEED_5_15625G) ?
    (0x0 << HPIPE_MISC_ICP_FORCE_OFFSET) : (0x1 << HPIPE_MISC_ICP_FORCE_OFFSET);
  MmioAndThenOr32 (HpipeAddr + HPIPE_MISC_REG, ~Mask, Data);

  /* Power and PLL Control */
  MmioAndThenOr32 (
          HpipeAddr + HPIPE_PWR_PLL_REG,
          ~(HPIPE_PWR_PLL_REF_FREQ_MASK | HPIPE_PWR_PLL_PHY_MODE_MASK),
          0x1 | (0x4 << HPIPE_PWR_PLL_PHY_MODE_OFFSET)
          );

  /* Loopback register */
  MmioAndThenOr32 (
          HpipeAddr + HPIPE_LOOPBACK_REG,
          ~HPIPE_LOOPBACK_SEL_MASK,
          0x1 << HPIPE_LOOPBACK_SEL_OFFSET
          );

  /* Rx control 1 */
  MmioOr32 (
          HpipeAddr + HPIPE_RX_CONTROL_1_REG,
          HPIPE_RX_CONTROL_1_RXCLK2X_SEL_MASK | HPIPE_RX_CONTROL_1_CLK8T_EN_MASK
          );

  /* DTL Control */
  MmioOr32 (HpipeAddr + HPIPE_PWR_CTR_DTL_REG, HPIPE_PWR_CTR_DTL_FLOOP_EN_MASK);

  /* Transmitter/Receiver Speed Divider Force */
  if (SfiSpeed == COMPHY_SPEED_5_15625G) {
    Mask = HPIPE_SPD_DIV_FORCE_RX_SPD_DIV_MASK |
           HPIPE_SPD_DIV_FORCE_RX_SPD_DIV_FORCE_MASK |
           HPIPE_SPD_DIV_FORCE_TX_SPD_DIV_MASK |
           HPIPE_SPD_DIV_FORCE_TX_SPD_DIV_FORCE_MASK;
    Data = (1 << HPIPE_SPD_DIV_FORCE_RX_SPD_DIV_OFFSET) |
           (1 << HPIPE_SPD_DIV_FORCE_RX_SPD_DIV_FORCE_OFFSET) |
           (1 << HPIPE_SPD_DIV_FORCE_TX_SPD_DIV_OFFSET) |
           (1 << HPIPE_SPD_DIV_FORCE_TX_SPD_DIV_FORCE_OFFSET);
    MmioAndThenOr32 (HpipeAddr + HPIPE_SPD_DIV_FORCE_REG, ~Mask, Data);
  } else {
    MmioOr32 (HpipeAddr + HPIPE_SPD_DIV_FORCE_REG, HPIPE_TXDIGCK_DIV_FORCE_MASK);
  }
}

STATIC
VOID
ComPhySfiSetAnalogParameters (
  IN EFI_PHYSICAL_ADDRESS HpipeAddr,
  IN EFI_PHYSICAL_ADDRESS SdIpAddr,
  IN UINT32 SfiSpeed
)
{
  UINT32 Mask, Data;

  /* SERDES External Configuration 2 */
  MmioOr32 (SdIpAddr + SD_EXTERNAL_CONFIG2_REG, SD_EXTERNAL_CONFIG2_PIN_DFE_EN_MASK);

  /* DFE Resolution control */
  MmioOr32 (HpipeAddr + HPIPE_DFE_REG0, HPIPE_DFE_RES_FORCE_MASK);

  /* Generation 1 setting_0 */
  if (SfiSpeed == COMPHY_SPEED_5_15625G) {
    Mask = HPIPE_GX_SET0_TX_EMPH1_MASK;
    Data = 0x6 << HPIPE_GX_SET0_TX_EMPH1_OFFSET;
  } else {
    Mask = HPIPE_GX_SET0_TX_AMP_MASK | HPIPE_GX_SET0_TX_EMPH1_MASK;
    Data = (0x1c << HPIPE_GX_SET0_TX_AMP_OFFSET) | (0xe << HPIPE_GX_SET0_TX_EMPH1_OFFSET);
  }
  MmioAndThenOr32 (HpipeAddr + HPIPE_G1_SET0_REG, ~Mask, Data);

  /* Generation 1 setting 2 */
  MmioAndThenOr32 (
          HpipeAddr + HPIPE_GX_SET2_REG,
          ~HPIPE_GX_SET2_TX_EMPH0_MASK,
          HPIPE_GX_SET2_TX_EMPH0_EN_MASK
          );

  /* Transmitter Slew Rate Control register */
  MmioAndThenOr32 (
          HpipeAddr + HPIPE_TX_REG1_REG,
          ~(HPIPE_TX_REG1_TX_EMPH_RES_MASK | HPIPE_TX_REG1_SLC_EN_MASK),
          (0x3 << HPIPE_TX_REG1_TX_EMPH_RES_OFFSET) | (0x3f << HPIPE_TX_REG1_SLC_EN_OFFSET)
          );

  /* Impedance Calibration Control register */
  MmioAndThenOr32 (
          HpipeAddr + HPIPE_CAL_REG1_REG,
          ~(HPIPE_CAL_REG_1_EXT_TXIMP_MASK | HPIPE_CAL_REG_1_EXT_TXIMP_EN_MASK),
          (0xe << HPIPE_CAL_REG_1_EXT_TXIMP_OFFSET) | HPIPE_CAL_REG_1_EXT_TXIMP_EN_MASK
          );

  /* Generation 1 setting 5 */
  MmioAnd32 (HpipeAddr + HPIPE_G1_SET5_REG, ~HPIPE_GX_SET5_ICP_MASK);

  /* Generation 1 setting 1 */
  if (SfiSpeed == COMPHY_SPEED_5_15625G) {
    Mask = HPIPE_GX_SET1_RX_SELMUPI_MASK | HPIPE_GX_SET1_RX_SELMUPP_MASK;
    Data = 0x1 | (0x1 << HPIPE_GX_SET1_RX_SELMUPP_OFFSET);
  } else {
    Mask = HPIPE_GX_SET1_RX_SELMUPI_MASK |
           HPIPE_GX_SET1_RX_SELMUPP_MASK |
           HPIPE_GX_SET1_RX_SELMUFI_MASK |
           HPIPE_GX_SET1_RX_SELMUFF_MASK |
           HPIPE_GX_SET1_RX_DIGCK_DIV_MASK;
    Data = 0x2 |
           (0x2 << HPIPE_GX_SET1_RX_SELMUPP_OFFSET) |
           (0x1 << HPIPE_GX_SET1_RX_SELMUFF_OFFSET) |
           (0x3 << HPIPE_GX_SET1_RX_DIGCK_DIV_OFFSET);
  }
  MmioAndThenOr32 (HpipeAddr + HPIPE_G1_SET1_REG, ~Mask, Data);
  MmioOr32 (HpipeAddr + HPIPE_G1_SET1_REG, HPIPE_GX_SET1_RX_DFE_EN_MASK);

  /* DFE F3-F5 Coefficient Control */
  MmioAnd32 (
          HpipeAddr + HPIPE_DFE_F3_F5_REG,
          ~(HPIPE_DFE_F3_F5_DFE_EN_MASK | HPIPE_DFE_F3_F5_DFE_CTRL_MASK)
          );

  /* Configure Generation 1 setting 4 (DFE) */
  MmioAndThenOr32 (
          HpipeAddr + HPIPE_G1_SET4_REG,
          ~HPIPE_GX_SET4_DFE_RES_MASK,
          0x1 << HPIPE_GX_SET4_DFE_RES_OFFSET
          );

  /* Generation 1 setting 3 */
  MmioOr32 (HpipeAddr + HPIPE_G1_SET3_REG, HPIPE_GX_SET3_FBCK_SEL_MASK);

  if (SfiSpeed == COMPHY_SPEED_5_15625G) {
    /* Force FFE (Feed Forward Equalization) to 5G */
    Mask = HPIPE_GX_SET3_FFE_CAP_SEL_MASK |
           HPIPE_GX_SET3_FFE_RES_SEL_MASK |
           HPIPE_GX_SET3_FFE_SETTING_FORCE_MASK;
    Data = 0xf | (0x4 << HPIPE_GX_SET3_FFE_RES_SEL_OFFSET) | HPIPE_GX_SET3_FFE_SETTING_FORCE_MASK;
    MmioAndThenOr32 (HpipeAddr + HPIPE_G1_SET3_REG, ~Mask, Data);
  }

  /* Configure RX training timer */
  MmioAndThenOr32 (HpipeAddr + HPIPE_TX_TRAIN_CTRL_5_REG, ~HPIPE_RX_TRAIN_TIMER_MASK, 0x13);

  /* Enable TX train peak to peak hold */
  MmioOr32 (HpipeAddr + HPIPE_TX_TRAIN_CTRL_0_REG, HPIPE_TX_TRAIN_P2P_HOLD_MASK);

  /* Configure TX preset index */
  MmioAndThenOr32 (
          HpipeAddr + HPIPE_TX_PRESET_INDEX_REG,
          ~HPIPE_TX_PRESET_INDEX_MASK,
          0x2 << HPIPE_TX_PRESET_INDEX_OFFSET
          );

  /* Disable pattern lock lost timeout */
  MmioAnd32 (HpipeAddr + HPIPE_FRAME_DETECT_CTRL_3_REG, ~HPIPE_PATTERN_LOCK_LOST_TIMEOUT_EN_MASK);

  /* Configure TX training pattern and TX training 16bit auto */
  MmioOr32 (
          HpipeAddr + HPIPE_TX_TRAIN_REG,
          HPIPE_TX_TRAIN_16BIT_AUTO_EN_MASK | HPIPE_TX_TRAIN_PAT_SEL_MASK
          );

  /* Configure training pattern number */
  MmioAndThenOr32 (
          HpipeAddr + HPIPE_FRAME_DETECT_CTRL_0_REG,
          ~HPIPE_TRAIN_PAT_NUM_MASK,
          0x88 << HPIPE_TRAIN_PAT_NUM_OFFSET
          );

  /* Configure differential manchester encoder to ethernet mode */
  MmioOr32 (HpipeAddr + HPIPE_DME_REG, HPIPE_DME_ETHERNET_MODE_MASK);

  /* Configure VDD Continuous Calibration */
  MmioOr32 (HpipeAddr + HPIPE_VDD_CAL_0_REG, HPIPE_CAL_VDD_CONT_MODE_MASK);

  /* Configure sampler gain */
  MmioAndThenOr32 (
          HpipeAddr + HPIPE_SAMPLER_N_PROC_CALIB_CTRL_REG,
          ~HPIPE_RX_SAMPLER_OS_GAIN_MASK,
          0x3 << HPIPE_RX_SAMPLER_OS_GAIN_OFFSET
          );

  /* Trigger sampler enable pulse (by toggling the bit) */
  MmioAndThenOr32 (
          HpipeAddr + HPIPE_SAMPLER_N_PROC_CALIB_CTRL_REG,
          ~HPIPE_SAMPLER_MASK,
          0x1 << HPIPE_SAMPLER_OFFSET
          );
  MmioAnd32 (
          HpipeAddr + HPIPE_SAMPLER_N_PROC_CALIB_CTRL_REG,
          ~HPIPE_SAMPLER_MASK
          );

  /* VDD calibration control */
  MmioAndThenOr32 (
          HpipeAddr + HPIPE_VDD_CAL_CTRL_REG,
          ~HPIPE_EXT_SELLV_RXSAMPL_MASK,
          0x1a << HPIPE_EXT_SELLV_RXSAMPL_OFFSET
          );
}

STATIC
EFI_STATUS
ComPhySfiPowerUp (
  IN UINT32 Lane,
  IN EFI_PHYSICAL_ADDRESS HpipeBase,
  IN EFI_PHYSICAL_ADDRESS ComPhyBase,
  IN UINT32 SfiSpeed
  )
{
  EFI_STATUS Status;
  EFI_PHYSICAL_ADDRESS HpipeAddr = HPIPE_ADDR(HpipeBase, Lane);
  EFI_PHYSICAL_ADDRESS SdIpAddr = SD_ADDR(HpipeBase, Lane);
  EFI_PHYSICAL_ADDRESS ComPhyAddr = COMPHY_ADDR(ComPhyBase, Lane);

  DEBUG ((DEBUG_INFO, "ComPhy: stage: RFU configurations - hard reset ComPhy\n"));

  ComPhySfiRFUConfiguration (ComPhyAddr, SdIpAddr);

  DEBUG ((DEBUG_INFO, "ComPhy: stage: ComPhy configuration\n"));

  ComPhySfiPhyConfiguration (HpipeAddr, SfiSpeed);

  DEBUG ((DEBUG_INFO, "ComPhy: stage: Set analog paramters\n"));

  ComPhySfiSetAnalogParameters (HpipeAddr, SdIpAddr, SfiSpeed);

  DEBUG ((DEBUG_INFO, "ComPhy: stage: RFU configurations - Power Up PLL,Tx,Rx\n"));

  Status = ComPhyEthCommonRFUPowerUp (SdIpAddr);

  return Status;
}

STATIC
EFI_STATUS
ComPhyRxauiRFUConfiguration (
  IN UINT32 Lane,
  IN EFI_PHYSICAL_ADDRESS ComPhyAddr,
  IN EFI_PHYSICAL_ADDRESS SdIpAddr
)
{
  UINT32 Mask, Data;

  MmioAndThenOr32 (
          ComPhyAddr + COMMON_PHY_CFG1_REG,
          ~(COMMON_PHY_CFG1_PWR_UP_MASK | COMMON_PHY_CFG1_PIPE_SELECT_MASK),
          COMMON_PHY_CFG1_PWR_UP_MASK
          );

  switch (Lane) {
  case 2:
  case 4:
    MmioOr32 (ComPhyAddr + COMMON_PHY_SD_CTRL1, COMMON_PHY_SD_CTRL1_RXAUI0_MASK);
  case 3:
  case 5:
    MmioOr32 (ComPhyAddr + COMMON_PHY_SD_CTRL1, COMMON_PHY_SD_CTRL1_RXAUI1_MASK);
    break;
  default:
    DEBUG ((DEBUG_ERROR, "RXAUI used on invalid lane %d\n", Lane));
    return EFI_INVALID_PARAMETER;
  }

  /* Select Baud Rate of Comphy And PD_PLL/Tx/Rx */
  Mask = SD_EXTERNAL_CONFIG0_SD_PU_PLL_MASK |
         SD_EXTERNAL_CONFIG0_SD_PHY_GEN_RX_MASK |
         SD_EXTERNAL_CONFIG0_SD_PHY_GEN_TX_MASK |
         SD_EXTERNAL_CONFIG0_SD_PU_RX_MASK |
         SD_EXTERNAL_CONFIG0_SD_PU_TX_MASK |
         SD_EXTERNAL_CONFIG0_HALF_BUS_MODE_MASK |
         SD_EXTERNAL_CONFIG0_MEDIA_MODE_MASK;
  Data = (0xb << SD_EXTERNAL_CONFIG0_SD_PHY_GEN_RX_OFFSET) |
         (0xb << SD_EXTERNAL_CONFIG0_SD_PHY_GEN_TX_OFFSET) |
         (0x1 << SD_EXTERNAL_CONFIG0_MEDIA_MODE_OFFSET);
  MmioAndThenOr32 (SdIpAddr + SD_EXTERNAL_CONFIG0_REG, ~Mask, Data);

  /* Release from hard reset */
  Mask = SD_EXTERNAL_CONFIG1_RESET_IN_MASK |
         SD_EXTERNAL_CONFIG1_RESET_CORE_MASK |
         SD_EXTERNAL_CONFIG1_RF_RESET_IN_MASK;
  Data = SD_EXTERNAL_CONFIG1_RESET_IN_MASK |
         SD_EXTERNAL_CONFIG1_RESET_CORE_MASK;
  MmioAndThenOr32 (SdIpAddr + SD_EXTERNAL_CONFIG1_REG, ~Mask, Data);

  /* Wait 1ms - until band gap and ref clock are ready */
  MicroSecondDelay (1000);
  MemoryFence ();

  return EFI_SUCCESS;
}

STATIC
VOID
ComPhyRxauiPhyConfiguration (
  IN EFI_PHYSICAL_ADDRESS HpipeAddr
)
{
  /* Set reference clock */
  MmioAnd32 (HpipeAddr + HPIPE_MISC_REG, ~HPIPE_MISC_REFCLK_SEL_MASK);

  /* Power and PLL Control */
  MmioAndThenOr32 (
          HpipeAddr + HPIPE_PWR_PLL_REG,
          ~(HPIPE_PWR_PLL_REF_FREQ_MASK | HPIPE_PWR_PLL_PHY_MODE_MASK),
          0x1 | (0x4 << HPIPE_PWR_PLL_PHY_MODE_OFFSET)
          );

  /* Loopback register */
  MmioAndThenOr32 (
          HpipeAddr + HPIPE_LOOPBACK_REG,
          ~HPIPE_LOOPBACK_SEL_MASK,
          0x1 << HPIPE_LOOPBACK_SEL_OFFSET
          );

  /* Rx control 1 */
  MmioOr32 (
          HpipeAddr + HPIPE_RX_CONTROL_1_REG,
          HPIPE_RX_CONTROL_1_RXCLK2X_SEL_MASK | HPIPE_RX_CONTROL_1_CLK8T_EN_MASK
          );

  /* DTL Control */
  MmioAnd32 (HpipeAddr + HPIPE_PWR_CTR_DTL_REG, ~HPIPE_PWR_CTR_DTL_FLOOP_EN_MASK);
}

STATIC
VOID
ComPhyRxauiSetAnalogParameters (
  IN EFI_PHYSICAL_ADDRESS HpipeAddr,
  IN EFI_PHYSICAL_ADDRESS SdIpAddr
)
{
  UINT32 Mask, Data;

  /* SERDES External Configuration 2 */
  MmioOr32 (SdIpAddr + SD_EXTERNAL_CONFIG2_REG, SD_EXTERNAL_CONFIG2_PIN_DFE_EN_MASK);

  /* DFE Resolution control */
  MmioOr32 (HpipeAddr + HPIPE_DFE_REG0, HPIPE_DFE_RES_FORCE_MASK);

  /* Generation 1 setting_0 */
  MmioAndThenOr32 (
          HpipeAddr + HPIPE_G1_SET0_REG,
          ~HPIPE_GX_SET0_TX_EMPH1_MASK,
          0xe << HPIPE_GX_SET0_TX_EMPH1_OFFSET
          );

  /* Generation 1 setting 1 */
  Mask = HPIPE_GX_SET1_RX_SELMUPI_MASK |
         HPIPE_GX_SET1_RX_SELMUPP_MASK |
         HPIPE_GX_SET1_RX_DFE_EN_MASK;
  Data = 0x1 |
         (0x1 << HPIPE_GX_SET1_RX_SELMUPP_OFFSET) |
         (0x1 << HPIPE_GX_SET1_RX_DFE_EN_OFFSET);
  MmioAndThenOr32 (HpipeAddr + HPIPE_G1_SET1_REG, ~Mask, Data);

  /* DFE F3-F5 Coefficient Control */
  MmioAnd32 (
          HpipeAddr + HPIPE_DFE_F3_F5_REG,
          ~(HPIPE_DFE_F3_F5_DFE_EN_MASK | HPIPE_DFE_F3_F5_DFE_CTRL_MASK)
          );

  /* Configure Generation 1 setting 4 (DFE) */
  MmioAndThenOr32 (
          HpipeAddr + HPIPE_G1_SET4_REG,
          ~HPIPE_GX_SET4_DFE_RES_MASK,
          0x1 << HPIPE_GX_SET4_DFE_RES_OFFSET
          );

  /* Generation 1 setting 3 */
  MmioOr32 (HpipeAddr + HPIPE_G1_SET3_REG, HPIPE_GX_SET3_FBCK_SEL_MASK);
}

STATIC
EFI_STATUS
ComPhyRxauiPowerUp (
  IN UINT32 Lane,
  IN EFI_PHYSICAL_ADDRESS HpipeBase,
  IN EFI_PHYSICAL_ADDRESS ComPhyBase
  )
{
  EFI_STATUS Status;
  EFI_PHYSICAL_ADDRESS HpipeAddr = HPIPE_ADDR(HpipeBase, Lane);
  EFI_PHYSICAL_ADDRESS SdIpAddr = SD_ADDR(HpipeBase, Lane);
  EFI_PHYSICAL_ADDRESS ComPhyAddr = COMPHY_ADDR(ComPhyBase, Lane);

  DEBUG ((DEBUG_INFO, "ComPhy: stage: RFU configurations - hard reset ComPhy\n"));

  Status = ComPhyRxauiRFUConfiguration (Lane, ComPhyAddr, SdIpAddr);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  DEBUG ((DEBUG_INFO, "ComPhy: stage: ComPhy configuration\n"));

  ComPhyRxauiPhyConfiguration (HpipeAddr);

  DEBUG ((DEBUG_INFO, "ComPhy: stage: Set analog paramters\n"));

  ComPhyRxauiSetAnalogParameters (HpipeAddr, SdIpAddr);

  DEBUG ((DEBUG_INFO, "ComPhy: stage: RFU configurations - Power Up PLL,Tx,Rx\n"));

  Status = ComPhyEthCommonRFUPowerUp (SdIpAddr);

  return Status;
}

STATIC
VOID
ComPhyMuxCp110 (
  IN CHIP_COMPHY_CONFIG *PtrChipCfg,
  IN COMPHY_MAP *SerdesMap
  )
{
  EFI_PHYSICAL_ADDRESS ComPhyBaseAddr;
  COMPHY_MAP ComPhyMapPipeData[MAX_LANE_OPTIONS];
  COMPHY_MAP ComPhyMapPhyData[MAX_LANE_OPTIONS];
  UINT32 Lane, ComPhyMaxCount;

  ComPhyMaxCount = PtrChipCfg->LanesCount;
  ComPhyBaseAddr = PtrChipCfg->ComPhyBaseAddr;

  /*
   * Copy the SerDes map configuration for PIPE map and PHY map.
   * The ComPhyMuxInit modifies the Type of the Lane if the Type is not valid.
   * Because we have 2 selectors, run the ComPhyMuxInit twice and after
   * that, update the original SerdesMap.
   */
  for (Lane = 0; Lane < ComPhyMaxCount; Lane++) {
    ComPhyMapPipeData[Lane].Type = SerdesMap[Lane].Type;
    ComPhyMapPipeData[Lane].Speed = SerdesMap[Lane].Speed;
    ComPhyMapPhyData[Lane].Type = SerdesMap[Lane].Type;
    ComPhyMapPhyData[Lane].Speed = SerdesMap[Lane].Speed;
  }
  PtrChipCfg->MuxData = Cp110ComPhyMuxData;
  ComPhyMuxInit(PtrChipCfg, ComPhyMapPhyData, ComPhyBaseAddr +
    COMMON_SELECTOR_PHY_OFFSET);

  PtrChipCfg->MuxData = Cp110ComPhyPipeMuxData;
  ComPhyMuxInit(PtrChipCfg, ComPhyMapPipeData, ComPhyBaseAddr +
    COMMON_SELECTOR_PIPE_OFFSET);

  /* Fix the Type after check the PHY and PIPE configuration */
  for (Lane = 0; Lane < ComPhyMaxCount; Lane++)
    if ((ComPhyMapPipeData[Lane].Type == COMPHY_TYPE_UNCONNECTED) &&
        (ComPhyMapPhyData[Lane].Type == COMPHY_TYPE_UNCONNECTED))
      SerdesMap[Lane].Type = COMPHY_TYPE_UNCONNECTED;
}

VOID
ComPhyCp110Init (
  IN CHIP_COMPHY_CONFIG *PtrChipCfg
  )
{
  EFI_STATUS Status;
  COMPHY_MAP *PtrComPhyMap, *SerdesMap;
  EFI_PHYSICAL_ADDRESS ComPhyBaseAddr, HpipeBaseAddr;
  UINT32 ComPhyMaxCount, Lane;
  UINT32 PcieBy4 = 1; // Indicating if first 4 lanes set to PCIE

  ComPhyMaxCount = PtrChipCfg->LanesCount;
  ComPhyBaseAddr = PtrChipCfg->ComPhyBaseAddr;
  HpipeBaseAddr = PtrChipCfg->Hpipe3BaseAddr;
  SerdesMap = PtrChipCfg->MapData;

  /* Config Comphy mux configuration */
  ComPhyMuxCp110(PtrChipCfg, SerdesMap);

  /* Check if the first 4 Lanes configured as By-4 */
  for (Lane = 0, PtrComPhyMap = SerdesMap; Lane < 4; Lane++, PtrComPhyMap++) {
    if (PtrComPhyMap->Type != COMPHY_TYPE_PCIE0) {
      PcieBy4 = 0;
      break;
    }
  }

  for (Lane = 0, PtrComPhyMap = SerdesMap; Lane < ComPhyMaxCount;
       Lane++, PtrComPhyMap++) {
    DEBUG((DEBUG_INFO, "ComPhy: Initialize serdes number %d\n", Lane));
    DEBUG((DEBUG_INFO, "ComPhy: Serdes Type = 0x%x\n", PtrComPhyMap->Type));
    switch (PtrComPhyMap->Type) {
    case COMPHY_TYPE_UNCONNECTED:
      continue;
      break;
    case COMPHY_TYPE_PCIE0:
    case COMPHY_TYPE_PCIE1:
    case COMPHY_TYPE_PCIE2:
    case COMPHY_TYPE_PCIE3:
      Status = ComPhyPciePowerUp(Lane, PcieBy4, HpipeBaseAddr, ComPhyBaseAddr);
      break;
    case COMPHY_TYPE_SATA0:
    case COMPHY_TYPE_SATA1:
      Status = ComPhySataPowerUp (Lane, HpipeBaseAddr, ComPhyBaseAddr, MVHW_CP0_AHCI0_ID);
      break;
    case COMPHY_TYPE_SATA2:
    case COMPHY_TYPE_SATA3:
      Status = ComPhySataPowerUp (Lane, HpipeBaseAddr, ComPhyBaseAddr, MVHW_CP1_AHCI0_ID);
      break;
    case COMPHY_TYPE_USB3_HOST0:
    case COMPHY_TYPE_USB3_HOST1:
      Status = ComphyUsb3PowerUp(Lane, HpipeBaseAddr, ComPhyBaseAddr);
      break;
    case COMPHY_TYPE_SGMII0:
    case COMPHY_TYPE_SGMII1:
    case COMPHY_TYPE_SGMII2:
    case COMPHY_TYPE_SGMII3:
      Status = ComPhySgmiiPowerUp(Lane, PtrComPhyMap->Speed, HpipeBaseAddr,
        ComPhyBaseAddr);
      break;
    case COMPHY_TYPE_SFI:
      Status = ComPhySfiPowerUp(Lane, HpipeBaseAddr, ComPhyBaseAddr, PtrComPhyMap->Speed);
      break;
    case COMPHY_TYPE_RXAUI0:
    case COMPHY_TYPE_RXAUI1:
      Status = ComPhyRxauiPowerUp(Lane, HpipeBaseAddr, ComPhyBaseAddr);
      break;
    default:
      DEBUG((DEBUG_ERROR, "Unknown SerDes Type, skip initialize SerDes %d\n",
        Lane));
      Status = EFI_INVALID_PARAMETER;
      ASSERT (FALSE);
      break;
    }
    if (EFI_ERROR(Status)) {
      DEBUG ((DEBUG_ERROR, "Failed to initialize Lane %d\n with Status = 0x%x", Lane, Status));
      PtrComPhyMap->Type = COMPHY_TYPE_UNCONNECTED;
    }
  }
}
