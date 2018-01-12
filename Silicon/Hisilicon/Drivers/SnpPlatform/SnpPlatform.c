/** @file
*
*  Copyright (c) 2017, Hisilicon Limited. All rights reserved.
*  Copyright (c) 2017, Linaro Limited. All rights reserved.
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

#include <PiDxe.h>
#include <Uefi.h>
#include <Guid/EventGroup.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiLib.h>

#include <Protocol/SnpPlatformProtocol.h>

typedef struct {
  UINTN                      Signature;
  EFI_HANDLE                 Handle;
  HISI_PLATFORM_SNP_PROTOCOL SnpPlatformProtocol;
} SNP_PLATFORM_INSTANCE;

STATIC HISI_PLATFORM_SNP_PROTOCOL mSnpPlatformProtocol[] = {
  {
    4,
    1
  },
  {
    5,
    1
  },
  {
    2,
    0
  },
  {
    3,
    0
  },
  {
    0,
    1
  },
 {
    1,
    1
  },
  {
    6,
    0
  },
  {
    7,
    0
  }
};


EFI_STATUS
EFIAPI
SnpPlatformInitialize (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  UINTN                    Loop;
  SNP_PLATFORM_INSTANCE    *PrivateData;
  EFI_STATUS               Status;

  for (Loop = 0; Loop < ARRAY_SIZE (mSnpPlatformProtocol); Loop++) {
    if(mSnpPlatformProtocol[Loop].Enable != 1) {
      continue;
    }
    PrivateData = AllocateZeroPool (sizeof(SNP_PLATFORM_INSTANCE));
    if (PrivateData == NULL) {
      DEBUG ((DEBUG_INFO,"SnpPlatformInitialize  error 1\n"));
      return EFI_OUT_OF_RESOURCES;
    }


    PrivateData->SnpPlatformProtocol = mSnpPlatformProtocol[Loop];

    //
    // Install the snp protocol, device path protocol
    //
    Status = gBS->InstallMultipleProtocolInterfaces (
                  &PrivateData->Handle,
                  &gHisiSnpPlatformProtocolGuid,
                  &PrivateData->SnpPlatformProtocol,
                  NULL
                  );
    if (EFI_ERROR (Status)) {
      FreePool (PrivateData);
      DEBUG ((DEBUG_ERROR, "InstallProtocolInterface fail. %r\n", Status));
      continue;
    }
  }

  DEBUG ((DEBUG_INFO,"SnpPlatformInitialize succes!\n"));

  return EFI_SUCCESS;
}
