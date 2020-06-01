/** @file

  Copyright 2018-2020 NXP

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef SOC_H__
#define SOC_H__

#include <Chassis.h>

/**
  Soc Memory Map
**/
#define LX2160A_DRAM0_PHYS_ADDRESS   (BASE_2GB)
#define LX2160A_DRAM0_SIZE           (SIZE_2GB)
#define LX2160A_DRAM1_PHYS_ADDRESS   (BASE_128GB + BASE_2GB)
#define LX2160A_DRAM1_SIZE           (SIZE_128GB - SIZE_2GB) // 126 GB
#define LX2160A_DRAM2_PHYS_ADDRESS   (BASE_256GB + BASE_128GB)
#define LX2160A_DRAM2_SIZE           (SIZE_128GB)

#define LX2160A_CCSR_PHYS_ADDRESS    (BASE_16MB)
#define LX2160A_CCSR_SIZE            (SIZE_256MB - SIZE_16MB) // 240MB

#define LX2160A_FSPI0_PHYS_ADDRESS   (BASE_512MB)
#define LX2160A_FSPI0_SIZE           (SIZE_256MB)

#define LX2160A_DCFG_ADDRESS         NXP_LAYERSCAPE_CHASSIS3V2_DCFG_ADDRESS

/**
  Reset Control Word (RCW) Bits
**/
#define SYS_PLL_RAT(x)  (((x) & 0x7c) >> 2) // Bits 2-6

typedef NXP_LAYERSCAPE_CHASSIS3V2_DEVICE_CONFIG LX2160A_DEVICE_CONFIG;

#endif // SOC_H__
