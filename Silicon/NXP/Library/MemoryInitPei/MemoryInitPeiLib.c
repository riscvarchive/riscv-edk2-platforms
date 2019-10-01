/** @file
*
*  Copyright (c) 2011-2015, ARM Limited. All rights reserved.
*
*  Copyright 2019-2020 NXP
*
*  SPDX-License-Identifier: BSD-2-Clause-Patent
*
**/

#include <PiPei.h>

#include <Library/ArmMmuLib.h>
#include <Library/ArmPlatformLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>

#include <DramInfo.h>

VOID
BuildMemoryTypeInformationHob (
  VOID
  );

VOID
InitMmu (
  IN ARM_MEMORY_REGION_DESCRIPTOR  *MemoryTable
  )
{

  VOID                          *TranslationTableBase;
  UINTN                         TranslationTableSize;
  RETURN_STATUS                 Status;

  //Note: Because we called PeiServicesInstallPeiMemory() before
  //to call InitMmu() the MMU Page Table resides in DRAM
  //(even at the top of DRAM as it is the first permanent memory allocation)
  Status = ArmConfigureMmu (MemoryTable, &TranslationTableBase, &TranslationTableSize);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Error: Failed to enable MMU\n"));
  }
}

/*++

Routine Description:



Arguments:

  FileHandle  - Handle of the file being invoked.
  PeiServices - Describes the list of possible PEI Services.

Returns:

  Status -  EFI_SUCCESS if the boot mode could be set

--*/
EFI_STATUS
EFIAPI
MemoryPeim (
  IN EFI_PHYSICAL_ADDRESS               UefiMemoryBase,
  IN UINT64                             UefiMemorySize
  )
{
  ARM_MEMORY_REGION_DESCRIPTOR *MemoryTable;
  EFI_RESOURCE_ATTRIBUTE_TYPE  ResourceAttributes;
  EFI_PEI_HOB_POINTERS         NextHob;
  BOOLEAN                      Found;
  DRAM_INFO                    DramInfo;

  // Get Virtual Memory Map from the Platform Library
  ArmPlatformGetVirtualMemoryMap (&MemoryTable);

  //
  // Ensure MemoryTable[0].Length which is size of DRAM has been set
  // by ArmPlatformGetVirtualMemoryMap ()
  //
  ASSERT (MemoryTable[0].Length != 0);

  //
  // Now, the permanent memory has been installed, we can call AllocatePages()
  //
  ResourceAttributes = (
    EFI_RESOURCE_ATTRIBUTE_PRESENT |
    EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
    EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE |
    EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE |
    EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE |
    EFI_RESOURCE_ATTRIBUTE_TESTED
  );

  if (GetDramBankInfo (&DramInfo)) {
    DEBUG ((DEBUG_ERROR, "Failed to get DRAM information, exiting...\n"));
    return EFI_UNSUPPORTED;
  }

  while (DramInfo.NumOfDrams--) {
    //
    // Check if the resource for the main system memory has been declared
    //
    Found = FALSE;
    NextHob.Raw = GetHobList ();
    while ((NextHob.Raw = GetNextHob (EFI_HOB_TYPE_RESOURCE_DESCRIPTOR, NextHob.Raw)) != NULL) {
      if ((NextHob.ResourceDescriptor->ResourceType == EFI_RESOURCE_SYSTEM_MEMORY) &&
          (DramInfo.DramRegion[DramInfo.NumOfDrams].BaseAddress >= NextHob.ResourceDescriptor->PhysicalStart) &&
          (NextHob.ResourceDescriptor->PhysicalStart + NextHob.ResourceDescriptor->ResourceLength <=
           DramInfo.DramRegion[DramInfo.NumOfDrams].BaseAddress + DramInfo.DramRegion[DramInfo.NumOfDrams].Size))
      {
        Found = TRUE;
        break;
      }
      NextHob.Raw = GET_NEXT_HOB (NextHob);
    }

    if (!Found) {
      // Reserved the memory space occupied by the firmware volume
      BuildResourceDescriptorHob (
        EFI_RESOURCE_SYSTEM_MEMORY,
        ResourceAttributes,
        DramInfo.DramRegion[DramInfo.NumOfDrams].BaseAddress,
        DramInfo.DramRegion[DramInfo.NumOfDrams].Size
      );
    }
  }

  // Build Memory Allocation Hob
  InitMmu (MemoryTable);

  if (FeaturePcdGet (PcdPrePiProduceMemoryTypeInformationHob)) {
    // Optional feature that helps prevent EFI memory map fragmentation.
    BuildMemoryTypeInformationHob ();
  }

  return EFI_SUCCESS;
}
