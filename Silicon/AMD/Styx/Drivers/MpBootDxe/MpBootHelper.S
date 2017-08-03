//
//  Copyright (c) 2011-2013, ARM Limited. All rights reserved.
//  Copyright (c) 2014 - 2016, AMD Inc. All rights reserved.<BR>
//
//  This program and the accompanying materials
//  are licensed and made available under the terms and conditions of the BSD License
//  which accompanies this distribution.  The full text of the license may be found at
//  http://opensource.org/licenses/bsd-license.php
//
//  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
//  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
//
//**
//  Derived from:
//   ArmPkg/Library/BdsLib/AArch64/BdsLinuxLoaderHelper.S
//
//**

/* Secondary core pens for AArch64 Linux booting.

   This code is placed in Linux kernel memory and marked reserved. UEFI ensures
   that the secondary cores get to this pen and the kernel can then start the
   cores from here.
   NOTE: This code must be self-contained.
*/

#include <Library/ArmLib.h>

.text
.align 3

GCC_ASM_EXPORT(SecondariesPenStart)
ASM_GLOBAL SecondariesPenEnd

ASM_PFX(SecondariesPenStart):
   // Registers x0-x3 are reserved for future use and should be set to zero.
   mov x0, xzr
   mov x1, xzr
   mov x2, xzr
   mov x3, xzr

   mrs x4, mpidr_el1             // Get MPCore register
   and x5, x4, #ARM_CORE_MASK    // Get core number
   and x4, x4, #ARM_CLUSTER_MASK // Get cluster number

   add x4, x5, x4, LSR #7        // Add scaled cluster number to core number
   mov x6, x4                    // Save a copy to compute mp-parking offset

   ldr x5, AsmMailboxBase   // Get mailbox addr relative to PC
   lsl x4, x4, 3            // Add 8-byte offset for this core
   add x4, x4, x5           //

   ldr x5, AsmParkingBase   // Get mp-parking addr relative to PC
   lsl x6, x6, 12           // Add 4K-byte offset for this core
   add x6, x6, x5           //

   mov x5, 1                // Get mp-parking id# at 2K offset
   lsl x5, x5, 11           //
   add x5, x5, x6           //
   ldr x10, [x5]            //

1: ldr x5, [x4]             // Load jump-addr from spin-table mailbox
   cmp xzr, x5              // Has the value been set?
   b.ne 4f                  // If so, break out of loop

   ldr x5, [x6]             // Load mp-parking id#
   cmp w10, w5              // Is it my id?
   b.ne 2f                  // If not, continue polling

   ldr x5, [x6, 8]          // Load jump-addr from mp-parking
   cmp xzr, x5              // Has the value been set?
   b.ne 3f                  // If so, break out of loop

2: wfe                      // Wait a bit
   b 1b                     // Wait over, check again

3: str xzr, [x6, 8]         // Clear to acknowledge
   mov x0, x6               // Return mp-parking address
4: br x5                    // Jump to new addr

.align 3 // Make sure the variable below is 8 byte aligned.
                .global     AsmParkingBase
AsmParkingBase: .xword      0xdeaddeadbeefbeef
                .global     AsmMailboxBase
AsmMailboxBase: .xword      0xdeaddeadbeefbeef

SecondariesPenEnd:
