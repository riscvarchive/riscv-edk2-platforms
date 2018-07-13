/**
*
*  Copyright (C) 2018, Marvell International Ltd. and its affiliates.
*
*  This program and the accompanying materials are licensed and made available
*  under the terms and conditions of the BSD License which accompanies this
*  distribution. The full text of the license may be found at
*  http://opensource.org/licenses/bsd-license.php
*
*  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
*  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
*
*  Glossary - abbreviations used in Marvell SampleAtReset library implementation:
*  ICU - Interrupt Consolidation Unit
*  AP - Application Processor hardware block (Armada 7k8k incorporates AP806)
*  CP - South Bridge hardware blocks (Armada 7k8k incorporates CP110)
*
**/

#include <Uefi.h>

#include <Library/ArmadaIcuLib.h>
#include <Library/ArmadaSoCDescLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiBootServicesTableLib.h>

#define ICU_REG_BASE(Cp)        (ArmadaSoCDescCpBaseGet (CpIndex) + 0x1E0000)

#define ICU_GROUP_REGISTER_BASE_OFFSET    0x10
#define ICU_SET_SPI_AL(x)       (0x10 + (ICU_GROUP_REGISTER_BASE_OFFSET * x))
#define ICU_SET_SPI_AH(x)       (0x14 + (ICU_GROUP_REGISTER_BASE_OFFSET * x))
#define ICU_CLR_SPI_AL(x)       (0x18 + (ICU_GROUP_REGISTER_BASE_OFFSET * x))
#define ICU_CLR_SPI_AH(x)       (0x1c + (ICU_GROUP_REGISTER_BASE_OFFSET * x))
#define ICU_INT_CFG(x)          (0x100 + (sizeof (UINT32) * x))

#define ICU_INT_ENABLE_OFFSET    24
#define ICU_IS_EDGE_OFFSET       28
#define ICU_GROUP_OFFSET         29

#define ICU_MAX_SUPPORTED_UNITS  2
#define ICU_MAX_IRQS_PER_CP      64

#define MAX_ICU_IRQS             207
