/** @file

  Copyright (c) 2016, AMD Inc. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Base.h>
#include <PiDxe.h>
#include <Uefi.h>
#include <Library/ArmLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>

#include <Common/CoreState.h>
#include <Library/ArmSmcLib.h>
#include <IndustryStandard/ArmStdSmc.h>
#include <Guid/ArmMpCoreInfo.h>

#include <Protocol/AmdMpCoreInfo.h>
#include <Protocol/AmdMpBoot.h>


STATIC AMD_MP_CORE_INFO_PROTOCOL  mAmdMpCoreInfoProtocol = { 0 };
STATIC AMD_MP_BOOT_PROTOCOL       mAmdMpBootProtocol = { 0 };
STATIC AMD_MP_BOOT_INFO           mAmdMpBootInfo = { 0 };


STATIC
ARM_CORE_INFO *
AmdStyxGetArmCoreInfoTable (
  OUT UINTN  *NumEntries
  );

STATIC
EFI_STATUS
AmdStyxGetPmuSpiFromMpId (
  IN  UINT32  MpId,
  OUT UINT32  *PmuSpi
  );

STATIC
EFI_PHYSICAL_ADDRESS
AmdStyxGetMpParkingBase (
  OUT UINTN  *MpParkingSize
  );

STATIC
VOID
AmdStyxParkSecondaryCore (
  ARM_CORE_INFO         *ArmCoreInfo,
  EFI_PHYSICAL_ADDRESS  SecondaryEntry
  );


#pragma pack(push, 1)
typedef struct _PMU_INFO {
  UINT32 MpId;
  UINT32 PmuSpi;
} PMU_INFO;

STATIC
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
EFIAPI
PlatInitDxeEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                Status;
  EFI_PHYSICAL_ADDRESS      MpParkingBase;
  UINTN                     MpParkingSize;
  ARM_CORE_INFO             *ArmCoreInfoTable;
  UINTN                     ArmCoreCount;
  EFI_HANDLE                Handle = NULL;

  DEBUG ((EFI_D_ERROR, "PlatInitDxe Loaded\n"));

  // Get core information
  ArmCoreCount = 0;
  ArmCoreInfoTable = AmdStyxGetArmCoreInfoTable (&ArmCoreCount);
  ASSERT (ArmCoreInfoTable != NULL);
  ASSERT (ArmCoreCount != 0);

  // Install CoreInfo Protocol
  mAmdMpCoreInfoProtocol.GetArmCoreInfoTable = AmdStyxGetArmCoreInfoTable;
  mAmdMpCoreInfoProtocol.GetPmuSpiFromMpId = AmdStyxGetPmuSpiFromMpId;
  mAmdMpCoreInfoProtocol.GetMpParkingBase = AmdStyxGetMpParkingBase;
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gAmdMpCoreInfoProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  (VOID *)&mAmdMpCoreInfoProtocol
                  );
  ASSERT_EFI_ERROR (Status);

  // Install MP-Boot Protocol
  if (!FixedPcdGetBool (PcdPsciOsSupport) &&
      FixedPcdGetBool (PcdTrustedFWSupport)) {
    // Allocate Parking area (4KB-aligned, 4KB per core) as Reserved memory
    MpParkingBase = 0;
    MpParkingSize = ArmCoreCount * SIZE_4KB;
    Status = gBS->AllocatePages (AllocateAnyPages, EfiReservedMemoryType,
                    EFI_SIZE_TO_PAGES (MpParkingSize),
                    &MpParkingBase);
    if (EFI_ERROR (Status) || MpParkingBase == 0) {
      DEBUG ((EFI_D_ERROR, "Warning: Failed to allocate MpParkingBase."));
    } else {
      mAmdMpBootInfo.MpParkingBase = MpParkingBase;
      mAmdMpBootInfo.MpParkingSize = MpParkingSize;
      mAmdMpBootInfo.ArmCoreInfoTable = ArmCoreInfoTable;
      mAmdMpBootInfo.ArmCoreCount = ArmCoreCount;

      mAmdMpBootProtocol.ParkSecondaryCore = AmdStyxParkSecondaryCore;
      mAmdMpBootProtocol.MpBootInfo = &mAmdMpBootInfo;

      Status = gBS->InstallProtocolInterface (
                      &Handle,
                      &gAmdMpBootProtocolGuid,
                      EFI_NATIVE_INTERFACE,
                      (VOID *)&mAmdMpBootProtocol
                      );
      if (EFI_ERROR (Status)) {
        DEBUG ((EFI_D_ERROR, "Warning: Failed to install MP-Boot Protocol."));
        gBS->FreePages (MpParkingBase, EFI_SIZE_TO_PAGES (MpParkingSize));
      }
    }
  }

  return Status;
}


STATIC
ARM_CORE_INFO *
AmdStyxGetArmCoreInfoTable (
  OUT UINTN  *NumEntries
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


STATIC
EFI_STATUS
AmdStyxGetPmuSpiFromMpId (
  IN  UINT32  MpId,
  OUT UINT32  *PmuSpi
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


STATIC
EFI_PHYSICAL_ADDRESS
AmdStyxGetMpParkingBase (
  OUT UINTN  *MpParkingSize
  )
{
  ASSERT (MpParkingSize != NULL);

  *MpParkingSize = mAmdMpBootInfo.MpParkingBase;
  return mAmdMpBootInfo.MpParkingBase;
}


STATIC
VOID
AmdStyxParkSecondaryCore (
  ARM_CORE_INFO         *ArmCoreInfo,
  EFI_PHYSICAL_ADDRESS  SecondaryEntry
  )
{
  ARM_SMC_ARGS  SmcRegs = {0};
  UINTN         MpId;

  MpId = GET_MPID (ArmCoreInfo->ClusterId, ArmCoreInfo->CoreId);

  SmcRegs.Arg0 = ARM_SMC_ID_PSCI_CPU_ON_AARCH64;
  SmcRegs.Arg1 = MpId;
  SmcRegs.Arg2 = SecondaryEntry;
  SmcRegs.Arg3 = FixedPcdGet64 (PcdPsciCpuOnContext);
  ArmCallSmc (&SmcRegs);

  if (SmcRegs.Arg0 == ARM_SMC_PSCI_RET_SUCCESS ||
      SmcRegs.Arg0 == ARM_SMC_PSCI_RET_ALREADY_ON) {
    DEBUG ((EFI_D_ERROR, "CPU[MpId] = 0x%X at RUN state.\n", MpId));
  } else {
    DEBUG ((EFI_D_ERROR, "Warning: Could not transition CPU[MpId] = 0x%X to RUN state.\n", MpId));
  }
}
