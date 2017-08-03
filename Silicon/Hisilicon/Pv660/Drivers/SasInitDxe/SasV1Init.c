/** @file
*
*  Copyright (c) 2016, Hisilicon Limited. All rights reserved.
*  Copyright (c) 2016, Linaro Limited. All rights reserved.
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


#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/TimerLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>

#include <Protocol/PlatformSasProtocol.h>

#define SAS0_BASE                   0xc0000000
#define SAS0_RESET                  0xa60
#define SAS0_DISABLE_CLK            0x33c
#define SAS0_DERESET                0xa64
#define SAS0_ENABLE_CLK             0x338

#define SAS1_BASE                   0xb0000000
#define SAS1_RESET                  0xa18
#define SAS1_DISABLE_CLK            0x31c
#define SAS1_DERESET                0xa1c
#define SAS1_ENABLE_CLK             0x318

#define SAS_RESET_VALUE             0x7ffff

STATIC
VOID
SasInit_0 (
    IN PLATFORM_SAS_PROTOCOL   *This
)
{
  // Apply reset and disable clock
  MmioWrite32(SAS0_BASE + SAS0_RESET, SAS_RESET_VALUE);
  MmioWrite32(SAS0_BASE + SAS0_DISABLE_CLK, SAS_RESET_VALUE);
  // Wait 1ms for reset takes effect, refer drivers/scsi/hisi_sas/hisi_sas_v1_hw.c
  MicroSecondDelay(1000);
  // De-reset and enable clock
  MmioWrite32(SAS0_BASE + SAS0_DERESET, SAS_RESET_VALUE);
  MmioWrite32(SAS0_BASE + SAS0_ENABLE_CLK, SAS_RESET_VALUE);
  // Wait 1ms for de-reset takes effect, refer drivers/scsi/hisi_sas/hisi_sas_v1_hw.c
  MicroSecondDelay(1000);
}

PLATFORM_SAS_PROTOCOL Sas0 = {
  0xc1000000,
  SasInit_0
};

STATIC
VOID
SasInit_1 (
    IN PLATFORM_SAS_PROTOCOL   *This
)
{
  // Apply reset and disable clock
  MmioWrite32(SAS1_BASE + SAS1_RESET, SAS_RESET_VALUE);
  MmioWrite32(SAS1_BASE + SAS1_DISABLE_CLK, SAS_RESET_VALUE);
  // Wait 1ms for reset takes effect, refer drivers/scsi/hisi_sas/hisi_sas_v1_hw.c
  MicroSecondDelay(1000);
  // De-reset and enable clock
  MmioWrite32(SAS1_BASE + SAS1_DERESET, SAS_RESET_VALUE);
  MmioWrite32(SAS1_BASE + SAS1_ENABLE_CLK, SAS_RESET_VALUE);
  // Wait 1ms for de-reset takes effect, refer drivers/scsi/hisi_sas/hisi_sas_v1_hw.c
  MicroSecondDelay(1000);
}

PLATFORM_SAS_PROTOCOL Sas1 = {
  0xb1000000,
  SasInit_1
};

EFI_STATUS
EFIAPI
SasV1InitEntry (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_HANDLE  Handle;
  EFI_STATUS  Status;

  Handle = NULL;
  Status = gBS->InstallMultipleProtocolInterfaces(
                  &Handle,
                  &gPlatformSasProtocolGuid, &Sas0,
                  NULL
                 );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Handle = NULL;
  Status = gBS->InstallMultipleProtocolInterfaces(
                  &Handle,
                  &gPlatformSasProtocolGuid, &Sas1,
                  NULL
                 );
  return Status;
}
