/**
*
*  Copyright (c) 2017, Linaro Ltd. All rights reserved.
*  Copyright (c) 2018, Marvell International Ltd. All rights reserved.
*
*  This program and the accompanying materials are licensed and made available
*  under the terms and conditions of the BSD License which accompanies this
*  distribution. The full text of the license may be found at
*  http://opensource.org/licenses/bsd-license.php
*
*  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
*  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
*
**/

#include <Uefi.h>

#include <Library/DebugLib.h>
#include <Library/DevicePathLib.h>
#include <Library/IoLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/MvGpioLib.h>
#include <Library/NonDiscoverableDeviceRegistrationLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include <Protocol/NonDiscoverableDevice.h>

#include "NonDiscoverableInitLib.h"

STATIC CONST MV_GPIO_PIN mXhciVbusPin = {
  MV_GPIO_CP0_CONTROLLER1,
  ARMADA_80x0_MCBIN_VBUS0_PIN,
  TRUE,
};

STATIC
EFI_STATUS
EFIAPI
XhciInit (
  IN  NON_DISCOVERABLE_DEVICE  *This
  )
{
  EMBEDDED_GPIO  *GpioProtocol;
  EFI_STATUS      Status;

  Status = MvGpioGetProtocol (MV_GPIO_DRIVER_TYPE_SOC_CONTROLLER, &GpioProtocol);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Unable to find GPIO protocol\n", __FUNCTION__));
    return Status;
  }

  GpioProtocol->Set (GpioProtocol,
                  GPIO (mXhciVbusPin.ControllerId, mXhciVbusPin.PinNumber),
                  GPIO_MODE_OUTPUT_1);

  return Status;
}

NON_DISCOVERABLE_DEVICE_INIT
EFIAPI
NonDiscoverableDeviceInitializerGet (
  IN  NON_DISCOVERABLE_DEVICE_TYPE  Type,
  IN  UINTN                         Index
  )
{
  if (Type == NonDiscoverableDeviceTypeXhci) {
        return XhciInit;
  }

  return NULL;
}
