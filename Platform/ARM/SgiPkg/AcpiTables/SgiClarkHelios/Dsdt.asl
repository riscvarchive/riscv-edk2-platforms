/** @file
*  Differentiated System Description Table Fields (DSDT)
*
*  Copyright (c) 2018, ARM Ltd. All rights reserved.
*
*  This program and the accompanying materials are licensed and made available
*  under the terms and conditions of the BSD License which accompanies this
*  distribution.  The full text of the license may be found at
*  http://opensource.org/licenses/bsd-license.php
*
*  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
*  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
*
**/

#include "SgiPlatform.h"
#include "SgiAcpiHeader.h"

DefinitionBlock ("DsdtTable.aml", "DSDT", 1, "ARMLTD", "ARMSGI",
                 EFI_ACPI_ARM_OEM_REVISION) {
  Scope (_SB) {
    //
    // HeliosCores 8X2 Processor declaration
    //
    Device (CP00) { // HeliosCore: Cluster 0, Cpu 0, Thread 0
      Name (_HID, "ACPI0007")
      Name (_UID, 0)
      Name (_STA, 0xF)
    }

    Device (CP01) { // HeliosCore: Cluster 0, Cpu 0, Thread 1
      Name (_HID, "ACPI0007")
      Name (_UID, 1)
      Name (_STA, 0xF)
    }

    Device (CP02) { // HeliosCore: Cluster 0, Cpu 1, Thread 0
      Name (_HID, "ACPI0007")
      Name (_UID, 2)
      Name (_STA, 0xF)
    }

    Device (CP03) { // HeliosCore: Cluster 0, Cpu 1, Thread 1
      Name (_HID, "ACPI0007")
      Name (_UID, 3)
      Name (_STA, 0xF)
    }

    Device (CP04) { // HeliosCore: Cluster 0, Cpu 2, Thread 0
      Name (_HID, "ACPI0007")
      Name (_UID, 4)
      Name (_STA, 0xF)
    }

    Device (CP05) { // HeliosCore: Cluster 0, Cpu 2, Thread 1
      Name (_HID, "ACPI0007")
      Name (_UID, 5)
      Name (_STA, 0xF)
    }

    Device (CP06) { // HeliosCore: Cluster 0, Cpu 3, Thread 0
      Name (_HID, "ACPI0007")
      Name (_UID, 6)
      Name (_STA, 0xF)
    }

    Device (CP07) { // HeliosCore: Cluster 0, Cpu 3, Thread 1
      Name (_HID, "ACPI0007")
      Name (_UID, 7)
      Name (_STA, 0xF)
    }

    Device (CP08) { // HeliosCore: Cluster 0, Cpu 4, Thread 0
      Name (_HID, "ACPI0007")
      Name (_UID, 8)
      Name (_STA, 0xF)
    }

    Device (CP09) { // HeliosCore: Cluster 0, Cpu 4, Thread 1
      Name (_HID, "ACPI0007")
      Name (_UID, 9)
      Name (_STA, 0xF)
    }

    Device (CP10) { // HeliosCore: Cluster 0, Cpu 5, Thread 0
      Name (_HID, "ACPI0007")
      Name (_UID, 10)
      Name (_STA, 0xF)
    }

    Device (CP11) { // HeliosCore: Cluster 0, Cpu 5, Thread 1
      Name (_HID, "ACPI0007")
      Name (_UID, 11)
      Name (_STA, 0xF)
    }

    Device (CP12) { // HeliosCore: Cluster 0, Cpu 6, Thread 0
      Name (_HID, "ACPI0007")
      Name (_UID, 12)
      Name (_STA, 0xF)
    }

    Device (CP13) { // HeliosCore: Cluster 0, Cpu 6, Thread 1
      Name (_HID, "ACPI0007")
      Name (_UID, 13)
      Name (_STA, 0xF)
    }

    Device (CP14) { // HeliosCore: Cluster 0, Cpu 7, Thread 0
      Name (_HID, "ACPI0007")
      Name (_UID, 14)
      Name (_STA, 0xF)
    }

    Device (CP15) { // HeliosCore: Cluster 0, Cpu 7, Thread 1
      Name (_HID, "ACPI0007")
      Name (_UID, 15)
      Name (_STA, 0xF)
    }

    Device (CP16) { // HeliosCore: Cluster 0, Cpu 0, Thread 0
      Name (_HID, "ACPI0007")
      Name (_UID, 16)
      Name (_STA, 0xF)
    }

    Device (CP17) { // HeliosCore: Cluster 0, Cpu 0, Thread 1
      Name (_HID, "ACPI0007")
      Name (_UID, 17)
      Name (_STA, 0xF)
    }

    Device (CP18) { // HeliosCore: Cluster 0, Cpu 1, Thread 0
      Name (_HID, "ACPI0007")
      Name (_UID, 18)
      Name (_STA, 0xF)
    }

    Device (CP19) { // HeliosCore: Cluster 0, Cpu 1, Thread 1
      Name (_HID, "ACPI0007")
      Name (_UID, 19)
      Name (_STA, 0xF)
    }

    Device (CP20) { // HeliosCore: Cluster 0, Cpu 2, Thread 0
      Name (_HID, "ACPI0007")
      Name (_UID, 20)
      Name (_STA, 0xF)
    }

    Device (CP21) { // HeliosCore: Cluster 0, Cpu 2, Thread 1
      Name (_HID, "ACPI0007")
      Name (_UID, 21)
      Name (_STA, 0xF)
    }

    Device (CP22) { // HeliosCore: Cluster 0, Cpu 3, Thread 0
      Name (_HID, "ACPI0007")
      Name (_UID, 22)
      Name (_STA, 0xF)
    }

    Device (CP23) { // HeliosCore: Cluster 0, Cpu 3, Thread 1
      Name (_HID, "ACPI0007")
      Name (_UID, 23)
      Name (_STA, 0xF)
    }

    Device (CP24) { // HeliosCore: Cluster 0, Cpu 4, Thread 0
      Name (_HID, "ACPI0007")
      Name (_UID, 24)
      Name (_STA, 0xF)
    }

    Device (CP25) { // HeliosCore: Cluster 0, Cpu 4, Thread 1
      Name (_HID, "ACPI0007")
      Name (_UID, 25)
      Name (_STA, 0xF)
    }

    Device (CP26) { // HeliosCore: Cluster 0, Cpu 5, Thread 0
      Name (_HID, "ACPI0007")
      Name (_UID, 26)
      Name (_STA, 0xF)
    }

    Device (CP27) { // HeliosCore: Cluster 0, Cpu 5, Thread 1
      Name (_HID, "ACPI0007")
      Name (_UID, 27)
      Name (_STA, 0xF)
    }

    Device (CP28) { // HeliosCore: Cluster 0, Cpu 6, Thread 0
      Name (_HID, "ACPI0007")
      Name (_UID, 28)
      Name (_STA, 0xF)
    }

    Device (CP29) { // HeliosCore: Cluster 0, Cpu 6, Thread 1
      Name (_HID, "ACPI0007")
      Name (_UID, 29)
      Name (_STA, 0xF)
    }

    Device (CP30) { // HeliosCore: Cluster 0, Cpu 7, Thread 0
      Name (_HID, "ACPI0007")
      Name (_UID, 30)
      Name (_STA, 0xF)
    }

    Device (CP31) { // HeliosCore: Cluster 0, Cpu 7, Thread 1
      Name (_HID, "ACPI0007")
      Name (_UID, 31)
      Name (_STA, 0xF)
    }

    // UART PL011
    Device (COM0) {
      Name (_HID, "ARMH0011")
      Name (_CID, "ARMH0011")
      Name (_UID, Zero)
      Name (_STA, 0xF)
      Name (_CRS, ResourceTemplate() {
        Memory32Fixed (
          ReadWrite,
          FixedPcdGet64 (PcdSerialDbgRegisterBase),
          0x1000
        )
        Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { 147 }
      })
    }

    // SMSC 91C111
    Device (ETH0) {
      Name (_HID, "LNRO0003")
      Name (_UID, Zero)
      Name (_STA, 0xF)
      Name (_CRS, ResourceTemplate() {
        Memory32Fixed (ReadWrite, 0x18000000, 0x1000)
        Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { 111 }
      })
      Name (_DSD, Package() {
        ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
        Package () {
          Package (2) {"reg-io-width", 4 },
        }
      })
    }

    // VIRTIO DISK
    Device (VR00) {
      Name (_HID, "LNRO0005")
      Name (_UID, 0)
      Name (_CCA, 1)    // mark the device coherent

      Name (_CRS, ResourceTemplate() {
        Memory32Fixed (
          ReadWrite,
          FixedPcdGet32 (PcdVirtioBlkBaseAddress),
          FixedPcdGet32 (PcdVirtioBlkSize)
        )
        Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) {
          FixedPcdGet32 (PcdVirtioBlkInterrupt)
        }
      })
    }
  } // Scope(_SB)
}
