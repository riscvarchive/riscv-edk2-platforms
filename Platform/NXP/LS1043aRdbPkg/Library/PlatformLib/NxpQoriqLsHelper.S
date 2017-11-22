#  @file
#
#  Copyright (c) 2012-2013, ARM Limited. All rights reserved.
#  Copyright 2017 NXP
#
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#

#include <AsmMacroIoLibV8.h>
#include <AutoGen.h>

.text
.align 2

GCC_ASM_IMPORT(ArmReadMpidr)

ASM_FUNC(ArmPlatformIsPrimaryCore)
  tst x0, #3
  cset x0, eq
  ret

ASM_FUNC(ArmPlatformPeiBootAction)
EL1_OR_EL2(x0)
1:
2:
  ret

ASM_FUNC(ArmPlatformGetPrimaryCoreMpId)
  MOV32 (x0, FixedPcdGet32(PcdArmPrimaryCore))
  ldrh   w0, [x0]
  ret
