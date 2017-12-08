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

STATIC
VOID
ComPhyMuxCheckConfig (
  IN COMPHY_MUX_DATA *MuxData,
  IN COMPHY_MAP *ComPhyMapData,
  IN UINTN ComPhyMaxLanes
  )
{
  COMPHY_MUX_OPTIONS *PtrMuxOpt;
  UINTN Lane, Opt, Valid;

  for (Lane = 0; Lane < ComPhyMaxLanes; Lane++, ComPhyMapData++, MuxData++) {
    PtrMuxOpt = MuxData->MuxValues;
    for (Opt = 0, Valid = 0; Opt < MuxData->MaxLaneValues; Opt++, PtrMuxOpt++) {
      if (PtrMuxOpt->Type == ComPhyMapData->Type) {
        Valid = 1;
        break;
      }
    }
    if (Valid == 0) {
      DEBUG((DEBUG_INFO, "Lane number %d, had invalid Type %d\n", Lane,
        ComPhyMapData->Type));
      DEBUG((DEBUG_INFO, "Set Lane %d as Type %d\n", Lane,
        COMPHY_TYPE_UNCONNECTED));
      ComPhyMapData->Type = COMPHY_TYPE_UNCONNECTED;
    } else {
      DEBUG((DEBUG_INFO, "Lane number %d, has Type %d\n", Lane,
        ComPhyMapData->Type));
    }
  }
}

STATIC
UINT32
ComPhyMuxGetMuxValue (
  IN COMPHY_MUX_DATA *MuxData,
  IN UINT32 Type,
  IN UINTN Lane
  )
{
  COMPHY_MUX_OPTIONS *PtrMuxOpt;
  UINTN Opt;
  UINT32 Value = 0;

  PtrMuxOpt = MuxData->MuxValues;
  for (Opt = 0 ; Opt < MuxData->MaxLaneValues; Opt++, PtrMuxOpt++)
    if (PtrMuxOpt->Type == Type) {
      Value = PtrMuxOpt->MuxValue;
      break;
    }

  return Value;
}

STATIC
VOID
ComPhyMuxRegWrite (
  IN COMPHY_MUX_DATA *MuxData,
  IN COMPHY_MAP *ComPhyMapData,
  IN UINTN ComPhyMaxLanes,
  IN EFI_PHYSICAL_ADDRESS SelectorBase,
  IN UINT32 BitCount
  )
{
  UINT32 Lane, Value, Offset, Mask;

  for (Lane = 0; Lane < ComPhyMaxLanes; Lane++, ComPhyMapData++, MuxData++) {
    Offset = Lane * BitCount;
    Mask = (((1 << BitCount) - 1) << Offset);
    Value = (ComPhyMuxGetMuxValue (MuxData, ComPhyMapData->Type, Lane) <<
      Offset);
    RegSet (SelectorBase, Value, Mask);
  }
}

VOID
ComPhyMuxInit (
  IN CHIP_COMPHY_CONFIG *PtrChipCfg,
  IN COMPHY_MAP *ComPhyMapData,
  IN EFI_PHYSICAL_ADDRESS SelectorBase
  )
{
  COMPHY_MUX_DATA *MuxData;
  UINT32 MuxBitCount;
  UINT32 ComPhyMaxLanes;

  ComPhyMaxLanes = PtrChipCfg->LanesCount;
  MuxData = PtrChipCfg->MuxData;
  MuxBitCount = PtrChipCfg->MuxBitCount;

  /* Check if the configuration is valid */
  ComPhyMuxCheckConfig (MuxData, ComPhyMapData, ComPhyMaxLanes);
  /* Init COMPHY selectors */
  ComPhyMuxRegWrite (MuxData, ComPhyMapData, ComPhyMaxLanes, SelectorBase,
    MuxBitCount);
}
