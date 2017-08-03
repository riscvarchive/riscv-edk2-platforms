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


#include <PiPei.h>
#include <PlatformArch.h>
#include <Uefi.h>
#include <Library/ArmLib.h>
#include <Library/CacheMaintenanceLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/OemAddressMapLib.h>
#include <Library/OemMiscLib.h>
#include <Library/PcdLib.h>
#include <Library/PlatformSysCtrlLib.h>

VOID
QResetAp (
  VOID
  )
{
  MmioWrite64(FixedPcdGet64(PcdMailBoxAddress), 0x0);
  (VOID)WriteBackInvalidateDataCacheRange((VOID *) FixedPcdGet64(PcdMailBoxAddress), 8);

  if (!PcdGet64 (PcdTrustedFirmwareEnable)) {
    StartupAp();
  }
}


EFI_STATUS
EFIAPI
EarlyConfigEntry (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  DEBUG((DEBUG_INFO,"SMMU CONFIG........."));
  (VOID)SmmuConfigForBios();
  DEBUG((DEBUG_INFO,"Done\n"));

  DEBUG((DEBUG_INFO,"AP CONFIG........."));
  (VOID)QResetAp();
  DEBUG((DEBUG_INFO,"Done\n"));

  DEBUG((DEBUG_INFO,"MN CONFIG........."));
  (VOID)MN_CONFIG();
  DEBUG((DEBUG_INFO,"Done\n"));

  return EFI_SUCCESS;
}

