/** NxpQoriqLsMem.c
*
*  Board memory specific Library.
*
*  Based on BeagleBoardPkg/Library/BeagleBoardLib/BeagleBoardMem.c
*
*  Copyright (c) 2011, ARM Limited. All rights reserved.
*  Copyright (c) 2016, Freescale Semiconductor, Inc. All rights reserved.
*  Copyright 2017, 2019 NXP
*
*  SPDX-License-Identifier: BSD-2-Clause-Patent
*
**/

#include <Library/ArmPlatformLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/MemoryAllocationLib.h>
#include <DramInfo.h>

#define MAX_VIRTUAL_MEMORY_MAP_DESCRIPTORS          25

/**
  Return the Virtual Memory Map of your platform

  This Virtual Memory Map is used by MemoryInitPei Module to initialize the MMU on your platform.

  @param  VirtualMemoryMap     Array of ARM_MEMORY_REGION_DESCRIPTOR describing a Physical-to-
                               Virtual Memory mapping. This array must be ended by a zero-filled
                               entry

**/

VOID
ArmPlatformGetVirtualMemoryMap (
  IN  ARM_MEMORY_REGION_DESCRIPTOR **VirtualMemoryMap
  )
{
  UINTN                            Index;
  ARM_MEMORY_REGION_DESCRIPTOR     *VirtualMemoryTable;
  DRAM_INFO                        DramInfo;

  Index = 0;

  ASSERT (VirtualMemoryMap != NULL);

  VirtualMemoryTable = (ARM_MEMORY_REGION_DESCRIPTOR*)AllocatePages (
          EFI_SIZE_TO_PAGES (sizeof (ARM_MEMORY_REGION_DESCRIPTOR) * MAX_VIRTUAL_MEMORY_MAP_DESCRIPTORS));

  if (VirtualMemoryTable == NULL) {
    return;
  }

  if (GetDramBankInfo (&DramInfo)) {
    DEBUG ((DEBUG_ERROR, "Failed to get DRAM information, exiting...\n"));
    return;
  }


  for (Index = 0; Index < DramInfo.NumOfDrams; Index++) {
    // DRAM1 (Must be 1st entry)
    VirtualMemoryTable[Index].PhysicalBase = DramInfo.DramRegion[Index].BaseAddress;
    VirtualMemoryTable[Index].VirtualBase  = DramInfo.DramRegion[Index].BaseAddress;
    VirtualMemoryTable[Index].Length       = DramInfo.DramRegion[Index].Size;
    VirtualMemoryTable[Index].Attributes   = ARM_MEMORY_REGION_ATTRIBUTE_WRITE_BACK;
  }

  // CCSR Space
  VirtualMemoryTable[Index].PhysicalBase = FixedPcdGet64 (PcdCcsrBaseAddr);
  VirtualMemoryTable[Index].VirtualBase  = FixedPcdGet64 (PcdCcsrBaseAddr);
  VirtualMemoryTable[Index].Length       = FixedPcdGet64 (PcdCcsrSize);
  VirtualMemoryTable[Index].Attributes   = ARM_MEMORY_REGION_ATTRIBUTE_DEVICE;

  // IFC region 1
  //
  // A-009241   : Unaligned write transactions to IFC may result in corruption of data
  // Affects    : IFC
  // Description: 16 byte unaligned write from system bus to IFC may result in extra unintended
  //              writes on external IFC interface that can corrupt data on external flash.
  // Impact     : Data corruption on external flash may happen in case of unaligned writes to
  //              IFC memory space.
  // Workaround: Following are the workarounds:
  //             For write transactions from core, IFC interface memories (including IFC SRAM)
  //                should be configured as device type memory in MMU.
  //             For write transactions from non-core masters (like system DMA), the address
  //                should be 16 byte aligned and the data size should be multiple of 16 bytes.
  //
  VirtualMemoryTable[++Index].PhysicalBase = FixedPcdGet64 (PcdIfcRegion1BaseAddr);
  VirtualMemoryTable[Index].VirtualBase  = FixedPcdGet64 (PcdIfcRegion1BaseAddr);
  VirtualMemoryTable[Index].Length       = FixedPcdGet64 (PcdIfcRegion1Size);
  VirtualMemoryTable[Index].Attributes   = ARM_MEMORY_REGION_ATTRIBUTE_DEVICE;

  // QMAN SWP
  VirtualMemoryTable[++Index].PhysicalBase = FixedPcdGet64 (PcdQmanSwpBaseAddr);
  VirtualMemoryTable[Index].VirtualBase  = FixedPcdGet64 (PcdQmanSwpBaseAddr);
  VirtualMemoryTable[Index].Length       = FixedPcdGet64 (PcdQmanSwpSize);
  VirtualMemoryTable[Index].Attributes   = ARM_MEMORY_REGION_ATTRIBUTE_UNCACHED_UNBUFFERED;

  // BMAN SWP
  VirtualMemoryTable[++Index].PhysicalBase = FixedPcdGet64 (PcdBmanSwpBaseAddr);
  VirtualMemoryTable[Index].VirtualBase  = FixedPcdGet64 (PcdBmanSwpBaseAddr);
  VirtualMemoryTable[Index].Length       = FixedPcdGet64 (PcdBmanSwpSize);
  VirtualMemoryTable[Index].Attributes   = ARM_MEMORY_REGION_ATTRIBUTE_UNCACHED_UNBUFFERED;

  // IFC region 2
  VirtualMemoryTable[++Index].PhysicalBase = FixedPcdGet64 (PcdIfcRegion2BaseAddr);
  VirtualMemoryTable[Index].VirtualBase  = FixedPcdGet64 (PcdIfcRegion2BaseAddr);
  VirtualMemoryTable[Index].Length       = FixedPcdGet64 (PcdIfcRegion2Size);
  VirtualMemoryTable[Index].Attributes   = ARM_MEMORY_REGION_ATTRIBUTE_DEVICE;

  // PCIe1
  VirtualMemoryTable[++Index].PhysicalBase = FixedPcdGet64 (PcdPciExp1BaseAddr);
  VirtualMemoryTable[Index].VirtualBase  = FixedPcdGet64 (PcdPciExp1BaseAddr);
  VirtualMemoryTable[Index].Length       = FixedPcdGet64 (PcdPciExp1BaseSize);
  VirtualMemoryTable[Index].Attributes   = ARM_MEMORY_REGION_ATTRIBUTE_DEVICE;

  // PCIe2
  VirtualMemoryTable[++Index].PhysicalBase = FixedPcdGet64 (PcdPciExp2BaseAddr);
  VirtualMemoryTable[Index].VirtualBase  = FixedPcdGet64 (PcdPciExp2BaseAddr);
  VirtualMemoryTable[Index].Length       = FixedPcdGet64 (PcdPciExp2BaseSize);
  VirtualMemoryTable[Index].Attributes   = ARM_MEMORY_REGION_ATTRIBUTE_DEVICE;

  // PCIe3
  VirtualMemoryTable[++Index].PhysicalBase = FixedPcdGet64 (PcdPciExp3BaseAddr);
  VirtualMemoryTable[Index].VirtualBase  = FixedPcdGet64 (PcdPciExp3BaseAddr);
  VirtualMemoryTable[Index].Length       = FixedPcdGet64 (PcdPciExp3BaseSize);
  VirtualMemoryTable[Index].Attributes   = ARM_MEMORY_REGION_ATTRIBUTE_DEVICE;

  // QSPI region
  VirtualMemoryTable[++Index].PhysicalBase = FixedPcdGet64 (PcdQspiRegionBaseAddr);
  VirtualMemoryTable[Index].VirtualBase  = FixedPcdGet64 (PcdQspiRegionBaseAddr);
  VirtualMemoryTable[Index].Length       = FixedPcdGet64 (PcdQspiRegionSize);
  VirtualMemoryTable[Index].Attributes   = ARM_MEMORY_REGION_ATTRIBUTE_UNCACHED_UNBUFFERED;

  // End of Table
  VirtualMemoryTable[++Index].PhysicalBase = 0;
  VirtualMemoryTable[Index].VirtualBase  = 0;
  VirtualMemoryTable[Index].Length       = 0;
  VirtualMemoryTable[Index].Attributes   = (ARM_MEMORY_REGION_ATTRIBUTES)0;

  ASSERT ((Index + 1) <= MAX_VIRTUAL_MEMORY_MAP_DESCRIPTORS);

  *VirtualMemoryMap = VirtualMemoryTable;
}
