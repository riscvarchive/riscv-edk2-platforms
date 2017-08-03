/** @file

  Copyright (c) 2017, Linaro Ltd. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

  .section  ".payload"
  .align    12

ASM_GLOBAL ASM_PFX(StyxFlashImageStart)
ASM_PFX(StyxFlashImageStart):
  .incbin   "STYX_EFI.Fv"

  .align    2
ASM_GLOBAL ASM_PFX(StyxFlashImageSize)
ASM_PFX(StyxFlashImageSize):
  .long    . - ASM_PFX(StyxFlashImageStart)
