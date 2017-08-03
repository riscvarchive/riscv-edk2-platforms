/*
#    Copyright (c) 2015, Hisilicon Limited. All rights reserved.
#    Copyright (c) 2015, Linaro Limited. All rights reserved.
#
#    This program and the accompanying materials
#    are licensed and made available under the terms and conditions of the BSD License
#    which accompanies this distribution. The full text of the license may be found at
#    http://opensource.org/licenses/bsd-license.php
#
#    THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
#    WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#
#**/

#ifndef _EFI_UNINSTALL_ACPI_H_
#define _EFI_UNINSTALL_ACPI_H_

#define EFI_SATA_CONTROL_GUID \
  { \
    0x287e41a8, 0x5108, 0x4faf, { 0xbe, 0x3d, 0xd4, 0xdd, 0xff, 0xcd, 0x4e, 0x9f } \
  }

#define SATA_ENABLE_FLAG       (L"SataEnableFlag")
#define EFI_SAS_SIGNATURE SIGNATURE_32 ('S', 'A', 'S', '0')
#define EFI_SATA_SIGNATURE SIGNATURE_32 ('S', 'A', 'T', 'A')
#define SATAENABLE 1
#define SATADISABLE 0


#endif
