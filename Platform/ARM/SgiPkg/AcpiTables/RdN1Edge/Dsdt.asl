/** @file
*  Differentiated System Description Table Fields (DSDT)
*
*  Copyright (c) 2018-2020, ARM Ltd. All rights reserved.
*
*  SPDX-License-Identifier: BSD-2-Clause-Patent
*
**/

#include "SgiPlatform.h"
#include "SgiAcpiHeader.h"

DefinitionBlock ("DsdtTable.aml", "DSDT", 1, "ARMLTD", "ARMSGI",
                 EFI_ACPI_ARM_OEM_REVISION) {
  Scope (_SB) {

    Device (CP00) { // Neoverse-N1: Cluster 0, Cpu 0
      Name (_HID, "ACPI0007")
      Name (_UID, 0)
      Name (_STA, 0xF)
    }

    Device (CP01) { // Neoverse-N1: Cluster 0, Cpu 1
      Name (_HID, "ACPI0007")
      Name (_UID, 1)
      Name (_STA, 0xF)
    }

    Device (CP02) { // Neoverse-N1: Cluster 0, Cpu 2
      Name (_HID, "ACPI0007")
      Name (_UID, 2)
      Name (_STA, 0xF)
    }

    Device (CP03) { // Neoverse-N1: Cluster 0, Cpu 3
      Name (_HID, "ACPI0007")
      Name (_UID, 3)
      Name (_STA, 0xF)
    }

    Device (CP04) { // Neoverse-N1: Cluster 1, Cpu 0
      Name (_HID, "ACPI0007")
      Name (_UID, 4)
      Name (_STA, 0xF)
    }

    Device (CP05) { // Neoverse-N1: Cluster 1, Cpu 1
      Name (_HID, "ACPI0007")
      Name (_UID, 5)
      Name (_STA, 0xF)
    }

    Device (CP06) { // Neoverse-N1: Cluster 1, Cpu 2
      Name (_HID, "ACPI0007")
      Name (_UID, 6)
      Name (_STA, 0xF)
    }

    Device (CP07) { // Neoverse-N1: Cluster 1, Cpu 3
      Name (_HID, "ACPI0007")
      Name (_UID, 7)
      Name (_STA, 0xF)
    }

  } // Scope(_SB)
}
