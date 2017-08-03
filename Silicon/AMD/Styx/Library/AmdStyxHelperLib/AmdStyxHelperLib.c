/** @file

  Copyright (c) 2014 - 2016, AMD Inc. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <AmdStyxHelperLib.h>

#include <PiDxe.h>
#include <Library/HobLib.h>

extern  EFI_SYSTEM_TABLE        *gST;

#pragma pack(push, 1)
typedef struct {
  UINT32 MpId;
  UINT32 PmuSpi;
} PMU_INFO;

PMU_INFO mPmuInfo[] = {
 {0x000, 7},
 {0x001, 8},
 {0x100, 9},
 {0x101, 10},
 {0x200, 11},
 {0x201, 12},
 {0x300, 13},
 {0x301, 14}
};
#pragma pack(pop)

#define MAX_CPUS   sizeof(mPmuInfo) / sizeof(PMU_INFO)

EFI_STATUS
AmdStyxGetPmuSpiFromMpId (
  UINT32 MpId,
  UINT32 *PmuSpi
  )
{
  UINT32 i;

  for (i = 0; i < MAX_CPUS; ++i) {
    if (mPmuInfo[ i ].MpId == MpId) {
      *PmuSpi = mPmuInfo[ i ].PmuSpi;
      return EFI_SUCCESS;
    }
  }

  return EFI_INVALID_PARAMETER;
}

ARM_CORE_INFO *
AmdStyxGetArmCoreInfoTable (
  OUT UINTN   *NumEntries
  )
{
  EFI_HOB_GUID_TYPE   *Hob;

  ASSERT (NumEntries != NULL);

  Hob = GetFirstGuidHob (&gAmdStyxMpCoreInfoGuid);
  if (Hob == NULL) {
    return NULL;
  }

  *NumEntries = GET_GUID_HOB_DATA_SIZE (Hob) / sizeof (ARM_CORE_INFO);

  return GET_GUID_HOB_DATA (Hob);
}
