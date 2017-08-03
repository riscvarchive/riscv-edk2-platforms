/** @file
  Differentiated System Description Table Fields (DSDT)

  Copyright (c) 2014, ARM Ltd. All rights reserved.<BR>
  Copyright (c) 2015-2016, Hisilicon Limited. All rights reserved.<BR>
  Copyright (c) 2015-2016, Linaro Limited. All rights reserved.<BR>
    This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

Scope(_SB)
{
  Device(COM0) {
    Name(_HID, "ARMH0011")
    Name(_CID, "PL011")
    Name(_UID, Zero)
    Name(_CRS, ResourceTemplate() {
      Memory32Fixed(ReadWrite, 0x602B0000, 0x1000)
      Interrupt(ResourceConsumer, Level, ActiveHigh, Exclusive, 0, "\\_SB.MBI0") { 807 }
    })
  }
}
