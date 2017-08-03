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

  Based on the files under ArmPlatformPkg/ArmJunoPkg/AcpiTables/

**/

Scope(_SB)
{
  Device(I2C0) {
    Name(_HID, "APMC0D0F")
    Name(_CID, "APMC0D0F")
    Name(_CRS, ResourceTemplate() {
      Memory32Fixed(ReadWrite, 0xd00e0000, 0x10000)
      Interrupt(ResourceConsumer, Level, ActiveHigh, Exclusive, 0, "\\_SB.MBI6") { 705 }
    })
    Name (_DSD, Package () {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
        Package () {"clock-frequency", 100000},
        Package () {"i2c-sda-falling-time-ns", 913},
        Package () {"i2c-scl-falling-time-ns", 303},
        Package () {"i2c-sda-hold-time-ns", 0x9c2},
      }
    })
  }

  Device(I2C2) {
    Name(_HID, "APMC0D0F")
    Name(_CID, "APMC0D0F")
    Name(_CRS, ResourceTemplate() {
      Memory32Fixed(ReadWrite, 0xd0100000, 0x10000)
      Interrupt(ResourceConsumer, Level, ActiveHigh, Exclusive, 0, "\\_SB.MBI7") { 707 }
    })
    Name (_DSD, Package () {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
        Package () {"clock-frequency", 100000},
        Package () {"i2c-sda-falling-time-ns", 913},
        Package () {"i2c-scl-falling-time-ns", 303},
        Package () {"i2c-sda-hold-time-ns", 0x9c2},
      }
    })
  }
}
