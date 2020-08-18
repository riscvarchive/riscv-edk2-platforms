/** @file
*  Secondary System Description Table Fields (SSDT)
*
*  Copyright (c) 2020, Arm Ltd. All rights reserved.
*
*  SPDX-License-Identifier: BSD-2-Clause-Patent
*
**/

#include "SgiPlatform.h"
#include "SgiAcpiHeader.h"

DefinitionBlock ("SsdtRosTable.aml", "SSDT", 1, "ARMLTD", "ARMSGI",
                 EFI_ACPI_ARM_OEM_REVISION) {
  Scope (_SB) {
    // UART PL011
    Device (COM0) {
      Name (_HID, "ARMH0011")
      Name (_CID, "ARMH0011")
      Name (_UID, Zero)
      Name (_STA, 0xF)
      Name (_CRS, ResourceTemplate () {
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
      Name (_CRS, ResourceTemplate () {
        Memory32Fixed (ReadWrite, 0x18000000, 0x1000)
        Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { 111 }
      })
      Name (_DSD, Package() {
        ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
                Package() {
                  Package(2) {"reg-io-width", 4 },
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

    // VIRTIO NET
    Device (VR01) {
      Name (_HID, "LNRO0005")
      Name (_UID, 1)
      Name (_CCA, 1)    // mark the device coherent

      Name (_CRS, ResourceTemplate() {
        Memory32Fixed (ReadWrite,
          FixedPcdGet32 (PcdVirtioNetBaseAddress),
          FixedPcdGet32 (PcdVirtioNetSize)
        )
        Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) {
          FixedPcdGet32 (PcdVirtioNetInterrupt)
        }
      })
    }
  } // Scope(_SB)
}
