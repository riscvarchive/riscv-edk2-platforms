/********************************************************************************
Copyright (C) 2016 Marvell International Ltd.

Marvell BSD License Option

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File under the following licensing terms.
Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.

* Neither the name of Marvell nor the names of its contributors may be
  used to endorse or promote products derived from this software without
  specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/

#include "ComPhyLib.h"
#include <Library/MvComPhyLib.h>
#include <Library/MvHwDescLib.h>

DECLARE_A7K8K_COMPHY_TEMPLATE;

CHAR16 * TypeStringTable [] = {L"unconnected", L"PCIE0", L"PCIE1", L"PCIE2",
                           L"PCIE3", L"SATA0", L"SATA1", L"SATA2", L"SATA3",
                           L"SGMII0", L"SGMII1", L"SGMII2", L"SGMII3",
                           L"QSGMII", L"USB3_HOST0", L"USB3_HOST1",
                           L"USB3_DEVICE", L"XAUI0", L"XAUI1", L"XAUI2",
                           L"XAUI3", L"RXAUI0", L"RXAUI1", L"SFI"};

CHAR16 * SpeedStringTable [] = {L"-", L"1.25 Gbps", L"1.5 Gbps", L"2.5 Gbps",
                                L"3.0 Gbps", L"3.125 Gbps", L"5 Gbps", L"5.156 Gbps",
                                L"6 Gbps", L"6.25 Gbps", L"10.31 Gbps"};

CHIP_COMPHY_CONFIG ChipCfgTbl[] = {
  {
    .ChipType = MvComPhyTypeCp110,
    .Init = ComPhyCp110Init
  },
};

VOID
RegSet (
  IN EFI_PHYSICAL_ADDRESS Addr,
  IN UINT32 Data,
  IN UINT32 Mask
  )
{
  DEBUG((DEBUG_INFO, "Write to address = %10x, data = %10x (mask = %10x)"
    "- ", Addr, Data, Mask));
  DEBUG((DEBUG_INFO, "old value = %10x ==> ", MmioRead32 (Addr)));
  RegSetSilent (Addr, Data, Mask);
  DEBUG((DEBUG_INFO, "new value %10x\n", MmioRead32 (Addr)));
}

VOID
RegSetSilent (
  IN EFI_PHYSICAL_ADDRESS Addr,
  IN UINT32 Data,
  IN UINT32 Mask
  )
{
  UINT32 RegData;

  RegData = MmioRead32 (Addr);
  RegData &= ~Mask;
  RegData |= Data;
  MmioWrite32 (Addr, RegData);
}

VOID
RegSet16 (
  IN EFI_PHYSICAL_ADDRESS Addr,
  IN UINT16 Data,
  IN UINT16 Mask
  )
{
  DEBUG((DEBUG_INFO, "Write to address = %#010lx, Data = %#06x (mask = %#06x)"
    "- ", Addr, Data, Mask));
  DEBUG((DEBUG_INFO, "old value = %#06x ==> ", MmioRead16 (Addr)));
  RegSetSilent16 (Addr, Data, Mask);
  DEBUG((DEBUG_INFO, "new value %#06x\n", MmioRead16 (Addr)));
}

VOID
RegSetSilent16(
  IN EFI_PHYSICAL_ADDRESS Addr,
  IN UINT16 Data,
  IN UINT16 Mask
  )
{
  UINT16 RegData;
  RegData = MmioRead16(Addr);
  RegData &= ~Mask;
  RegData |= Data;
  MmioWrite16 (Addr, RegData);
}

/* This function returns enum with SerDesType */
UINT32
ParseSerdesTypeString (
  CHAR16* String
  )
{
  UINT32 i;

  if (String == NULL)
    return COMPHY_TYPE_INVALID;

  for (i = 0; i < COMPHY_TYPE_MAX; i++) {
    if (StrCmp (String, TypeStringTable[i]) == 0) {
      return i;
    }
  }

  /* PCD string doesn't match any supported SerDes Type */
  return COMPHY_TYPE_INVALID;
}

/* This function converts SerDes speed in MHz to enum with SerDesSpeed */
UINT32
ParseSerdesSpeed (
  UINT32 Value
  )
{
  UINT32 i;
  UINT32 ValueTable [] = {0, 1250, 1500, 2500, 3000, 3125,
                          5000, 5156, 6000, 6250, 10310};

  for (i = 0; i < COMPHY_SPEED_MAX; i++) {
    if (Value == ValueTable[i]) {
      return i;
    }
  }

  /* PCD SerDes speed value doesn't match any supported SerDes speed */
  return COMPHY_SPEED_INVALID;
}

CHAR16 *
GetTypeString (
  UINT32 Type
  )
{

  if (Type < 0 || Type > COMPHY_TYPE_MAX) {
    return L"invalid";
  }

  return TypeStringTable[Type];
}

CHAR16 *
GetSpeedString (
  UINT32 Speed
  )
{

  if (Speed < 0 || Speed > 10) {
    return L"invalid";
  }

  return SpeedStringTable[Speed];
}

VOID
ComPhyPrint (
  IN CHIP_COMPHY_CONFIG *PtrChipCfg
  )
{
  UINT32 Lane;
  CHAR16 *SpeedStr, *TypeStr;

  for (Lane = 0; Lane < PtrChipCfg->LanesCount; Lane++) {
    SpeedStr = GetSpeedString(PtrChipCfg->MapData[Lane].Speed);
    TypeStr = GetTypeString(PtrChipCfg->MapData[Lane].Type);
    DEBUG((DEBUG_ERROR, "Comphy-%d: %-13s %-10s\n", Lane, TypeStr, SpeedStr));
  }

  DEBUG((DEBUG_ERROR, "\n"));
}

EFI_STATUS
GetChipComPhyInit (
  IN CHIP_COMPHY_CONFIG *PtrChipCfg
  )
{
  UINTN i, TblSize;


  TblSize = sizeof(ChipCfgTbl) / sizeof(ChipCfgTbl[0]);

  for (i = 0; i < TblSize ; i++) {
    if (PtrChipCfg->ChipType == ChipCfgTbl[i].ChipType) {
      PtrChipCfg->Init = ChipCfgTbl[i].Init;
      return EFI_SUCCESS;
    }
  }

  return EFI_D_ERROR;
}

STATIC
VOID
InitComPhyConfig (
  IN  OUT  CHIP_COMPHY_CONFIG *ChipConfig,
  IN  OUT  PCD_LANE_MAP       *LaneData,
  IN       UINT8               Id
  )
{
  MVHW_COMPHY_DESC *Desc = &mA7k8kComPhyDescTemplate;

  ChipConfig->ChipType = Desc->ComPhyChipType[Id];
  ChipConfig->ComPhyBaseAddr = Desc->ComPhyBaseAddresses[Id];
  ChipConfig->Hpipe3BaseAddr = Desc->ComPhyHpipe3BaseAddresses[Id];
  ChipConfig->LanesCount = Desc->ComPhyLaneCount[Id];
  ChipConfig->MuxBitCount = Desc->ComPhyMuxBitCount[Id];

  /*
   * Below macro contains variable name concatenation (used to form PCD's name).
   */
  switch (Id) {
  case 0:
    GetComPhyPcd (ChipConfig, LaneData, 0);
    break;
  case 1:
    GetComPhyPcd (ChipConfig, LaneData, 1);
    break;
  case 2:
    GetComPhyPcd (ChipConfig, LaneData, 2);
    break;
  case 3:
    GetComPhyPcd (ChipConfig, LaneData, 3);
    break;
  }
}

EFI_STATUS
MvComPhyInit (
  VOID
  )
{
  EFI_STATUS Status;
  CHIP_COMPHY_CONFIG ChipConfig[MVHW_MAX_COMPHY_DEVS], *PtrChipCfg;
  PCD_LANE_MAP LaneData[MVHW_MAX_COMPHY_DEVS];
  UINT32 Lane, MaxComphyCount;
  UINT8 *ComPhyDeviceTable, Index;

  /* Obtain table with enabled ComPhy devices */
  ComPhyDeviceTable = (UINT8 *)PcdGetPtr (PcdComPhyDevices);
  if (ComPhyDeviceTable == NULL) {
    DEBUG ((DEBUG_ERROR, "Missing PcdComPhyDevices\n"));
    return EFI_INVALID_PARAMETER;
  }

  if (PcdGetSize (PcdComPhyDevices) > MVHW_MAX_COMPHY_DEVS) {
    DEBUG ((DEBUG_ERROR, "Wrong PcdComPhyDevices format\n"));
    return EFI_INVALID_PARAMETER;
  }

  /* Initialize enabled chips */
  for (Index = 0; Index < PcdGetSize (PcdComPhyDevices); Index++) {
    if (!MVHW_DEV_ENABLED (ComPhy, Index)) {
      DEBUG ((DEBUG_ERROR, "Skip ComPhy chip %d\n", Index));
      continue;
    }

    PtrChipCfg = &ChipConfig[Index];
    InitComPhyConfig(PtrChipCfg, LaneData, Index);

    /* Get the count of the SerDes of the specific chip */
    MaxComphyCount = PtrChipCfg->LanesCount;
    for (Lane = 0; Lane < MaxComphyCount; Lane++) {
      /* Parse PCD with string indicating SerDes Type */
      PtrChipCfg->MapData[Lane].Type =
        ParseSerdesTypeString (LaneData[Index].TypeStr[Lane]);
      PtrChipCfg->MapData[Lane].Speed =
        ParseSerdesSpeed (LaneData[Index].SpeedValue[Lane]);
      PtrChipCfg->MapData[Lane].Invert = (UINT32)LaneData[Index].InvFlag[Lane];

      if ((PtrChipCfg->MapData[Lane].Speed == COMPHY_SPEED_INVALID) ||
          (PtrChipCfg->MapData[Lane].Speed == COMPHY_SPEED_ERROR) ||
          (PtrChipCfg->MapData[Lane].Type == COMPHY_TYPE_INVALID)) {
        DEBUG((DEBUG_ERROR, "ComPhy: No valid phy speed or type for lane %d, "
          "setting lane as unconnected\n", Lane + 1));
        PtrChipCfg->MapData[Lane].Type = COMPHY_TYPE_UNCONNECTED;
        PtrChipCfg->MapData[Lane].Speed = COMPHY_SPEED_INVALID;
      }
    };

    Status = GetChipComPhyInit (PtrChipCfg);
    if (EFI_ERROR(Status)) {
     DEBUG ((DEBUG_ERROR, "ComPhy: Invalid Chip%d type\n", Index));
     return Status;
    }

    ComPhyPrint (PtrChipCfg);

    /* PHY power UP sequence */
    PtrChipCfg->Init (PtrChipCfg);
  }

  return EFI_SUCCESS;
}
