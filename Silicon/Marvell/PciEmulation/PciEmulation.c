/********************************************************************************
Copyright (C) 2016 Marvell International Ltd.

Marvell BSD License Option

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File under the following licensing terms.
Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.

  * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

  * Neither the name of Marvell nor the names of its contributors may be
    used to endorse or promote products derived from this software without
    specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/

#include <PiDxe.h>

#include <Library/DebugLib.h>
#include <Library/MvHwDescLib.h>
#include <Library/NonDiscoverableDeviceRegistrationLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include <Protocol/EmbeddedExternalDevice.h>

DECLARE_A7K8K_NONDISCOVERABLE_TEMPLATE;

//
// Tables with used devices
//
STATIC UINT8 * CONST XhciDeviceTable = FixedPcdGetPtr (PcdPciEXhci);
STATIC UINT8 * CONST AhciDeviceTable = FixedPcdGetPtr (PcdPciEAhci);
STATIC UINT8 * CONST SdhciDeviceTable = FixedPcdGetPtr (PcdPciESdhci);

//
// NonDiscoverable devices registration
//
STATIC
EFI_STATUS
PciEmulationInitXhci (
  )
{
  MVHW_NONDISCOVERABLE_DESC *Desc = &mA7k8kNonDiscoverableDescTemplate;
  EFI_STATUS Status;
  UINT8 i;

  if (PcdGetSize (PcdPciEXhci) < Desc->XhciDevCount) {
    DEBUG((DEBUG_ERROR, "PciEmulation: Wrong PcdPciEXhci format\n"));
    return EFI_INVALID_PARAMETER;
  }

  for (i = 0; i < Desc->XhciDevCount; i++) {
    if (!MVHW_DEV_ENABLED (Xhci, i)) {
      continue;
    }

    Status = RegisterNonDiscoverableMmioDevice (
                     NonDiscoverableDeviceTypeXhci,
                     Desc->XhciDmaType[i],
                     NULL,
                     NULL,
                     1,
                     Desc->XhciBaseAddresses[i], Desc->XhciMemSize[i]
                   );

    if (EFI_ERROR(Status)) {
      DEBUG((DEBUG_ERROR, "PciEmulation: Cannot install Xhci device %d\n", i));
      return Status;
    }
  }

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
PciEmulationInitAhci (
  )
{
  MVHW_NONDISCOVERABLE_DESC *Desc = &mA7k8kNonDiscoverableDescTemplate;
  EFI_STATUS Status;
  UINT8 i;

  if (PcdGetSize (PcdPciEAhci) < Desc->AhciDevCount) {
    DEBUG((DEBUG_ERROR, "PciEmulation: Wrong PcdPciEAhci format\n"));
    return EFI_INVALID_PARAMETER;
  }

  for (i = 0; i < Desc->AhciDevCount; i++) {
    if (!MVHW_DEV_ENABLED (Ahci, i)) {
      continue;
    }

    Status = RegisterNonDiscoverableMmioDevice (
                     NonDiscoverableDeviceTypeAhci,
                     Desc->AhciDmaType[i],
                     NULL,
                     NULL,
                     1,
                     Desc->AhciBaseAddresses[i], Desc->AhciMemSize[i]
                   );

    if (EFI_ERROR(Status)) {
      DEBUG((DEBUG_ERROR, "PciEmulation: Cannot install Ahci device %d\n", i));
      return Status;
    }
  }

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
PciEmulationInitSdhci (
  )
{
  MVHW_NONDISCOVERABLE_DESC *Desc = &mA7k8kNonDiscoverableDescTemplate;
  EFI_STATUS Status;
  UINT8 i;

  if (PcdGetSize (PcdPciESdhci) < Desc->SdhciDevCount) {
    DEBUG((DEBUG_ERROR, "PciEmulation: Wrong PcdPciESdhci format\n"));
    return EFI_INVALID_PARAMETER;
  }

  for (i = 0; i < Desc->SdhciDevCount; i++) {
    if (!MVHW_DEV_ENABLED (Sdhci, i)) {
      continue;
    }

    Status = RegisterNonDiscoverableMmioDevice (
                     NonDiscoverableDeviceTypeSdhci,
                     Desc->SdhciDmaType[i],
                     NULL,
                     NULL,
                     1,
                     Desc->SdhciBaseAddresses[i], Desc->SdhciMemSize[i]
                   );

    if (EFI_ERROR(Status)) {
      DEBUG((DEBUG_ERROR, "PciEmulation: Cannot install Sdhci device %d\n", i));
      return Status;
    }
  }

  return EFI_SUCCESS;
}

//
// Entry point
//
EFI_STATUS
EFIAPI
PciEmulationEntryPoint (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS Status;

  Status = PciEmulationInitXhci();
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = PciEmulationInitAhci();
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = PciEmulationInitSdhci();
  if (EFI_ERROR(Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}
