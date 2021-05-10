/** @file
*  Differentiated System Description Table Fields (DSDT)
*
*  Copyright (c) 2021, ARM Ltd. All rights reserved.
*
*  SPDX-License-Identifier: BSD-2-Clause-Patent
*
*  @par Specification Reference:
*  - ACPI 6.3, Section 5.2.11.1, Differentiated System Description Table
**/

#include "SgiAcpiHeader.h"
#include "SgiPlatform.h"

DefinitionBlock ("DsdtTable.aml", "DSDT", 2, "ARMLTD", "ARMSGI",
                 EFI_ACPI_ARM_OEM_REVISION) {
  Scope (_SB) {
    /* Chip 0 CPUs */
    Device (CLU0) {   // Cluster 0
      Name (_HID, "ACPI0010")
      Name (_UID, 0)

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
    }

    Device (CLU1) {   // Cluster 1
      Name (_HID, "ACPI0010")
      Name (_UID, 1)

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
    }

    /* Chip 1 CPUs */
    Device (CLU2) {   // Cluster 2
      Name (_HID, "ACPI0010")
      Name (_UID, 2)

      Device (CP08) { // Neoverse-N1: Cluster 2, Cpu 0
        Name (_HID, "ACPI0007")
        Name (_UID, 8)
        Name (_STA, 0xF)
      }

      Device (CP09) { // Neoverse-N1: Cluster 2, Cpu 1
        Name (_HID, "ACPI0007")
        Name (_UID, 9)
        Name (_STA, 0xF)
      }

      Device (CP10) { // Neoverse-N1: Cluster 2, Cpu 2
        Name (_HID, "ACPI0007")
        Name (_UID, 10)
        Name (_STA, 0xF)
      }

      Device (CP11) { // Neoverse-N1: Cluster 2, Cpu 3
        Name (_HID, "ACPI0007")
        Name (_UID, 11)
        Name (_STA, 0xF)
      }
    }

    Device (CLU3) {   // Cluster 3
      Name (_HID, "ACPI0010")
      Name (_UID, 3)

      Device (CP12) { // Neoverse-N1: Cluster 3, Cpu 0
        Name (_HID, "ACPI0007")
        Name (_UID, 12)
        Name (_STA, 0xF)
      }

      Device (CP13) { // Neoverse-N1: Cluster 3, Cpu 1
        Name (_HID, "ACPI0007")
        Name (_UID, 13)
        Name (_STA, 0xF)
      }

      Device (CP14) { // Neoverse-N1: Cluster 3, Cpu 2
        Name (_HID, "ACPI0007")
        Name (_UID, 14)
        Name (_STA, 0xF)
      }

      Device (CP15) { // Neoverse-N1: Cluster 3, Cpu 3
        Name (_HID, "ACPI0007")
        Name (_UID, 15)
        Name (_STA, 0xF)
      }
    }
  } // Scope(_SB)
}
