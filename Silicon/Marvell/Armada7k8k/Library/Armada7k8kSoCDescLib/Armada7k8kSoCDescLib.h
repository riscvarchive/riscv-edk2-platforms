/**
*
*  Copyright (C) 2018, Marvell International Ltd. and its affiliates.
*
*  This program and the accompanying materials are licensed and made available
*  under the terms and conditions of the BSD License which accompanies this
*  distribution. The full text of the license may be found at
*  http://opensource.org/licenses/bsd-license.php
*
*  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
*  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
*
*  Glossary - abbreviations used in Marvell SampleAtReset library implementation:
*  AP - Application Processor hardware block (Armada 7k8k incorporates AP806)
*  CP - South Bridge hardware blocks (Armada 7k8k incorporates CP110)
**/

#ifndef __ARMADA7K8K_SOC_DESC_LIB_H__
#define __ARMADA7K8K_SOC_DESC_LIB_H__

//
// Common macros
//
#define MV_SOC_CP_BASE(Cp)               (0xF2000000 + ((Cp) * 0x2000000))

//
// Platform description of AHCI controllers
//
#define MV_SOC_AHCI_BASE(Cp)             (MV_SOC_CP_BASE (Cp) + 0x540000)
#define MV_SOC_AHCI_ID(Cp)               ((Cp) % 2)

//
// Platform description of PP2 NIC
//
#define MV_SOC_PP2_BASE(Cp)              MV_SOC_CP_BASE (Cp)
#define MV_SOC_PP2_CLK_FREQ              333333333

//
// Platform description of SDMMC controllers
//
#define MV_SOC_MAX_SDMMC_COUNT           2
#define MV_SOC_SDMMC_BASE(Index)         ((Index) == 0 ? 0xF06E0000 : 0xF2780000)

//
// Platform description of UTMI PHY's
//
#define MV_SOC_UTMI_PER_CP_COUNT         2
#define MV_SOC_UTMI_ID(Utmi)             (Utmi)
#define MV_SOC_UTMI_BASE(Utmi)           (0x580000 + ((Utmi) * 0x1000))
#define MV_SOC_UTMI_CFG_BASE             0x440440
#define MV_SOC_UTMI_USB_CFG_BASE         0x440420

//
// Platform description of XHCI controllers
//
#define MV_SOC_XHCI_PER_CP_COUNT         2
#define MV_SOC_XHCI_BASE(Xhci)           (0x500000 + ((Xhci) * 0x10000))

#endif
