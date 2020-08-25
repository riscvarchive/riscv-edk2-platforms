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

#endif
