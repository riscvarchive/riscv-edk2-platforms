/** @file
  Physical memory map for SynQuacer

  Copyright (c) 2017, Linaro Ltd. All rights reserved.<BR>

  This program and the accompanying materials are licensed and made available
  under the terms and conditions of the BSD License which accompanies this
  distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS, WITHOUT
  WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SYNQUACER_PLATFORM_MEMORYMAP_H_
#define _SYNQUACER_PLATFORM_MEMORYMAP_H_

// Memory mapped SPI NOR
#define SYNQUACER_SPI_NOR_BASE          0x08000000
#define SYNQUACER_SPI_NOR_SIZE          SIZE_128MB

// On-Chip non-secure ROM
#define SYNQUACER_NON_SECURE_ROM_BASE   0x1F000000
#define SYNQUACER_NON_SECURE_ROM_SZ     SIZE_512KB

// On-Chip Peripherals
#define SYNQUACER_PERIPHERALS_BASE      0x20000000
#define SYNQUACER_PERIPHERALS_SZ        0x0E000000

// On-Chip non-secure SRAM
#define SYNQUACER_NON_SECURE_SRAM_BASE  0x2E000000
#define SYNQUACER_NON_SECURE_SRAM_SZ    SIZE_64KB

// GIC-500
#define SYNQUACER_GIC500_DIST_BASE      FixedPcdGet64 (PcdGicDistributorBase)
#define SYNQUACER_GIC500_DIST_SIZE      SIZE_256KB
#define SYNQUACER_GIC500_RDIST_BASE     FixedPcdGet64 (PcdGicRedistributorsBase)
#define SYNQUACER_GIC500_RDIST_SIZE     SIZE_8MB

// GPIO block
#define SYNQUACER_GPIO_BASE             0x51000000
#define SYNQUACER_GPIO_SIZE             SIZE_4KB

// I2C0 block
#define SYNQUACER_I2C0_BASE             0x51200000
#define SYNQUACER_I2C0_SIZE             SIZE_4KB

// I2C1 block
#define SYNQUACER_I2C1_BASE             0x51210000
#define SYNQUACER_I2C1_SIZE             SIZE_4KB

// eMMC(SDH30)
#define SYNQUACER_EMMC_BASE             0x52300000
#define SYNQUACER_EMMC_BASE_SZ          SIZE_4KB

#define SYNQUACER_EEPROM_BASE           0x10000000
#define SYNQUACER_EEPROM_BASE_SZ        SIZE_64KB

// NETSEC
#define SYNQUACER_NETSEC1_BASE          0x522D0000
#define SYNQUACER_NETSEC1_BASE_SZ       SIZE_64KB

// PCI
#define SYNQUACER_PCIE_BASE             0x58200000
#define SYNQUACER_PCIE_SIZE             0x00200000

#endif
