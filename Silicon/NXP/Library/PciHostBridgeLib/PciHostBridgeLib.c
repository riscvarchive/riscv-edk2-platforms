/** @file
  PCI Host Bridge Library instance for NXP SoCs

  Copyright 2018-2020 NXP

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <PiDxe.h>
#include <IndustryStandard/Pci22.h>
#include <Library/DebugLib.h>
#include <Library/DevicePathLib.h>
#include <Library/IoAccessLib.h>
#include <Library/IoLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <Library/PciHostBridgeLib.h>
#include <Pcie.h>
#include <Protocol/PciHostBridgeResourceAllocation.h>
#include <Protocol/PciRootBridgeIo.h>

#pragma pack(1)
typedef struct {
  ACPI_HID_DEVICE_PATH     AcpiDevicePath;
  EFI_DEVICE_PATH_PROTOCOL EndDevicePath;
} EFI_PCI_ROOT_BRIDGE_DEVICE_PATH;
#pragma pack ()

STATIC CONST EFI_PCI_ROOT_BRIDGE_DEVICE_PATH mEfiPciRootBridgeDevicePath[] = {
  {
    {
      {
        ACPI_DEVICE_PATH,
        ACPI_DP,
        {
          (UINT8)(sizeof (ACPI_HID_DEVICE_PATH)),
          (UINT8)(sizeof (ACPI_HID_DEVICE_PATH) >> 8)
        }
      },
      EISA_PNP_ID (0x0A08), // PCI Express
      PCI_SEG0_NUM
    },

    {
      END_DEVICE_PATH_TYPE,
      END_ENTIRE_DEVICE_PATH_SUBTYPE,
      {
        END_DEVICE_PATH_LENGTH,
        0
      }
    }
  },
  {
    {
      {
        ACPI_DEVICE_PATH,
        ACPI_DP,
        {
          (UINT8)(sizeof (ACPI_HID_DEVICE_PATH)),
          (UINT8)(sizeof (ACPI_HID_DEVICE_PATH) >> 8)
        }
      },
      EISA_PNP_ID (0x0A08), // PCI Express
      PCI_SEG1_NUM
    },

    {
      END_DEVICE_PATH_TYPE,
      END_ENTIRE_DEVICE_PATH_SUBTYPE,
      {
        END_DEVICE_PATH_LENGTH,
        0
      }
    }
  },
  {
    {
      {
        ACPI_DEVICE_PATH,
        ACPI_DP,
        {
          (UINT8)(sizeof (ACPI_HID_DEVICE_PATH)),
          (UINT8)(sizeof (ACPI_HID_DEVICE_PATH) >> 8)
        }
      },
      EISA_PNP_ID (0x0A08), // PCI Express
      PCI_SEG2_NUM
    },

    {
      END_DEVICE_PATH_TYPE,
      END_ENTIRE_DEVICE_PATH_SUBTYPE,
      {
        END_DEVICE_PATH_LENGTH,
        0
      }
    }
  },
  {
    {
      {
        ACPI_DEVICE_PATH,
        ACPI_DP,
        {
          (UINT8)(sizeof (ACPI_HID_DEVICE_PATH)),
          (UINT8)(sizeof (ACPI_HID_DEVICE_PATH) >> 8)
        }
      },
      EISA_PNP_ID (0x0A08), // PCI Express
      PCI_SEG3_NUM
    },

    {
      END_DEVICE_PATH_TYPE,
      END_ENTIRE_DEVICE_PATH_SUBTYPE,
      {
        END_DEVICE_PATH_LENGTH,
        0
      }
    }
  },
  {
    {
      {
        ACPI_DEVICE_PATH,
        ACPI_DP,
        {
          (UINT8)(sizeof (ACPI_HID_DEVICE_PATH)),
          (UINT8)(sizeof (ACPI_HID_DEVICE_PATH) >> 8)
        }
      },
      EISA_PNP_ID (0x0A08), // PCI Express
      PCI_SEG4_NUM
    },

    {
      END_DEVICE_PATH_TYPE,
      END_ENTIRE_DEVICE_PATH_SUBTYPE,
      {
        END_DEVICE_PATH_LENGTH,
        0
      }
    }
  },
  {
    {
      {
        ACPI_DEVICE_PATH,
        ACPI_DP,
        {
          (UINT8)(sizeof (ACPI_HID_DEVICE_PATH)),
          (UINT8)(sizeof (ACPI_HID_DEVICE_PATH) >> 8)
        }
      },
      EISA_PNP_ID (0x0A08), // PCI Express
      PCI_SEG5_NUM
    },

    {
      END_DEVICE_PATH_TYPE,
      END_ENTIRE_DEVICE_PATH_SUBTYPE,
      {
        END_DEVICE_PATH_LENGTH,
        0
      }
    }
  }
};

STATIC
GLOBAL_REMOVE_IF_UNREFERENCED
CHAR16 *mPciHostBridgeLibAcpiAddressSpaceTypeStr[] = {
  L"Mem", L"I/O", L"Bus"
};

#define PCI_ALLOCATION_ATTRIBUTES       EFI_PCI_HOST_BRIDGE_COMBINE_MEM_PMEM | \
                                        EFI_PCI_HOST_BRIDGE_MEM64_DECODE

#define PCI_SUPPORT_ATTRIBUTES          EFI_PCI_ATTRIBUTE_ISA_IO_16 | \
                                        EFI_PCI_ATTRIBUTE_ISA_MOTHERBOARD_IO | \
                                        EFI_PCI_ATTRIBUTE_VGA_MEMORY | \
                                        EFI_PCI_ATTRIBUTE_VGA_IO_16  | \
                                        EFI_PCI_ATTRIBUTE_VGA_PALETTE_IO_16

PCI_ROOT_BRIDGE mPciRootBridges[NUM_PCIE_CONTROLLER];

/**
   Helper function to check PCIe link state

   @param Pcie Address of PCIe host controller.

**/
STATIC
INTN
PcieLinkUp (
  IN EFI_PHYSICAL_ADDRESS Pcie,
  IN UINT32 Idx
  )
{
  MMIO_OPERATIONS *PcieOps;
  UINT32 State;
  UINT32 LtssmMask;

  LtssmMask = 0x3f;

  PcieOps = GetMmioOperations (FeaturePcdGet (PcdPciLutBigEndian));
  State = PcieOps->Read32 ((UINTN)Pcie + PCI_LUT_BASE + PCI_LUT_DBG) & LtssmMask;

  if (State < LTSSM_PCIE_L0) {
    DEBUG ((DEBUG_INFO,"PCIE%d : reg @ 0x%lx, no link: LTSSM=0x%02x\n",
            Idx + 1, Pcie, State));
    return PCI_LINK_DOWN;
  }

  return PCI_LINK_UP;
}

/**
  Function to set-up PCIe outbound window

  @param Dbi     Address of PCIe host controller.
  @param Idx     Index of iATU outbound window.
  @param Type    Type(Cfg0/Cfg1/Mem/IO) of iATU outbound window.
  @param Phys    PCIe controller phy address for outbound window.
  @param BusAdr  PCIe controller bus address for outbound window.
  @param Size    Window size

**/
STATIC
VOID
PcieOutboundSet (
  IN EFI_PHYSICAL_ADDRESS Dbi,
  IN UINT32 Idx,
  IN UINT32 Type,
  IN UINT64 Phys,
  IN UINT64 BusAddr,
  IN UINT64 Size
  )
{
  // PCIe Layerscape : Outbound Window
  MmioWrite32 (Dbi + IATU_VIEWPORT_OFF,
                (UINT32)(IATU_VIEWPORT_OUTBOUND | Idx));

  MmioWrite32 (Dbi + IATU_LWR_BASE_ADDR_OFF_OUTBOUND_0,
                (UINT32)Phys);

  MmioWrite32 (Dbi + IATU_UPPER_BASE_ADDR_OFF_OUTBOUND_0,
                (UINT32)(Phys >> 32));

  MmioWrite32 (Dbi + IATU_LIMIT_ADDR_OFF_OUTBOUND_0,
                (UINT32)(Phys + Size - BIT0));

  MmioWrite32 (Dbi + IATU_LWR_TARGET_ADDR_OFF_OUTBOUND_0,
                (UINT32)BusAddr);

  MmioWrite32 (Dbi + IATU_UPPER_TARGET_ADDR_OFF_OUTBOUND_0,
                (UINT32)(BusAddr >> 32));

  MmioWrite32 (Dbi + IATU_REGION_CTRL_1_OFF_OUTBOUND_0,
                (UINT32)Type);

  MmioWrite32 (Dbi + IATU_REGION_CTRL_2_OFF_OUTBOUND_0,
                IATU_REGION_CTRL_2_OFF_OUTBOUND_0_REGION_EN);
}

/**
  Function to set-up iATU windows for Layerscape PCIe controller

  @param Pcie      Address of PCIe host controller
  @param Cfg0Base  PCIe controller phy address Type0 Configuration Space.
  @param Cfg1Base  PCIe controller phy address Type1 Configuration Space.
  @param MemBase   PCIe controller phy address Memory Space.
  @param Mem64Base PCIe controller phy address MMIO64 Space.
  @param IoBase    PCIe controller phy address IO Space.
**/
STATIC
VOID
PcieLsSetupAtu (
  IN EFI_PHYSICAL_ADDRESS Pcie,
  IN EFI_PHYSICAL_ADDRESS Cfg0Base,
  IN EFI_PHYSICAL_ADDRESS Cfg1Base,
  IN EFI_PHYSICAL_ADDRESS MemBase,
  IN EFI_PHYSICAL_ADDRESS Mem64Base,
  IN EFI_PHYSICAL_ADDRESS IoBase
  )
{
  UINT64 Cfg0BaseAddr;
  UINT64 Cfg1BaseAddr;
  UINT64 Cfg0BusAddress;
  UINT64 Cfg1BusAddress;
  UINT64 Cfg0Size;
  UINT64 Cfg1Size;
  UINT64 Mem64End;
  UINT32 Index;

  Cfg0BaseAddr = Cfg0Base;
  Cfg1BaseAddr = Cfg1Base;
  Cfg0BusAddress = SEG_CFG_BUS;
  Cfg1BusAddress = SEG_CFG_BUS;
  Cfg0Size = SEG_CFG_SIZE;
  Cfg1Size = SEG_CFG_SIZE;

  Index = 0;
  // iATU : OUTBOUND WINDOW 1 : CFG0
  PcieOutboundSet (Pcie,
    Index++,
    IATU_REGION_CTRL_1_OFF_OUTBOUND_0_TYPE_CFG0,
    Cfg0BaseAddr,
    Cfg0BusAddress,
    Cfg0Size);

  // iATU : OUTBOUND WINDOW 2 : CFG1
  PcieOutboundSet (Pcie,
    Index++,
    IATU_REGION_CTRL_1_OFF_OUTBOUND_0_TYPE_CFG1,
    Cfg1BaseAddr,
    Cfg1BusAddress,
    Cfg1Size);

  // iATU : OUTBOUND WINDOW 3 : MEM
  PcieOutboundSet (Pcie,
    Index++,
    IATU_REGION_CTRL_1_OFF_OUTBOUND_0_TYPE_MEM,
    MemBase,
    SEG_MEM_BUS,
    SEG_MEM_SIZE);

  //
  // To allow maximum MMIO64 space, MMIO64 window
  // size must be multiple of max iATU size (4GB)
  //
  ASSERT ((PCI_MMIO64_WIN_SIZE & (SIZE_4GB - 1)) == 0);

  Mem64End = Mem64Base + PCI_MMIO64_WIN_SIZE - 1;
  while (Mem64Base < Mem64End) {
    // iATU : OUTBOUND WINDOWs : MMIO64
    PcieOutboundSet (Pcie,
      Index++,
      IATU_REGION_CTRL_1_OFF_OUTBOUND_0_TYPE_MEM,
      Mem64Base,
      Mem64Base,
      SIZE_4GB);
    Mem64Base += SIZE_4GB;
  }

  // iATU : OUTBOUND WINDOW : IO
  PcieOutboundSet (Pcie,
    Index++,
    IATU_REGION_CTRL_1_OFF_OUTBOUND_0_TYPE_IO,
    IoBase,
    SEG_IO_BUS,
    SEG_IO_SIZE
    );
}
/**
  Helper function to set-up PCIe controller

  @param Pcie       Address of PCIe host controller
  @param Cfg0Base   PCIe controller phy address Type0 Configuration Space.
  @param Cfg1Base   PCIe controller phy address Type1 Configuration Space.
  @param MemBase    PCIe controller phy address Memory Space.
  @param Mem64Base  PCIe controller phy address MMIO64 Space.
  @param IoBase     PCIe controller phy address IO Space.

**/
STATIC
VOID
PcieSetupCntrl (
  IN EFI_PHYSICAL_ADDRESS Pcie,
  IN EFI_PHYSICAL_ADDRESS Cfg0Base,
  IN EFI_PHYSICAL_ADDRESS Cfg1Base,
  IN EFI_PHYSICAL_ADDRESS MemBase,
  IN EFI_PHYSICAL_ADDRESS Mem64Base,
  IN EFI_PHYSICAL_ADDRESS IoBase
  )
{
  UINT32 Val;

  // PCIe Layerscape Controller Setup
  PcieLsSetupAtu (Pcie, Cfg0Base, Cfg1Base, MemBase, Mem64Base, IoBase);

  // Program Class code for Layerscape PCIe controller
  MmioWrite32 ((UINTN)Pcie + PCI_DBI_RO_WR_EN, 1);
  Val = MmioRead32 ((UINTN)Pcie + PCI_CLASS_DEVICE);
  Val &= ~(CLASS_CODE_MASK << CLASS_CODE_SHIFT);
  Val |= (PCI_CLASS_BRIDGE_PCI << CLASS_CODE_SHIFT);
  MmioWrite32 ((UINTN)Pcie + PCI_CLASS_DEVICE, Val);
  MmioWrite32 ((UINTN)Pcie + PCI_DBI_RO_WR_EN, 0);
}

/**
  Return all the root bridge instances in an array.

  @param Count  Return the count of root bridge instances.

  @return All the root bridge instances in an array.

**/
PCI_ROOT_BRIDGE *
EFIAPI
PciHostBridgeGetRootBridges (
  OUT UINTN     *Count
  )
{
  UINTN         Idx;
  UINTN         Loop;
  UINT64        PciPhyMemAddr[NUM_PCIE_CONTROLLER];
  UINT64        PciPhyMem64Addr[NUM_PCIE_CONTROLLER];
  UINT64        PciPhyCfg0Addr[NUM_PCIE_CONTROLLER];
  UINT64        PciPhyCfg1Addr[NUM_PCIE_CONTROLLER];
  UINT64        PciPhyIoAddr[NUM_PCIE_CONTROLLER];
  UINT64        Regs[NUM_PCIE_CONTROLLER];
  INTN          LinkUp;

  for  (Idx = 0, Loop = 0; Idx < NUM_PCIE_CONTROLLER; Idx++) {
    PciPhyMemAddr[Idx] = PCI_SEG0_PHY_MEM_BASE + (PCI_BASE_DIFF * Idx);
    PciPhyMem64Addr[Idx] = PCI_SEG0_PHY_MEM64_BASE + (PCI_BASE_DIFF * Idx);
    PciPhyCfg0Addr[Idx] = PCI_SEG0_PHY_CFG0_BASE + (PCI_BASE_DIFF * Idx);
    PciPhyCfg1Addr[Idx] = PCI_SEG0_PHY_CFG1_BASE + (PCI_BASE_DIFF * Idx);
    PciPhyIoAddr [Idx] =  PCI_SEG0_PHY_IO_BASE + (PCI_BASE_DIFF * Idx);
    Regs[Idx] =  PCI_SEG0_DBI_BASE + (PCI_DBI_SIZE_DIFF * Idx);

    // Check PCIe Link
    LinkUp = PcieLinkUp(Regs[Idx], Idx);

    if (!LinkUp) {
      continue;
    }
    DEBUG ((DEBUG_INFO, "PCIE%d Passed Linkup Phase\n", Idx + 1));
    // Set up PCIe Controller and ATU windows
    PcieSetupCntrl (Regs[Idx],
                    PciPhyCfg0Addr[Idx],
                    PciPhyCfg1Addr[Idx],
                    PciPhyMemAddr[Idx],
                    PciPhyMem64Addr[Idx],
                    PciPhyIoAddr[Idx]);

    mPciRootBridges[Loop].Segment               = Idx;
    mPciRootBridges[Loop].Supports              = PCI_SUPPORT_ATTRIBUTES;
    mPciRootBridges[Loop].Attributes            = PCI_SUPPORT_ATTRIBUTES;
    mPciRootBridges[Loop].DmaAbove4G            = TRUE;
    mPciRootBridges[Loop].NoExtendedConfigSpace = FALSE;
    mPciRootBridges[Loop].ResourceAssigned      = FALSE;
    mPciRootBridges[Loop].AllocationAttributes  = PCI_ALLOCATION_ATTRIBUTES;

    mPciRootBridges[Loop].Bus.Base              = PCI_SEG_BUSNUM_MIN;
    mPciRootBridges[Loop].Bus.Limit             = PCI_SEG_BUSNUM_MAX;

    mPciRootBridges[Loop].Io.Base               = PCI_SEG_PORTIO_MIN;
    mPciRootBridges[Loop].Io.Limit              = PCI_SEG_PORTIO_MAX;
    mPciRootBridges[Loop].Io.Translation        = MAX_UINT64 -
                                                  (SEG_IO_SIZE * Idx) + 1;

    mPciRootBridges[Loop].Mem.Base              = SEG_MEM_BASE;
    mPciRootBridges[Loop].Mem.Limit             = SEG_MEM_LIMIT;
    mPciRootBridges[Loop].Mem.Translation       = MAX_UINT64 -
                                                  (PCI_SEG0_MMIO_MEMBASE +
                                                  (PCI_BASE_DIFF *
                                                  Idx)) + 1;

    mPciRootBridges[Loop].MemAbove4G.Base       = PciPhyMem64Addr[Idx];
    mPciRootBridges[Loop].MemAbove4G.Limit      = PciPhyMem64Addr[Idx] +
                                                  (PCI_MMIO64_WIN_SIZE - 1);

    mPciRootBridges[Loop].PMem.Base             = MAX_UINT64;
    mPciRootBridges[Loop].PMem.Limit            = 0;
    mPciRootBridges[Loop].PMemAbove4G.Base      = MAX_UINT64;
    mPciRootBridges[Loop].PMemAbove4G.Limit     = 0;
    mPciRootBridges[Loop].DevicePath            = (EFI_DEVICE_PATH_PROTOCOL *)&mEfiPciRootBridgeDevicePath[Idx];
    Loop++;
  }

  if (Loop == 0) {
    return NULL;
  }

  *Count = Loop;
  return mPciRootBridges;
}

/**
  Free the root bridge instances array returned from PciHostBridgeGetRootBridges().

  @param Bridges The root bridge instances array.
  @param Count   The count of the array.
**/
VOID
EFIAPI
PciHostBridgeFreeRootBridges (
  PCI_ROOT_BRIDGE *Bridges,
  UINTN           Count
  )
{
}

/**
  Inform the platform that the resource conflict happens.

  @param HostBridgeHandle Handle of the Host Bridge.
  @param Configuration    Pointer to PCI I/O and PCI memory resource
                          descriptors. The Configuration contains the resources
                          for all the root bridges. The resource for each root
                          bridge is terminated with END descriptor and an
                          additional END is appended indicating the end of the
                          entire resources. The resource descriptor field
                          values follow the description in
                          EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL
                          .SubmitResources().

**/
VOID
EFIAPI
PciHostBridgeResourceConflict (
  EFI_HANDLE                        HostBridgeHandle,
  VOID                              *Configuration
  )
{
  EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *Descriptor;
  UINTN                             RootBridgeIndex;

  DEBUG ((DEBUG_ERROR, "PciHostBridge: Resource conflict happens!\n"));

  RootBridgeIndex = 0;
  Descriptor = (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *) Configuration;

  while (Descriptor->Desc == ACPI_ADDRESS_SPACE_DESCRIPTOR) {

    DEBUG ((DEBUG_ERROR, "RootBridge[%d]:\n", RootBridgeIndex++));

    for (; Descriptor->Desc == ACPI_ADDRESS_SPACE_DESCRIPTOR; Descriptor++) {
      ASSERT (Descriptor->ResType <
              ARRAY_SIZE (mPciHostBridgeLibAcpiAddressSpaceTypeStr));

      DEBUG ((DEBUG_ERROR, " %s: Length/Alignment = 0x%lx / 0x%lx\n",
              mPciHostBridgeLibAcpiAddressSpaceTypeStr[Descriptor->ResType],
              Descriptor->AddrLen, Descriptor->AddrRangeMax
              ));

      if (Descriptor->ResType == ACPI_ADDRESS_SPACE_TYPE_MEM) {
        DEBUG ((DEBUG_ERROR, "     Granularity/SpecificFlag = %ld / %02x%s\n",
                Descriptor->AddrSpaceGranularity, Descriptor->SpecificFlag,
                ((Descriptor->SpecificFlag &
                  EFI_ACPI_MEMORY_RESOURCE_SPECIFIC_FLAG_CACHEABLE_PREFETCHABLE
                  ) != 0) ? L" (Prefetchable)" : L""
                ));
      }
    }
    //
    // Skip the END descriptor for root bridge
    //
    ASSERT (Descriptor->Desc == ACPI_END_TAG_DESCRIPTOR);
    Descriptor = (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *)(
                   (EFI_ACPI_END_TAG_DESCRIPTOR *)Descriptor + 1
                   );
  }

  return;
}
