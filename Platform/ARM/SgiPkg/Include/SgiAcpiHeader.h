/** @file
*
*  Copyright (c) 2018-2020, ARM Limited. All rights reserved.
*
*  SPDX-License-Identifier: BSD-2-Clause-Patent
*
**/

#ifndef __SGI_ACPI_HEADER__
#define __SGI_ACPI_HEADER__

#include <IndustryStandard/Acpi.h>

//
// ACPI table information used to initialize tables.
//
#define EFI_ACPI_ARM_OEM_ID           'A','R','M','L','T','D'   // OEMID 6 bytes long
#define EFI_ACPI_ARM_OEM_TABLE_ID     SIGNATURE_64 ('A','R','M','S','G','I',' ',' ') // OEM table id 8 bytes long
#define EFI_ACPI_ARM_OEM_REVISION     0x20140727
#define EFI_ACPI_ARM_CREATOR_ID       SIGNATURE_32('A','R','M',' ')
#define EFI_ACPI_ARM_CREATOR_REVISION 0x00000099

// A macro to initialise the common header part of EFI ACPI tables as defined by
// EFI_ACPI_DESCRIPTION_HEADER structure.
#define ARM_ACPI_HEADER(Signature, Type, Revision) {             \
    Signature,                      /* UINT32  Signature */       \
    sizeof (Type),                  /* UINT32  Length */          \
    Revision,                       /* UINT8   Revision */        \
    0,                              /* UINT8   Checksum */        \
    { EFI_ACPI_ARM_OEM_ID },        /* UINT8   OemId[6] */        \
    EFI_ACPI_ARM_OEM_TABLE_ID,      /* UINT64  OemTableId */      \
    EFI_ACPI_ARM_OEM_REVISION,      /* UINT32  OemRevision */     \
    EFI_ACPI_ARM_CREATOR_ID,        /* UINT32  CreatorId */       \
    EFI_ACPI_ARM_CREATOR_REVISION   /* UINT32  CreatorRevision */ \
  }

// EFI_ACPI_6_2_GIC_STRUCTURE
#define EFI_ACPI_6_2_GICC_STRUCTURE_INIT(GicId, AcpiCpuUid, Mpidr, Flags,      \
  PmuIrq, GicBase, GicVBase, GicHBase, GsivId, GicRBase, Efficiency)           \
  {                                                                            \
    EFI_ACPI_6_2_GIC,                     /* Type */                           \
    sizeof (EFI_ACPI_6_2_GIC_STRUCTURE),  /* Length */                         \
    EFI_ACPI_RESERVED_WORD,               /* Reserved */                       \
    GicId,                                /* CPUInterfaceNumber */             \
    AcpiCpuUid,                           /* AcpiProcessorUid */               \
    Flags,                                /* Flags */                          \
    0,                                    /* ParkingProtocolVersion */         \
    PmuIrq,                               /* PerformanceInterruptGsiv */       \
    0,                                    /* ParkedAddress */                  \
    GicBase,                              /* PhysicalBaseAddress */            \
    GicVBase,                             /* GICV */                           \
    GicHBase,                             /* GICH */                           \
    GsivId,                               /* VGICMaintenanceInterrupt */       \
    GicRBase,                             /* GICRBaseAddress */                \
    Mpidr,                                /* MPIDR */                          \
    Efficiency,                           /* ProcessorPowerEfficiencyClass */  \
    {                                                                          \
      EFI_ACPI_RESERVED_BYTE,             /* Reserved2[0] */                   \
      EFI_ACPI_RESERVED_BYTE,             /* Reserved2[1] */                   \
      EFI_ACPI_RESERVED_BYTE              /* Reserved2[2] */                   \
    }                                                                          \
  }

// EFI_ACPI_6_2_GIC_DISTRIBUTOR_STRUCTURE
#define EFI_ACPI_6_2_GIC_DISTRIBUTOR_INIT(GicDistHwId, GicDistBase,            \
  GicDistVector, GicVersion)                                                   \
  {                                                                            \
    EFI_ACPI_6_2_GICD,                    /* Type */                           \
    sizeof (EFI_ACPI_6_2_GIC_DISTRIBUTOR_STRUCTURE),                           \
    EFI_ACPI_RESERVED_WORD,               /* Reserved1 */                      \
    GicDistHwId,                          /* GicId */                          \
    GicDistBase,                          /* PhysicalBaseAddress */            \
    GicDistVector,                        /* SystemVectorBase */               \
    GicVersion,                           /* GicVersion */                     \
    {                                                                          \
      EFI_ACPI_RESERVED_BYTE,             /* Reserved2[0] */                   \
      EFI_ACPI_RESERVED_BYTE,             /* Reserved2[1] */                   \
      EFI_ACPI_RESERVED_BYTE              /* Reserved2[2] */                   \
    }                                                                          \
  }

// EFI_ACPI_6_2_GICR_STRUCTURE
#define EFI_ACPI_6_2_GIC_REDISTRIBUTOR_INIT(RedisRegionAddr, RedisDiscLength)  \
  {                                                                            \
    EFI_ACPI_6_2_GICR,                    /* Type */                           \
    sizeof (EFI_ACPI_6_2_GICR_STRUCTURE), /* Length */                         \
    EFI_ACPI_RESERVED_WORD,               /* Reserved */                       \
    RedisRegionAddr,                      /* DiscoveryRangeBaseAddress */      \
    RedisDiscLength                       /* DiscoveryRangeLength */           \
  }

// EFI_ACPI_6_2_GIC_ITS_STRUCTURE
#define EFI_ACPI_6_2_GIC_ITS_INIT(GicItsId, GicItsBase)                        \
  {                                                                            \
    EFI_ACPI_6_2_GIC_ITS,                 /* Type */                           \
    sizeof (EFI_ACPI_6_2_GIC_ITS_STRUCTURE),                                   \
    EFI_ACPI_RESERVED_WORD,               /* Reserved */                       \
    GicItsId,                             /* GicItsId */                       \
    GicItsBase,                           /* PhysicalBaseAddress */            \
    EFI_ACPI_RESERVED_DWORD               /* DiscoveryRangeLength */           \
  }

// EFI_ACPI_6_3_MEMORY_AFFINITY_STRUCTURE
#define EFI_ACPI_6_3_MEMORY_AFFINITY_STRUCTURE_INIT(                           \
          ProximityDomain, Base, Length, Flags)                                \
  {                                                                            \
    1, sizeof (EFI_ACPI_6_3_MEMORY_AFFINITY_STRUCTURE), ProximityDomain,       \
    EFI_ACPI_RESERVED_WORD, (Base) & 0xffffffff,                               \
    (Base) >> 32, (Length) & 0xffffffff,                                       \
    (Length) >> 32, EFI_ACPI_RESERVED_DWORD, Flags,                            \
    EFI_ACPI_RESERVED_QWORD                                                    \
  }

// EFI_ACPI_6_3_GICC_AFFINITY_STRUCTURE
#define EFI_ACPI_6_3_GICC_AFFINITY_STRUCTURE_INIT(                             \
          ProximityDomain, ACPIProcessorUID, Flags, ClockDomain)               \
  {                                                                            \
    3, sizeof (EFI_ACPI_6_3_GICC_AFFINITY_STRUCTURE), ProximityDomain,         \
    ACPIProcessorUID,  Flags,  ClockDomain                                     \
  }

#endif /* __SGI_ACPI_HEADER__ */
