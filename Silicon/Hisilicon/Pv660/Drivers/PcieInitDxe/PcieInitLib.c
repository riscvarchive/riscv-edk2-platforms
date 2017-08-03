/** @file
*
*  Copyright (c) 2016, Hisilicon Limited. All rights reserved.
*  Copyright (c) 2016, Linaro Limited. All rights reserved.
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

#include "PcieInitLib.h"
#include <Library/DebugLib.h>
#include <Library/ArmLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/IoLib.h>
#include <Library/TimerLib.h>

static PCIE_INIT_CFG mPcieIntCfg;
UINT64 pcie_subctrl_base[2] = {0xb0000000, BASE_4TB + 0xb0000000};
UINT64 pcie_serders_base[2][4] = {{0xB2080000,0xB2000000,0xB2100000,0xB2200000},{BASE_4TB + 0xB2080000,BASE_4TB + 0xB2000000,BASE_4TB + 0xB2100000,BASE_4TB + 0xB2200000}};
UINT64 io_sub0_base = 0xa0000000;
UINT64 PCIE_APB_SLVAE_BASE[2] = {0xb0070000, BASE_4TB + 0xb0070000};
#define PCIE_REG_BASE(HostBridgeNum,port)              (PCIE_APB_SLVAE_BASE[HostBridgeNum] + (UINT32)(port * 0x10000))
UINT32 loop_test_flag[4] = {0,0,0,0};
UINT64 pcie_dma_des_base = PCIE_ADDR_BASE_HOST_ADDR;
#define PCIE_GEN1 0    /* PCIE 1.0 */
#define  PCIE_GEN2 1    /* PCIE 2.0 */
#define  PCIE_GEN3 2    /* PCIE 3.0 */
#define DS_API(lane)           ((0x1FF6c + 8*(15-lane))*2)

extern PCIE_DRIVER_CFG gastr_pcie_driver_cfg;
extern PCIE_IATU gastr_pcie_iatu_cfg;
extern PCIE_IATU_VA mPcieIatuTable;

VOID PcieRegWrite(UINT32 Port, UINTN Offset, UINT32 Value)
{
    RegWrite((UINT64)mPcieIntCfg.RegResource[Port] + Offset, Value);

}

UINT32 PcieRegRead(UINT32 Port, UINTN Offset)
{
    UINT32 Value = 0;

    RegRead((UINT64)mPcieIntCfg.RegResource[Port] + Offset, Value);
    return Value;
}

VOID PcieMmioWrite(UINT32 Port, UINTN Offset0, UINTN Offset1, UINT32 Value)
{
    RegWrite((UINT64)mPcieIntCfg.CfgResource[Port] + Offset0 + Offset1, Value);
}

UINT32 PcieMmioRead(UINT32 Port, UINTN Offset0, UINTN Offset1)
{
    UINT32 Value = 0;
    RegRead((UINT64)mPcieIntCfg.CfgResource[Port] + Offset0 + Offset1, Value);
    return Value;
}

VOID PcieChangeRwMode(UINT32 HostBridgeNum, UINT32 Port, PCIE_RW_MODE Mode)
{
    u_sc_pcie0_clkreq pcie0;
    u_sc_pcie1_clkreq pcie1;
    u_sc_pcie2_clkreq pcie2;
    u_sc_pcie3_clkreq pcie3;

    switch(Port)
    {
        case 0:
            RegRead(pcie_subctrl_base[HostBridgeNum] + PCIE_SUBCTRL_SC_PCIE0_CLKREQ_REG, pcie0.UInt32);
            pcie0.Bits.pcie0_apb_cfg_sel = Mode;
            RegWrite(pcie_subctrl_base[HostBridgeNum] + PCIE_SUBCTRL_SC_PCIE0_CLKREQ_REG, pcie0.UInt32);
            break;
        case 1:
            RegRead(pcie_subctrl_base[HostBridgeNum] + PCIE_SUBCTRL_SC_PCIE1_CLKREQ_REG, pcie1.UInt32);
            pcie1.Bits.pcie1_apb_cfg_sel = Mode;
            RegWrite(pcie_subctrl_base[HostBridgeNum] + PCIE_SUBCTRL_SC_PCIE1_CLKREQ_REG, pcie1.UInt32);
            break;
        case 2:
            RegRead(pcie_subctrl_base[HostBridgeNum] + PCIE_SUBCTRL_SC_PCIE2_CLKREQ_REG, pcie2.UInt32);
            pcie2.Bits.pcie2_apb_cfg_sel = Mode;
            RegWrite(pcie_subctrl_base[HostBridgeNum] + PCIE_SUBCTRL_SC_PCIE2_CLKREQ_REG, pcie2.UInt32);
            break;
        case 3:
            RegRead(pcie_subctrl_base[HostBridgeNum] + PCIE_SUBCTRL_SC_PCIE3_CLKREQ_REG, pcie3.UInt32);
            pcie3.Bits.pcie3_apb_cfg_sel = Mode;
            RegWrite(pcie_subctrl_base[HostBridgeNum] + PCIE_SUBCTRL_SC_PCIE3_CLKREQ_REG, pcie3.UInt32);
            break;
        default:
            break;
    }
}



EFI_STATUS PcieEnableItssm(UINT32 HostBridgeNum, UINT32 Port)
{
    PCIE_CTRL_7_U pcie_ctrl7;

    if(Port >= PCIE_MAX_PORT_NUM)
    {
        return EFI_INVALID_PARAMETER;
    }

    if(mPcieIntCfg.PortIsInitilized[Port])
    {
        return PCIE_ERR_ALREADY_INIT;
    }

    PcieChangeRwMode(HostBridgeNum, Port, PCIE_SYS_CONTROL);

    pcie_ctrl7.UInt32 = PcieRegRead(Port, PCIE_CTRL_7_REG);
    pcie_ctrl7.Bits.pcie2_app_ltssm_enable = 0x1;
    PcieRegWrite(Port, PCIE_CTRL_7_REG, pcie_ctrl7.UInt32);

    PcieChangeRwMode(HostBridgeNum, Port, PCIE_CONFIG_REG);

    return EFI_SUCCESS;
}

EFI_STATUS PcieDisableItssm(UINT32 HostBridgeNum, UINT32 Port)
{
    PCIE_CTRL_7_U pcie_ctrl7;

    if(Port >= PCIE_MAX_PORT_NUM)
    {
        return PCIE_ERR_PARAM_INVALID;
    }


    if(mPcieIntCfg.PortIsInitilized[Port])
    {
        return PCIE_ERR_ALREADY_INIT;
    }

    PcieChangeRwMode(HostBridgeNum, Port, PCIE_SYS_CONTROL);

    pcie_ctrl7.UInt32 = PcieRegRead(Port, PCIE_CTRL_7_REG);
    pcie_ctrl7.Bits.pcie2_app_ltssm_enable = 0x0;
    PcieRegWrite(Port,PCIE_CTRL_7_REG, pcie_ctrl7.UInt32);

    PcieChangeRwMode(HostBridgeNum, Port, PCIE_CONFIG_REG);

    return EFI_SUCCESS;
}


EFI_STATUS PcieLinkSpeedSet(UINT32 Port,PCIE_PORT_GEN Speed)
{
    PCIE_EP_PCIE_CAP12_U pcie_cap12;

    if(Port >= PCIE_MAX_PORT_NUM)
    {
        return EFI_INVALID_PARAMETER;
    }

    pcie_cap12.UInt32 = PcieRegRead(Port, PCIE_EP_PCIE_CAP12_REG);
    pcie_cap12.Bits.targetlinkspeed = Speed;
    PcieRegWrite(Port, PCIE_EP_PCIE_CAP12_REG, pcie_cap12.UInt32);

    if(mPcieIntCfg.Dev[Port].PcieDevice.PortInfo.PortType == PCIE_NTB_TO_NTB ||
        mPcieIntCfg.Dev[Port].PcieDevice.PortInfo.PortType == PCIE_NTB_TO_RP)
    {
        pcie_cap12.UInt32 = PcieMmioRead(Port, PCIE_MMIO_EEP_CFG, PCIE_EP_PCIE_CAP12_REG);
        pcie_cap12.Bits.targetlinkspeed = Speed;
        PcieMmioWrite(Port, PCIE_MMIO_EEP_CFG, PCIE_EP_PCIE_CAP12_REG, pcie_cap12.UInt32);
    }
    return EFI_SUCCESS;
}

EFI_STATUS PcieLinkWidthSet(UINT32 Port, PCIE_PORT_WIDTH Width)
{
    PCIE_EP_PORT_LOGIC4_U pcie_logic4;
    PCIE_EP_PORT_LOGIC22_U logic22;

    if(Port >= PCIE_MAX_PORT_NUM)
    {
        return PCIE_ERR_PARAM_INVALID;
    }

    pcie_logic4.UInt32 = PcieRegRead(Port, PCIE_EP_PORT_LOGIC4_REG);
    pcie_logic4.Bits.linkmodeenable = Width;
    pcie_logic4.Bits.crosslinkenable = 0;
    pcie_logic4.Bits.fastlinkmode = 1;
    PcieRegWrite(Port, PCIE_EP_PORT_LOGIC4_REG, pcie_logic4.UInt32);

    logic22.UInt32 = PcieRegRead(Port, PCIE_EP_PORT_LOGIC22_REG);
    logic22.Bits.n_fts = 0xff;
    if(Width == PCIE_WITDH_X1)
    {
        logic22.Bits.pre_determ_num_of_lane = 1;
    }
    else if(Width == PCIE_WITDH_X2)
    {
        logic22.Bits.pre_determ_num_of_lane = 2;
    }
    else
    {
        logic22.Bits.pre_determ_num_of_lane = 3;
    }
    PcieRegWrite(Port, PCIE_EP_PORT_LOGIC22_REG, logic22.UInt32);

    if(mPcieIntCfg.Dev[Port].PcieDevice.PortInfo.PortType == PCIE_NTB_TO_NTB ||
        mPcieIntCfg.Dev[Port].PcieDevice.PortInfo.PortType == PCIE_NTB_TO_RP)
    {
        pcie_logic4.UInt32 = PcieMmioRead(Port, PCIE_MMIO_EEP_CFG, PCIE_EP_PORT_LOGIC4_REG);
        pcie_logic4.Bits.linkmodeenable = Width;
        pcie_logic4.Bits.crosslinkenable = 0;
        pcie_logic4.Bits.fastlinkmode = 1;
        PcieMmioWrite(Port, PCIE_MMIO_EEP_CFG, PCIE_EP_PORT_LOGIC4_REG, pcie_logic4.UInt32);

        logic22.UInt32 = PcieMmioRead(Port, PCIE_MMIO_EEP_CFG, PCIE_EP_PORT_LOGIC22_REG);
        logic22.Bits.n_fts = 0xff;
        if(Width == PCIE_WITDH_X1)
        {
            logic22.Bits.pre_determ_num_of_lane = 1;
        }
        else if(Width == PCIE_WITDH_X2)
        {
            logic22.Bits.pre_determ_num_of_lane = 2;
        }
        else
        {
            logic22.Bits.pre_determ_num_of_lane = 3;
        }
        PcieMmioWrite(Port,PCIE_MMIO_EEP_CFG, PCIE_EP_PORT_LOGIC22_REG, logic22.UInt32);
    }
    return EFI_SUCCESS;
}

EFI_STATUS PcieSetupRC(UINT32 Port, PCIE_PORT_WIDTH Width)
{
    PCIE_EP_PORT_LOGIC22_U logic22;
    PCIE_EEP_PCI_CFG_HDR15_U hdr15;
    UINT32 Value = 0;
    if(Port >= PCIE_MAX_PORT_NUM)
    {
        return EFI_INVALID_PARAMETER;
    }

    Value = PcieRegRead(Port, PCIE_EP_PORT_LOGIC4_REG);
    Value &= ~(0x3f<<16);

    if(Width == PCIE_WITDH_X1)
    {
        Value |= (0x1 << 16);
    }
    else if(Width == PCIE_WITDH_X2)
    {
        Value |= (0x3 << 16);
    }
    else if(Width == PCIE_WITDH_X4)
    {
        Value |= (0x7 << 16);
    }
    else if(Width == PCIE_WITDH_X8)
    {
        Value |= (0xf << 16);
    }
    else
    {
        DEBUG((EFI_D_ERROR,"Width is not invalid\n"));
    }

    PcieRegWrite(Port, PCIE_EP_PORT_LOGIC4_REG, Value);

    logic22.UInt32 = PcieRegRead(Port, PCIE_EP_PORT_LOGIC22_REG);
    logic22.UInt32 &= ~(0x1f<<8);

    if(Width == PCIE_WITDH_X1)
    {
        logic22.Bits.pre_determ_num_of_lane = 1;
    }
    else if(Width == PCIE_WITDH_X2)
    {
        logic22.Bits.pre_determ_num_of_lane = 2;
    }
    else if(Width == PCIE_WITDH_X4)
    {
        logic22.Bits.pre_determ_num_of_lane = 4;
    }
    else if(Width == PCIE_WITDH_X8)
    {
        logic22.Bits.pre_determ_num_of_lane = 8;
    }
    else
    {
        DEBUG((EFI_D_ERROR,"Width is not invalid\n"));
    }

    PcieRegWrite(Port, PCIE_EP_PORT_LOGIC22_REG, logic22.UInt32);

    /* setup RC BARs */
    PcieRegWrite(Port, PCIE_EP_PCI_CFG_HDR4_REG, 0x00000004);
    PcieRegWrite(Port, PCIE_EP_PCI_CFG_HDR5_REG, 0x00000000);

    /* setup interrupt pins */
    hdr15.UInt32 = PcieRegRead(Port, PCIE_EP_PCI_CFG_HDR15_REG);
    hdr15.UInt32 &= 0xffff00ff;
    hdr15.UInt32 |= 0x00000100;
    PcieRegWrite(Port, PCIE_EP_PCI_CFG_HDR15_REG, hdr15.UInt32);

    /* setup bus numbers */
    Value = PcieRegRead(Port, PCIE_EP_PCI_CFG_HDR6_REG);
    Value &= 0xff000000;
    Value |= 0x00010100;
    PcieRegWrite(Port, PCIE_EP_PCI_CFG_HDR6_REG, Value);

    /* setup command register */
    Value = PcieRegRead(Port, PCIE_EP_PCI_CFG_HDR1_REG);
    Value &= 0xffff0000;
    Value |= 0x1|0x2|0x4|0x100;
    PcieRegWrite(Port, PCIE_EP_PCI_CFG_HDR1_REG, Value);

    return EFI_SUCCESS;
}


EFI_STATUS PcieModeSet(UINT32 HostBridgeNum, UINT32 Port, PCIE_PORT_TYPE PcieType)
{
    PCIE_CTRL_0_U str_pcie_ctrl_0;

    if(Port >= PCIE_MAX_PORT_NUM)
    {
        return EFI_INVALID_PARAMETER;
    }

    if(mPcieIntCfg.PortIsInitilized[Port])
    {
        return PCIE_ERR_ALREADY_INIT;
    }

    PcieChangeRwMode(HostBridgeNum, Port, PCIE_SYS_CONTROL);

    str_pcie_ctrl_0.UInt32 = PcieRegRead(Port, PCIE_CTRL_0_REG);
    if(PcieType == PCIE_END_POINT)
    {
        str_pcie_ctrl_0.Bits.pcie2_slv_device_type = PCIE_EP_DEVICE;
    }
    else
    {
        str_pcie_ctrl_0.Bits.pcie2_slv_device_type = RP_OF_PCIE_RC;
    }
    PcieRegWrite(Port, PCIE_CTRL_0_REG, str_pcie_ctrl_0.UInt32);

    PcieChangeRwMode(HostBridgeNum, Port, PCIE_CONFIG_REG);

    return EFI_SUCCESS;
}

VOID PciePcsInit(UINT32 HostBridgeNum, UINT32 Port)
{

    if(Port<=2)
    {
        RegWrite(pcie_serders_base[HostBridgeNum][Port] + 0xc088, 0x212);
        RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0x8020, 0x2026044);
        RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0x8060, 0x2126044);
        RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0x80c4, 0x2126044);
        RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0x80e4, 0x2026044);

        RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0x80a0, 0x4018);
        RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0x80a4, 0x804018);
        RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0x80c0, 0x11201100);
        RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0x15c, 0x3);
        RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0x158, 0);
    }
    else

    {
        RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0x74, 0x46e000);
        RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0x78, 0x46e000);
        RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0x7c, 0x46e000);
        RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0x80, 0x46e000);
        RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0x84, 0x46e000);
        RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0x88, 0x46e000);
        RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0x8c, 0x46e000);
        RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0x90, 0x46e000);

        RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0x34, 0x1001);
        RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0x38, 0x1001);
        RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0x3c, 0x1001);
        RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0x40, 0x1001);
        RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0x44, 0x1001);
        RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0x48, 0x1001);
        RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0x4c, 0x1001);
        RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0x50, 0x1001);

        RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0xe4, 0xffff);
    }

}

VOID PcieEqualization(UINT32 Port)
{
    UINT32  Value;

    PcieRegWrite(Port, 0x890, 0x1400);
    PcieRegWrite(Port, 0x894, 0xfd7);

    PcieRegWrite(Port, 0x89c, 0x0);
    PcieRegWrite(Port, 0x898, 0xfc00);
    PcieRegWrite(Port, 0x89c, 0x1);
    PcieRegWrite(Port, 0x898, 0xbd00);
    PcieRegWrite(Port, 0x89c, 0x2);
    PcieRegWrite(Port, 0x898, 0xccc0);
    PcieRegWrite(Port, 0x89c, 0x3);
    PcieRegWrite(Port, 0x898, 0x8dc0);
    PcieRegWrite(Port, 0x89c, 0x4);
    PcieRegWrite(Port, 0x898, 0xfc0);
    PcieRegWrite(Port, 0x89c, 0x5);
    PcieRegWrite(Port, 0x898, 0xe46);
    PcieRegWrite(Port, 0x89c, 0x6);
    PcieRegWrite(Port, 0x898, 0xdc8);
    PcieRegWrite(Port, 0x89c, 0x7);
    PcieRegWrite(Port, 0x898, 0xcb46);
    PcieRegWrite(Port, 0x89c, 0x8);
    PcieRegWrite(Port, 0x898, 0x8c07);
    PcieRegWrite(Port, 0x89c, 0x9);
    PcieRegWrite(Port, 0x898, 0xd0b);
    PcieRegWrite(Port, 0x8a8, 0x103ff21);

    Value = PcieRegRead(Port, 0x80);
    Value |= 0x80;
    PcieRegWrite(Port, 0x80, Value);

    PcieRegWrite(Port, 0x184, 0x44444444);
    PcieRegWrite(Port, 0x188, 0x44444444);
    PcieRegWrite(Port, 0x18c, 0x44444444);
    PcieRegWrite(Port, 0x190, 0x44444444);

}


EFI_STATUS PciePortReset(UINT32 HostBridgeNum, UINT32 Port)
{
    if(Port >= PCIE_MAX_PORT_NUM)
    {
        return EFI_INVALID_PARAMETER;
    }

    if(PcieIsLinkUp(HostBridgeNum, Port) && mPcieIntCfg.PortIsInitilized[Port])
    {
        (VOID)PcieDisableItssm(HostBridgeNum, Port);
    }

    mPcieIntCfg.PortIsInitilized[Port] = FALSE;
    mPcieIntCfg.DmaResource[Port] = (VOID *)NULL;
    mPcieIntCfg.DmaChannel[Port][PCIE_DMA_CHANLE_READ] = 0;
    mPcieIntCfg.DmaChannel[Port][PCIE_DMA_CHANLE_WRITE] = 0;
    ZeroMem(&mPcieIntCfg.Dev[Port], sizeof(DRIVER_CFG_U));

    if(Port <= 2)
    {
        RegWrite(pcie_subctrl_base[HostBridgeNum]+ PCIE_SUBCTRL_SC_PCIE0_RESET_REQ_REG + (UINT32)(8 * Port), 0x1);
        MicroSecondDelay(0x1000);

        RegWrite(pcie_subctrl_base[HostBridgeNum] + PCIE_SUBCTRL_SC_PCIE0_RESET_DREQ_REG + (UINT32)(8 * Port), 0x1);
        MicroSecondDelay(0x1000);
    }
    else
    {
        RegWrite(pcie_subctrl_base[HostBridgeNum] + PCIE_SUBCTRL_SC_PCIE3_RESET_REQ_REG,0x1);
        MicroSecondDelay(0x1000);

        RegWrite(pcie_subctrl_base[HostBridgeNum] + PCIE_SUBCTRL_SC_PCIE3_RESET_DREQ_REG,0x1);
        MicroSecondDelay(0x1000);
    }
    return EFI_SUCCESS;
}

EFI_STATUS AssertPcieCoreReset(UINT32 HostBridgeNum, UINT32 Port)
{
    if(Port >= PCIE_MAX_PORT_NUM)
    {
        return EFI_INVALID_PARAMETER;
    }

    if(PcieIsLinkUp(HostBridgeNum, Port) && mPcieIntCfg.PortIsInitilized[Port])
    {
        (VOID)PcieDisableItssm(HostBridgeNum, Port);
    }

    mPcieIntCfg.PortIsInitilized[Port] = FALSE;
    mPcieIntCfg.DmaResource[Port] = (VOID *)NULL;
    mPcieIntCfg.DmaChannel[Port][PCIE_DMA_CHANLE_READ] = 0;
    mPcieIntCfg.DmaChannel[Port][PCIE_DMA_CHANLE_WRITE] = 0;
    ZeroMem(&mPcieIntCfg.Dev[Port], sizeof(DRIVER_CFG_U));

    if(Port <= 2)
    {
        RegWrite(pcie_subctrl_base[HostBridgeNum] + PCIE_SUBCTRL_SC_PCIE0_RESET_REQ_REG + (UINT32)(8 * Port), 0x1);
        MicroSecondDelay(0x1000);
    }
    else
    {
        RegWrite(pcie_subctrl_base[HostBridgeNum] + PCIE_SUBCTRL_SC_PCIE3_RESET_REQ_REG,0x1);
        MicroSecondDelay(0x1000);
    }
    return EFI_SUCCESS;
}

EFI_STATUS DeassertPcieCoreReset(UINT32 HostBridgeNum, UINT32 Port)
{
    if(Port >= PCIE_MAX_PORT_NUM)
    {
        return EFI_INVALID_PARAMETER;
    }

    if(PcieIsLinkUp(HostBridgeNum, Port) && mPcieIntCfg.PortIsInitilized[Port])
    {
        (VOID)PcieDisableItssm(HostBridgeNum, Port);
    }

    mPcieIntCfg.PortIsInitilized[Port] = FALSE;
    mPcieIntCfg.DmaResource[Port] = (VOID *)NULL;
    mPcieIntCfg.DmaChannel[Port][PCIE_DMA_CHANLE_READ] = 0;
    mPcieIntCfg.DmaChannel[Port][PCIE_DMA_CHANLE_WRITE] = 0;
    ZeroMem(&mPcieIntCfg.Dev[Port], sizeof(DRIVER_CFG_U));

    if(Port <= 2)
    {
        RegWrite(pcie_subctrl_base[HostBridgeNum] + PCIE_SUBCTRL_SC_PCIE0_RESET_DREQ_REG + (UINT32)(8 * Port), 0x1);
        MicroSecondDelay(0x1000);
    }
    else
    {
        RegWrite(pcie_subctrl_base[HostBridgeNum] + PCIE_SUBCTRL_SC_PCIE3_RESET_DREQ_REG,0x1);
        MicroSecondDelay(0x1000);
    }
    return EFI_SUCCESS;
}

EFI_STATUS AssertPciePcsReset(UINT32 HostBridgeNum,UINT32 Port)
{
    u_sc_pcie_hilink_pcs_reset_req reset_req;
    UINT32 pcs_local_reset_status;
    UINT32 pcs_local_status_checked;
    UINT32 hilink_reset_status;
    UINT32 hilink_status_checked;
    UINT32 count = 0;

    reset_req.UInt32 = 0;
    reset_req.UInt32 = reset_req.UInt32 | (0x1 << Port);
    RegWrite(pcie_subctrl_base[HostBridgeNum] + PCIE_SUBCTRL_SC_PCS_LOCAL_RESET_REQ_REG, reset_req.UInt32);

    reset_req.UInt32 = 0;
    reset_req.UInt32 = reset_req.UInt32 | (0xFF << (8 * Port));
    RegWrite(pcie_subctrl_base[HostBridgeNum] + PCIE_SUBCTRL_SC_PCIE_HILINK_PCS_RESET_REQ_REG, reset_req.UInt32);
    MicroSecondDelay(0x1000);

  /* read reset status, make sure pcs is reset */
  do {
        MicroSecondDelay(1000);
        count ++;
        RegRead(pcie_subctrl_base[HostBridgeNum] + PCIE_SUBCTRL_SC_PCS_LOCAL_RESET_ST_REG, pcs_local_reset_status);
    pcs_local_status_checked =
        ((pcs_local_reset_status & (1 << Port)) !=
         (1 << Port));

  } while ((pcs_local_status_checked) && (count < 1000));

  if (pcs_local_status_checked)
        DEBUG((EFI_D_ERROR, "pcs local reset status read failed\n"));

    count = 0;
  do {
        MicroSecondDelay(1000);
        count ++;
        RegRead(pcie_subctrl_base[HostBridgeNum] + PCIE_SUBCTRL_SC_PCIE_HILINK_PCS_RESET_ST_REG, hilink_reset_status);
    hilink_status_checked =
        ((hilink_reset_status & (0xff << (Port << 3))) !=
         (0xff << (Port << 3)));
  } while ((hilink_status_checked) && (count < 1000));

  if (hilink_status_checked)
        DEBUG((EFI_D_ERROR, "error:pcs assert reset failed\n"));

    return EFI_SUCCESS;
}

EFI_STATUS DeassertPciePcsReset(UINT32 HostBridgeNum, UINT32 Port)
{
    u_sc_pcie_hilink_pcs_reset_req reset_req;
    UINT32 pcs_local_status;
    UINT32 pcs_local_status_checked;
    UINT32 hilink_reset_status;
    UINT32 hilink_status_checked;
    UINT32 count = 0;


    reset_req.UInt32 = 0;
    reset_req.UInt32 = reset_req.UInt32 | (0x1 << Port);
    RegWrite(pcie_subctrl_base[HostBridgeNum] + PCIE_SUBCTRL_SC_PCS_LOCAL_RESET_DREQ_REG, reset_req.UInt32);

    reset_req.UInt32 = 0;
    reset_req.UInt32 = reset_req.UInt32 | (0xFF << (8 * Port));
    RegWrite(pcie_subctrl_base[HostBridgeNum] + PCIE_SUBCTRL_SC_PCIE_HILINK_PCS_RESET_DREQ_REG, reset_req.UInt32);

  /* read reset status, make sure pcs is deassert */
  do {
        MicroSecondDelay(1000);
        count ++;
        RegRead(pcie_subctrl_base[HostBridgeNum] + PCIE_SUBCTRL_SC_PCS_LOCAL_RESET_ST_REG, pcs_local_status);
    pcs_local_status_checked = (pcs_local_status & (1 << Port));
  } while ((pcs_local_status_checked) && (count < 1000));

  /* get a timeout error */
  if (pcs_local_status_checked)
        DEBUG((EFI_D_ERROR, "pcs deassert reset failed!\n"));

    count = 0;
  do {
        MicroSecondDelay(1000);
        RegRead(pcie_subctrl_base[HostBridgeNum] + PCIE_SUBCTRL_SC_PCIE_HILINK_PCS_RESET_ST_REG, hilink_reset_status);
    hilink_status_checked = (hilink_reset_status &
           (0xff << (Port << 3)));
  } while ((hilink_status_checked) && (count < 1000));

  if (hilink_status_checked)
        DEBUG((EFI_D_ERROR, "pcs deassert reset failed!\n"));

    return EFI_SUCCESS;
}

VOID PcieGen3Config(UINT32 HostBridgeNum, UINT32 Port)
{
  UINT32 val;
    UINT32 current_speed;
  UINT32 ltssm_state;
  UINT32 timeout = 0;
  UINT32 eq = 0;
  UINT32 loop = 100000;
    U_SC_PCIE0_SYS_STATE4      PcieStat;

  while (loop)
  {
      MicroSecondDelay(10);
      RegRead(pcie_subctrl_base[HostBridgeNum] + PCIE_SUBCTRL_SC_PCIE0_SYS_STATE4_REG + (UINT32)(0x100 * Port), PcieStat.UInt32);
        val = PcieStat.UInt32;
      current_speed = (val >> 6) & 0x3;
      if (current_speed == PCIE_GEN3)
        break;
      loop--;
  }
  if (!loop) {
        DEBUG((EFI_D_ERROR, "current_speed GEN%d\n",current_speed + 1));
    return;
  }

  ltssm_state = val & PCIE_LTSSM_STATE_MASK;
  while ((current_speed == PCIE_GEN3) &&
         (ltssm_state != PCIE_LTSSM_LINKUP_STATE) && (timeout < 200)) {
    if ((ltssm_state & 0x30) == 0x20)
      eq = 1;

        if ((ltssm_state == 0xd) && (eq == 1))
        {
            MicroSecondDelay(5000);
      RegRead(pcie_subctrl_base[HostBridgeNum] + PCIE_SUBCTRL_SC_PCIE0_SYS_STATE4_REG + (UINT32)(0x100 * Port), PcieStat.UInt32);
            val = PcieStat.UInt32;
      ltssm_state = val & PCIE_LTSSM_STATE_MASK;
      current_speed = (val >> 6) & 0x3;
            if (ltssm_state == 0xd)
            {
                DEBUG((EFI_D_ERROR, "Do symbol align reset rate %d ltssm 0x%x\n",current_speed, ltssm_state));
                RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0x74, 0x8000000);
                RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0x78, 0x8000000);
                RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0x7c, 0x8000000);
                RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0x80, 0x8000000);
                RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0x84, 0x8000000);
                RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0x88, 0x8000000);
                RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0x8c, 0x8000000);
                RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0x90, 0x8000000);

                RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0x74, 0);
                RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0x78, 0);
                RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0x7c, 0);
                RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0x80, 0);
                RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0x84, 0);
                RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0x88, 0);
                RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0x8c, 0);
                RegWrite(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(Port * 0x10000) + 0x90, 0);
      }
            break;
    }

        RegRead(pcie_subctrl_base[HostBridgeNum] + PCIE_SUBCTRL_SC_PCIE0_SYS_STATE4_REG + (UINT32)(0x100 * Port), PcieStat.UInt32);
        val = PcieStat.UInt32;
    ltssm_state = val & PCIE_LTSSM_STATE_MASK;
    current_speed = (val >> 6) & 0x3;

    MicroSecondDelay(1000);
    timeout++;
  }

  if (timeout >= 200) {
    DEBUG((EFI_D_ERROR, "current_speed GEN%d\n",current_speed + 1));
    return;
  }
  DEBUG((EFI_D_ERROR, "current_speed GEN%d\n",current_speed + 1));
}

VOID Gen3DfeEnable(UINT32 HostBridgeNum, UINT32 Port)
{
  UINT32 val;
  UINT32 lane;
  UINT32 current_speed;
    U_SC_PCIE0_SYS_STATE4      PcieStat;

  if (Port == 3)
    return;
  RegRead(pcie_subctrl_base[HostBridgeNum] + PCIE_SUBCTRL_SC_PCIE0_SYS_STATE4_REG + (UINT32)(0x100 * Port), PcieStat.UInt32);
    val = PcieStat.UInt32;
  current_speed = (val >> 6) & 0x3;
  if (current_speed != PCIE_GEN3)
    return;
  for (lane = 0; lane < 8; lane++)
    RegWrite(pcie_serders_base[HostBridgeNum][Port] +  (UINT32)DS_API(lane) + 4, 0x3851);

    DEBUG((EFI_D_ERROR, "enable DFE success\n"));
}

EFI_STATUS HisiPcieClockCtrl(UINT32 soctype, UINT32 HostBridgeNum, UINT32 Port, BOOLEAN Clock)
{
  UINT32 reg_clock_disable;
  UINT32 reg_clock_enable;
  UINT32 reg_clock_status;
    UINT32 clock_status;
    UINT32 clock_status_checked;
    UINT32  clock_ctrl;
    UINT32 count = 0;

  if (Port == 3) {
    reg_clock_disable = PCIE_SUBCTRL_SC_PCIE3_CLK_DIS_REG;
    reg_clock_enable = PCIE_SUBCTRL_SC_PCIE3_CLK_EN_REG;
    reg_clock_status = PCIE_SUBCTRL_SC_PCIE3_CLK_ST_REG;
  } else {
    reg_clock_disable = PCIE_SUBCTRL_SC_PCIE0_2_CLK_DIS_REG(Port);
    reg_clock_enable = PCIE_SUBCTRL_SC_PCIE0_2_CLK_EN_REG(Port);
    reg_clock_status = PCIE_SUBCTRL_SC_PCIE0_2_CLK_ST_REG(Port);
  }

    if (0x1610 == soctype)
    {
        clock_ctrl = 0x7;
    }
    else
    {
        clock_ctrl = 0x3;
        if (Clock)
          RegWrite(pcie_subctrl_base[HostBridgeNum] + reg_clock_enable, clock_ctrl);
        else
          RegWrite(pcie_subctrl_base[HostBridgeNum] + reg_clock_disable, clock_ctrl);
    }

    do {
        count ++;
        MicroSecondDelay(1000);
        RegRead(pcie_subctrl_base[HostBridgeNum] + reg_clock_status, clock_status);
    if (Clock)
      clock_status_checked =
          ((clock_status & clock_ctrl) != clock_ctrl);
    else
      clock_status_checked =
          ((clock_status & clock_ctrl) != 0);
  } while ((clock_status_checked) && (count < 1000)); //1S

  /* get a timeout error */
  if (clock_status_checked)
        DEBUG((EFI_D_ERROR, "clock operation failed!\n"));

    return EFI_SUCCESS;
}

VOID PcieSpdSet(UINT32 soctype, UINT32 HostBridgeNum, UINT32 Port, UINT8 Spd)
{
    UINT32 Value = 0;

    if (0x1610 == soctype)
    {
    }
    else
    {
        RegRead(PCIE_APB_SLVAE_BASE[HostBridgeNum] + (UINT32)(Port * 0x10000) + 0xa0, Value);
        Value &= ~(0xf);
      Value |= Spd;
      RegWrite(PCIE_APB_SLVAE_BASE[HostBridgeNum] + (UINT32)(Port * 0x10000) + 0xa0, Value);
        return;
    }
    return;
}

VOID PcieSpdControl(UINT32 HostBridgeNum, UINT32 Port)
{
  UINT32 Value = 0;;

  /* set link width speed control register */
    RegRead(PCIE_APB_SLVAE_BASE[HostBridgeNum] + (UINT32)(Port * 0x10000) + 0x80c, Value);
  /*
   * set the Directed Speed Change field of the Link Width and Speed
   * Change Control register
   */
  Value |= (1 << 17);
    RegWrite( PCIE_APB_SLVAE_BASE[HostBridgeNum] + (UINT32)(Port * 0x10000) + 0x80c, Value);
}

VOID PcieSetDb2Enable(UINT32 HostBridgeNum, UINT32 Port, UINT32 enable)
{
    UINT32 dbi_ctrl;

    PcieChangeRwMode(HostBridgeNum, Port, PCIE_SYS_CONTROL);

    RegRead(PCIE_APB_SLVAE_BASE[HostBridgeNum] + (UINT32)(Port * 0x10000) + PCIE_SYS_CTRL20_REG, dbi_ctrl);
  if (enable)
    dbi_ctrl |= BIT0;
  else
    dbi_ctrl &= ~BIT0;
    RegWrite( PCIE_APB_SLVAE_BASE[HostBridgeNum] + (UINT32)(Port * 0x10000) + PCIE_SYS_CTRL20_REG, dbi_ctrl);

  PcieChangeRwMode(HostBridgeNum, Port, PCIE_CONFIG_REG);
}

VOID PcieDisabledBar0(UINT32 HostBridgeNum, UINT32 Port)
{
  PcieSetDb2Enable(HostBridgeNum, Port, PCIE_DBI_CS2_ENABLE);
    RegWrite( PCIE_APB_SLVAE_BASE[HostBridgeNum] + (UINT32)(Port * 0x10000) + 0x10,0);
  PcieSetDb2Enable(HostBridgeNum, Port, PCIE_DBI_CS2_DISABLE);
}

/* Configure vmid/asid table in PCIe host */
VOID PcieConfigContextP660(UINT32 HostBridgeNum, UINT32 Port)
{
  UINT32 i = 0;
  UINTN val = 0;;

  /*
   * enable to clean vmid and asid tables though apb bus
   * */
  PcieChangeRwMode(HostBridgeNum, Port, PCIE_SYS_CONTROL);
    RegRead(PCIE_APB_SLVAE_BASE[HostBridgeNum] + (UINT32)(Port * 0x10000) + PCIE_SYS_CTRL20_REG, val);
  /* enable ar channel */
  val |= PCIE_RD_TAB_SEL | PCIE_RD_TAB_EN;
  RegWrite( PCIE_APB_SLVAE_BASE[HostBridgeNum] + (UINT32)(Port * 0x10000) + PCIE_SYS_CTRL20_REG, val);

  PcieChangeRwMode(HostBridgeNum, Port, PCIE_SLV_CONTENT_MODE);
  for (i = 0; i < 0x800; i++)
        PcieRegWrite(Port, i * 4, 0);

  PcieChangeRwMode(HostBridgeNum, Port, PCIE_SYS_CONTROL);
  /* enable aw channel */
  val &= (~PCIE_RD_TAB_SEL);
  val |= PCIE_RD_TAB_EN;
  RegWrite( PCIE_APB_SLVAE_BASE[HostBridgeNum] + (UINT32)(Port * 0x10000) + PCIE_SYS_CTRL20_REG, val);

  PcieChangeRwMode(HostBridgeNum, Port, PCIE_SLV_CONTENT_MODE);

  /*
   * init vmid and asid tables for all PCIe devices as 0
   * vmid table: 0 ~ 0x3ff, asid table: 0x400 ~ 0x7ff
   */
  for (i = 0; i < 0x800; i++)
        PcieRegWrite(Port, i * 4, 0);

  PcieChangeRwMode(HostBridgeNum, Port, PCIE_SYS_CONTROL);

    RegRead(PCIE_APB_SLVAE_BASE[HostBridgeNum] + (UINT32)(Port * 0x10000) + PCIE_SYS_CTRL20_REG, val);
  /* disable ar channel */
  val |= PCIE_RD_TAB_SEL;
  val &= (~PCIE_RD_TAB_EN);
  RegWrite( PCIE_APB_SLVAE_BASE[HostBridgeNum] + (UINT32)(Port * 0x10000) + PCIE_SYS_CTRL20_REG, val);
  /* disable aw channel */
  val &= ((~PCIE_RD_TAB_SEL) & (~PCIE_RD_TAB_EN));
    RegWrite( PCIE_APB_SLVAE_BASE[HostBridgeNum] + (UINT32)(Port * 0x10000) + PCIE_SYS_CTRL20_REG, val);

  RegWrite( PCIE_APB_SLVAE_BASE[HostBridgeNum] + (UINT32)(Port * 0x10000) + PCIE_SYS_CTRL24_REG, 0xb7010040 & 0xffffffff);
    RegWrite( PCIE_APB_SLVAE_BASE[HostBridgeNum] + (UINT32)(Port * 0x10000) + PCIE_SYS_CTRL28_REG, 0);

    RegWrite( PCIE_APB_SLVAE_BASE[HostBridgeNum] + (UINT32)(Port * 0x10000) + PCIE_SYS_CTRL16_REG, (1<<12)|(1<<16));
    RegWrite( PCIE_APB_SLVAE_BASE[HostBridgeNum] + (UINT32)(Port * 0x10000) + PCIE_SYS_CTRL29_REG, (1<<12));

  PcieChangeRwMode(HostBridgeNum, Port, PCIE_CONFIG_REG);;
}

EFI_STATUS PcieMaskLinkUpInit(UINT32 HostBridgeNum, UINT32 Port)
{
    UINT32 Value = 0;
    PcieChangeRwMode(HostBridgeNum, Port, PCIE_SYS_CONTROL);
    Value = PcieRegRead(Port, 0x1d0);
    Value |= 1 << 12;
    PcieRegWrite(Port,0x1d0, Value);
    PcieChangeRwMode(HostBridgeNum, Port, PCIE_CONFIG_REG);
    return EFI_SUCCESS;
}

BOOLEAN PcieIsLinkUp(UINT32 HostBridgeNum, UINT32 Port)
{
    UINT32                     Value = 0;
    U_SC_PCIE0_SYS_STATE4      PcieStat;

    RegRead(pcie_subctrl_base[HostBridgeNum] + PCIE_SUBCTRL_SC_PCIE0_SYS_STATE4_REG + (UINT32)(0x100 * Port), PcieStat.UInt32);
    Value = PcieStat.UInt32;
    if ((Value & PCIE_LTSSM_STATE_MASK) == PCIE_LTSSM_LINKUP_STATE)
        return TRUE;

    return FALSE;
}

VOID PcieWriteOwnConfig(UINT32 Port, UINT32 Offset)
{
    UINT32 Value = 0;
    Value = PcieRegRead(Port,Offset & (~0x3));
    Value &= 0x0000ffff;
    Value |= 0x06040000;
    PcieRegWrite(Port, Offset & (~0x3), Value);
    return;
}

EFI_STATUS
EFIAPI
PciePortInit (
  IN UINT32                 HostBridgeNum,
  IN PCIE_DRIVER_CFG        *PcieCfg
  )
{
     UINT32              Count = 0;
     UINT32             PortIndex = PcieCfg->PortIndex;
     UINT32             Value = 0;


     if(PortIndex >= PCIE_MAX_PORT_NUM)
     {
        return EFI_INVALID_PARAMETER;
     }

     if(mPcieIntCfg.PortIsInitilized[PortIndex])
     {
        return PCIE_ERR_ALREADY_INIT;
     }

     mPcieIntCfg.RegResource[PortIndex] = (VOID *)(UINTN)PCIE_REG_BASE(HostBridgeNum, PortIndex);

     /* assert reset signals */
     (VOID)AssertPcieCoreReset(HostBridgeNum, PortIndex);
     (VOID)HisiPcieClockCtrl(0x660, HostBridgeNum, PortIndex, 0);
     (VOID)AssertPciePcsReset(HostBridgeNum, PortIndex);

     /* de-assert phy reset */
     (VOID)DeassertPciePcsReset(HostBridgeNum, PortIndex);

     /* de-assert core reset */
     (VOID)DeassertPcieCoreReset(HostBridgeNum, PortIndex);
     (VOID)HisiPcieClockCtrl(0x660, HostBridgeNum, PortIndex, 1);

     do {
    RegRead(pcie_subctrl_base[HostBridgeNum] + 0xc0000 + (UINT32)(PortIndex * 0x10000) + 0x8108, Value);
        if (Count == 10) {
            DEBUG((EFI_D_ERROR, "PCIe Failed! PLL Locked: 0x%x\n\n",Value));
      return EFI_NOT_READY;
        }
        Count++;
    MicroSecondDelay(100000);
   } while ((Value & 0x3) == 0);
   Count = 0;

     /* initialize phy */
     (VOID)PciePcsInit(HostBridgeNum, PortIndex);

     (VOID)PcieModeSet(HostBridgeNum, PortIndex,PcieCfg->PortInfo.PortType);
     (VOID)PcieSpdSet(0x660, HostBridgeNum, PortIndex, 3);
     (VOID)PcieSpdControl(HostBridgeNum, PortIndex);
     /* setup root complex */
     (VOID)PcieSetupRC(PortIndex,PcieCfg->PortInfo.PortWidth);

     /* Pcie Equalization*/
     (VOID)PcieEqualization(PortIndex);

     /* assert LTSSM enable */
     (VOID)PcieEnableItssm(HostBridgeNum, PortIndex);

     /*
      * This is a PCS hardware bug, we fix it by resetting
      * PCS symalign module state machine
     */
     (VOID)PcieGen3Config(HostBridgeNum, PortIndex);
     PcieConfigContextP660(HostBridgeNum, PortIndex);
     (VOID)PcieDisabledBar0(HostBridgeNum, PortIndex);
     (VOID)PcieWriteOwnConfig(PortIndex, 0xa);
     /* check if the link is up or not */
   while (!PcieIsLinkUp(HostBridgeNum, PortIndex)) {
         MicroSecondDelay(1000);
     Count++;
     if (Count >= 1000) {
      DEBUG((EFI_D_ERROR, "HostBridge %d, Port %d link up failed\n", HostBridgeNum, PortIndex));
      return PCIE_ERR_LINK_OVER_TIME;
     }
   }
     DEBUG((EFI_D_ERROR, "HostBridge %d, Port %d Link up ok\n", HostBridgeNum, PortIndex));

     /* dfe enable is just for 660 */
     (VOID)Gen3DfeEnable(HostBridgeNum, PortIndex);


     PcieRegWrite(PortIndex, 0x80c, 0x208FF);

     return EFI_SUCCESS;
}




EFI_STATUS PcieSetDBICS2Enable(UINT32 HostBridgeNum, UINT32 Port, UINT32 Enable)
{
    PCIE_SYS_CTRL20_U dbi_ro_enable;

    if(Port >= PCIE_MAX_PORT_NUM)
    {
        return EFI_INVALID_PARAMETER;
    }

    PcieChangeRwMode(HostBridgeNum, Port, PCIE_SYS_CONTROL);
    dbi_ro_enable.UInt32 = PcieRegRead(Port, PCIE_SYS_CTRL20_REG);
    dbi_ro_enable.Bits.ro_sel = Enable;
    PcieRegWrite(Port, PCIE_SYS_CTRL20_REG, dbi_ro_enable.UInt32);
    PcieChangeRwMode(HostBridgeNum, Port, PCIE_CONFIG_REG);

    return EFI_SUCCESS;

}

VOID PcieDelay(UINT32 dCount)
{
    volatile UINT32 *uwCnt = &dCount;

    while(*uwCnt > 0)
    {
        *uwCnt = *uwCnt - 1;
    }

}

