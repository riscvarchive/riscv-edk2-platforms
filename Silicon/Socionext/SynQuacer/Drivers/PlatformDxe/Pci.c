 /** @file
  SynQuacer DXE platform driver - PCIe support

  Copyright (c) 2017, Linaro, Ltd. All rights reserved.<BR>

  This program and the accompanying materials are licensed and made available
  under the terms and conditions of the BSD License which accompanies this
  distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
**/

#include "PlatformDxe.h"

#define ASMEDIA_VID                         0x1b21
#define ASM1061_PID                         0x0612
#define ASM1182E_PID                        0x1182
#define ASM1184E_PID                        0x1184

#define ASM1061_SSC_OFFSET                  0xA10

#define ASM118x_PCIE_CAPABILITY_OFFSET      0x80
#define ASM118x_PCIE_LINK_CONTROL_OFFSET    (ASM118x_PCIE_CAPABILITY_OFFSET + \
                                             OFFSET_OF (PCI_CAPABILITY_PCIEXP, \
                                                        LinkControl))

STATIC VOID         *mPciProtocolNotifyRegistration;
STATIC EFI_EVENT    mPciProtocolNotifyEvent;

#pragma pack(1)
typedef struct {
  EFI_PCI_CAPABILITY_HDR    CapHdr;
  PCI_REG_PCIE_CAPABILITY   Pcie;
} PCIE_CAP;
#pragma pack()

STATIC
VOID
RetrainAsm1184eDownstreamPort (
  IN  EFI_PCI_IO_PROTOCOL   *PciIo
  )
{
  EFI_STATUS                Status;
  PCIE_CAP                  Cap;
  PCI_REG_PCIE_LINK_CONTROL LinkControl;

  //
  // The upstream and downstream ports share the same PID/VID, so check
  // the port type. This assumes the PCIe Express capability block lives
  // at offset 0x80 in the port's config space, which is known to be the
  // case for these particular chips.
  //
  ASSERT (sizeof (Cap) == sizeof (UINT32));
  ASSERT (sizeof (LinkControl) == sizeof (UINT16));

  Status = PciIo->Pci.Read (PciIo, EfiPciIoWidthUint32,
                        ASM118x_PCIE_CAPABILITY_OFFSET, 1, &Cap);
  ASSERT_EFI_ERROR (Status);
  ASSERT (Cap.CapHdr.CapabilityID == EFI_PCI_CAPABILITY_ID_PCIEXP);

  if (Cap.Pcie.Bits.DevicePortType != PCIE_DEVICE_PORT_TYPE_DOWNSTREAM_PORT) {
    return;
  }

  DEBUG ((DEBUG_INFO, "%a: retraining ASM118x downstream PCIe port\n",
    __FUNCTION__));

  Status = PciIo->Pci.Read (PciIo, EfiPciIoWidthUint16,
                        ASM118x_PCIE_LINK_CONTROL_OFFSET, 1, &LinkControl);
  ASSERT_EFI_ERROR (Status);

  LinkControl.Bits.RetrainLink = 1;

  Status = PciIo->Pci.Write (PciIo, EfiPciIoWidthUint16,
                        ASM118x_PCIE_LINK_CONTROL_OFFSET, 1, &LinkControl);
  ASSERT_EFI_ERROR (Status);
}

STATIC
VOID
EnableAsm1061SpreadSpectrum (
  IN  EFI_PCI_IO_PROTOCOL   *PciIo
  )
{
  EFI_STATUS  Status;
  UINT8       SscVal;

  DEBUG ((DEBUG_INFO, "%a: enabling spread spectrum mode 0 for ASM1061\n",
    __FUNCTION__));

  // SSC mode 0~-4000 ppm, 1:1 modulation

  SscVal = 0;
  Status = PciIo->Pci.Write (PciIo, EfiPciIoWidthUint8, ASM1061_SSC_OFFSET, 1,
                        &SscVal);
  ASSERT_EFI_ERROR (Status);

  MemoryFence ();
  gBS->Stall (1); // delay at least 100 ns between writes of the same register

  SscVal = 1;
  Status = PciIo->Pci.Write (PciIo, EfiPciIoWidthUint8, ASM1061_SSC_OFFSET, 1,
                        &SscVal);
  ASSERT_EFI_ERROR (Status);
}

STATIC
VOID
EFIAPI
OnPciIoProtocolNotify (
  IN EFI_EVENT      Event,
  IN VOID           *Context
  )
{
  EFI_PCI_IO_PROTOCOL       *PciIo;
  EFI_STATUS                Status;
  EFI_HANDLE                HandleBuffer;
  UINTN                     BufferSize;
  UINT16                    PciVidPid[2];

  while (TRUE) {
    BufferSize = sizeof (EFI_HANDLE);
    Status = gBS->LocateHandle (ByRegisterNotify, NULL,
                    mPciProtocolNotifyRegistration, &BufferSize, &HandleBuffer);
    if (EFI_ERROR (Status)) {
      break;
    }

    Status = gBS->HandleProtocol (HandleBuffer, &gEfiPciIoProtocolGuid,
                    (VOID **)&PciIo);
    ASSERT_EFI_ERROR (Status);

    Status = PciIo->Pci.Read (PciIo, EfiPciIoWidthUint16, PCI_VENDOR_ID_OFFSET,
                          ARRAY_SIZE (PciVidPid), &PciVidPid);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_WARN, "%a: failed to read PCI vendor/product ID - %r\n",
        __FUNCTION__, Status));
      continue;
    }

    if (PciVidPid[0] != ASMEDIA_VID) {
      continue;
    }

    switch (PciVidPid[1]) {
    case ASM1061_PID:
      //
      // The ASM1061 SATA controller as integrated into the DeveloperBox design
      // emits too much electromagnetic radiation. So enable spread spectrum
      // mode.
      //
      EnableAsm1061SpreadSpectrum (PciIo);
      break;
    case ASM1182E_PID:
    case ASM1184E_PID:
      //
      // The ASM1184E 4-port PCIe switch on the DeveloperBox board (and its
      // 2-port sibling of which samples were used in development) needs a
      // little nudge to get it to train the downstream links at Gen2 speed.
      //
      RetrainAsm1184eDownstreamPort (PciIo);
      break;
    }
  }
}

EFI_STATUS
EFIAPI
RegisterPcieNotifier (
  VOID
  )
{
  mPciProtocolNotifyEvent = EfiCreateProtocolNotifyEvent (
                              &gEfiPciIoProtocolGuid,
                              TPL_CALLBACK,
                              OnPciIoProtocolNotify,
                              NULL,
                              &mPciProtocolNotifyRegistration);

  return EFI_SUCCESS;
}
