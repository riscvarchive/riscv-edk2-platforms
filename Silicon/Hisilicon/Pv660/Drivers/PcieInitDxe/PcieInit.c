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


#include "PcieInit.h"
#include <Library/UefiBootServicesTableLib.h>
#include <Library/PcdLib.h>

extern VOID PcieRegWrite(UINT32 Port, UINTN Offset, UINT32 Value);
extern EFI_STATUS PciePortReset(UINT32 HostBridgeNum, UINT32 Port);
extern EFI_STATUS PciePortInit (UINT32 HostBridgeNum, PCIE_DRIVER_CFG *PcieCfg);

PCIE_DRIVER_CFG gastr_pcie_driver_cfg[PCIE_MAX_PORT_NUM] =
{
    //Port 0
    {
        0x0,                        //Portindex

        {
            PCIE_ROOT_COMPLEX,      //PortType
            PCIE_WITDH_X8,          //PortWidth
            PCIE_GEN3_0,            //PortGen
        }, //PortInfo

    },

    //Port 1
    {
        0x1,                        //Portindex
        {
            PCIE_ROOT_COMPLEX,      //PortType
            PCIE_WITDH_X8,          //PortWidth
            PCIE_GEN3_0,            //PortGen
        },

    },

    //Port 2
    {
        0x2,                        //Portindex
        {
            PCIE_ROOT_COMPLEX,      //PortType
            PCIE_WITDH_X8,          //PortWidth
            PCIE_GEN3_0,            //PortGen
        },

    },

    //Port 3
    {
        0x3,                        //Portindex
        {
            PCIE_ROOT_COMPLEX,      //PortType
            PCIE_WITDH_X8,          //PortWidth
            PCIE_GEN3_0,            //PortGen
        },

    },
};

EFI_STATUS
PcieInitEntry (
  IN EFI_HANDLE                 ImageHandle,
  IN EFI_SYSTEM_TABLE           *SystemTable
  )

{
    UINT32             Port;
    EFI_STATUS         Status = EFI_SUCCESS;
    UINT32             HostBridgeNum = 0;

    for (HostBridgeNum = 0; HostBridgeNum < PCIE_HOST_BRIDGE_NUM; HostBridgeNum++)
    {
        for (Port = 0; Port < PCIE_MAX_PORT_NUM; Port++)
        {
            if (!((((PcdGet32(PcdPcieRootBridgeMask) >> (4 * HostBridgeNum))) >> Port) & 0x1))
            {
                continue;
            }

            Status = PciePortInit(HostBridgeNum, &gastr_pcie_driver_cfg[Port]);
            if(EFI_ERROR(Status))
            {
                DEBUG((EFI_D_ERROR, "HostBridge %d, Pcie Port %d Init Failed! \n", HostBridgeNum, Port));
            }

        }
    }

    return EFI_SUCCESS;
}
