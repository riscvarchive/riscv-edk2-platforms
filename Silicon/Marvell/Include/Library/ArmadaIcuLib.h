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
#ifndef __ARMADA_ICU_LIB_H__
#define __ARMADA_ICU_LIB_H__

typedef enum {
  IcuIrqTypeLevel = 0,
  IcuIrqTypeEdge  = 1
} ICU_IRQ_TYPE;

typedef struct {
  UINTN IcuId;
  UINTN SpiId;
  ICU_IRQ_TYPE IrqType;
} ICU_IRQ;

typedef struct {
  CONST ICU_IRQ   *Map;
  UINTN           Size;
} ICU_CONFIG_ENTRY;

typedef struct {
  ICU_CONFIG_ENTRY NonSecure;
  ICU_CONFIG_ENTRY Sei;
  ICU_CONFIG_ENTRY Rei;
} ICU_CONFIG;

EFI_STATUS
EFIAPI
ArmadaIcuInitialize (
  VOID
  );

#endif /* __ARMADA_ICU_LIB_H__ */
