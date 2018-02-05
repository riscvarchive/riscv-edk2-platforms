/*******************************************************************************
Copyright (C) 2018 Marvell International Ltd.

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
#include "MvBoardDescDxe.h"

MV_BOARD_DESC *mBoardDescInstance;

STATIC
EFI_STATUS
MvBoardDescUtmiGet (
  IN MARVELL_BOARD_DESC_PROTOCOL  *This,
  IN OUT MV_BOARD_UTMI_DESC      **UtmiDesc
  )
{
  UINT8 *UtmiDeviceEnabled, *XhciDeviceEnabled, *UtmiPortType;
  UINTN UtmiCount, UtmiDeviceTableSize, UtmiIndex, Index;
  MV_BOARD_UTMI_DESC *BoardDesc;
  MV_SOC_UTMI_DESC *SoCDesc;
  EFI_STATUS Status;

  /* Get SoC data about all available UTMI controllers */
  Status = ArmadaSoCDescUtmiGet (&SoCDesc, &UtmiCount);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  /*
   * Obtain table with enabled Utmi PHY's,
   * which is represented as an array of UINT8 values
   * (0x0 - disabled, 0x1 enabled).
   */
  UtmiDeviceEnabled = PcdGetPtr (PcdUtmiControllersEnabled);
  if (UtmiDeviceEnabled == NULL) {
    /* No UTMI PHY on platform */
    return EFI_SUCCESS;
  }

  /* Make sure XHCI controllers table is present */
  XhciDeviceEnabled = PcdGetPtr (PcdPciEXhci);
  if (XhciDeviceEnabled == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: Missing PcdPciEXhci\n", __FUNCTION__));
    return EFI_INVALID_PARAMETER;
  }

  UtmiDeviceTableSize = PcdGetSize (PcdUtmiControllersEnabled);

  /* Check if PCD with UTMI PHYs is correctly defined */
  if ((UtmiDeviceTableSize > UtmiCount) ||
      (UtmiDeviceTableSize > PcdGetSize (PcdPciEXhci))) {
    DEBUG ((DEBUG_ERROR,
      "%a: Wrong PcdUtmiControllersEnabled format\n",
      __FUNCTION__));
    return EFI_INVALID_PARAMETER;
  }

  /* Obtain port type table - also stored as UINT8 array */
  UtmiPortType = PcdGetPtr (PcdUtmiPortType);
  if ((UtmiPortType == NULL) ||
      (PcdGetSize (PcdUtmiPortType) != UtmiDeviceTableSize)) {
    DEBUG ((DEBUG_ERROR, "%a: Wrong PcdUtmiPortType format\n", __FUNCTION__));
    return EFI_INVALID_PARAMETER;
  }

  /* Allocate and fill board description */
  BoardDesc = AllocateZeroPool (UtmiDeviceTableSize * sizeof (MV_BOARD_UTMI_DESC));
  if (BoardDesc == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: Cannot allocate memory\n", __FUNCTION__));
    return EFI_OUT_OF_RESOURCES;
  }

  UtmiIndex = 0;
  for (Index = 0; Index < UtmiDeviceTableSize; Index++) {
    if (!UtmiDeviceEnabled[Index]) {
      continue;
    }

    /* UTMI PHY without enabled XHCI controller is useless */
    if (!XhciDeviceEnabled[Index]) {
      DEBUG ((DEBUG_ERROR,
             "%a: Disabled Xhci controller %d\n",
             Index,
             __FUNCTION__));
      return EFI_INVALID_PARAMETER;
    }

    BoardDesc[UtmiIndex].SoC = &SoCDesc[Index];
    BoardDesc[UtmiIndex].UtmiPortType = UtmiPortType[Index];
    UtmiIndex++;
  }

  BoardDesc->UtmiDevCount = UtmiIndex;

  *UtmiDesc = BoardDesc;

  return EFI_SUCCESS;
}

STATIC
VOID
MvBoardDescFree (
  IN VOID *BoardDesc
  )
{
  FreePool (BoardDesc);
}

STATIC
EFI_STATUS
MvBoardDescInitProtocol (
  IN MARVELL_BOARD_DESC_PROTOCOL *BoardDescProtocol
  )
{
  BoardDescProtocol->BoardDescUtmiGet = MvBoardDescUtmiGet;
  BoardDescProtocol->BoardDescFree = MvBoardDescFree;

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
MvBoardDescEntryPoint (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS Status;

  mBoardDescInstance = AllocateZeroPool (sizeof (MV_BOARD_DESC));
  if (mBoardDescInstance == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  MvBoardDescInitProtocol (&mBoardDescInstance->BoardDescProtocol);

  mBoardDescInstance->Signature = MV_BOARD_DESC_SIGNATURE;

  Status = gBS->InstallMultipleProtocolInterfaces (&(mBoardDescInstance->Handle),
                  &gMarvellBoardDescProtocolGuid,
                  &(mBoardDescInstance->BoardDescProtocol));
  if (EFI_ERROR (Status)) {
    FreePool (mBoardDescInstance);
    return Status;
  }

  return EFI_SUCCESS;
}
