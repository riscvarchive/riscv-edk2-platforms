/** @file
  Copyright (c) 2016, Hisilicon Limited. All rights reserved.
  This program and the accompanying materials are licensed and made available
  under the terms and conditions of the BSD License which accompanies this
  distribution. The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS, WITHOUT
  WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
**/
#include <PlatformArch.h>
#include <IndustryStandard/Acpi.h>
#include <Library/AcpiNextLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/HwMemInitLib.h>
#include <Library/OemMiscLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>

#define CORE_NUM_PER_SOCKET  32
#define NODE_IN_SOCKET       2
#define CORECOUNT(X) ((X) * CORE_NUM_PER_SOCKET)

STATIC
VOID
RemoveUnusedMemoryNode (
  IN OUT EFI_ACPI_STATIC_RESOURCE_AFFINITY_TABLE  *Table,
  IN     UINTN                        MemoryNodeNum
)
{
  UINTN                   CurrPtr, NewPtr;

  if (MemoryNodeNum >= EFI_ACPI_MEMORY_AFFINITY_STRUCTURE_COUNT) {
    return;
  }

  CurrPtr = (UINTN) &(Table->Memory[EFI_ACPI_MEMORY_AFFINITY_STRUCTURE_COUNT]);
  NewPtr = (UINTN) &(Table->Memory[MemoryNodeNum]);

  CopyMem ((VOID *)NewPtr, (VOID *)CurrPtr, (UINTN)Table + Table->Header.Header.Length - CurrPtr);

  Table->Header.Header.Length -= CurrPtr - NewPtr;

  return;
}

STATIC
EFI_STATUS
UpdateSrat (
  IN OUT EFI_ACPI_STATIC_RESOURCE_AFFINITY_TABLE *Table
  )
{
  UINT8               Skt = 0;
  UINTN               Index = 0;
  VOID                *HobList;
  GBL_DATA            *Gbl_Data;
  UINTN               Base;
  UINTN               Size;
  UINT8               NodeId;
  UINT32              ScclInterleaveEn;
  UINTN               MemoryNode = 0;

  DEBUG((DEBUG_INFO, "SRAT: Updating SRAT memory information.\n"));

  HobList = GetHobList();
  if (HobList == NULL) {
    return EFI_UNSUPPORTED;
  }
  Gbl_Data = (GBL_DATA*)GetNextGuidHob(&gHisiEfiMemoryMapGuid, HobList);
  if (Gbl_Data == NULL) {
    DEBUG((DEBUG_ERROR, "Get next Guid HOb fail.\n"));
    return EFI_NOT_FOUND;
  }
  Gbl_Data = GET_GUID_HOB_DATA(Gbl_Data);
  for(Skt = 0; Skt < MAX_SOCKET; Skt++) {
    for(Index = 0; Index < MAX_NUM_PER_TYPE; Index++) {
      NodeId = Gbl_Data->NumaInfo[Skt][Index].NodeId;
      Base = Gbl_Data->NumaInfo[Skt][Index].Base;
      Size = Gbl_Data->NumaInfo[Skt][Index].Length;
      DEBUG((DEBUG_INFO, "Skt %d Index %d: NodeId = %d, Base = 0x%lx, Size = 0x%lx\n", Skt, Index, NodeId, Base, Size));
      if (Size > 0) {
        Table->Memory[MemoryNode].ProximityDomain = NodeId;
        Table->Memory[MemoryNode].AddressBaseLow = Base;
        Table->Memory[MemoryNode].AddressBaseHigh = Base >> 32;
        Table->Memory[MemoryNode].LengthLow = Size;
        Table->Memory[MemoryNode].LengthHigh = Size >> 32;
        MemoryNode = MemoryNode + 1;
      }
    }
    ScclInterleaveEn = Gbl_Data->NumaInfo[Skt][0].ScclInterleaveEn;
    DEBUG((DEBUG_INFO, "ScclInterleaveEn = %d\n", ScclInterleaveEn));
    //update gicc structure
    if (ScclInterleaveEn != 0) {
      DEBUG((DEBUG_INFO, "SRAT: Updating SRAT Gicc information.\n"));
      for (Index = CORECOUNT (Skt); Index < CORECOUNT (Skt + 1); Index++) {
        Table->Gicc[Index].ProximityDomain = Skt * NODE_IN_SOCKET;
      }
    }
  }

  //remove invalid memory node
  RemoveUnusedMemoryNode (Table, MemoryNode);

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
UpdateSlit (
  IN OUT EFI_ACPI_DESCRIPTION_HEADER  *Table
  )
{
  return  EFI_SUCCESS;
}

EFI_STATUS
UpdateAcpiTable (
  IN OUT EFI_ACPI_DESCRIPTION_HEADER      *TableHeader
)
{
  EFI_STATUS Status = EFI_SUCCESS;

  switch (TableHeader->Signature) {

  case EFI_ACPI_6_0_SYSTEM_RESOURCE_AFFINITY_TABLE_SIGNATURE:
    Status = UpdateSrat ((EFI_ACPI_STATIC_RESOURCE_AFFINITY_TABLE *) TableHeader);
    break;

  case EFI_ACPI_6_0_SYSTEM_LOCALITY_INFORMATION_TABLE_SIGNATURE:
    Status = UpdateSlit (TableHeader);
    break;
  }
  return Status;
}
