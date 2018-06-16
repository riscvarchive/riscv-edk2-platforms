/**
*
*  Copyright (C) 2018, Marvell International Ltd. and its affiliates
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
#ifndef __ARMADA_SOC_DESC_LIB_H__
#define __ARMADA_SOC_DESC_LIB_H__

//
// PP2 NIC devices SoC description
//
typedef struct {
  UINTN Pp2BaseAddress;
  UINTN Pp2ClockFrequency;
} MV_SOC_PP2_DESC;

EFI_STATUS
EFIAPI
ArmadaSoCDescPp2Get (
  IN OUT MV_SOC_PP2_DESC  **Pp2Desc,
  IN OUT UINTN             *DescCount
  );

//
// UTMI PHY devices SoC description
//
typedef struct {
  UINT8 UtmiPhyId;
  UINTN UtmiBaseAddress;
  UINTN UtmiConfigAddress;
  UINTN UsbConfigAddress;
} MV_SOC_UTMI_DESC;

EFI_STATUS
EFIAPI
ArmadaSoCDescUtmiGet (
  IN OUT MV_SOC_UTMI_DESC  **UtmiDesc,
  IN OUT UINTN              *DescCount
  );
#endif /* __ARMADA_SOC_DESC_LIB_H__ */
