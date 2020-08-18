/** @file
*  Differentiated System Description Table Fields (DSDT)
*
*  Copyright (c) 2020, Arm Limited. All rights reserved.
*
*  SPDX-License-Identifier: BSD-2-Clause-Patent
*
**/

#include "SgiPlatform.h"
#include "SgiAcpiHeader.h"

DefinitionBlock ("DsdtTable.aml", "DSDT", 1, "ARMLTD", "ARMSGI",
                 EFI_ACPI_ARM_OEM_REVISION) {
  Scope (_SB) {

    Device (CP00) { // Zeus core 0
      Name (_HID, "ACPI0007")
      Name (_UID, 0)
      Name (_STA, 0xF)
    }

    Device (CP01) { // Zeus core 1
      Name (_HID, "ACPI0007")
      Name (_UID, 1)
      Name (_STA, 0xF)
    }

    Device (CP02) { // Zeus core 2
      Name (_HID, "ACPI0007")
      Name (_UID, 2)
      Name (_STA, 0xF)
    }

    Device (CP03) { // Zeus core 3
      Name (_HID, "ACPI0007")
      Name (_UID, 3)
      Name (_STA, 0xF)
    }

    Device (CP04) { // Zeus core 4
      Name (_HID, "ACPI0007")
      Name (_UID, 4)
      Name (_STA, 0xF)
    }

    Device (CP05) { // Zeus core 5
      Name (_HID, "ACPI0007")
      Name (_UID, 5)
      Name (_STA, 0xF)
    }

    Device (CP06) { // Zeus core 6
      Name (_HID, "ACPI0007")
      Name (_UID, 6)
      Name (_STA, 0xF)
    }

    Device (CP07) { // Zeus core 7
      Name (_HID, "ACPI0007")
      Name (_UID, 7)
      Name (_STA, 0xF)
    }

    Device (CP08) { // Zeus core 8
      Name (_HID, "ACPI0007")
      Name (_UID, 8)
      Name (_STA, 0xF)
    }

    Device (CP09) { // Zeus core 9
      Name (_HID, "ACPI0007")
      Name (_UID, 9)
      Name (_STA, 0xF)
    }

    Device (CP10) { // Zeus core 10
      Name (_HID, "ACPI0007")
      Name (_UID, 10)
      Name (_STA, 0xF)
    }

    Device (CP11) { // Zeus core 11
      Name (_HID, "ACPI0007")
      Name (_UID, 11)
      Name (_STA, 0xF)
    }

    Device (CP12) { // Zeus core 12
      Name (_HID, "ACPI0007")
      Name (_UID, 12)
      Name (_STA, 0xF)
    }

    Device (CP13) { // Zeus core 13
      Name (_HID, "ACPI0007")
      Name (_UID, 13)
      Name (_STA, 0xF)
    }

    Device (CP14) { // Zeus core 14
      Name (_HID, "ACPI0007")
      Name (_UID, 14)
      Name (_STA, 0xF)
    }

    Device (CP15) { // Zeus core 15
      Name (_HID, "ACPI0007")
      Name (_UID, 15)
      Name (_STA, 0xF)
    }

    Device (CP16) { // Zeus core 16
      Name (_HID, "ACPI0007")
      Name (_UID, 16)
      Name (_STA, 0xF)
    }
  } // Scope(_SB)
}
