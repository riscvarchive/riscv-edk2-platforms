/** @file

  Multiple APIC Description Table (MADT)

  Copyright (c) 2012 - 2014, ARM Ltd. All rights reserved.<BR>
  Copyright (c) 2014 - 2016, AMD Inc. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
/**

  Derived from:
   ArmPlatformPkg/ArmJunoPkg/AcpiTables/Madt.aslc

**/

#include <Uefi.h>
#include <Library/ArmLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Guid/ArmMpCoreInfo.h>

#include <AmdStyxAcpiLib.h>
#include <Protocol/AmdMpCoreInfo.h>

AMD_MP_CORE_INFO_PROTOCOL  *mAmdMpCoreInfoProtocol = NULL;


// ARM PL390 General Interrupt Controller
#define GIC_BASE                             (FixedPcdGet64 (PcdGicInterruptInterfaceBase))
#define GICD_BASE                            (FixedPcdGet64 (PcdGicDistributorBase))
#define GICV_BASE                            (FixedPcdGet64 (PcdGicVirtualInterruptInterfaceBase))
#define GICH_BASE                            (FixedPcdGet64 (PcdGicHypervisorInterruptInterfaceBase))
#define VGIC_MAINT_INT                       (FixedPcdGet32 (PcdGicVirtualMaintenanceInterrupt))
#define GICVR_BASE                           (FixedPcdGet64 (PcdGicVirtualRegisterInterfaceBase))
#define GIC_MSI_FRAME                        (FixedPcdGet64 (PcdGicMSIFrameBase))
#define GIC_VERSION                          (FixedPcdGet8  (PcdGicVersion))

#define GICD_ID                              ( 0 )
#define GICD_VECTOR                          ( 0 )

#define GICM_ID                              ( 0 )
#define GICM_SPI_COUNT                       ( 0x100 )
#define GICM_SPI_BASE                        ( 0x40 )
#define GSIV_SPI_OFFSET                      ( 32 )

#if STYX_A0
  #define MSI_TYPER_FLAG                     ( 1 ) // Ignore TYPER register and use Count/Base fields
#else
  #define MSI_TYPER_FLAG                     ( 0 ) // Use TYPER register and ignore Count/Base fields
#endif

#define PARKING_PROTOCOL_VERSION             (FixedPcdGet32 (PcdParkingProtocolVersion))
#define PARKED_OFFSET                        ( 4096 )

#define CORES_PER_CLUSTER                    (FixedPcdGet32 (PcdSocCoresPerCluster))
#define PARKED_ADDRESS(Base, ClusterId, CoreId) \
        ((Base) + (CORES_PER_CLUSTER * ClusterId + CoreId) * PARKED_OFFSET)


/* Macro to populate EFI_ACPI_5_1_GIC_STRUCTURE */
#define AMD_GIC(CpuNum, ClusterId, CoreId, PerfInt)  {                        \
  EFI_ACPI_5_1_GIC,                     /* UINT8 Type */                      \
  sizeof (EFI_ACPI_5_1_GIC_STRUCTURE),  /* UINT8 Length */                    \
  EFI_ACPI_RESERVED_WORD,               /* UINT16 Reserved */                 \
  CpuNum,                               /* UINT32 CPUInterfaceNumber */       \
  (ClusterId << 8) | CoreId,            /* UINT32 AcpiProcessorUid */         \
  EFI_ACPI_5_1_GIC_ENABLED,             /* UINT32 Flags */                    \
  PARKING_PROTOCOL_VERSION,             /* UINT32 ParkingProtocolVersion */   \
  PerfInt,                              /* UINT32 PerformanceInterruptGsiv */ \
  0,                                    /* UINT64 ParkedAddress */            \
  GIC_BASE,                             /* UINT64 PhysicalBaseAddress */      \
  GICV_BASE,                            /* UINT64 GICV */                     \
  GICH_BASE,                            /* UINT64 GICH */                     \
  VGIC_MAINT_INT,                       /* UINT32 VGICMaintenanceInterrupt */ \
  GICVR_BASE,                           /* UINT64 GICRBaseAddress */          \
  (ClusterId << 8) | CoreId             /* UINT64 MPIDR */                    \
  }

/* Macro to initialise EFI_ACPI_5_1_GIC_DISTRIBUTOR_STRUCTURE */
#define AMD_GICD(Id, Vec) {                                                             \
  EFI_ACPI_5_1_GICD,                                /* UINT8 Type */                    \
  sizeof (EFI_ACPI_5_1_GIC_DISTRIBUTOR_STRUCTURE),  /* UINT8 Length */                  \
  EFI_ACPI_RESERVED_WORD,                           /* UINT16 Reserved1 */              \
  Id,                                               /* UINT32 GicId */                  \
  GICD_BASE,                                        /* UINT64 PhysicalBaseAddress */    \
  Vec,                                              /* UINT32 SystemVectorBase */       \
  EFI_ACPI_RESERVED_DWORD                           /* UINT32 Reserved2 */              \
  }

/* Macro to initialise EFI_ACPI_5_1_GIC_MSI_FRAME_STRUCTURE */
#define AMD_GICM(Id, SpiCount, SpiBase) {                                               \
   EFI_ACPI_5_1_GIC_MSI_FRAME,                      /* UINT8 Type */                    \
   sizeof(EFI_ACPI_5_1_GIC_MSI_FRAME_STRUCTURE),    /* UINT8 Length */                  \
   EFI_ACPI_RESERVED_WORD,                          /* UINT16 Reserved1 */              \
   Id,                                              /* UINT32 GicMsiFrameId */          \
   GIC_MSI_FRAME,                                   /* UINT64 PhysicalBaseAddress */    \
   MSI_TYPER_FLAG,                                  /* UINT32 Flags */                  \
   SpiCount,                                        /* UINT16 SPICount */               \
   SpiBase                                          /* UINT16 SPIBase */                \
   }


//
// NOTE: NUM_CORES is a pre-processor macro passed in with -D option
//
#pragma pack(push, 1)
typedef struct {
  EFI_ACPI_5_1_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER Header;
  EFI_ACPI_5_1_GIC_STRUCTURE                          GicC[NUM_CORES];
  EFI_ACPI_5_1_GIC_DISTRIBUTOR_STRUCTURE              GicD;
  EFI_ACPI_5_1_GIC_MSI_FRAME_STRUCTURE                GicM;
} EFI_ACPI_5_1_ARM_MADT_STRUCTURE;
#pragma pack(pop)


STATIC EFI_ACPI_5_1_ARM_MADT_STRUCTURE AcpiMadt = {
  {
    AMD_ACPI_HEADER (EFI_ACPI_5_1_MULTIPLE_APIC_DESCRIPTION_TABLE_SIGNATURE,
                     EFI_ACPI_5_1_ARM_MADT_STRUCTURE,
                     EFI_ACPI_5_1_MULTIPLE_APIC_DESCRIPTION_TABLE_REVISION),
    GIC_BASE,                                 // UINT32  LocalApicAddress
    0                                         // UINT32  Flags
  },
  {
    /*
     * GIC Interface for Cluster 0 CPU 0
     */
    AMD_GIC(0, 0, 0, 39),                     // EFI_ACPI_5_1_GIC_STRUCTURE
#if (NUM_CORES > 1)
    /*
     * GIC Interface for Cluster 0 CPU 1
     */
    AMD_GIC(1, 0, 1, 40),                     // EFI_ACPI_5_1_GIC_STRUCTURE
#endif
#if (NUM_CORES > 2)
    /*
     * GIC Interface for Cluster 1 CPU 0
     */
    AMD_GIC(2, 1, 0, 41),                     // EFI_ACPI_5_1_GIC_STRUCTURE
#endif
#if (NUM_CORES > 3)
    /*
     * GIC Interface for Cluster 1 CPU 1
     */
    AMD_GIC(3, 1, 1, 42),                     // EFI_ACPI_5_1_GIC_STRUCTURE
#endif
#if (NUM_CORES > 4)
    /*
     * GIC Interface for Cluster 2 CPU 0
     */
    AMD_GIC(4, 2, 0, 43),                     // EFI_ACPI_5_1_GIC_STRUCTURE
#endif
#if (NUM_CORES > 5)
    /*
     * GIC Interface for Cluster 2 CPU 1
     */
    AMD_GIC(5, 2, 1, 44),                     // EFI_ACPI_5_1_GIC_STRUCTURE
#endif
#if (NUM_CORES > 6)
    /*
     * GIC Interface for Cluster 3 CPU 0
     */
    AMD_GIC(6, 3, 0, 45),                     // EFI_ACPI_5_1_GIC_STRUCTURE
#endif
#if (NUM_CORES > 7)
    /*
     * GIC Interface for Cluster 3 CPU 1
     */
    AMD_GIC(7, 3, 1, 46),                     // EFI_ACPI_5_1_GIC_STRUCTURE
#endif
  },
  /*
   * GIC Distributor
   */
    AMD_GICD(GICD_ID, GICD_VECTOR),           // EFI_ACPI_5_1_GIC_DISTRIBUTOR_STRUCTURE
  /*
   * GIC MSI Frame
   */
  AMD_GICM(GICM_ID, GICM_SPI_COUNT, GICM_SPI_BASE),
};


STATIC
EFI_STATUS
BuildGicC (
  EFI_ACPI_5_1_GIC_STRUCTURE *GicC,
  UINT32 CpuNum,
  UINT32 ClusterId,
  UINT32 CoreId,
  EFI_PHYSICAL_ADDRESS MpParkingBase
  )
{
  UINT32 MpId, PmuSpi;
  EFI_STATUS Status;

  MpId = (UINT32) GET_MPID (ClusterId, CoreId);
  Status = mAmdMpCoreInfoProtocol->GetPmuSpiFromMpId (MpId, &PmuSpi);
  if (EFI_ERROR (Status))
    return Status;

  GicC->Type = EFI_ACPI_5_1_GIC;
  GicC->Length = sizeof (EFI_ACPI_5_1_GIC_STRUCTURE);
  GicC->Reserved = EFI_ACPI_RESERVED_WORD;
  GicC->CPUInterfaceNumber = CpuNum;
  GicC->AcpiProcessorUid = MpId;
  GicC->Flags = EFI_ACPI_5_1_GIC_ENABLED;
  GicC->ParkingProtocolVersion = PARKING_PROTOCOL_VERSION;
  GicC->ParkedAddress = PARKED_ADDRESS(MpParkingBase, ClusterId, CoreId);
  GicC->PhysicalBaseAddress = GIC_BASE;
  GicC->GICV = GICV_BASE;
  GicC->GICH = GICH_BASE;
  GicC->VGICMaintenanceInterrupt = VGIC_MAINT_INT;
  GicC->GICRBaseAddress = GICVR_BASE;
  GicC->PerformanceInterruptGsiv = PmuSpi + GSIV_SPI_OFFSET;
  GicC->MPIDR = MpId;

  return EFI_SUCCESS;
}

STATIC
VOID
BuildGicD (
  EFI_ACPI_5_1_GIC_DISTRIBUTOR_STRUCTURE *GicD,
  UINT32 GicId,
  UINT32 SystemVectorBase
  )
{
  GicD->Type = EFI_ACPI_5_1_GICD;
  GicD->Length = sizeof (EFI_ACPI_5_1_GIC_DISTRIBUTOR_STRUCTURE);
  GicD->Reserved1 = EFI_ACPI_RESERVED_WORD;
  GicD->GicId = GicId;
  GicD->PhysicalBaseAddress = GICD_BASE;
  GicD->SystemVectorBase = SystemVectorBase;
#if 0
  GicD->Reserved2 = EFI_ACPI_RESERVED_DWORD;
#else
  GicD->GicVersion = EFI_ACPI_RESERVED_BYTE;
  GicD->Reserved2[0] = EFI_ACPI_RESERVED_BYTE;
  GicD->Reserved2[1] = EFI_ACPI_RESERVED_BYTE;
  GicD->Reserved2[2] = EFI_ACPI_RESERVED_BYTE;
#endif
}


STATIC
VOID
BuildGicM (
  EFI_ACPI_5_1_GIC_MSI_FRAME_STRUCTURE *GicM,
  UINT32 MsiFrameId,
  UINT16 SpiCount,
  UINT16 SpiBase
  )
{
  GicM->Type = EFI_ACPI_5_1_GIC_MSI_FRAME;
  GicM->Length = sizeof(EFI_ACPI_5_1_GIC_MSI_FRAME_STRUCTURE);
  GicM->Reserved1 = EFI_ACPI_RESERVED_WORD;
  GicM->GicMsiFrameId = MsiFrameId;
  GicM->PhysicalBaseAddress = GIC_MSI_FRAME;
  GicM->Flags = MSI_TYPER_FLAG;
  GicM->SPICount = SpiCount;
  GicM->SPIBase = SpiBase;
}


EFI_ACPI_DESCRIPTION_HEADER *
MadtHeader (
  VOID
  )
{
  EFI_ACPI_5_1_GIC_STRUCTURE             *GicC;
  EFI_ACPI_5_1_GIC_DISTRIBUTOR_STRUCTURE *GicD;
  EFI_ACPI_5_1_GIC_MSI_FRAME_STRUCTURE   *GicM;
  ARM_CORE_INFO                          *ArmCoreInfoTable;
  UINTN                                  CoreCount, CpuNum;
  EFI_STATUS                             Status;
  EFI_PHYSICAL_ADDRESS                   MpParkingBase;
  UINTN                                  MpParkingSize;

  Status = gBS->LocateProtocol (
               &gAmdMpCoreInfoProtocolGuid,
               NULL,
               (VOID **)&mAmdMpCoreInfoProtocol
               );
  ASSERT_EFI_ERROR (Status);

  // Get pointer to ARM core info table
  ArmCoreInfoTable = mAmdMpCoreInfoProtocol->GetArmCoreInfoTable (&CoreCount);
  ASSERT (ArmCoreInfoTable != NULL);

  // Make sure SoC's core count does not exceed what we want to build
  ASSERT (CoreCount <= NUM_CORES);
  ASSERT (CoreCount <= PcdGet32(PcdSocCoreCount));

  MpParkingSize = 0;
  MpParkingBase =  mAmdMpCoreInfoProtocol->GetMpParkingBase(&MpParkingSize);
  if (MpParkingBase && MpParkingSize < (CoreCount * SIZE_4KB)) {
    DEBUG ((EFI_D_ERROR, "MADT: Parking Protocol not supported.\n"));
    MpParkingBase = 0;
  }

  GicC = (EFI_ACPI_5_1_GIC_STRUCTURE *)&AcpiMadt.GicC[0];
  AcpiMadt.Header.Header.Length = sizeof (EFI_ACPI_5_1_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER);

  for (CpuNum = 0; CpuNum < CoreCount; ++CpuNum, ++GicC) {
    DEBUG ((EFI_D_ERROR, "MADT: Core[%d]: ClusterId = %d   CoreId = %d\n",
            CpuNum, ArmCoreInfoTable[CpuNum].ClusterId, ArmCoreInfoTable[CpuNum].CoreId));

    Status = BuildGicC (GicC, CpuNum,
                ArmCoreInfoTable[CpuNum].ClusterId,
                ArmCoreInfoTable[CpuNum].CoreId,
                MpParkingBase
                );
    ASSERT_EFI_ERROR (Status);

    AcpiMadt.Header.Header.Length += sizeof (EFI_ACPI_5_1_GIC_STRUCTURE);
  }

  GicD = (EFI_ACPI_5_1_GIC_DISTRIBUTOR_STRUCTURE *)(UINT8 *)((UINTN)&AcpiMadt + (UINTN)AcpiMadt.Header.Header.Length);
  BuildGicD (GicD, GICD_ID, GICD_VECTOR);
  AcpiMadt.Header.Header.Length += sizeof (EFI_ACPI_5_1_GIC_DISTRIBUTOR_STRUCTURE);

  GicM = (EFI_ACPI_5_1_GIC_MSI_FRAME_STRUCTURE *)(UINT8 *)((UINTN)&AcpiMadt + (UINTN)AcpiMadt.Header.Header.Length);
  BuildGicM (GicM, GICM_ID, GICM_SPI_COUNT, GICM_SPI_BASE);
  AcpiMadt.Header.Header.Length += sizeof (EFI_ACPI_5_1_GIC_MSI_FRAME_STRUCTURE);

  return &AcpiMadt.Header.Header;
}

