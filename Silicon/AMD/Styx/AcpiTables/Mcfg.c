/** @file

  ACPI Memory mapped configuration space base address Description Table (MCFG).
  Implementation based on PCI Firmware Specification Revision 3.0 final draft,
  downloadable at http://www.pcisig.com/home

  Copyright (c) 2014 - 2016, AMD Inc. All rights reserved.

  This program and the accompanying materials are licensed and
  made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the
  license may be found at http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <AmdStyxAcpiLib.h>
#include <IndustryStandard/MemoryMappedConfigurationSpaceAccessTable.h>

#if STYX_A0
#define END_PCI_BUS_NUMBER                15
#else
#define END_PCI_BUS_NUMBER                255
#endif

#pragma pack(push, 1)

typedef struct {
  EFI_ACPI_MEMORY_MAPPED_CONFIGURATION_BASE_ADDRESS_TABLE_HEADER Header;
  EFI_ACPI_MEMORY_MAPPED_ENHANCED_CONFIGURATION_SPACE_BASE_ADDRESS_ALLOCATION_STRUCTURE Structure;
} EFI_ACPI_MEMORY_MAPPED_CONFIGURATION_SPACE_ACCESS_DESCRIPTION_TABLE;

EFI_ACPI_MEMORY_MAPPED_CONFIGURATION_SPACE_ACCESS_DESCRIPTION_TABLE AcpiMcfg = {
  { AMD_ACPI_HEADER (EFI_ACPI_5_0_PCI_EXPRESS_MEMORY_MAPPED_CONFIGURATION_SPACE_BASE_ADDRESS_DESCRIPTION_TABLE_SIGNATURE,
    EFI_ACPI_MEMORY_MAPPED_CONFIGURATION_SPACE_ACCESS_DESCRIPTION_TABLE,
    EFI_ACPI_MEMORY_MAPPED_CONFIGURATION_SPACE_ACCESS_TABLE_REVISION),
    EFI_ACPI_RESERVED_QWORD },
  { 0xF0000000ULL, 0, 0, END_PCI_BUS_NUMBER, EFI_ACPI_RESERVED_DWORD }
};

#pragma pack(pop)

EFI_ACPI_DESCRIPTION_HEADER *
McfgHeader (
  VOID
  )
{
  return &AcpiMcfg.Header.Header;
}
