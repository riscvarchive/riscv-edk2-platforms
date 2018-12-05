/** @file

  C language wrapper to build DSDT generated data.

  Copyright (c) 2014 - 2016, AMD Inc. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <AmdStyxAcpiLib.h>
#include <Dsdt.hex>

EFI_ACPI_DESCRIPTION_HEADER *
DsdtHeader (
  VOID
  )
{
  return (EFI_ACPI_DESCRIPTION_HEADER *) &AmlCode[0];
}
