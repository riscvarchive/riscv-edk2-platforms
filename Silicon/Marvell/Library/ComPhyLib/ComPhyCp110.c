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

#include <Library/ArmSmcLib.h>
#include <Library/SampleAtResetLib.h>

#include <IndustryStandard/ArmStdSmc.h>

#define SD_LANE_ADDR_WIDTH          0x1000
#define HPIPE_ADDR_OFFSET           0x800
#define COMPHY_ADDR_LANE_WIDTH      0x28
#define SD_ADDR(base, Lane)         (base + SD_LANE_ADDR_WIDTH * Lane)
#define HPIPE_ADDR(base, Lane)      (SD_ADDR(base, Lane) + HPIPE_ADDR_OFFSET)
#define COMPHY_ADDR(base, Lane)     (base + COMPHY_ADDR_LANE_WIDTH * Lane)

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
ComPhySmc (
    IN UINT32 FunctionId,
    EFI_PHYSICAL_ADDRESS ComPhyBaseAddr,
    IN UINT32 Lane,
    IN UINT32 Mode
    )
{
  ARM_SMC_ARGS  SmcRegs = {0};

  SmcRegs.Arg0 = FunctionId;
  SmcRegs.Arg1 = (UINTN)ComPhyBaseAddr;
  SmcRegs.Arg2 = Lane;
  SmcRegs.Arg3 = Mode;
  ArmCallSmc (&SmcRegs);

  if (SmcRegs.Arg0 != 0) {
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}

STATIC
UINTN
ComPhySataPowerUp (
  IN UINTN ChipId,
  IN UINT32 Lane,
  IN EFI_PHYSICAL_ADDRESS HpipeBase,
  IN EFI_PHYSICAL_ADDRESS ComPhyBase,
  IN MV_BOARD_AHCI_DESC *Desc
  )
{
  EFI_STATUS Status;

  DEBUG ((DEBUG_INFO, "ComPhySata: Initialize SATA PHYs\n"));

  DEBUG((DEBUG_INFO, "ComPhySataPowerUp: stage: MAC configuration - power down ComPhy\n"));

  ComPhySataMacPowerDown (Desc[ChipId].SoC->AhciBaseAddress);

  Status = ComPhySmc (MV_SIP_COMPHY_POWER_ON,
             ComPhyBase,
             Lane,
             COMPHY_FW_FORMAT (COMPHY_SATA_MODE,
               Desc[ChipId].SoC->AhciId,
               COMPHY_SPEED_DEFAULT));
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ComPhySataPhyPowerUp (Desc[ChipId].SoC->AhciBaseAddress);

  Status = ComPhySmc (MV_SIP_COMPHY_PLL_LOCK,
             ComPhyBase,
             Lane,
             COMPHY_FW_FORMAT (COMPHY_SATA_MODE,
               Desc[ChipId].SoC->AhciId,
               COMPHY_SPEED_DEFAULT));
  if (EFI_ERROR (Status)) {
    return Status;
  }

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
  MARVELL_BOARD_DESC_PROTOCOL *BoardDescProtocol;
  MV_BOARD_AHCI_DESC *AhciBoardDesc;
  UINT32 ComPhyMaxCount, Lane;
  UINT32 PcieWidth = 0;
  UINT8 ChipId;

  ComPhyMaxCount = PtrChipCfg->LanesCount;
  ComPhyBaseAddr = PtrChipCfg->ComPhyBaseAddr;
  HpipeBaseAddr = PtrChipCfg->Hpipe3BaseAddr;
  SerdesMap = PtrChipCfg->MapData;
  ChipId = PtrChipCfg->ChipId;

  /* Config Comphy mux configuration */
  ComPhyMuxCp110(PtrChipCfg, SerdesMap);

  /* Check if the first 4 Lanes configured as By-4 */
  for (Lane = 0, PtrComPhyMap = SerdesMap; Lane < 4; Lane++, PtrComPhyMap++) {
    if (PtrComPhyMap->Type != COMPHY_TYPE_PCIE0) {
      break;
    }
    PcieWidth++;
  }

  for (Lane = 0, PtrComPhyMap = SerdesMap; Lane < ComPhyMaxCount;
       Lane++, PtrComPhyMap++) {
    DEBUG((DEBUG_INFO, "ComPhy: Initialize serdes number %d\n", Lane));
    DEBUG((DEBUG_INFO, "ComPhy: Serdes Type = 0x%x\n", PtrComPhyMap->Type));

    if (Lane >= 4) {
      /* PCIe lanes above the first 4 lanes, can be only by1 */
      PcieWidth = 1;
    }

    switch (PtrComPhyMap->Type) {
    case COMPHY_TYPE_UNCONNECTED:
      continue;
      break;
    case COMPHY_TYPE_PCIE0:
    case COMPHY_TYPE_PCIE1:
    case COMPHY_TYPE_PCIE2:
    case COMPHY_TYPE_PCIE3:
      Status = ComPhySmc (MV_SIP_COMPHY_POWER_ON,
                 PtrChipCfg->ComPhyBaseAddr,
                 Lane,
                 COMPHY_FW_PCIE_FORMAT (PcieWidth,
                   COMPHY_PCIE_MODE,
                   PtrComPhyMap->Speed));
      break;
    case COMPHY_TYPE_SATA0:
    case COMPHY_TYPE_SATA1:
    case COMPHY_TYPE_SATA2:
    case COMPHY_TYPE_SATA3:
      /* Obtain AHCI board description */
      Status = gBS->LocateProtocol (&gMarvellBoardDescProtocolGuid,
                      NULL,
                      (VOID **)&BoardDescProtocol);
      if (EFI_ERROR (Status)) {
        break;
      }

      Status = BoardDescProtocol->BoardDescAhciGet (BoardDescProtocol,
                                    &AhciBoardDesc);
      if (EFI_ERROR (Status)) {
        break;
      }

      Status = ComPhySataPowerUp (ChipId,
                 Lane,
                 HpipeBaseAddr,
                 ComPhyBaseAddr,
                 AhciBoardDesc);

      BoardDescProtocol->BoardDescFree (AhciBoardDesc);
      break;
    case COMPHY_TYPE_USB3_HOST0:
    case COMPHY_TYPE_USB3_HOST1:
      Status = ComphyUsb3PowerUp(Lane, HpipeBaseAddr, ComPhyBaseAddr);
      break;
    case COMPHY_TYPE_SGMII0:
    case COMPHY_TYPE_SGMII1:
    case COMPHY_TYPE_SGMII2:
    case COMPHY_TYPE_SGMII3:
      Status = ComPhySmc (MV_SIP_COMPHY_POWER_ON,
                 PtrChipCfg->ComPhyBaseAddr,
                 Lane,
                 COMPHY_FW_FORMAT (COMPHY_SGMII_MODE,
                   (PtrComPhyMap->Type - COMPHY_TYPE_SGMII0),
                   PtrComPhyMap->Speed));
      break;
    case COMPHY_TYPE_SFI:
      Status = ComPhySmc (MV_SIP_COMPHY_POWER_ON,
                 PtrChipCfg->ComPhyBaseAddr,
                 Lane,
                 COMPHY_FW_FORMAT (COMPHY_SFI_MODE,
                   COMPHY_UNIT_ID0,
                   PtrComPhyMap->Speed));
      break;
    case COMPHY_TYPE_RXAUI0:
    case COMPHY_TYPE_RXAUI1:
      Status = ComPhySmc (MV_SIP_COMPHY_POWER_ON,
                 PtrChipCfg->ComPhyBaseAddr,
                 Lane,
                 COMPHY_FW_MODE_FORMAT (COMPHY_RXAUI_MODE));
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
