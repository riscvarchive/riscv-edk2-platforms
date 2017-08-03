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
#ifndef __MV_PHY_DXE_H__
#define __MV_PHY_DXE_H__

#define MII_BMCR      0x00  /* Basic mode control Register */
#define MII_BMSR      0x01  /* Basic mode status Register  */
#define MII_PHYSID1      0x02  /* PHYS ID 1           */
#define MII_PHYSID2      0x03  /* PHYS ID 2           */
#define MII_ADVERTISE      0x04  /* Advertisement control Reg   */
#define MII_LPA        0x05  /* Link partner ability Reg    */
#define MII_EXPANSION      0x06  /* Expansion Register         */
#define MII_CTRL1000      0x09  /* 1000BASE-T control         */
#define MII_STAT1000      0x0a  /* 1000BASE-T status         */
#define MII_ESTATUS      0x0f  /* Extended Status */
#define MII_DCOUNTER      0x12  /* Disconnect counter         */
#define MII_FCSCOUNTER      0x13  /* False carrier counter       */
#define MII_NWAYTEST      0x14  /* N-way auto-neg test Reg     */
#define MII_RERRCOUNTER     0x15  /* Receive error counter       */
#define MII_SREVISION      0x16  /* Silicon revision         */
#define MII_RESV1      0x17  /* Reserved...           */
#define MII_LBRERROR      0x18  /* Lpback, rx, bypass error    */
#define MII_PHYADDR      0x19  /* PHY address           */
#define MII_RESV2      0x1a  /* Reserved...           */
#define MII_TPISTATUS      0x1b  /* TPI status for 10mbps       */
#define MII_NCONFIG      0x1c  /* Network interface config    */

/* Basic mode control Register. */
#define BMCR_RESV    0x003f  /* Unused...           */
#define BMCR_SPEED1000    0x0040  /* MSB of Speed (1000)         */
#define BMCR_CTST    0x0080  /* Collision test         */
#define BMCR_FULLDPLX    0x0100  /* Full duplex           */
#define BMCR_ANRESTART    0x0200  /* Auto negotiation restart    */
#define BMCR_ISOLATE    0x0400  /* Disconnect DP83840 from MII */
#define BMCR_PDOWN    0x0800  /* Powerdown the DP83840       */
#define BMCR_ANENABLE    0x1000  /* Enable auto negotiation     */
#define BMCR_SPEED100    0x2000  /* Select 100Mbps         */
#define BMCR_LOOPBACK    0x4000  /* TXD loopback bits         */
#define BMCR_RESET    0x8000  /* Reset the DP83840         */

/* Basic mode status Register. */
#define BMSR_ERCAP    0x0001  /* Ext-Reg capability         */
#define BMSR_JCD    0x0002  /* Jabber detected         */
#define BMSR_LSTATUS    0x0004  /* Link status           */
#define BMSR_ANEGCAPABLE  0x0008  /* Able to do auto-negotiation */
#define BMSR_RFAULT    0x0010  /* Remote fault detected       */
#define BMSR_ANEGCOMPLETE  0x0020  /* Auto-negotiation complete   */
#define BMSR_RESV    0x00c0  /* Unused...           */
#define BMSR_ESTATEN    0x0100  /* Extended Status in R15 */
#define BMSR_100HALF2    0x0200  /* Can do 100BASE-T2 HDX */
#define BMSR_100FULL2    0x0400  /* Can do 100BASE-T2 FDX */
#define BMSR_10HALF    0x0800  /* Can do 10mbps, half-duplex  */
#define BMSR_10FULL    0x1000  /* Can do 10mbps, full-duplex  */
#define BMSR_100HALF    0x2000  /* Can do 100mbps, half-duplex */
#define BMSR_100FULL    0x4000  /* Can do 100mbps, full-duplex */
#define BMSR_100BASE4    0x8000  /* Can do 100mbps, 4k packets  */

#define PHY_ANEG_TIMEOUT 4000

#define PHY_INTERFACE_MODE_RGMII 0
#define PHY_INTERFACE_MODE_RGMII_ID 1
#define PHY_INTERFACE_MODE_RGMII_RXID 2
#define PHY_INTERFACE_MODE_RGMII_TXID 3
#define PHY_INTERFACE_MODE_SGMII 4
#define PHY_INTERFACE_MODE_RTBI 5

#define PHY_AUTONEGOTIATE_TIMEOUT 5000

/* 88E1011 PHY Status Register */
#define MIIM_88E1xxx_PHY_STATUS    0x11
#define MIIM_88E1xxx_PHYSTAT_SPEED  0xc000
#define MIIM_88E1xxx_PHYSTAT_GBIT  0x8000
#define MIIM_88E1xxx_PHYSTAT_100  0x4000
#define MIIM_88E1xxx_PHYSTAT_DUPLEX  0x2000
#define MIIM_88E1xxx_PHYSTAT_SPDDONE  0x0800
#define MIIM_88E1xxx_PHYSTAT_LINK  0x0400

#define MIIM_88E1xxx_PHY_SCR    0x10
#define MIIM_88E1xxx_PHY_MDI_X_AUTO  0x0060

/* 88E1111 PHY LED Control Register */
#define MIIM_88E1111_PHY_LED_CONTROL  24
#define MIIM_88E1111_PHY_LED_DIRECT  0x4100
#define MIIM_88E1111_PHY_LED_COMBINE  0x411C

/* 88E1111 Extended PHY Specific Control Register */
#define MIIM_88E1111_PHY_EXT_CR    0x14
#define MIIM_88E1111_RX_DELAY    0x80
#define MIIM_88E1111_TX_DELAY    0x2

/* 88E1111 Extended PHY Specific Status Register */
#define MIIM_88E1111_PHY_EXT_SR    0x1b
#define MIIM_88E1111_HWCFG_MODE_MASK    0xf
#define MIIM_88E1111_HWCFG_MODE_COPPER_RGMII  0xb
#define MIIM_88E1111_HWCFG_MODE_FIBER_RGMII  0x3
#define MIIM_88E1111_HWCFG_MODE_SGMII_NO_CLK  0x4
#define MIIM_88E1111_HWCFG_MODE_COPPER_RTBI  0x9
#define MIIM_88E1111_HWCFG_FIBER_COPPER_AUTO  0x8000
#define MIIM_88E1111_HWCFG_FIBER_COPPER_RES  0x2000

#define MIIM_88E1111_COPPER    0
#define MIIM_88E1111_FIBER    1

/* 88E1118 PHY defines */
#define MIIM_88E1118_PHY_PAGE    22
#define MIIM_88E1118_PHY_LED_PAGE  3

/* 88E1121 PHY LED Control Register */
#define MIIM_88E1121_PHY_LED_CTRL  16
#define MIIM_88E1121_PHY_LED_PAGE  3
#define MIIM_88E1121_PHY_LED_DEF  0x0030

/* 88E1121 PHY IRQ Enable/Status Register */
#define MIIM_88E1121_PHY_IRQ_EN    18
#define MIIM_88E1121_PHY_IRQ_STATUS  19

#define MIIM_88E1121_PHY_PAGE    22

/* 88E1145 Extended PHY Specific Control Register */
#define MIIM_88E1145_PHY_EXT_CR 20
#define MIIM_M88E1145_RGMII_RX_DELAY  0x0080
#define MIIM_M88E1145_RGMII_TX_DELAY  0x0002

#define MIIM_88E1145_PHY_LED_CONTROL  24
#define MIIM_88E1145_PHY_LED_DIRECT  0x4100

#define MIIM_88E1145_PHY_PAGE  29
#define MIIM_88E1145_PHY_CAL_OV 30

#define MIIM_88E1149_PHY_PAGE  29

/* 88E1310 PHY defines */
#define MIIM_88E1310_PHY_LED_CTRL  16
#define MIIM_88E1310_PHY_IRQ_EN    18
#define MIIM_88E1310_PHY_RGMII_CTRL  21
#define MIIM_88E1310_PHY_PAGE    22

typedef enum {
  MV_PHY_DEVICE_1512
} MV_PHY_DEVICE_ID;

typedef
EFI_STATUS
(*MV_PHY_DEVICE_INIT) (
    IN CONST MARVELL_PHY_PROTOCOL *Snp,
    IN UINT32 PhyAddr,
    IN OUT PHY_DEVICE *PhyDev
    );

typedef struct {
  MV_PHY_DEVICE_ID DevId;
  MV_PHY_DEVICE_INIT DevInit;
} MV_PHY_DEVICE;

STATIC
EFI_STATUS
MvPhyInit1512 (
    IN CONST MARVELL_PHY_PROTOCOL *Snp,
    IN UINT32 PhyAddr,
    IN OUT PHY_DEVICE *PhyDev
    );

#endif
