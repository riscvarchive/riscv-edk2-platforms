/** @file

  Serial Port Console Redirection Table
  © 2000 - 2014 Microsoft Corporation. All rights reserved.
  http://go.microsoft.com/fwlink/?linkid=403368

  Copyright (c) 2014 - 2016, AMD Inc. All rights reserved.

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <AmdStyxAcpiLib.h>
#include <IndustryStandard/SerialPortConsoleRedirectionTable.h>

#pragma pack(push, 1)

#define EFI_ACPI_SERIAL_PORT_CONSOLE_REDIRECTION_TABLE_INTERFACE_TYPE_PL011   3

STATIC EFI_ACPI_SERIAL_PORT_CONSOLE_REDIRECTION_TABLE AcpiSpcr = {
  //
  // Header
  //
  AMD_ACPI_HEADER (EFI_ACPI_5_0_SERIAL_PORT_CONSOLE_REDIRECTION_TABLE_SIGNATURE,
                   EFI_ACPI_SERIAL_PORT_CONSOLE_REDIRECTION_TABLE,
                   2), /* New MS definition for PL011 support */
  //
  // InterfaceType
  //
  EFI_ACPI_SERIAL_PORT_CONSOLE_REDIRECTION_TABLE_INTERFACE_TYPE_PL011,
  //
  // Reserved[3]
  //
  {EFI_ACPI_RESERVED_BYTE, EFI_ACPI_RESERVED_BYTE, EFI_ACPI_RESERVED_BYTE},
  //
  // BaseAddress
  //
  AMD_GASN(FixedPcdGet64(PcdSerialRegisterBase)),
  //
  // InterruptType
  //
  0,
  //
  // Irq
  //
  0,
  //
  // GlobalSystemInterrupt
  //
  0x148,
  //
  // BaudRate
  //
  EFI_ACPI_SERIAL_PORT_CONSOLE_REDIRECTION_TABLE_BAUD_RATE_115200,
  //
  // Parity
  //
  EFI_ACPI_SERIAL_PORT_CONSOLE_REDIRECTION_TABLE_PARITY_NO_PARITY,
  //
  // StopBits
  //
  EFI_ACPI_SERIAL_PORT_CONSOLE_REDIRECTION_TABLE_STOP_BITS_1,
  //
  // FlowControl
  //
  0,
  //
  // TerminalType
  //
  EFI_ACPI_SERIAL_PORT_CONSOLE_REDIRECTION_TABLE_TERMINAL_TYPE_ANSI,
  //
  // Language
  //
  EFI_ACPI_RESERVED_BYTE,
  //
  // PciDeviceId
  //
  0xFFFF,
  //
  // PciVendorId
  //
  0xFFFF,
  //
  // PciBusNumber
  //
  0x00,
  //
  // PciDeviceNumber
  //
  0x00,
  //
  // PciFunctionNumber
  //
  0x00,
  //
  // PciFlags
  //
  0,
  //
  // PciSegment
  //
  0,
  //
  // Reserved2
  //
  EFI_ACPI_RESERVED_DWORD
};

#pragma pack(pop)

EFI_ACPI_DESCRIPTION_HEADER *
SpcrHeader (
  VOID
  )
{
  return &AcpiSpcr.Header;
}

