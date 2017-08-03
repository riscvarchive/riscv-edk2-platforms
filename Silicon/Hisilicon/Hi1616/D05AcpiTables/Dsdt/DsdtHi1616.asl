/** @file
  Differentiated System Description Table Fields (DSDT)

  Copyright (c) 2014, ARM Ltd. All rights reserved.<BR>
  Copyright (c) 2015-2016, Hisilicon Limited. All rights reserved.<BR>
  Copyright (c) 2015-2016, Linaro Limited. All rights reserved.<BR>
    This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

  Based on the files under ArmPlatformPkg/ArmJunoPkg/AcpiTables/

**/

#include "Hi1616Platform.h"

DefinitionBlock("DsdtTable.aml", "DSDT", 2, "HISI  ", "HIP07   ", EFI_ACPI_ARM_OEM_REVISION) {
  include ("Lpc.asl")
  include ("D05Mbig.asl")
  include ("Com.asl")
  include ("CPU.asl")
  include ("D05I2c.asl")
  include ("D05Usb.asl")
  include ("D05Hns.asl")
  include ("D05Sas.asl")
  include ("D05Pci.asl")
}
