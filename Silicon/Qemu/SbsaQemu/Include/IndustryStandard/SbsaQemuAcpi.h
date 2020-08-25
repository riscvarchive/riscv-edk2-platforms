/** @file
*
*  Copyright (c) 2020, Linaro Limited. All rights reserved.
*
*  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef SBSAQEMUACPI_H
#define SBSAQEMUACPI_H

// A macro to initialise the common header part of EFI ACPI tables as defined by
// EFI_ACPI_DESCRIPTION_HEADER structure.
#define SBSAQEMU_ACPI_HEADER(Signature, Type, Revision) {                      \
    Signature,                                   /* UINT32  Signature */       \
    sizeof (Type),                               /* UINT32  Length */          \
    Revision,                                    /* UINT8   Revision */        \
    0,                                           /* UINT8   Checksum */        \
    { 'L', 'I', 'N', 'A', 'R', 'O' },            /* UINT8   OemId[6] */        \
    FixedPcdGet64 (PcdAcpiDefaultOemTableId),    /* UINT64  OemTableId */      \
    FixedPcdGet32 (PcdAcpiDefaultOemRevision),   /* UINT32  OemRevision */     \
    FixedPcdGet32 (PcdAcpiDefaultCreatorId),     /* UINT32  CreatorId */       \
    FixedPcdGet32 (PcdAcpiDefaultCreatorRevision)/* UINT32  CreatorRevision */ \
  }

// Defines for MADT
#define SBSAQEMU_MADT_GIC_VBASE          0x2c020000
#define SBSAQEMU_MADT_GIC_HBASE          0x2c010000
#define SBSAQEMU_MADT_GIC_PMU_IRQ        23
#define SBSAQEMU_MADT_GICR_SIZE          0x4000000

// Macro for MADT GIC Redistributor Structure
#define SBSAQEMU_MADT_GICR_INIT() {                                            \
   EFI_ACPI_6_0_GICR,                        /* Type */                        \
   sizeof (EFI_ACPI_6_0_GICR_STRUCTURE),     /* Length */                      \
   EFI_ACPI_RESERVED_WORD,                   /* Reserved */                    \
   FixedPcdGet32 (PcdGicRedistributorsBase), /* DiscoveryRangeBaseAddress */   \
   SBSAQEMU_MADT_GICR_SIZE                   /* DiscoveryRangeLength */        \
   }

#define SBSAQEMU_UART0_BASE              0x60000000

#define SBSAQEMU_PCI_SEG0_CONFIG_BASE    0xf0000000
#define SBSAQEMU_PCI_SEG0_BUSNUM_MIN     0x00
#define SBSAQEMU_PCI_SEG0_BUSNUM_MAX     0xFF

#define SBSAQEMU_ACPI_SCOPE_OP_MAX_LENGTH 5

#define SBSAQEMU_ACPI_SCOPE_NAME         { '_', 'S', 'B', '_' }

#define SBSAQEMU_ACPI_CPU_DEV_LEN        0x1C
#define SBSAQEMU_ACPI_CPU_DEV_NAME       { 'C', '0', '0', '0' }

// Macro to convert Integer to Character
#define SBSAQEMU_ACPI_ITOA(Byte)         (0x30 + (Byte > 9 ? (Byte + 1) : Byte))

#define SBSAQEMU_ACPI_CPU_HID           {                                      \
  AML_NAME_OP, AML_NAME_CHAR__, 'H', 'I', 'D',                                 \
  AML_STRING_PREFIX, 'A', 'C', 'P', 'I', '0', '0', '0', '7',                   \
  AML_ZERO_OP                                                                  \
  }

#define SBSAQEMU_ACPI_CPU_UID            {                                     \
   AML_NAME_OP, AML_NAME_CHAR__, 'U', 'I', 'D', AML_BYTE_PREFIX,               \
   AML_ZERO_OP, AML_ZERO_OP                                                    \
   }

typedef struct {
  UINT8         device_header[2];
  UINT8         length;
  UINT8         dev_name[4];
  UINT8         hid[15];
  UINT8         uid[8];
} SBSAQEMU_ACPI_CPU_DEVICE;

#endif
