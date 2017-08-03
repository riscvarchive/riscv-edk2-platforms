/********************************************************************************
Copyright (C) 2016 Marvell International Ltd.

Marvell BSD License Option

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File under the following licensing terms.
Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice,
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

#ifndef __COMPHY_H__
#define __COMPHY_H__

#include <Library/ArmLib.h>
#include <Library/ArmPlatformLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/MvComPhyLib.h>
#include <Library/IoLib.h>
#include <Library/TimerLib.h>
#include <Library/ParsePcdLib.h>

#define MAX_LANE_OPTIONS          10

/***** Parsing PCD *****/
#define GET_LANE_TYPE(id)         PcdGetPtr(PcdChip##id##ComPhyTypes)
#define GET_LANE_SPEED(id)        PcdGetPtr(PcdChip##id##ComPhySpeeds)
#define GET_LANE_INV(id)          PcdGetPtr(PcdChip##id##ComPhyInvFlags)

#define FillLaneMap(chip_struct, lane_struct, id) { \
  ParsePcdString((CHAR16 *) GET_LANE_TYPE(id), chip_struct[id].LanesCount, NULL, lane_struct[id].TypeStr);     \
  ParsePcdString((CHAR16 *) GET_LANE_SPEED(id), chip_struct[id].LanesCount, lane_struct[id].SpeedValue, NULL); \
  ParsePcdString((CHAR16 *) GET_LANE_INV(id), chip_struct[id].LanesCount, lane_struct[id].InvFlag, NULL);      \
}

#define GetComPhyPcd(chip_struct, lane_struct, id) {               \
  FillLaneMap(chip_struct, lane_struct, id);                       \
}

/***** ComPhy *****/
#define COMPHY_SPEED_ERROR                           0
#define COMPHY_SPEED_1_25G                           1
#define COMPHY_SPEED_1_5G                            2
#define COMPHY_SPEED_2_5G                            3
#define COMPHY_SPEED_3G                              4
#define COMPHY_SPEED_3_125G                          5
#define COMPHY_SPEED_5G                              6
#define COMPHY_SPEED_5_15625G                        7
#define COMPHY_SPEED_6G                              8
#define COMPHY_SPEED_6_25G                           9
#define COMPHY_SPEED_10_3125G                        10
#define COMPHY_SPEED_MAX                             11
#define COMPHY_SPEED_INVALID                         0xff

#define COMPHY_TYPE_UNCONNECTED                      0
#define COMPHY_TYPE_PCIE0                            1
#define COMPHY_TYPE_PCIE1                            2
#define COMPHY_TYPE_PCIE2                            3
#define COMPHY_TYPE_PCIE3                            4
#define COMPHY_TYPE_SATA0                            5
#define COMPHY_TYPE_SATA1                            6
#define COMPHY_TYPE_SATA2                            7
#define COMPHY_TYPE_SATA3                            8
#define COMPHY_TYPE_SGMII0                           9
#define COMPHY_TYPE_SGMII1                           10
#define COMPHY_TYPE_SGMII2                           11
#define COMPHY_TYPE_SGMII3                           12
#define COMPHY_TYPE_QSGMII                           13
#define COMPHY_TYPE_USB3_HOST0                       14
#define COMPHY_TYPE_USB3_HOST1                       15
#define COMPHY_TYPE_USB3_DEVICE                      16
#define COMPHY_TYPE_XAUI0                            17
#define COMPHY_TYPE_XAUI1                            18
#define COMPHY_TYPE_XAUI2                            19
#define COMPHY_TYPE_XAUI3                            20
#define COMPHY_TYPE_RXAUI0                           21
#define COMPHY_TYPE_RXAUI1                           22
#define COMPHY_TYPE_SFI                              23
#define COMPHY_TYPE_MAX                              24
#define COMPHY_TYPE_INVALID                          0xff

#define COMPHY_POLARITY_NO_INVERT                    0
#define COMPHY_POLARITY_TXD_INVERT                   1
#define COMPHY_POLARITY_RXD_INVERT                   2
#define COMPHY_POLARITY_ALL_INVERT                   (COMPHY_POLARITY_TXD_INVERT | COMPHY_POLARITY_RXD_INVERT)

/***** SerDes IP registers *****/
#define SD_EXTERNAL_CONFIG0_REG                   0
#define SD_EXTERNAL_CONFIG0_SD_PU_PLL_OFFSET      1
#define SD_EXTERNAL_CONFIG0_SD_PU_PLL_MASK        (1 << SD_EXTERNAL_CONFIG0_SD_PU_PLL_OFFSET)
#define SD_EXTERNAL_CONFIG0_SD_PHY_GEN_RX_OFFSET  3
#define SD_EXTERNAL_CONFIG0_SD_PHY_GEN_RX_MASK    (0xf << SD_EXTERNAL_CONFIG0_SD_PHY_GEN_RX_OFFSET)
#define SD_EXTERNAL_CONFIG0_SD_PHY_GEN_TX_OFFSET  7
#define SD_EXTERNAL_CONFIG0_SD_PHY_GEN_TX_MASK    (0xf << SD_EXTERNAL_CONFIG0_SD_PHY_GEN_TX_OFFSET)
#define SD_EXTERNAL_CONFIG0_SD_PU_RX_OFFSET       11
#define SD_EXTERNAL_CONFIG0_SD_PU_RX_MASK         (1 << SD_EXTERNAL_CONFIG0_SD_PU_RX_OFFSET)
#define SD_EXTERNAL_CONFIG0_SD_PU_TX_OFFSET       12
#define SD_EXTERNAL_CONFIG0_SD_PU_TX_MASK         (1 << SD_EXTERNAL_CONFIG0_SD_PU_TX_OFFSET)
#define SD_EXTERNAL_CONFIG0_HALF_BUS_MODE_OFFSET  14
#define SD_EXTERNAL_CONFIG0_HALF_BUS_MODE_MASK    (1 << SD_EXTERNAL_CONFIG0_HALF_BUS_MODE_OFFSET)
#define SD_EXTERNAL_CONFIG0_MEDIA_MODE_OFFSET     15
#define SD_EXTERNAL_CONFIG0_MEDIA_MODE_MASK       (0x1 << SD_EXTERNAL_CONFIG0_MEDIA_MODE_OFFSET)

#define SD_EXTERNAL_CONFIG1_REG                   0x4
#define SD_EXTERNAL_CONFIG1_RESET_IN_OFFSET       3
#define SD_EXTERNAL_CONFIG1_RESET_IN_MASK         (0x1 << SD_EXTERNAL_CONFIG1_RESET_IN_OFFSET)
#define SD_EXTERNAL_CONFIG1_RX_INIT_OFFSET        4
#define SD_EXTERNAL_CONFIG1_RX_INIT_MASK          (0x1 << SD_EXTERNAL_CONFIG1_RX_INIT_OFFSET)
#define SD_EXTERNAL_CONFIG1_RESET_CORE_OFFSET     5
#define SD_EXTERNAL_CONFIG1_RESET_CORE_MASK       (0x1 << SD_EXTERNAL_CONFIG1_RESET_CORE_OFFSET)
#define SD_EXTERNAL_CONFIG1_RF_RESET_IN_OFFSET    6
#define SD_EXTERNAL_CONFIG1_RF_RESET_IN_MASK      (0x1 << SD_EXTERNAL_CONFIG1_RF_RESET_IN_OFFSET)

#define SD_EXTERNAL_CONFIG2_REG                   0x8
#define SD_EXTERNAL_CONFIG2_PIN_DFE_EN_OFFSET     4
#define SD_EXTERNAL_CONFIG2_PIN_DFE_EN_MASK       (0x1 << SD_EXTERNAL_CONFIG2_PIN_DFE_EN_OFFSET)
#define SD_EXTERNAL_CONFIG2_SSC_ENABLE_OFFSET     7
#define SD_EXTERNAL_CONFIG2_SSC_ENABLE_MASK       (0x1 << SD_EXTERNAL_CONFIG2_SSC_ENABLE_OFFSET)

#define SD_EXTERNAL_STATUS0_REG                   0x18
#define SD_EXTERNAL_STATUS0_PLL_TX_OFFSET         2
#define SD_EXTERNAL_STATUS0_PLL_TX_MASK           (0x1 << SD_EXTERNAL_STATUS0_PLL_TX_OFFSET)
#define SD_EXTERNAL_STATUS0_PLL_RX_OFFSET         3
#define SD_EXTERNAL_STATUS0_PLL_RX_MASK           (0x1 << SD_EXTERNAL_STATUS0_PLL_RX_OFFSET)
#define SD_EXTERNAL_STATUS0_RX_INIT_OFFSET        4
#define SD_EXTERNAL_STATUS0_RX_INIT_MASK          (0x1 << SD_EXTERNAL_STATUS0_RX_INIT_OFFSET)

/***** HPIPE registers *****/
#define HPIPE_PWR_PLL_REG                         0x4
#define HPIPE_PWR_PLL_REF_FREQ_OFFSET             0
#define HPIPE_PWR_PLL_REF_FREQ_MASK               (0x1f << HPIPE_PWR_PLL_REF_FREQ_OFFSET)
#define HPIPE_PWR_PLL_PHY_MODE_OFFSET             5
#define HPIPE_PWR_PLL_PHY_MODE_MASK               (0x7 << HPIPE_PWR_PLL_PHY_MODE_OFFSET)

#define HPIPE_KVCO_CALIB_CTRL_REG                 0x8
#define HPIPE_KVCO_CALIB_CTRL_MAX_PLL_OFFSET      12
#define HPIPE_KVCO_CALIB_CTRL_MAX_PLL_MASK        (0x1 << HPIPE_KVCO_CALIB_CTRL_MAX_PLL_OFFSET)

#define HPIPE_CAL_REG1_REG                        0xc
#define HPIPE_CAL_REG_1_EXT_TXIMP_OFFSET          10
#define HPIPE_CAL_REG_1_EXT_TXIMP_MASK            (0x1f << HPIPE_CAL_REG_1_EXT_TXIMP_OFFSET)
#define HPIPE_CAL_REG_1_EXT_TXIMP_EN_OFFSET       15
#define HPIPE_CAL_REG_1_EXT_TXIMP_EN_MASK         (0x1 << HPIPE_CAL_REG_1_EXT_TXIMP_EN_OFFSET)

#define HPIPE_SQUELCH_FFE_SETTING_REG             0x018

#define HPIPE_DFE_REG0                            0x01C
#define HPIPE_DFE_RES_FORCE_OFFSET                15
#define HPIPE_DFE_RES_FORCE_MASK                  (0x1 << HPIPE_DFE_RES_FORCE_OFFSET)

#define HPIPE_DFE_F3_F5_REG                       0x028
#define HPIPE_DFE_F3_F5_DFE_EN_OFFSET             14
#define HPIPE_DFE_F3_F5_DFE_EN_MASK               (0x1 << HPIPE_DFE_F3_F5_DFE_EN_OFFSET)
#define HPIPE_DFE_F3_F5_DFE_CTRL_OFFSET           15
#define HPIPE_DFE_F3_F5_DFE_CTRL_MASK             (0x1 << HPIPE_DFE_F3_F5_DFE_CTRL_OFFSET)

#define HPIPE_G1_SET0_REG                         0x034
#define HPIPE_G2_SET0_REG                         0x03c
#define HPIPE_G3_SET0_REG                         0x044
#define HPIPE_GX_SET0_TX_AMP_OFFSET               1
#define HPIPE_GX_SET0_TX_AMP_MASK                 (0x1f << HPIPE_GX_SET0_TX_AMP_OFFSET)
#define HPIPE_GX_SET0_TX_AMP_ADJ_OFFSET           6
#define HPIPE_GX_SET0_TX_AMP_ADJ_MASK             (0x1 << HPIPE_GX_SET0_TX_AMP_ADJ_OFFSET)
#define HPIPE_GX_SET0_TX_EMPH1_OFFSET             7
#define HPIPE_GX_SET0_TX_EMPH1_MASK               (0xf << HPIPE_GX_SET0_TX_EMPH1_OFFSET)
#define HPIPE_GX_SET0_TX_EMPH1_EN_OFFSET          11
#define HPIPE_GX_SET0_TX_EMPH1_EN_MASK            (0x1 << HPIPE_GX_SET0_TX_EMPH1_EN_OFFSET)
#define HPIPE_GX_SET0_TX_SLEW_RATE_SEL_OFFSET     12
#define HPIPE_GX_SET0_TX_SLEW_RATE_SEL_MASK       (0x7 << HPIPE_GX_SET0_TX_SLEW_RATE_SEL_OFFSET)
#define HPIPE_GX_SET0_TX_SLEW_CTRL_EN_OFFSET      15
#define HPIPE_GX_SET0_TX_SLEW_CTRL_EN_MASK        (0x1 << HPIPE_GX_SET0_TX_SLEW_CTRL_EN_OFFSET)

#define HPIPE_G1_SET1_REG                         0x038
#define HPIPE_G2_SET1_REG                         0x040
#define HPIPE_G3_SET1_REG                         0x048
#define HPIPE_GX_SET1_RX_SELMUPI_OFFSET           0
#define HPIPE_GX_SET1_RX_SELMUPI_MASK             (0x7 << HPIPE_GX_SET1_RX_SELMUPI_OFFSET)
#define HPIPE_GX_SET1_RX_SELMUPP_OFFSET           3
#define HPIPE_GX_SET1_RX_SELMUPP_MASK             (0x7 << HPIPE_GX_SET1_RX_SELMUPP_OFFSET)
#define HPIPE_GX_SET1_RX_SELMUFI_OFFSET           6
#define HPIPE_GX_SET1_RX_SELMUFI_MASK             (0x3 << HPIPE_GX_SET1_RX_SELMUFI_OFFSET)
#define HPIPE_GX_SET1_RX_SELMUFF_OFFSET           8
#define HPIPE_GX_SET1_RX_SELMUFF_MASK             (0x3 << HPIPE_GX_SET1_RX_SELMUFF_OFFSET)
#define HPIPE_GX_SET1_RX_DFE_EN_OFFSET            10
#define HPIPE_GX_SET1_RX_DFE_EN_MASK              (0x1 << HPIPE_GX_SET1_RX_DFE_EN_OFFSET)
#define HPIPE_GX_SET1_RX_DIGCK_DIV_OFFSET         11
#define HPIPE_GX_SET1_RX_DIGCK_DIV_MASK           (0x3 << HPIPE_GX_SET1_RX_DIGCK_DIV_OFFSET)
#define HPIPE_GX_SET1_SAMPLER_INPAIRX2_EN_OFFSET  13
#define HPIPE_GX_SET1_SAMPLER_INPAIRX2_EN_MASK    (0x1 << HPIPE_GX_SET1_SAMPLER_INPAIRX2_EN_OFFSET)

#define HPIPE_LOOPBACK_REG                        0x08c
#define HPIPE_LOOPBACK_SEL_OFFSET                 1
#define HPIPE_LOOPBACK_SEL_MASK                   (0x7 << HPIPE_LOOPBACK_SEL_OFFSET)

#define HPIPE_SYNC_PATTERN_REG                    0x090

#define HPIPE_INTERFACE_REG                       0x94
#define HPIPE_INTERFACE_GEN_MAX_OFFSET            10
#define HPIPE_INTERFACE_GEN_MAX_MASK              (0x3 << HPIPE_INTERFACE_GEN_MAX_OFFSET)
#define HPIPE_INTERFACE_DET_BYPASS_OFFSET         12
#define HPIPE_INTERFACE_DET_BYPASS_MASK           (0x1 << HPIPE_INTERFACE_DET_BYPASS_OFFSET)
#define HPIPE_INTERFACE_LINK_TRAIN_OFFSET         14
#define HPIPE_INTERFACE_LINK_TRAIN_MASK           (0x1 << HPIPE_INTERFACE_LINK_TRAIN_OFFSET)

#define HPIPE_ISOLATE_MODE_REG                    0x98
#define HPIPE_ISOLATE_MODE_GEN_RX_OFFSET          0
#define HPIPE_ISOLATE_MODE_GEN_RX_MASK            (0xf << HPIPE_ISOLATE_MODE_GEN_RX_OFFSET)
#define HPIPE_ISOLATE_MODE_GEN_TX_OFFSET          4
#define HPIPE_ISOLATE_MODE_GEN_TX_MASK            (0xf << HPIPE_ISOLATE_MODE_GEN_TX_OFFSET)

#define HPIPE_GX_SET2_REG                         0xf4
#define HPIPE_GX_SET2_TX_EMPH0_OFFSET             0
#define HPIPE_GX_SET2_TX_EMPH0_MASK               (0xf << HPIPE_GX_SET2_TX_EMPH0_OFFSET)
#define HPIPE_GX_SET2_TX_EMPH0_EN_OFFSET          4
#define HPIPE_GX_SET2_TX_EMPH0_EN_MASK            (0x1 << HPIPE_GX_SET2_TX_EMPH0_MASK)

#define HPIPE_VTHIMPCAL_CTRL_REG                  0x104

#define HPIPE_VDD_CAL_CTRL_REG                    0x114
#define HPIPE_EXT_SELLV_RXSAMPL_OFFSET            5
#define HPIPE_EXT_SELLV_RXSAMPL_MASK              (0x1f << HPIPE_EXT_SELLV_RXSAMPL_OFFSET)

#define HPIPE_VDD_CAL_0_REG                       0x108
#define HPIPE_CAL_VDD_CONT_MODE_OFFSET            15
#define HPIPE_CAL_VDD_CONT_MODE_MASK              (0x1 << HPIPE_CAL_VDD_CONT_MODE_OFFSET)

#define HPIPE_PCIE_REG0                           0x120
#define HPIPE_PCIE_IDLE_SYNC_OFFSET               12
#define HPIPE_PCIE_IDLE_SYNC_MASK                 (0x1 << HPIPE_PCIE_IDLE_SYNC_OFFSET)
#define HPIPE_PCIE_SEL_BITS_OFFSET                13
#define HPIPE_PCIE_SEL_BITS_MASK                  (0x3 << HPIPE_PCIE_SEL_BITS_OFFSET)

#define HPIPE_LANE_ALIGN_REG                      0x124
#define HPIPE_LANE_ALIGN_OFF_OFFSET               12
#define HPIPE_LANE_ALIGN_OFF_MASK                 (0x1 << HPIPE_LANE_ALIGN_OFF_OFFSET)

#define HPIPE_MISC_REG                            0x13C
#define HPIPE_MISC_CLK100M_125M_OFFSET            4
#define HPIPE_MISC_CLK100M_125M_MASK              (0x1 << HPIPE_MISC_CLK100M_125M_OFFSET)
#define HPIPE_MISC_ICP_FORCE_OFFSET               5
#define HPIPE_MISC_ICP_FORCE_MASK                 (0x1 << HPIPE_MISC_ICP_FORCE_OFFSET)
#define HPIPE_MISC_TXDCLK_2X_OFFSET               6
#define HPIPE_MISC_TXDCLK_2X_MASK                 (0x1 << HPIPE_MISC_TXDCLK_2X_OFFSET)
#define HPIPE_MISC_CLK500_EN_OFFSET               7
#define HPIPE_MISC_CLK500_EN_MASK                 (0x1 << HPIPE_MISC_CLK500_EN_OFFSET)
#define HPIPE_MISC_REFCLK_SEL_OFFSET              10
#define HPIPE_MISC_REFCLK_SEL_MASK                (0x1 << HPIPE_MISC_REFCLK_SEL_OFFSET)

#define HPIPE_RX_CONTROL_1_REG                    0x140
#define HPIPE_RX_CONTROL_1_RXCLK2X_SEL_OFFSET     11
#define HPIPE_RX_CONTROL_1_RXCLK2X_SEL_MASK       (0x1 << HPIPE_RX_CONTROL_1_RXCLK2X_SEL_OFFSET)
#define HPIPE_RX_CONTROL_1_CLK8T_EN_OFFSET        12
#define HPIPE_RX_CONTROL_1_CLK8T_EN_MASK          (0x1 << HPIPE_RX_CONTROL_1_CLK8T_EN_OFFSET)

#define HPIPE_PWR_CTR_REG                         0x148
#define HPIPE_PWR_CTR_RST_DFE_OFFSET              0
#define HPIPE_PWR_CTR_RST_DFE_MASK                (0x1 << HPIPE_PWR_CTR_RST_DFE_OFFSET)
#define HPIPE_PWR_CTR_SFT_RST_OFFSET              10
#define HPIPE_PWR_CTR_SFT_RST_MASK                (0x1 << HPIPE_PWR_CTR_SFT_RST_OFFSET)

#define HPIPE_PLLINTP_REG1                        0x150

#define HPIPE_SPD_DIV_FORCE_REG                       0x154
#define HPIPE_TXDIGCK_DIV_FORCE_OFFSET                7
#define HPIPE_TXDIGCK_DIV_FORCE_MASK                  (0x1 << HPIPE_TXDIGCK_DIV_FORCE_OFFSET)
#define HPIPE_SPD_DIV_FORCE_RX_SPD_DIV_OFFSET         8
#define HPIPE_SPD_DIV_FORCE_RX_SPD_DIV_MASK           (0x3 << HPIPE_SPD_DIV_FORCE_RX_SPD_DIV_OFFSET)
#define HPIPE_SPD_DIV_FORCE_RX_SPD_DIV_FORCE_OFFSET   10
#define HPIPE_SPD_DIV_FORCE_RX_SPD_DIV_FORCE_MASK     (0x1 << HPIPE_SPD_DIV_FORCE_RX_SPD_DIV_FORCE_OFFSET)
#define HPIPE_SPD_DIV_FORCE_TX_SPD_DIV_OFFSET         13
#define HPIPE_SPD_DIV_FORCE_TX_SPD_DIV_MASK           (0x3 << HPIPE_SPD_DIV_FORCE_TX_SPD_DIV_OFFSET)
#define HPIPE_SPD_DIV_FORCE_TX_SPD_DIV_FORCE_OFFSET   15
#define HPIPE_SPD_DIV_FORCE_TX_SPD_DIV_FORCE_MASK     (0x1 << HPIPE_SPD_DIV_FORCE_TX_SPD_DIV_FORCE_OFFSET)

#define HPIPE_SAMPLER_N_PROC_CALIB_CTRL_REG       0x16C
#define HPIPE_RX_SAMPLER_OS_GAIN_OFFSET           6
#define HPIPE_RX_SAMPLER_OS_GAIN_MASK             (0x3 << HPIPE_RX_SAMPLER_OS_GAIN_OFFSET)
#define HPIPE_SAMPLER_OFFSET                      12
#define HPIPE_SAMPLER_MASK                        (0x1 << HPIPE_SAMPLER_OFFSET)

#define HPIPE_TX_REG1_REG                         0x174
#define HPIPE_TX_REG1_TX_EMPH_RES_OFFSET          5
#define HPIPE_TX_REG1_TX_EMPH_RES_MASK            (0x3 << HPIPE_TX_REG1_TX_EMPH_RES_OFFSET)
#define HPIPE_TX_REG1_SLC_EN_OFFSET               10
#define HPIPE_TX_REG1_SLC_EN_MASK                 (0x3f << HPIPE_TX_REG1_SLC_EN_OFFSET)

#define HPIPE_TX_REG1_REG                         0x174
#define HPIPE_TX_REG1_TX_EMPH_RES_OFFSET          5
#define HPIPE_TX_REG1_TX_EMPH_RES_MASK            (0x3 << HPIPE_TX_REG1_TX_EMPH_RES_OFFSET)
#define HPIPE_TX_REG1_SLC_EN_OFFSET               10
#define HPIPE_TX_REG1_SLC_EN_MASK                 (0x3f << HPIPE_TX_REG1_SLC_EN_OFFSET)

#define HPIPE_PWR_CTR_DTL_REG                         0x184
#define HPIPE_PWR_CTR_DTL_SQ_DET_EN_OFFSET            0
#define HPIPE_PWR_CTR_DTL_SQ_DET_EN_MASK              (0x1 << HPIPE_PWR_CTR_DTL_SQ_DET_EN_OFFSET)
#define HPIPE_PWR_CTR_DTL_SQ_PLOOP_EN_OFFSET          1
#define HPIPE_PWR_CTR_DTL_SQ_PLOOP_EN_MASK            (0x1 << HPIPE_PWR_CTR_DTL_SQ_PLOOP_EN_OFFSET)
#define HPIPE_PWR_CTR_DTL_FLOOP_EN_OFFSET             2
#define HPIPE_PWR_CTR_DTL_FLOOP_EN_MASK               (0x1 << HPIPE_PWR_CTR_DTL_FLOOP_EN_OFFSET)
#define HPIPE_PWR_CTR_DTL_CLAMPING_SEL_OFFSET         4
#define HPIPE_PWR_CTR_DTL_CLAMPING_SEL_MASK           (0x7 << HPIPE_PWR_CTR_DTL_CLAMPING_SEL_OFFSET)
#define HPIPE_PWR_CTR_DTL_INTPCLK_DIV_FORCE_OFFSET    10
#define HPIPE_PWR_CTR_DTL_INTPCLK_DIV_FORCE_MASK      (0x1 << HPIPE_PWR_CTR_DTL_INTPCLK_DIV_FORCE_OFFSET)
#define HPIPE_PWR_CTR_DTL_CLK_MODE_OFFSET             12
#define HPIPE_PWR_CTR_DTL_CLK_MODE_MASK               (0x3 << HPIPE_PWR_CTR_DTL_CLK_MODE_OFFSET)
#define HPIPE_PWR_CTR_DTL_CLK_MODE_FORCE_OFFSET       14
#define HPIPE_PWR_CTR_DTL_CLK_MODE_FORCE_MASK         (1 << HPIPE_PWR_CTR_DTL_CLK_MODE_FORCE_OFFSET)

#define HPIPE_PHASE_CONTROL_REG                   0x188
#define HPIPE_OS_PH_OFFSET_OFFSET                 0
#define HPIPE_OS_PH_OFFSET_MASK                   (0x7f << HPIPE_OS_PH_OFFSET_OFFSET)
#define HPIPE_OS_PH_OFFSET_FORCE_OFFSET           7
#define HPIPE_OS_PH_OFFSET_FORCE_MASK             (0x1 << HPIPE_OS_PH_OFFSET_FORCE_OFFSET)
#define HPIPE_OS_PH_VALID_OFFSET                  8
#define HPIPE_OS_PH_VALID_MASK                    (0x1 << HPIPE_OS_PH_VALID_OFFSET)

#define HPIPE_FRAME_DETECT_CTRL_0_REG             0x214
#define HPIPE_TRAIN_PAT_NUM_OFFSET                0x7
#define HPIPE_TRAIN_PAT_NUM_MASK                  (0x1FF << HPIPE_TRAIN_PAT_NUM_OFFSET)

#define HPIPE_FRAME_DETECT_CTRL_3_REG             0x220
#define HPIPE_PATTERN_LOCK_LOST_TIMEOUT_EN_OFFSET 12
#define HPIPE_PATTERN_LOCK_LOST_TIMEOUT_EN_MASK   (0x1 << HPIPE_PATTERN_LOCK_LOST_TIMEOUT_EN_OFFSET)

#define HPIPE_DME_REG                             0x228
#define HPIPE_DME_ETHERNET_MODE_OFFSET            7
#define HPIPE_DME_ETHERNET_MODE_MASK              (0x1 << HPIPE_DME_ETHERNET_MODE_OFFSET)

#define HPIPE_TX_TRAIN_CTRL_0_REG                 0x268
#define HPIPE_TX_TRAIN_P2P_HOLD_OFFSET            15
#define HPIPE_TX_TRAIN_P2P_HOLD_MASK              (0x1 << HPIPE_TX_TRAIN_P2P_HOLD_OFFSET)

#define HPIPE_TX_TRAIN_CTRL_REG                   0x26C
#define HPIPE_TX_TRAIN_CTRL_G1_OFFSET             0
#define HPIPE_TX_TRAIN_CTRL_G1_MASK               (0x1 << HPIPE_TX_TRAIN_CTRL_G1_OFFSET)
#define HPIPE_TX_TRAIN_CTRL_GN1_OFFSET            1
#define HPIPE_TX_TRAIN_CTRL_GN1_MASK              (0x1 << HPIPE_TX_TRAIN_CTRL_GN1_OFFSET)
#define HPIPE_TX_TRAIN_CTRL_G0_OFFSET             2
#define HPIPE_TX_TRAIN_CTRL_G0_MASK               (0x1 << HPIPE_TX_TRAIN_CTRL_G0_OFFSET)

#define HPIPE_TX_TRAIN_CTRL_4_REG                 0x278
#define HPIPE_TRX_TRAIN_TIMER_OFFSET              0
#define HPIPE_TRX_TRAIN_TIMER_MASK                (0x3FF << HPIPE_TRX_TRAIN_TIMER_OFFSET)

#define HPIPE_PCIE_REG1                           0x288
#define HPIPE_PCIE_REG3                           0x290

#define HPIPE_TX_TRAIN_CTRL_5_REG                 0x2A4
#define HPIPE_RX_TRAIN_TIMER_OFFSET               0
#define HPIPE_RX_TRAIN_TIMER_MASK                 (0x3ff << HPIPE_RX_TRAIN_TIMER_OFFSET)
#define HPIPE_TX_TRAIN_START_SQ_EN_OFFSET         11
#define HPIPE_TX_TRAIN_START_SQ_EN_MASK           (0x1 << HPIPE_TX_TRAIN_START_SQ_EN_OFFSET)
#define HPIPE_TX_TRAIN_START_FRM_DET_EN_OFFSET    12
#define HPIPE_TX_TRAIN_START_FRM_DET_EN_MASK      (0x1 << HPIPE_TX_TRAIN_START_FRM_DET_EN_OFFSET)
#define HPIPE_TX_TRAIN_START_FRM_LOCK_EN_OFFSET   13
#define HPIPE_TX_TRAIN_START_FRM_LOCK_EN_MASK     (0x1 << HPIPE_TX_TRAIN_START_FRM_LOCK_EN_OFFSET)
#define HPIPE_TX_TRAIN_WAIT_TIME_EN_OFFSET        14
#define HPIPE_TX_TRAIN_WAIT_TIME_EN_MASK          (0x1 << HPIPE_TX_TRAIN_WAIT_TIME_EN_OFFSET)

#define HPIPE_TX_TRAIN_REG                        0x31C
#define HPIPE_TX_TRAIN_CHK_INIT_OFFSET            4
#define HPIPE_TX_TRAIN_CHK_INIT_MASK              (0x1 << HPIPE_TX_TRAIN_CHK_INIT_OFFSET)
#define HPIPE_TX_TRAIN_COE_FM_PIN_PCIE3_OFFSET    7
#define HPIPE_TX_TRAIN_COE_FM_PIN_PCIE3_MASK      (0x1 << HPIPE_TX_TRAIN_COE_FM_PIN_PCIE3_OFFSET)
#define HPIPE_TX_TRAIN_16BIT_AUTO_EN_OFFSET       8
#define HPIPE_TX_TRAIN_16BIT_AUTO_EN_MASK         (0x1 << HPIPE_TX_TRAIN_16BIT_AUTO_EN_OFFSET)
#define HPIPE_TX_TRAIN_PAT_SEL_OFFSET             9
#define HPIPE_TX_TRAIN_PAT_SEL_MASK               (0x1 << HPIPE_TX_TRAIN_PAT_SEL_OFFSET)

#define HPIPE_CDR_CONTROL_REG                     0x418
#define HPIPE_CDR_MAX_DFE_ADAPT_1_OFFSET          6
#define HPIPE_CDR_MAX_DFE_ADAPT_1_MASK            (0x7 << HPIPE_CDR_MAX_DFE_ADAPT_1_OFFSET)
#define HPIPE_CDR_MAX_DFE_ADAPT_0_OFFSET          9
#define HPIPE_CDR_MAX_DFE_ADAPT_0_MASK            (0x7 << HPIPE_CDR_MAX_DFE_ADAPT_0_OFFSET)
#define HPIPE_CDR_RX_MAX_DFE_ADAPT_1_OFFSET       12
#define HPIPE_CDR_RX_MAX_DFE_ADAPT_1_MASK         (0x3 << HPIPE_CDR_RX_MAX_DFE_ADAPT_1_OFFSET)

#define HPIPE_TX_TRAIN_CTRL_11_REG                0x438
#define HPIPE_TX_STATUS_CHECK_MODE_OFFSET         6
#define HPIPE_TX_TX_STATUS_CHECK_MODE_MASK        (0x1 << HPIPE_TX_STATUS_CHECK_MODE_OFFSET)
#define HPIPE_TX_NUM_OF_PRESET_OFFSET             10
#define HPIPE_TX_NUM_OF_PRESET_MASK               (0x7 << HPIPE_TX_NUM_OF_PRESET_OFFSET)
#define HPIPE_TX_SWEEP_PRESET_EN_OFFSET           15
#define HPIPE_TX_SWEEP_PRESET_EN_MASK             (0x1 << HPIPE_TX_SWEEP_PRESET_EN_OFFSET)

#define HPIPE_G1_SET3_REG                         0x440
#define HPIPE_G2_SET3_REG                         0x448
#define HPIPE_G3_SET3_REG                         0x450
#define HPIPE_GX_SET3_FFE_CAP_SEL_OFFSET          0
#define HPIPE_GX_SET3_FFE_CAP_SEL_MASK            (0xf << HPIPE_GX_SET3_FFE_CAP_SEL_OFFSET)
#define HPIPE_GX_SET3_FFE_RES_SEL_OFFSET          4
#define HPIPE_GX_SET3_FFE_RES_SEL_MASK            (0x7 << HPIPE_GX_SET3_FFE_RES_SEL_OFFSET)
#define HPIPE_GX_SET3_FFE_SETTING_FORCE_OFFSET    7
#define HPIPE_GX_SET3_FFE_SETTING_FORCE_MASK      (0x1 << HPIPE_GX_SET3_FFE_SETTING_FORCE_OFFSET)
#define HPIPE_GX_SET3_FBCK_SEL_OFFSET             9
#define HPIPE_GX_SET3_FBCK_SEL_MASK               (0x1 << HPIPE_GX_SET3_FBCK_SEL_OFFSET)
#define HPIPE_GX_SET3_FFE_DEG_RES_LEVEL_OFFSET    12
#define HPIPE_GX_SET3_FFE_DEG_RES_LEVEL_MASK      (0x3 << HPIPE_GX_SET3_FFE_DEG_RES_LEVEL_OFFSET)
#define HPIPE_GX_SET3_FFE_LOAD_RES_LEVEL_OFFSET   14
#define HPIPE_GX_SET3_FFE_LOAD_RES_LEVEL_MASK     (0x3 << HPIPE_GX_SET3_FFE_LOAD_RES_LEVEL_OFFSET)

#define HPIPE_G1_SET4_REG                         0x444
#define HPIPE_G2_SET4_REG                         0x44C
#define HPIPE_G3_SET4_REG                         0x454
#define HPIPE_GX_SET4_DFE_RES_OFFSET              8
#define HPIPE_GX_SET4_DFE_RES_MASK                (0x3 << HPIPE_GX_SET4_DFE_RES_OFFSET)

#define HPIPE_TX_PRESET_INDEX_REG                 0x468
#define HPIPE_TX_PRESET_INDEX_OFFSET              0
#define HPIPE_TX_PRESET_INDEX_MASK                (0xf << HPIPE_TX_PRESET_INDEX_OFFSET)

#define HPIPE_DFE_CONTROL_REG                     0x470
#define HPIPE_DFE_TX_MAX_DFE_ADAPT_OFFSET         14
#define HPIPE_DFE_TX_MAX_DFE_ADAPT_MASK           (0x3 << HPIPE_DFE_TX_MAX_DFE_ADAPT_OFFSET)

#define HPIPE_DFE_CTRL_28_REG                     0x49C
#define HPIPE_DFE_CTRL_28_PIPE4_OFFSET            7
#define HPIPE_DFE_CTRL_28_PIPE4_MASK              (0x1 << HPIPE_DFE_CTRL_28_PIPE4_OFFSET)

#define HPIPE_G1_SET5_REG                         0x538
#define HPIPE_G3_SET5_REG                         0x548
#define HPIPE_GX_SET5_ICP_OFFSET                  0
#define HPIPE_GX_SET5_ICP_MASK                    (0xf << HPIPE_GX_SET5_ICP_OFFSET)

#define HPIPE_LANE_CONFIG0_REG                    0x604
#define HPIPE_LANE_CONFIG0_MAX_PLL_OFFSET         9
#define HPIPE_LANE_CONFIG0_MAX_PLL_MASK           (0x1 << HPIPE_LANE_CONFIG0_MAX_PLL_OFFSET)
#define HPIPE_LANE_CONFIG0_GEN2_PLL_OFFSET        10
#define HPIPE_LANE_CONFIG0_GEN2_PLL_MASK          (0x1 << HPIPE_LANE_CONFIG0_GEN2_PLL_OFFSET)

#define HPIPE_LANE_STATUS0_REG                    0x60C
#define HPIPE_LANE_STATUS0_PCLK_EN_OFFSET         0
#define HPIPE_LANE_STATUS0_PCLK_EN_MASK           (0x1 << HPIPE_LANE_STATUS0_PCLK_EN_OFFSET)

#define HPIPE_LANE_CFG4_REG                       0x620
#define HPIPE_LANE_CFG4_DFE_CTRL_OFFSET           0
#define HPIPE_LANE_CFG4_DFE_CTRL_MASK             (0x7 << HPIPE_LANE_CFG4_DFE_CTRL_OFFSET)
#define HPIPE_LANE_CFG4_DFE_EN_SEL_OFFSET         3
#define HPIPE_LANE_CFG4_DFE_EN_SEL_MASK           (0x1 << HPIPE_LANE_CFG4_DFE_EN_SEL_OFFSET)
#define HPIPE_LANE_CFG4_DFE_OVER_OFFSET           6
#define HPIPE_LANE_CFG4_DFE_OVER_MASK             (0x1 << HPIPE_LANE_CFG4_DFE_OVER_OFFSET)
#define HPIPE_LANE_CFG4_SSC_CTRL_OFFSET           7
#define HPIPE_LANE_CFG4_SSC_CTRL_MASK             (0x1 << HPIPE_LANE_CFG4_SSC_CTRL_OFFSET)

#define HPIPE_LANE_EQU_CONFIG_0_REG               0x69C
#define HPIPE_CFG_PHY_RC_EP_OFFSET                12
#define HPIPE_CFG_PHY_RC_EP_MASK                  (0x1 << HPIPE_CFG_PHY_RC_EP_OFFSET)

#define HPIPE_LANE_EQ_CFG1_REG                    0x6a0
#define HPIPE_CFG_UPDATE_POLARITY_OFFSET          12
#define HPIPE_CFG_UPDATE_POLARITY_MASK            (0x1 << HPIPE_CFG_UPDATE_POLARITY_OFFSET)

#define HPIPE_LANE_EQ_REMOTE_SETTING_REG          0x6f8
#define HPIPE_LANE_CFG_FOM_DIRN_OVERRIDE_OFFSET   0
#define HPIPE_LANE_CFG_FOM_DIRN_OVERRIDE_MASK     (0x1 << HPIPE_LANE_CFG_FOM_DIRN_OVERRIDE_OFFSET)
#define HPIPE_LANE_CFG_FOM_ONLY_MODE_OFFFSET      1
#define HPIPE_LANE_CFG_FOM_ONLY_MODE_MASK         (0x1 << HPIPE_LANE_CFG_FOM_ONLY_MODE_OFFFSET)
#define HPIPE_LANE_CFG_FOM_PRESET_VECTOR_OFFSET   2
#define HPIPE_LANE_CFG_FOM_PRESET_VECTOR_MASK     (0xf << HPIPE_LANE_CFG_FOM_PRESET_VECTOR_OFFSET)

#define HPIPE_RST_CLK_CTRL_REG                    0x704
#define HPIPE_RST_CLK_CTRL_PIPE_RST_OFFSET        0
#define HPIPE_RST_CLK_CTRL_PIPE_RST_MASK          (0x1 << HPIPE_RST_CLK_CTRL_PIPE_RST_OFFSET)
#define HPIPE_RST_CLK_CTRL_FIXED_PCLK_OFFSET      2
#define HPIPE_RST_CLK_CTRL_FIXED_PCLK_MASK        (0x1 << HPIPE_RST_CLK_CTRL_FIXED_PCLK_OFFSET)
#define HPIPE_RST_CLK_CTRL_PIPE_WIDTH_OFFSET      3
#define HPIPE_RST_CLK_CTRL_PIPE_WIDTH_MASK        (0x1 << HPIPE_RST_CLK_CTRL_PIPE_WIDTH_OFFSET)
#define HPIPE_RST_CLK_CTRL_CORE_FREQ_SEL_OFFSET   9
#define HPIPE_RST_CLK_CTRL_CORE_FREQ_SEL_MASK     (0x1 << HPIPE_RST_CLK_CTRL_CORE_FREQ_SEL_OFFSET)

#define HPIPE_CLK_SRC_LO_REG                      0x70c
#define HPIPE_CLK_SRC_LO_PLL_RDY_DL_OFFSET        5
#define HPIPE_CLK_SRC_LO_PLL_RDY_DL_MASK          (0x7 << HPIPE_CLK_SRC_LO_PLL_RDY_DL_OFFSET)

#define HPIPE_CLK_SRC_HI_REG                      0x710
#define HPIPE_CLK_SRC_HI_LANE_STRT_OFFSET         0
#define HPIPE_CLK_SRC_HI_LANE_STRT_MASK           (0x1 << HPIPE_CLK_SRC_HI_LANE_STRT_OFFSET)
#define HPIPE_CLK_SRC_HI_LANE_BREAK_OFFSET        1
#define HPIPE_CLK_SRC_HI_LANE_BREAK_MASK          (0x1 << HPIPE_CLK_SRC_HI_LANE_BREAK_OFFSET)
#define HPIPE_CLK_SRC_HI_LANE_MASTER_OFFSET       2
#define HPIPE_CLK_SRC_HI_LANE_MASTER_MASK         (0x1 << HPIPE_CLK_SRC_HI_LANE_MASTER_OFFSET)
#define HPIPE_CLK_SRC_HI_MODE_PIPE_OFFSET         7
#define HPIPE_CLK_SRC_HI_MODE_PIPE_MASK           (0x1 << HPIPE_CLK_SRC_HI_MODE_PIPE_OFFSET)

#define HPIPE_GLOBAL_MISC_CTRL                    0x718
#define HPIPE_GLOBAL_PM_CTRL                      0x740
#define HPIPE_GLOBAL_PM_RXDLOZ_WAIT_OFFSET        0
#define HPIPE_GLOBAL_PM_RXDLOZ_WAIT_MASK          (0xFF << HPIPE_GLOBAL_PM_RXDLOZ_WAIT_OFFSET)

/***** COMPHY registers *****/
#define COMMON_PHY_CFG1_REG                       0x0
#define COMMON_PHY_CFG1_PWR_UP_OFFSET             1
#define COMMON_PHY_CFG1_PWR_UP_MASK               (0x1 << COMMON_PHY_CFG1_PWR_UP_OFFSET)
#define COMMON_PHY_CFG1_PIPE_SELECT_OFFSET        2
#define COMMON_PHY_CFG1_PIPE_SELECT_MASK          (0x1 << COMMON_PHY_CFG1_PIPE_SELECT_OFFSET)
#define COMMON_PHY_CFG1_PWR_ON_RESET_OFFSET       13
#define COMMON_PHY_CFG1_PWR_ON_RESET_MASK         (0x1 << COMMON_PHY_CFG1_PWR_ON_RESET_OFFSET)
#define COMMON_PHY_CFG1_CORE_RSTN_OFFSET          14
#define COMMON_PHY_CFG1_CORE_RSTN_MASK            (0x1 << COMMON_PHY_CFG1_CORE_RSTN_OFFSET)
#define COMMON_PHY_PHY_MODE_OFFSET                15
#define COMMON_PHY_PHY_MODE_MASK                  (0x1 << COMMON_PHY_PHY_MODE_OFFSET)

#define COMMON_PHY_CFG6_REG                       0x14
#define COMMON_PHY_CFG6_IF_40_SEL_OFFSET          18
#define COMMON_PHY_CFG6_IF_40_SEL_MASK            (0x1 << COMMON_PHY_CFG6_IF_40_SEL_OFFSET)

#define COMMON_SELECTOR_PHY_OFFSET                0x140
#define COMMON_SELECTOR_PIPE_OFFSET               0x144

#define COMMON_PHY_SD_CTRL1                       0x148
#define COMMON_PHY_SD_CTRL1_RXAUI1_OFFSET         26
#define COMMON_PHY_SD_CTRL1_RXAUI1_MASK           (0x1 << COMMON_PHY_SD_CTRL1_RXAUI1_OFFSET)
#define COMMON_PHY_SD_CTRL1_RXAUI0_OFFSET         27
#define COMMON_PHY_SD_CTRL1_RXAUI0_MASK           (0x1 << COMMON_PHY_SD_CTRL1_RXAUI0_OFFSET)

/***** SATA registers *****/
#define SATA3_VENDOR_ADDRESS                      0xA0
#define SATA3_VENDOR_ADDR_OFSSET                  0
#define SATA3_VENDOR_ADDR_MASK                    (0xFFFFFFFF << SATA3_VENDOR_ADDR_OFSSET)
#define SATA3_VENDOR_DATA                         0xA4

#define SATA_CONTROL_REG                          0x0
#define SATA3_CTRL_SATA0_PD_OFFSET                6
#define SATA3_CTRL_SATA0_PD_MASK                  (1 << SATA3_CTRL_SATA0_PD_OFFSET)
#define SATA3_CTRL_SATA1_PD_OFFSET                14
#define SATA3_CTRL_SATA1_PD_MASK                  (1 << SATA3_CTRL_SATA1_PD_OFFSET)
#define SATA3_CTRL_SATA1_ENABLE_OFFSET            22
#define SATA3_CTRL_SATA1_ENABLE_MASK              (1 << SATA3_CTRL_SATA1_ENABLE_OFFSET)
#define SATA3_CTRL_SATA_SSU_OFFSET                23
#define SATA3_CTRL_SATA_SSU_MASK                  (1 << SATA3_CTRL_SATA_SSU_OFFSET)

#define SATA_MBUS_SIZE_SELECT_REG                 0x4
#define SATA_MBUS_REGRET_EN_OFFSET                7
#define SATA_MBUS_REGRET_EN_MASK                  (0x1 << SATA_MBUS_REGRET_EN_OFFSET)

/***************************/

typedef struct _CHIP_COMPHY_CONFIG CHIP_COMPHY_CONFIG;

typedef struct {
  UINT32 Type;
  UINT32 MuxValue;
} COMPHY_MUX_OPTIONS;

typedef struct {
  UINT32 MaxLaneValues;
  COMPHY_MUX_OPTIONS MuxValues[MAX_LANE_OPTIONS];
} COMPHY_MUX_DATA;

typedef struct {
  UINT32 Type;
  UINT32 Speed;
  UINT32 Invert;
} COMPHY_MAP;

typedef struct {
  CHAR16 *TypeStr[MAX_LANE_OPTIONS];
  UINTN SpeedValue[MAX_LANE_OPTIONS];
  UINTN InvFlag[MAX_LANE_OPTIONS];
} PCD_LANE_MAP;

typedef
VOID
(*COMPHY_CHIP_INIT) (
  IN CHIP_COMPHY_CONFIG *PtrChipCfg
  );

struct _CHIP_COMPHY_CONFIG {
  MV_COMPHY_CHIP_TYPE ChipType;
  COMPHY_MAP MapData[MAX_LANE_OPTIONS];
  COMPHY_MUX_DATA *MuxData;
  EFI_PHYSICAL_ADDRESS ComPhyBaseAddr;
  EFI_PHYSICAL_ADDRESS Hpipe3BaseAddr;
  COMPHY_CHIP_INIT Init;
  UINT32 LanesCount;
  UINT32 MuxBitCount;
};

VOID
ComPhyMuxInit (
  IN CHIP_COMPHY_CONFIG *PtrChipCfg,
  IN COMPHY_MAP *ComPhyMapData,
  IN EFI_PHYSICAL_ADDRESS SelectorBase
  );

VOID
ComPhyCp110Init (
  IN CHIP_COMPHY_CONFIG * First
  );

VOID
RegSet (
  IN EFI_PHYSICAL_ADDRESS Addr,
  IN UINT32 Data,
  IN UINT32 Mask
  );

VOID
RegSetSilent (
  IN EFI_PHYSICAL_ADDRESS Addr,
  IN UINT32 Data,
  IN UINT32 Mask
  );

VOID
RegSet16 (
  IN EFI_PHYSICAL_ADDRESS Addr,
  IN UINT16 Data,
  IN UINT16 Mask
  );

VOID
RegSetSilent16(
  IN EFI_PHYSICAL_ADDRESS Addr,
  IN UINT16 Data,
  IN UINT16 Mask
  );
#endif // __COMPHY_H__
