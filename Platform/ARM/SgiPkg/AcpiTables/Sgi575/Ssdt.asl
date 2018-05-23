/** @file
*  Secondary System Description Table (SSDT)
*
*  Copyright (c) 2018, ARM Limited. All rights reserved.
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

#include "SgiAcpiHeader.h"

DefinitionBlock("SsdtPci.aml", "SSDT", 1, "ARMLTD", "SGI575",
                EFI_ACPI_ARM_OEM_REVISION)
{
  Scope (_SB) {
    // PCI Root Complex
    Device(PCI0) {
      Name (_HID, EISAID("PNP0A08")) // PCI Express Root Bridge
      Name (_CID, EISAID("PNP0A03")) // Compatible PCI Root Bridge
      Name (_SEG, Zero)              // PCI Segment Group number
      Name (_BBN, Zero)              // PCI Base Bus Number
      Name (_CCA, 1)                 // Cache Coherency Attribute

      // Root complex resources
      Method (_CRS, 0, Serialized) {
        Name (RBUF, ResourceTemplate () {
          WordBusNumber (      // Bus numbers assigned to this root
            ResourceProducer,
            MinFixed,
            MaxFixed,
            PosDecode,
            0,                 // AddressGranularity
            0,                 // AddressMinimum - Minimum Bus Number
            255,               // AddressMaximum - Maximum Bus Number
            0,                 // AddressTranslation - Set to 0
            256                // RangeLength - Number of Busses
          )

          DWordMemory (        // 32-bit BAR Windows
            ResourceProducer,
            PosDecode,
            MinFixed,
            MaxFixed,
            Cacheable,
            ReadWrite,
            0x00000000,        // Granularity
            0x70000000,        // Min Base Address
            0x777FFFFF,        // Max Base Address
            0x00000000,        // Translate
            0x07800000         // Length
          )

          QWordMemory (        // 64-bit BAR Windows
            ResourceProducer,
            PosDecode,
            MinFixed,
            MaxFixed,
            Cacheable,
            ReadWrite,
            0x00000000,        // Granularity
            0x500000000,       // Min Base Address
            0x7FFFFFFFF,       // Max Base Address
            0x00000000,        // Translate
            0x300000000        // Length
          )

          DWordIo (             // IO window
            ResourceProducer,
            MinFixed,
            MaxFixed,
            PosDecode,
            EntireRange,
            0x00000000,         // Granularity
            0x00000000,         // Min Base Address
            0x007FFFFF,         // Max Base Address
            0x77800000,         // Translate
            0x00800000,         // Length
            ,
            ,
            ,
            TypeTranslation
          )
        }) // Name(RBUF)

        Return (RBUF)
      } // Method (_CRS)
    }
  }
}
