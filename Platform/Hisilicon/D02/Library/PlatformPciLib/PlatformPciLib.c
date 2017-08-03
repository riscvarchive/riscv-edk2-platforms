/** @file

  Copyright (c) 2016, Hisilicon Limited. All rights reserved.<BR>
  Copyright (c) 2016, Linaro Limited. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Library/PcdLib.h>
#include <Library/PlatformPciLib.h>

PCI_ROOT_BRIDGE_RESOURCE_APPETURE  mResAppeture[PCIE_MAX_HOSTBRIDGE][PCIE_MAX_ROOTBRIDGE] = {
 {// HostBridge 0
  /* Port 0 */
  {
      PCI_HB0RB0_ECAM_BASE,
      0,  //BusBase
      63, //BusLimit
      (PCI_HB0RB0_ECAM_BASE + SIZE_64MB),  //MemBase
      (PCI_HB0RB0_ECAM_BASE + PCI_HB0RB0_ECAM_SIZE - 1), //MemLimit
      (0), //IoBase
      (0),  //IoLimit
      0,
      0,
      (PCI_HB0RB0_PCI_BASE),  //RbPciBar
      0,
      0
  },
  /* Port 1 */
  {
      PCI_HB0RB1_ECAM_BASE,
      64,  //BusBase
      127, //BusLimit
      PCI_HB0RB1_PCIREGION_BASE,
      PCI_HB0RB1_PCIREGION_BASE + PCI_HB0RB1_PCIREGION_SIZE - 1,
      PCI_HB0RB1_IO_BASE, //IoBase
      PCI_HB0RB1_IO_BASE + PCI_HB0RB1_IO_SIZE - 1,  //IoLimit
      PCI_HB0RB1_CPUMEMREGIONBASE,
      PCI_HB0RB2_CPUIOREGIONBASE,
      (PCI_HB0RB1_PCI_BASE),  //RbPciBar
      PCI_HB0RB1_PCIREGION_BASE,
      PCI_HB0RB1_PCIREGION_BASE + PCI_HB0RB1_PCIREGION_SIZE - 1
  },
  /* Port 2 */
  {
      PCI_HB0RB2_ECAM_BASE,
      128,  //BusBase
      191, //BusLimit
      PCI_HB0RB2_PCIREGION_BASE ,
      PCI_HB0RB2_PCIREGION_BASE + PCI_HB0RB2_PCIREGION_SIZE - 1,
      PCI_HB0RB2_IO_BASE, //IoBase
      PCI_HB0RB2_IO_BASE + PCI_HB0RB2_IO_SIZE - 1,  //IoLimit
      PCI_HB0RB2_CPUMEMREGIONBASE,
      PCI_HB0RB2_CPUIOREGIONBASE,
      (PCI_HB0RB2_PCI_BASE),  //RbPciBar
      PCI_HB0RB2_PCIREGION_BASE ,
      PCI_HB0RB2_PCIREGION_BASE + PCI_HB0RB2_PCIREGION_SIZE - 1
  },

  /* Port 3 */
  {
      PCI_HB0RB3_ECAM_BASE,
      192,  //BusBase
      255, //BusLimit
      (PCI_HB0RB3_ECAM_BASE),  //MemBase
      (PCI_HB0RB3_ECAM_BASE + PCI_HB0RB3_ECAM_SIZE - 1), //MemLimit
      (0), //IoBase
      (0),  //IoLimit
      0,
      0,
      (PCI_HB0RB3_PCI_BASE),  //RbPciBar
      0,
      0
  }
 },
{// HostBridge 1
  /* Port 0 */
  {
      PCI_HB1RB0_ECAM_BASE,
      0,  //BusBase
      0x1, //BusLimit
      (PCI_HB1RB0_ECAM_BASE),  //MemBase
      (PCI_HB1RB0_ECAM_BASE + PCI_HB1RB0_ECAM_SIZE - 1), //MemLimit
      (0), //IoBase
      (0),  //IoLimit
      0,
      0,
      (PCI_HB1RB0_PCI_BASE),  //RbPciBar
      0,
      0
  },
  /* Port 1 */
  {
       PCI_HB1RB1_ECAM_BASE,
      0x2,  //BusBase
      0x3, //BusLimit
      (PCI_HB1RB1_ECAM_BASE),  //MemBase
      (PCI_HB1RB1_ECAM_BASE + PCI_HB1RB1_ECAM_SIZE - 1), //MemLimit
      (0), //IoBase
      (0),  //IoLimit
      0,
      0,
      (PCI_HB1RB1_PCI_BASE),  //RbPciBar
      0,
      0
  },
  /* Port 2 */
  {
      PCI_HB1RB2_ECAM_BASE,
      0x4,  //BusBase
      0x5, //BusLimit
      (PCI_HB1RB2_ECAM_BASE),  //MemBase
      (PCI_HB1RB2_ECAM_BASE + PCI_HB1RB2_ECAM_SIZE - 1), //MemLimit
      (0), //IoBase
      (0),  //IoLimit
      0,
      0,
      (PCI_HB1RB2_PCI_BASE),  //RbPciBar
      0,
      0
  },

  /* Port 3 */
  {
      PCI_HB1RB3_ECAM_BASE,
      0x6,  //BusBase
      0x7, //BusLimit
      (PCI_HB1RB3_ECAM_BASE),  //MemBase
      (PCI_HB1RB3_ECAM_BASE + PCI_HB1RB3_ECAM_SIZE - 1), //MemLimit
      (0), //IoBase
      (0),  //IoLimit
      0,
      0,
      (PCI_HB1RB3_PCI_BASE),  //RbPciBar
      0,
      0
  }
 }
};

