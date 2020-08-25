/** @file
*  Differentiated System Description Table Fields (DSDT).
*
*  Copyright (c) 2020, Linaro Ltd. All rights reserved.
*
*  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <IndustryStandard/SbsaQemuAcpi.h>

DefinitionBlock ("DsdtTable.aml", "DSDT", 1, "LINARO", "SBSAQEMU",
                 FixedPcdGet32 (PcdAcpiDefaultOemRevision)) {
  Scope (_SB) {
    // UART PL011
    Device (COM0) {
      Name (_HID, "ARMH0011")
      Name (_UID, Zero)
      Name (_CRS, ResourceTemplate () {
        Memory32Fixed (ReadWrite, 0x60000000, 0x00001000)
        Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { 33 }
      })
    }

    // AHCI Host Controller
    Device (AHC0) {
      Name (_HID, "LNRO001E")
      Name (_CLS, Package (3) {
        0x01,
        0x06,
        0x01,
      })
      Name (_CCA, 1)
      Name (_CRS, ResourceTemplate() {
        Memory32Fixed (ReadWrite, 0x60100000, 0x1000)
        Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { 42 }
      })
    }

    // USB EHCI Host Controller
    Device (USB0) {
        Name (_HID, "LNRO0D20")
        Name (_CID, "PNP0D20")

        Method (_CRS, 0x0, Serialized) {
            Name (RBUF, ResourceTemplate() {
                Memory32Fixed (ReadWrite, 0x60110000, 0x00010000)
                Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { 43 }
            })
            Return (RBUF)
        }

        // Root Hub
        Device (RHUB) {
            Name (_ADR, 0x00000000)  // Address of Root Hub should be 0 as per ACPI 5.0 spec

            // Ports connected to Root Hub
            Device (HUB1) {
                Name (_ADR, 0x00000001)
                Name (_UPC, Package() {
                    0x00,       // Port is NOT connectable
                    0xFF,       // Don't care
                    0x00000000, // Reserved 0 must be zero
                    0x00000000  // Reserved 1 must be zero
                })

                Device (PRT1) {
                    Name (_ADR, 0x00000001)
                    Name (_UPC, Package() {
                        0xFF,        // Port is connectable
                        0x00,        // Port connector is A
                        0x00000000,
                        0x00000000
                    })
                    Name (_PLD, Package() {
                        Buffer(0x10) {
                            0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x31, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
                        }
                    })
                } // USB0_RHUB_HUB1_PRT1
                Device (PRT2) {
                    Name (_ADR, 0x00000002)
                    Name (_UPC, Package() {
                        0xFF,        // Port is connectable
                        0x00,        // Port connector is A
                        0x00000000,
                        0x00000000
                    })
                    Name (_PLD, Package() {
                        Buffer(0x10) {
                            0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x31, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
                        }
                    })
                } // USB0_RHUB_HUB1_PRT2

                Device (PRT3) {
                    Name (_ADR, 0x00000003)
                    Name (_UPC, Package() {
                        0xFF,        // Port is connectable
                        0x00,        // Port connector is A
                        0x00000000,
                        0x00000000
                    })
                    Name (_PLD, Package() {
                        Buffer (0x10) {
                            0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x31, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
                        }
                    })
                } // USB0_RHUB_HUB1_PRT3

                Device (PRT4) {
                    Name (_ADR, 0x00000004)
                    Name (_UPC, Package() {
                        0xFF,        // Port is connectable
                        0x00,        // Port connector is A
                        0x00000000,
                        0x00000000
                    })
                    Name (_PLD, Package() {
                        Buffer (0x10){
                            0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x31, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
                        }
                    })
                } // USB0_RHUB_HUB1_PRT4
            } // USB0_RHUB_HUB1
        } // USB0_RHUB
    } // USB0

  } // Scope (_SB)
}
