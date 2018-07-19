/** @file
*
*  Copyright (c) 2018, Hisilicon Limited. All rights reserved.
*  Copyright (c) 2018, Linaro Limited. All rights reserved.
*
*  This program and the accompanying materials
*  are licensed and made available under the terms and conditions of the BSD License
*  which accompanies this distribution.  The full text of the license may be found at
*  http://opensource.org/licenses/bsd-license.php
*
*  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
*  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
*
**/

#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/OemMiscLib.h>
#include <Library/PcdLib.h>
#include <Library/PciExpressLib.h>
#include <Library/PlatformPciLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Regs/HisiPcieV1RegOffset.h>


STATIC
UINT64
GetPcieCfgAddress (
    UINT64 Ecam,
    UINTN Bus,
    UINTN Device,
    UINTN Function,
    UINTN Reg
    )
{
  return Ecam + PCI_EXPRESS_LIB_ADDRESS (Bus, Device, Function, Reg);
}

STATIC
VOID
SetAtuConfig0RW (
    PCI_ROOT_BRIDGE_RESOURCE_APPETURE *Private,
    UINT32 Index
    )
{
  UINTN RbPciBase = Private->RbPciBar;
  UINT64 MemLimit = GetPcieCfgAddress (Private->Ecam, Private->BusBase + 1, 1, 0, 0) - 1;
  UINT64 MemBase = GetPcieCfgAddress (Private->Ecam, Private->BusBase, 0, 0, 0);


  MmioWrite32 (RbPciBase + IATU_OFFSET + IATU_VIEW_POINT, Index);
  MmioWrite32 (RbPciBase + IATU_OFFSET + IATU_REGION_BASE_LOW, (UINT32)MemBase);
  MmioWrite32 (RbPciBase + IATU_OFFSET + IATU_REGION_BASE_HIGH, (UINT32)(MemBase >> 32));
  MmioWrite32 (RbPciBase + IATU_OFFSET + IATU_REGION_BASE_LIMIT, (UINT32) MemLimit);
  MmioWrite32 (RbPciBase + IATU_OFFSET + IATU_REGION_TARGET_LOW, 0);
  MmioWrite32 (RbPciBase + IATU_OFFSET + IATU_REGION_TARGET_HIGH, 0);
  MmioWrite32 (RbPciBase + IATU_OFFSET + IATU_REGION_CTRL1, IATU_CTRL1_TYPE_CONFIG0);
  MmioWrite32 (RbPciBase + IATU_OFFSET + IATU_REGION_CTRL2, IATU_SHIIF_MODE);

  {
    UINTN i;
    for (i=0; i<0x20; i+=4) {
      DEBUG ((DEBUG_ERROR, "[%a:%d] - Base=%p value=%x\n", __FUNCTION__, __LINE__, RbPciBase + 0x900 + i, MmioRead32(RbPciBase + 0x900 + i)));
    }
  }
}

STATIC
VOID
SetAtuConfig1RW (
    PCI_ROOT_BRIDGE_RESOURCE_APPETURE *Private,
    UINT32 Index
    )
{
  UINTN RbPciBase = Private->RbPciBar;
  UINT64 MemLimit = GetPcieCfgAddress (Private->Ecam, Private->BusLimit + 1, 0, 0, 0) - 1;
  UINT64 MemBase = GetPcieCfgAddress (Private->Ecam, Private->BusBase + 2, 0, 0, 0);


  MmioWrite32 (RbPciBase + IATU_OFFSET + IATU_VIEW_POINT, Index);
  MmioWrite32 (RbPciBase + IATU_OFFSET + IATU_REGION_CTRL1, IATU_CTRL1_TYPE_CONFIG1);
  MmioWrite32 (RbPciBase + IATU_OFFSET + IATU_REGION_BASE_LOW, (UINT32)MemBase);
  MmioWrite32 (RbPciBase + IATU_OFFSET + IATU_REGION_BASE_HIGH, (UINT32)(MemBase >> 32));
  MmioWrite32 (RbPciBase + IATU_OFFSET + IATU_REGION_BASE_LIMIT, (UINT32) MemLimit);
  MmioWrite32 (RbPciBase + IATU_OFFSET + IATU_REGION_TARGET_LOW, 0);
  MmioWrite32 (RbPciBase + IATU_OFFSET + IATU_REGION_TARGET_HIGH, 0);
  MmioWrite32 (RbPciBase + IATU_OFFSET + IATU_REGION_CTRL2, IATU_SHIIF_MODE);

  {
    UINTN i;
    for (i=0; i<0x20; i+=4) {
      DEBUG ((DEBUG_ERROR, "[%a:%d] - Base=%p value=%x\n", __FUNCTION__, __LINE__, RbPciBase + 0x900 + i, MmioRead32(RbPciBase + 0x900 + i)));
    }
  }
}

STATIC
VOID
SetAtuIoRW (UINT64 RbPciBase,UINT64 IoBase,UINT64 CpuIoRegionLimit, UINT64 CpuIoRegionBase, UINT32 Index)
{

    MmioWrite32 (RbPciBase + IATU_OFFSET + IATU_VIEW_POINT, Index);
    MmioWrite32 (RbPciBase + IATU_OFFSET + IATU_REGION_CTRL1, IATU_CTRL1_TYPE_IO);

    MmioWrite32 (RbPciBase + IATU_OFFSET + IATU_REGION_BASE_LOW, (UINT32)(CpuIoRegionBase));
    MmioWrite32 (RbPciBase + IATU_OFFSET + IATU_REGION_BASE_HIGH, (UINT32)((UINT64)CpuIoRegionBase >> 32));
    MmioWrite32 (RbPciBase + IATU_OFFSET + IATU_REGION_BASE_LIMIT, (UINT32)(CpuIoRegionLimit));
    MmioWrite32 (RbPciBase + IATU_OFFSET + IATU_REGION_TARGET_LOW, (UINT32)(IoBase));
    MmioWrite32 (RbPciBase + IATU_OFFSET + IATU_REGION_TARGET_HIGH, (UINT32)((UINT64)(IoBase) >> 32));

    MmioWrite32 (RbPciBase + IATU_OFFSET + IATU_REGION_CTRL2, IATU_NORMAL_MODE);

    {
      UINTN i;
      for (i=0; i<0x20; i+=4) {
        DEBUG ((DEBUG_ERROR, "[%a:%d] - Base=%p value=%x\n", __FUNCTION__, __LINE__, RbPciBase + 0x900 + i, MmioRead32(RbPciBase + 0x900 + i)));
      }
    }
}

STATIC
VOID
SetAtuMemRW(UINT64 RbPciBase,UINT64 MemBase,UINT64 CpuMemRegionLimit, UINT64 CpuMemRegionBase, UINT32 Index)
{

    MmioWrite32 (RbPciBase + IATU_OFFSET + IATU_VIEW_POINT, Index);
    MmioWrite32 (RbPciBase + IATU_OFFSET + IATU_REGION_CTRL1, IATU_CTRL1_TYPE_MEM);

    MmioWrite32 (RbPciBase + IATU_OFFSET + IATU_REGION_BASE_LOW, (UINT32)(CpuMemRegionBase));
    MmioWrite32 (RbPciBase + IATU_OFFSET + IATU_REGION_BASE_HIGH, (UINT32)((UINT64)(CpuMemRegionBase) >> 32));
    MmioWrite32 (RbPciBase + IATU_OFFSET + IATU_REGION_BASE_LIMIT, (UINT32)(CpuMemRegionLimit));
    MmioWrite32 (RbPciBase + IATU_OFFSET + IATU_REGION_TARGET_LOW, (UINT32)(MemBase));
    MmioWrite32 (RbPciBase + IATU_OFFSET + IATU_REGION_TARGET_HIGH, (UINT32)((UINT64)(MemBase) >> 32));

    MmioWrite32 (RbPciBase + IATU_OFFSET + IATU_REGION_CTRL2, IATU_NORMAL_MODE);

    {
      UINTN i;
      for (i=0; i<0x20; i+=4) {
        DEBUG ((DEBUG_ERROR, "[%a:%d] - Base=%p value=%x\n", __FUNCTION__, __LINE__, RbPciBase + 0x900 + i, MmioRead32(RbPciBase + 0x900 + i)));
      }
    }
}

VOID
InitAtu (PCI_ROOT_BRIDGE_RESOURCE_APPETURE *Private)
{
  SetAtuMemRW (Private->RbPciBar, Private->PciRegionBase, Private->PciRegionLimit, Private->CpuMemRegionBase, 0);
  SetAtuConfig0RW (Private, 1);
  SetAtuConfig1RW (Private, 2);
  SetAtuIoRW (Private->RbPciBar, Private->IoBase, Private->IoLimit, Private->CpuIoRegionBase, 3);
}

/*++

Routine Description:

  Perform Platform initialization first in PciPlatform.

Arguments:

Returns:

 VOID.

--*/
VOID
EFIAPI
PciInitPlatform (
  VOID
  )
{
  UINT32             Port;
  UINT32             HostBridgeNum = 0;

  for (HostBridgeNum = 0; HostBridgeNum < PCIE_MAX_HOSTBRIDGE; HostBridgeNum++) {
    for (Port = 0; Port < PCIE_MAX_ROOTBRIDGE; Port++) {
      InitAtu (&mResAppeture[HostBridgeNum][Port]);
    }
  }

  return;
}

