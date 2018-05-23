/** @file
*
*  Copyright (c) 2018, ARM Limited. All rights reserved.
*
*  This program and the accompanying materials are licensed and made available
*  under the terms and conditions of the BSD License which accompanies this
*  distribution.  The full text of the license may be found at
*  http://opensource.org/licenses/bsd-license.php
*
*  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
*  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
*
**/

#include <Library/ArmPlatformLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/MemoryAllocationLib.h>

#include <SgiPlatform.h>

// Total number of descriptors, including the final "end-of-table" descriptor.
#define MAX_VIRTUAL_MEMORY_MAP_DESCRIPTORS  8

/**
  Returns the Virtual Memory Map of the platform.

  This Virtual Memory Map is used by MemoryInitPei Module to initialize the MMU
  on your platform.

  @param[out] VirtualMemoryMap Array of ARM_MEMORY_REGION_DESCRIPTOR describing
                               a Physical-to-Virtual Memory mapping. This array
                               must be ended by a zero-filled entry.
**/
VOID
ArmPlatformGetVirtualMemoryMap (
  IN ARM_MEMORY_REGION_DESCRIPTOR** VirtualMemoryMap
  )
{
  UINTN                         Index;
  ARM_MEMORY_REGION_DESCRIPTOR  *VirtualMemoryTable;

  ASSERT (VirtualMemoryMap != NULL);
  Index = 0;

  VirtualMemoryTable = (ARM_MEMORY_REGION_DESCRIPTOR*)AllocatePages
                       (EFI_SIZE_TO_PAGES (sizeof (ARM_MEMORY_REGION_DESCRIPTOR) *
                       MAX_VIRTUAL_MEMORY_MAP_DESCRIPTORS));
  if (VirtualMemoryTable == NULL) {
    return;
  }

  // Expansion AXI - SMC Chip Select 0 (NOR Flash)
  VirtualMemoryTable[Index].PhysicalBase    = SGI_EXP_SMC_CS0_BASE;
  VirtualMemoryTable[Index].VirtualBase     = SGI_EXP_SMC_CS0_BASE;
  VirtualMemoryTable[Index].Length          = SIZE_64MB;
  VirtualMemoryTable[Index].Attributes      = ARM_MEMORY_REGION_ATTRIBUTE_DEVICE;

  // Expansion AXI - SMC Chip Select 1 (NOR Flash)
  VirtualMemoryTable[++Index].PhysicalBase  = SGI_EXP_SMC_CS1_BASE;
  VirtualMemoryTable[Index].VirtualBase     = SGI_EXP_SMC_CS1_BASE;
  VirtualMemoryTable[Index].Length          = SIZE_64MB;
  VirtualMemoryTable[Index].Attributes      = ARM_MEMORY_REGION_ATTRIBUTE_DEVICE;

  // Expansion AXI - System Peripherals
  VirtualMemoryTable[++Index].PhysicalBase  = SGI_EXP_SYS_PERIPH_BASE;
  VirtualMemoryTable[Index].VirtualBase     = SGI_EXP_SYS_PERIPH_BASE;
  VirtualMemoryTable[Index].Length          = SGI_EXP_SYS_PERIPH_SZ;
  VirtualMemoryTable[Index].Attributes      = ARM_MEMORY_REGION_ATTRIBUTE_DEVICE;

  // Sub System Peripherals - Generic Watchdog
  VirtualMemoryTable[++Index].PhysicalBase  = SGI_SUBSYS_GENERIC_WDOG_BASE;
  VirtualMemoryTable[Index].VirtualBase     = SGI_SUBSYS_GENERIC_WDOG_BASE;
  VirtualMemoryTable[Index].Length          = SGI_SUBSYS_GENERIC_WDOG_SZ;
  VirtualMemoryTable[Index].Attributes      = ARM_MEMORY_REGION_ATTRIBUTE_DEVICE;

  // Sub System Peripherals - GIC-600
  VirtualMemoryTable[++Index].PhysicalBase  = SGI_SUBSYS_GENERIC_GIC_BASE;
  VirtualMemoryTable[Index].VirtualBase     = SGI_SUBSYS_GENERIC_GIC_BASE;
  VirtualMemoryTable[Index].Length          = SGI_SUBSYS_GENERIC_GIC_SZ;
  VirtualMemoryTable[Index].Attributes      = ARM_MEMORY_REGION_ATTRIBUTE_DEVICE;

  // Expansion AXI - Platform Peripherals - UART1
  VirtualMemoryTable[++Index].PhysicalBase  = SGI_EXP_PLAT_PERIPH_UART1_BASE;
  VirtualMemoryTable[Index].VirtualBase     = SGI_EXP_PLAT_PERIPH_UART1_BASE;
  VirtualMemoryTable[Index].Length          = SGI_EXP_PLAT_PERIPH_UART1_SZ;
  VirtualMemoryTable[Index].Attributes      = ARM_MEMORY_REGION_ATTRIBUTE_DEVICE;

  // DDR - (2GB - 16MB)
  VirtualMemoryTable[++Index].PhysicalBase  = PcdGet64 (PcdSystemMemoryBase);
  VirtualMemoryTable[Index].VirtualBase     = PcdGet64 (PcdSystemMemoryBase);
  VirtualMemoryTable[Index].Length          = PcdGet64 (PcdSystemMemorySize);
  VirtualMemoryTable[Index].Attributes      = ARM_MEMORY_REGION_ATTRIBUTE_WRITE_BACK;

  // End of Table
  VirtualMemoryTable[++Index].PhysicalBase  = 0;
  VirtualMemoryTable[Index].VirtualBase     = 0;
  VirtualMemoryTable[Index].Length          = 0;
  VirtualMemoryTable[Index].Attributes      = (ARM_MEMORY_REGION_ATTRIBUTES)0;

  ASSERT ((Index + 1) <= MAX_VIRTUAL_MEMORY_MAP_DESCRIPTORS);
  *VirtualMemoryMap = VirtualMemoryTable;
}
