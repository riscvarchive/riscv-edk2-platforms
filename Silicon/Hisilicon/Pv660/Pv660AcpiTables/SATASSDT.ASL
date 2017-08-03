/** @file
  Differentiated System Description Table Fields (DSDT)

  Copyright (c) 2014, ARM Ltd. All rights reserved.<BR>
  Copyright (c) 2015, Hisilicon Limited. All rights reserved.<BR>
  Copyright (c) 2015, Linaro Limited. All rights reserved.<BR>
    This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

  Based on the files under ArmPlatformPkg/ArmJunoPkg/AcpiTables/

**/
#include "Pv660Platform.h"

DefinitionBlock (
    "SATASSDT.aml", // Output Filename
    "SSDT",         // Signature
    0x01,           // DSDT Compliance Revision
    "HISI  ",       // OEM ID
    "SATA",     // Table ID
    EFI_ACPI_ARM_OEM_REVISION          // OEM Revision
    )
{
External(\_SB.MBI3)
Scope(_SB) {
    Device (AHCI)
    {
      Name(_HID, "HISI0001")  // HiSi AHCI
      Name (_CCA, 1)           // Cache-coherent controller
      Name (_CRS, ResourceTemplate () {
        Memory32Fixed (ReadWrite, 0xb1002800, 0x00000B00)
        Memory32Fixed (ReadWrite, 0xb1000000, 0x00002800)
        Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive,,,) { 382 }
      })

      Name (_DSD, Package () {
        ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
        Package () {
          Package () {"interrupt-parent",Package() {\_SB.MBI3}}
        }
      })
    }

}

}
