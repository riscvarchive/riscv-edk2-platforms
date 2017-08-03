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

#include <PlatformArch.h>
#include <Uefi.h>

#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/LpcLib.h>
#include <Library/OemAddressMapLib.h>
#include <Library/OemMiscLib.h>
#include <Library/PcdLib.h>
#include <Library/PlatformPciLib.h>
#include <Library/PlatformSysCtrlLib.h>
#include <Library/SerialPortLib.h>
#include <Library/TimerLib.h>

#define OEM_SINGLE_SOCKET 1
#define OEM_DUAL_SOCKET 2

REPORT_PCIEDIDVID2BMC  PcieDeviceToReport[PCIEDEVICE_REPORT_MAX] = {
  {67,0,0,0},
  {225,0,0,3},
  {0xFFFF,0xFFFF,0xFFFF,0xFFFF},
  {0xFFFF,0xFFFF,0xFFFF,0xFFFF}
};


BOOLEAN OemIsSocketPresent (UINTN Socket)
{
  if (PcdGet32(PcdSocketMask) & (1 << Socket)) {
    return TRUE;
  } else {
    return FALSE;
  }
}


UINTN OemGetSocketNumber (VOID)
{

  if(!OemIsMpBoot()) {
    return OEM_SINGLE_SOCKET;
  }

  return OEM_DUAL_SOCKET;
}


UINTN OemGetDdrChannel (VOID)
{
  return 4;
}


UINTN OemGetDimmSlot(UINTN Socket, UINTN Channel)
{
  return 2;
}

VOID CoreSelectBoot(VOID)
{
  if (!PcdGet64 (PcdTrustedFirmwareEnable)) {
      StartupAp ();
  }

  return;
}

BOOLEAN OemIsMpBoot()
{
  return PcdGet32(PcdIsMPBoot);
}

VOID OemLpcInit(VOID)
{
  LpcInit();
  return;
}

UINT32 OemIsWarmBoot(VOID)
{
  return 0;
}

VOID OemBiosSwitch(UINT32 Master)
{
  (VOID)Master;
  return;
}

BOOLEAN OemIsNeedDisableExpanderBuffer(VOID)
{
  return TRUE;
}
