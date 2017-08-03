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

#ifndef _EHCI_PCIIO_H_
#define _EHCI_PCIIO_H_

#include <Uefi.h>
#include <PiDxe.h>

#include <Protocol/Usb2HostController.h>
#include <Protocol/PciIo.h>
#include <Protocol/Cpu.h>

#include <Guid/EventGroup.h>

#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/DevicePathLib.h>
#include <Library/ArmLib.h>
#include <Library/UncachedMemoryAllocationLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/TimerLib.h>
#include <Library/CacheMaintenanceLib.h>

#include <Library/ReportStatusCodeLib.h>

#include <IndustryStandard/Pci.h>
#include <Library/PlatformSysCtrlLib.h>

#define PCI_CLASS_SERIAL              0x0C
#define PCI_CLASS_SERIAL_FIREWIRE     0x00
#define PCI_CLASS_SERIAL_ACCESS_BUS   0x01
#define PCI_CLASS_SERIAL_SSA          0x02
#define PCI_CLASS_SERIAL_USB          0x03
#define PCI_IF_EHCI                   0x20
#define PCI_CLASS_SERIAL_FIBRECHANNEL 0x04
#define PCI_CLASS_SERIAL_SMB          0x05

//
// Capability register offset
//
#define EHC_CAPLENGTH_OFFSET    0    // Capability register length offset
#define EHC_HCSPARAMS_OFFSET    0x04 // Structural Parameters 04-07h
#define EHC_HCCPARAMS_OFFSET    0x08 // Capability parameters offset

//
// Capability register bit definition
//
#define HCSP_NPORTS             0x0F // Number of root hub port
#define HCSP_PPC                0x10 // Port Power Control
#define HCCP_64BIT              0x01 // 64-bit addressing capability


typedef struct {
  PCI_DEVICE_INDEPENDENT_REGION Hdr;
  PCI_DEVICE_HEADER_TYPE_REGION Device;
  UINT8 CapabilityID;
  UINT8 NextItemPtr;
  UINT16 DebugPort;
} EHCI_PCI_CONFIG;


#endif    // _EHCI_PCIIO_H_
