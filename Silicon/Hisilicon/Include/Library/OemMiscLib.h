/** @file
*
*  Copyright (c) 2015, Hisilicon Limited. All rights reserved.
*  Copyright (c) 2015, Linaro Limited. All rights reserved.
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


#ifndef _OEM_MISC_LIB_H_
#define _OEM_MISC_LIB_H_

#include <Uefi.h>

#include <PlatformArch.h>
#include <Library/I2CLib.h>

#define PCIEDEVICE_REPORT_MAX      4
typedef struct _REPORT_PCIEDIDVID2BMC{
    UINTN   Bus;
    UINTN   Device;
    UINTN   Function;
    UINTN   Slot;
}REPORT_PCIEDIDVID2BMC;
extern REPORT_PCIEDIDVID2BMC PcieDeviceToReport[PCIEDEVICE_REPORT_MAX];

BOOLEAN OemIsSocketPresent (UINTN Socket);
VOID CoreSelectBoot(VOID);
VOID OemPcieResetAndOffReset(void);
extern I2C_DEVICE gDS3231RtcDevice;

UINTN OemGetSocketNumber(VOID);
UINTN OemGetDdrChannel (VOID);
UINTN OemGetDimmSlot(UINTN Socket, UINTN Channel);

BOOLEAN OemIsMpBoot();
UINT32 OemIsWarmBoot();

VOID OemBiosSwitch(UINT32 Master);
BOOLEAN OemIsNeedDisableExpanderBuffer(VOID);

extern EFI_STRING_ID gDimmToDevLocator[MAX_SOCKET][MAX_CHANNEL][MAX_DIMM];
EFI_HII_HANDLE EFIAPI OemGetPackages ();
#endif
