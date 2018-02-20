/** @file

  Copyright (c) 2018, Linaro, Ltd. All rights reserved.<BR>

  This program and the accompanying materials are licensed and made available
  under the terms and conditions of the BSD License which accompanies this
  distribution. The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _96BOARDS_MEZZANINE_H_
#define _96BOARDS_MEZZANINE_H_

#include <Pi/PiI2c.h>
#include <Protocol/SpiConfiguration.h>

#define MEZZANINE_PROTOCOL_GUID \
  { 0xf0467a37, 0x3436, 0x40ef, { 0x94, 0x09, 0x4d, 0x1d, 0x7f, 0x51, 0x06, 0xd3 } }

typedef struct _MEZZANINE_PROTOCOL MEZZANINE_PROTOCOL;

/**
  Apply the mezzanine's DT overlay

  @param[in]      This      Pointer to the MEZZANINE_PROTOCOL instance.
  @param[in,out]  Dtb       Pointer to the device tree blob

  @return   EFI_SUCCESS     Operation succeeded.
  @return   other           An error has occurred.
**/
typedef
EFI_STATUS
(EFIAPI *APPLY_DEVICE_TREE_OVERLAY) (
  IN      MEZZANINE_PROTOCOL    *This,
  IN  OUT VOID                  *Dtb
  );

struct _MEZZANINE_PROTOCOL {
  //
  // Get the device tree overlay for this mezzanine board
  //
  APPLY_DEVICE_TREE_OVERLAY   ApplyDeviceTreeOverlay;
  //
  // The number of devices on LS connector I2C bus #0
  //
  UINT32                      I2c0NumDevices;
  //
  // The number of devices on LS connector I2C bus #1
  //
  UINT32                      I2c1NumDevices;
  //
  // Linear array of I2C devices on LS connector bus #0
  //
  CONST EFI_I2C_DEVICE        *I2c0DeviceArray;
  //
  // Linear array of I2C devices on LS connector bus #0
  //
  CONST EFI_I2C_DEVICE        *I2c1DeviceArray;
  //
  // NULL terminated linked list of SPI devices attached to the LS connector
  //
  CONST EFI_SPI_PERIPHERAL    *SpiDeviceLinkedList;
};

extern EFI_GUID g96BoardsMezzanineProtocolGuid;

#endif // _96BOARDS_MEZZANINE_H_
