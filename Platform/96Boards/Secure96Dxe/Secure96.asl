/** @file
 * Copyright (c) 2019, Linaro Limited. All rights reserved.
 *
 * This program and the accompanying materials are licensed and made
 * available under the terms and conditions of the BSD License which
 * accompanies this distribution.  The full text of the license may be
 * found at http://opensource.org/licenses/bsd-license.php
 *
 * THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
 * WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR
 * IMPLIED.
 */

#include "Secure96.h"

DefinitionBlock ("Secure96.aml", "SSDT", 2, "96BRDS", "SECURE96", 1)
{
    Scope (_SB)
    {
        Device (LD96)
        {
            Name (_HID, "LNRO9601")
            Name (_UID, 0x0)
            Name (_CID, "PRP0001")
            Name (_DSD, Package () {
                ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
                Package () { Package () { "compatible", "gpio-leds" }, }
            })
            Name (_CRS, ResourceTemplate() {
                GpioIo (Exclusive, PullDefault, 0, 0, IoRestrictionOutputOnly, SECURE96_ACPI_GPIO)
                {
                    FixedPcdGet32 (PcdGpioPinG),
                    FixedPcdGet32 (PcdGpioPinF),
                    FixedPcdGet32 (PcdGpioPinI),
                    FixedPcdGet32 (PcdGpioPinH),
                }
            })

            Device (LDU1)
            {
                Name (_ADR, 0x1)
                Name (_DSD, Package () {
                    ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
                    Package () {
                        Package () { "label", "secure96-u1" },
                        Package () { "gpios", Package () { ^^LD96, 0, 0, 0 }, },
                    }
                })
            }

            Device (LDU2)
            {
                Name (_ADR, 0x2)
                Name (_DSD, Package () {
                    ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
                    Package () {
                        Package () { "label", "secure96-u2" },
                        Package () { "gpios", Package () { ^^LD96, 0, 1, 0 }, },
                    }
                })
            }

            Device (LDU3)
            {
                Name (_ADR, 0x3)
                Name (_DSD, Package () {
                    ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
                    Package () {
                        Package () { "label", "secure96-u3" },
                        Package () { "gpios", Package () { ^^LD96, 0, 2, 0 }, },
                    }
                })
            }

            Device (LDU4)
            {
                Name (_ADR, 0x4)
                Name (_DSD, Package () {
                    ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
                    Package () {
                        Package () { "label", "secure96-u4" },
                        Package () { "gpios", Package () { ^^LD96, 0, 3, 0 }, },
                    }
                })
            }
        }

        Device (SH96)
        {
            Name (_HID, "LNRO9602")
            Name (_UID, 0x0)
            Name (_CID, "PRP0001")
            Name (_DSD, Package () {
                ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
                Package () { Package () { "compatible", "atmel,atsha204a" }, }
            })
            Name (_CRS, ResourceTemplate() {
                I2CSerialBus (ATSHA204A_SLAVE_ADDRESS,, 100000,, SECURE96_ACPI_I2C0,,,,)
            })
        }

        Device (EC96)
        {
            Name (_HID, "LNRO9603")
            Name (_UID, 0x0)
            Name (_CID, "PRP0001")
            Name (_DSD, Package () {
                ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
                Package () { Package () { "compatible", "atmel,atecc508a" }, }
            })
            Name (_CRS, ResourceTemplate() {
                I2CSerialBus (ATECC508A_SLAVE_ADDRESS,, 100000,, SECURE96_ACPI_I2C0,,,,)
            })
        }
    }
}
