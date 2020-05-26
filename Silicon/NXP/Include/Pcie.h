/** @file
  PCI memory configuration for NXP

  Copyright 2018-2020 NXP

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef PCI_H
#define PCI_H

#define PCI_SEG0_NUM              0
#define PCI_SEG1_NUM              1
#define PCI_SEG2_NUM              2
#define PCI_SEG3_NUM              3
#define PCI_SEG4_NUM              4
#define PCI_SEG5_NUM              5
#define PCI_SEG0_MMIO_MEMBASE     FixedPcdGet64 (PcdPciExp1BaseAddr)
#define PCI_SEG0_DBI_BASE         0x03400000

#define PCI_LINK_DOWN             0x0
#define PCI_LINK_UP               0x1

// Segment configuration
#define PCI_SEG_BUSNUM_MIN        0x0
#define PCI_SEG_BUSNUM_MAX        0xff
#define PCI_SEG_PORTIO_MIN        0x0
#define PCI_SEG_PORTIO_MAX        0xffff
#define SEG_CFG_SIZE              0x00001000
#define SEG_MEM_BASE              0x40000000
#define SEG_MEM_SIZE              0xC0000000
#define SEG_MEM_LIMIT             (SEG_MEM_BASE + (SEG_MEM_SIZE -1))
#define SEG_IO_BASE               0x10000000
#define SEG_MEM64_BASE            0x400000000
#define PCI_BASE_DIFF             0x800000000
#define PCI_DBI_SIZE_DIFF         0x100000
#define PCI_SEG0_PHY_CFG0_BASE    PCI_SEG0_MMIO_MEMBASE
#define PCI_SEG0_PHY_CFG1_BASE    (PCI_SEG0_PHY_CFG0_BASE + SEG_CFG_SIZE)
#define PCI_SEG0_PHY_MEM_BASE     (PCI_SEG0_MMIO_MEMBASE + SEG_MEM_BASE)
#define PCI_SEG0_PHY_MEM64_BASE   (PCI_SEG0_MMIO_MEMBASE + SEG_MEM64_BASE)
#define PCI_MMIO64_WIN_SIZE       SIZE_16GB
#define PCI_SEG0_PHY_IO_BASE      (PCI_SEG0_MMIO_MEMBASE + SEG_IO_BASE)

// PCIe Controller configuration
#define NUM_PCIE_CONTROLLER       FixedPcdGet32 (PcdNumPciController)
#define PCI_LUT_DBG               FixedPcdGet32 (PcdPcieLutDbg)
#define PCI_LUT_BASE              FixedPcdGet32 (PcdPcieLutBase)
#define LTSSM_PCIE_L0             0x11

#define PCI_CLASS_BRIDGE_PCI      0x0604
#define PCI_CLASS_DEVICE          0x8
#define PCI_DBI_RO_WR_EN          0x8bc
#define CLASS_CODE_MASK           0xffff
#define CLASS_CODE_SHIFT          0x10

// PCIe Layerscape Controller
#define IATU_VIEWPORT_OFF                            0x900
#define IATU_REGION_CTRL_1_OFF_OUTBOUND_0            0x904
#define IATU_REGION_CTRL_2_OFF_OUTBOUND_0            0x908
#define IATU_LWR_BASE_ADDR_OFF_OUTBOUND_0            0x90C
#define IATU_UPPER_BASE_ADDR_OFF_OUTBOUND_0          0x910
#define IATU_LIMIT_ADDR_OFF_OUTBOUND_0               0x914
#define IATU_LWR_TARGET_ADDR_OFF_OUTBOUND_0          0x918
#define IATU_UPPER_TARGET_ADDR_OFF_OUTBOUND_0        0x91C
#define IATU_VIEWPORT_OUTBOUND                       0x0
#define IATU_REGION_CTRL_2_OFF_OUTBOUND_0_REGION_EN  BIT31

// ATU Programming
#define IATU_REGION_CTRL_1_OFF_OUTBOUND_0_TYPE_MEM   0x0
#define IATU_REGION_CTRL_1_OFF_OUTBOUND_0_TYPE_IO    0x2
#define IATU_REGION_CTRL_1_OFF_OUTBOUND_0_TYPE_CFG0  0x4
#define IATU_REGION_CTRL_1_OFF_OUTBOUND_0_TYPE_CFG1  0x5
#define IATU_REGION_INDEX0        0x0
#define IATU_REGION_INDEX1        0x1
#define SEG_CFG_BUS               0x00000000
#define SEG_MEM_BUS               0x40000000
#define SEG_IO_SIZE               0x10000
#define SEG_IO_BUS                0x0

#endif
