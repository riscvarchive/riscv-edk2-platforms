/** @file
*  Differentiated System Description Table Fields (DSDT)
*
*  Copyright (c) 2020, Arm Ltd. All rights reserved.
*
*  SPDX-License-Identifier: BSD-2-Clause-Patent
*
**/

#include "SgiPlatform.h"
#include "SgiAcpiHeader.h"

DefinitionBlock ("DsdtTable.aml", "DSDT", 1, "ARMLTD", "ARMSGI",
                 EFI_ACPI_ARM_OEM_REVISION) {
  Scope (_SB) {
    Device (CP00) { // Neoverse N2 core 0
      Name (_HID, "ACPI0007")
      Name (_UID, 0)
      Name (_STA, 0xF)
    }

    Device (CP01) { // Neoverse N2 core 1
      Name (_HID, "ACPI0007")
      Name (_UID, 1)
      Name (_STA, 0xF)
    }

    Device (CP02) { // Neoverse N2 core 2
      Name (_HID, "ACPI0007")
      Name (_UID, 2)
      Name (_STA, 0xF)
    }

    Device (CP03) { // Neoverse N2 core 3
      Name (_HID, "ACPI0007")
      Name (_UID, 3)
      Name (_STA, 0xF)
    }

    Device (CP04) { // Neoverse N2 core 4
      Name (_HID, "ACPI0007")
      Name (_UID, 4)
      Name (_STA, 0xF)
    }

    Device (CP05) { // Neoverse N2 core 5
      Name (_HID, "ACPI0007")
      Name (_UID, 5)
      Name (_STA, 0xF)
    }

    Device (CP06) { // Neoverse N2 core 6
      Name (_HID, "ACPI0007")
      Name (_UID, 6)
      Name (_STA, 0xF)
    }

    Device (CP07) { // Neoverse N2 core 7
      Name (_HID, "ACPI0007")
      Name (_UID, 7)
      Name (_STA, 0xF)
    }

    Device (CP08) { // Neoverse N2 core 8
      Name (_HID, "ACPI0007")
      Name (_UID, 8)
      Name (_STA, 0xF)
    }

   Device (CP09) { // Neoverse N2 core 9
      Name (_HID, "ACPI0007")
      Name (_UID, 9)
      Name (_STA, 0xF)
    }

   Device (CP10) { // Neoverse N2 core 10
      Name (_HID, "ACPI0007")
      Name (_UID, 10)
      Name (_STA, 0xF)
    }

   Device (CP11) { // Neoverse N2 core 11
      Name (_HID, "ACPI0007")
      Name (_UID, 11)
      Name (_STA, 0xF)
    }

    Device (CP12) { // Neoverse N2 core 12
      Name (_HID, "ACPI0007")
      Name (_UID, 12)
      Name (_STA, 0xF)
    }

   Device (CP13) { // Neoverse N2 core 13
      Name (_HID, "ACPI0007")
      Name (_UID, 13)
      Name (_STA, 0xF)
    }

   Device (CP14) { // Neoverse N2 core 14
      Name (_HID, "ACPI0007")
      Name (_UID, 14)
      Name (_STA, 0xF)
    }

   Device (CP15) { // Neoverse N2 core 15
      Name (_HID, "ACPI0007")
      Name (_UID, 15)
      Name (_STA, 0xF)
    }
  } // Scope(_SB)
}
