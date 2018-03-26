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
#ifndef __ARMADA_BOARD_DESC_LIB_H__
#define __ARMADA_BOARD_DESC_LIB_H__

#include <Library/ArmadaSoCDescLib.h>

//
// UTMI PHY devices per-board description
//
typedef struct {
  MV_SOC_UTMI_DESC *SoC;
  UINTN             UtmiDevCount;
  UINTN             UtmiPortType;
} MV_BOARD_UTMI_DESC;
#endif /* __ARMADA_SOC_DESC_LIB_H__ */
