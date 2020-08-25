/** @file
*  This file is an ACPI driver for the Qemu SBSA platform.
*
*  Copyright (c) 2020, Linaro Ltd. All rights reserved.
*
*  SPDX-License-Identifier: BSD-2-Clause-Patent
*
**/
#include <IndustryStandard/Acpi.h>
#include <IndustryStandard/AcpiAml.h>
#include <IndustryStandard/SbsaQemuAcpi.h>
#include <Library/AcpiLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiLib.h>
#include <Protocol/AcpiTable.h>
#include <Protocol/FdtClient.h>
#include <libfdt.h>

/*
 * A function that walks through the Device Tree created
 * by Qemu and counts the number of CPUs present in it.
 */
STATIC
VOID
CountCpusFromFdt (
  VOID
)
{
  VOID           *DeviceTreeBase;
  INT32          Node, Prev;
  RETURN_STATUS  PcdStatus;
  INT32          CpuNode;
  INT32          CpuCount;

  DeviceTreeBase = (VOID *)(UINTN)PcdGet64 (PcdDeviceTreeBaseAddress);
  ASSERT (DeviceTreeBase != NULL);

  // Make sure we have a valid device tree blob
  ASSERT (fdt_check_header (DeviceTreeBase) == 0);

  CpuNode = fdt_path_offset (DeviceTreeBase, "/cpus");
  if (CpuNode <= 0) {
    DEBUG ((DEBUG_ERROR, "Unable to locate /cpus in device tree\n"));
    return;
  }

  CpuCount = 0;

  // Walk through /cpus node and count the number of subnodes.
  // The count of these subnodes corresponds to the number of
  // CPUs created by Qemu.
  Prev = fdt_first_subnode (DeviceTreeBase, CpuNode);
  while (1) {
    CpuCount++;
    Node = fdt_next_subnode (DeviceTreeBase, Prev);
    if (Node < 0) {
      break;
    }
    Prev = Node;
  }

  PcdStatus = PcdSet32S (PcdCoreCount, CpuCount);
  ASSERT_RETURN_ERROR (PcdStatus);
}

/*
 * A Function to Compute the ACPI Table Checksum
 */
VOID
AcpiPlatformChecksum (
  IN UINT8      *Buffer,
  IN UINTN      Size
  )
{
  UINTN ChecksumOffset;

  ChecksumOffset = OFFSET_OF (EFI_ACPI_DESCRIPTION_HEADER, Checksum);

  // Set checksum field to 0 since it is used as part of the calculation
  Buffer[ChecksumOffset] = 0;

  Buffer[ChecksumOffset] = CalculateCheckSum8(Buffer, Size);
}

/*
 * A function that add the MADT ACPI table.
  IN EFI_ACPI_COMMON_HEADER    *CurrentTable
 */
EFI_STATUS
AddMadtTable (
  IN EFI_ACPI_TABLE_PROTOCOL   *AcpiTable
  )
{
  EFI_STATUS            Status;
  UINTN                 TableHandle;
  UINT32                TableSize;
  EFI_PHYSICAL_ADDRESS  PageAddress;
  UINT8                 *New;
  UINT32                NumCores;

  // Initialize MADT ACPI Header
  EFI_ACPI_6_0_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER Header = {
     SBSAQEMU_ACPI_HEADER (EFI_ACPI_6_0_MULTIPLE_APIC_DESCRIPTION_TABLE_SIGNATURE,
                           EFI_ACPI_6_0_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER,
                           EFI_ACPI_6_0_MULTIPLE_APIC_DESCRIPTION_TABLE_REVISION),
      0, 0 };

  // Initialize GICC Structure
  EFI_ACPI_6_0_GIC_STRUCTURE Gicc = EFI_ACPI_6_0_GICC_STRUCTURE_INIT (
    0,                                     /* GicID */
    0,                                     /* AcpiCpuUid */
    0,                                     /* Mpidr */
    EFI_ACPI_6_0_GIC_ENABLED,              /* Flags */
    SBSAQEMU_MADT_GIC_PMU_IRQ,             /* PMU Irq */
    FixedPcdGet32 (PcdGicDistributorBase), /* PhysicalBaseAddress */
    SBSAQEMU_MADT_GIC_VBASE,               /* GicVBase */
    SBSAQEMU_MADT_GIC_HBASE,               /* GicHBase */
    25,                                    /* GsivId */
    0,                                     /* GicRBase */
    0                                      /* Efficiency */
    );

  // Initialize GIC Distributor Structure
  EFI_ACPI_6_0_GIC_DISTRIBUTOR_STRUCTURE Gicd =
    EFI_ACPI_6_0_GIC_DISTRIBUTOR_INIT (
      0,
      FixedPcdGet32 (PcdGicDistributorBase),
      0,
      3 /* GicVersion */
    );

 // Initialize GIC Redistributor Structure
  EFI_ACPI_6_0_GICR_STRUCTURE Gicr = SBSAQEMU_MADT_GICR_INIT();

  // Get CoreCount which was determined eariler after parsing device tree
  NumCores = PcdGet32 (PcdCoreCount);

  // Calculate the new table size based on the number of cores
  TableSize = sizeof (EFI_ACPI_6_0_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER) +
               (sizeof (EFI_ACPI_6_0_GIC_STRUCTURE) * NumCores) +
               sizeof (EFI_ACPI_6_0_GIC_DISTRIBUTOR_STRUCTURE) +
               sizeof (EFI_ACPI_6_0_GICR_STRUCTURE);

  Status = gBS->AllocatePages (
                  AllocateAnyPages,
                  EfiACPIReclaimMemory,
                  EFI_SIZE_TO_PAGES (TableSize),
                  &PageAddress
                  );
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to allocate pages for MADT table\n"));
    return EFI_OUT_OF_RESOURCES;
  }

  New = (UINT8 *)(UINTN) PageAddress;
  ZeroMem (New, TableSize);

  // Add the  ACPI Description table header
  CopyMem (New, &Header, sizeof (EFI_ACPI_6_0_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER));
  ((EFI_ACPI_DESCRIPTION_HEADER*) New)->Length = TableSize;
  New += sizeof (EFI_ACPI_6_0_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER);

  // Add new GICC structures for the Cores
  for (NumCores = 0; NumCores < PcdGet32 (PcdCoreCount); NumCores++) {
    EFI_ACPI_6_0_GIC_STRUCTURE *GiccPtr;

    CopyMem (New, &Gicc, sizeof (EFI_ACPI_6_0_GIC_STRUCTURE));
    GiccPtr = (EFI_ACPI_6_0_GIC_STRUCTURE *) New;
    GiccPtr->AcpiProcessorUid = NumCores;
    GiccPtr->MPIDR = NumCores;
    New += sizeof (EFI_ACPI_6_0_GIC_STRUCTURE);
  }

  // GIC Distributor Structure
  CopyMem (New, &Gicd, sizeof (EFI_ACPI_6_0_GIC_DISTRIBUTOR_STRUCTURE));
  New += sizeof (EFI_ACPI_6_0_GIC_DISTRIBUTOR_STRUCTURE);

  // GIC ReDistributor Structure
  CopyMem (New, &Gicr, sizeof (EFI_ACPI_6_0_GICR_STRUCTURE));
  New += sizeof (EFI_ACPI_6_0_GICR_STRUCTURE);

  AcpiPlatformChecksum ((UINT8*) PageAddress, TableSize);

  Status = AcpiTable->InstallAcpiTable (
                        AcpiTable,
                        (EFI_ACPI_COMMON_HEADER *)PageAddress,
                        TableSize,
                        &TableHandle
                        );
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to install MADT table\n"));
  }

  return Status;
}

/*
 * Function to calculate the PkgLength field in ACPI tables
 */
STATIC
UINT32
SetPkgLength (
  IN UINT8  *TablePtr,
  IN UINT32 Length
)
{
  UINT8  ByteCount;
  UINT8  *PkgLeadByte = TablePtr;

  if (Length < 64) {
    *TablePtr = Length;
    return 1;
  }

  // Set the LSB of Length in PkgLeadByte and advance Length
  *PkgLeadByte = Length & 0xF;
  Length = Length >> 4;

  while (Length) {
    TablePtr++;
    *TablePtr = (Length & 0xFF);
    Length = (Length >> 8);
  }

  // Calculate the number of bytes the Length field uses
  // and set the ByteCount field in PkgLeadByte.
  ByteCount = (TablePtr - PkgLeadByte) & 0xF;
  *PkgLeadByte |= (ByteCount << 6);

  return ByteCount + 1;
}

/*
 * A function that adds SSDT ACPI table.
 */
EFI_STATUS
AddSsdtTable (
  IN EFI_ACPI_TABLE_PROTOCOL   *AcpiTable
  )
{
  EFI_STATUS            Status;
  UINTN                 TableHandle;
  UINT32                TableSize;
  EFI_PHYSICAL_ADDRESS  PageAddress;
  UINT8                 *New;
  UINT32                CpuId;
  UINT32                Offset;
  UINT8                 ScopeOpName[] =  SBSAQEMU_ACPI_SCOPE_NAME;
  UINT32                NumCores = PcdGet32 (PcdCoreCount);

  EFI_ACPI_DESCRIPTION_HEADER Header =
    SBSAQEMU_ACPI_HEADER (
      EFI_ACPI_6_0_SECONDARY_SYSTEM_DESCRIPTION_TABLE_SIGNATURE,
      EFI_ACPI_DESCRIPTION_HEADER,
      EFI_ACPI_6_0_SECONDARY_SYSTEM_DESCRIPTION_TABLE_REVISION);

  SBSAQEMU_ACPI_CPU_DEVICE CpuDevice = {
    { AML_EXT_OP, AML_EXT_DEVICE_OP }, /* Device () */
    SBSAQEMU_ACPI_CPU_DEV_LEN,         /* Length */
    SBSAQEMU_ACPI_CPU_DEV_NAME,        /* Device Name "C000" */
    SBSAQEMU_ACPI_CPU_HID,             /* Name (HID, "ACPI0007") */
    SBSAQEMU_ACPI_CPU_UID,             /* Name (UID, 0) */
  };

  // Calculate the new table size based on the number of cores
  TableSize = sizeof (EFI_ACPI_DESCRIPTION_HEADER) +
              SBSAQEMU_ACPI_SCOPE_OP_MAX_LENGTH + sizeof (ScopeOpName) +
              (sizeof (CpuDevice) * NumCores);

  Status = gBS->AllocatePages (
                  AllocateAnyPages,
                  EfiACPIReclaimMemory,
                  EFI_SIZE_TO_PAGES (TableSize),
                  &PageAddress
                  );
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to allocate pages for SSDT table\n"));
    return EFI_OUT_OF_RESOURCES;
  }

  New = (UINT8 *)(UINTN) PageAddress;
  ZeroMem (New, TableSize);

  // Add the ACPI Description table header
  CopyMem (New, &Header, sizeof (EFI_ACPI_DESCRIPTION_HEADER));
  ((EFI_ACPI_DESCRIPTION_HEADER*) New)->Length = TableSize;
  New += sizeof (EFI_ACPI_DESCRIPTION_HEADER);

  // Insert the top level ScopeOp
  *New = AML_SCOPE_OP;
  New++;
  Offset = SetPkgLength (New,
             (TableSize - sizeof (EFI_ACPI_DESCRIPTION_HEADER) - 1));
  New += Offset;
  CopyMem (New, &ScopeOpName, sizeof (ScopeOpName));
  New += sizeof (ScopeOpName);

  // Add new Device structures for the Cores
  for (CpuId = 0; CpuId < NumCores; CpuId++) {
    SBSAQEMU_ACPI_CPU_DEVICE *CpuDevicePtr;
    UINT8 CpuIdByte1, CpuIdByte2, CpuIdByte3;

    CopyMem (New, &CpuDevice, sizeof (SBSAQEMU_ACPI_CPU_DEVICE));
    CpuDevicePtr = (SBSAQEMU_ACPI_CPU_DEVICE *) New;

    CpuIdByte1 = CpuId & 0xF;
    CpuIdByte2 = (CpuId >> 4) & 0xF;
    CpuIdByte3 = (CpuId >> 8) & 0xF;

    CpuDevicePtr->dev_name[1] = SBSAQEMU_ACPI_ITOA(CpuIdByte3);
    CpuDevicePtr->dev_name[2] = SBSAQEMU_ACPI_ITOA(CpuIdByte2);
    CpuDevicePtr->dev_name[3] = SBSAQEMU_ACPI_ITOA(CpuIdByte1);

    CpuDevicePtr->uid[6] = CpuIdByte1 | CpuIdByte2;
    CpuDevicePtr->uid[7] = CpuIdByte3;
    New += sizeof (SBSAQEMU_ACPI_CPU_DEVICE);
  }

  // Perform Checksum
  AcpiPlatformChecksum ((UINT8*) PageAddress, TableSize);

  Status = AcpiTable->InstallAcpiTable (
                        AcpiTable,
                        (EFI_ACPI_COMMON_HEADER *)PageAddress,
                        TableSize,
                        &TableHandle
                        );
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to install SSDT table\n"));
  }

  return Status;
}

EFI_STATUS
EFIAPI
InitializeSbsaQemuAcpiDxe (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS                     Status;
  EFI_ACPI_TABLE_PROTOCOL        *AcpiTable;

  // Parse the device tree and get the number of CPUs
  CountCpusFromFdt ();

  // Check if ACPI Table Protocol has been installed
  Status = gBS->LocateProtocol (
                  &gEfiAcpiTableProtocolGuid,
                  NULL,
                  (VOID **)&AcpiTable
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to locate ACPI Table Protocol\n"));
    return Status;
  }

  Status = AddMadtTable (AcpiTable);
  if (EFI_ERROR(Status)) {
     DEBUG ((DEBUG_ERROR, "Failed to add MADT table\n"));
  }

  Status = AddSsdtTable (AcpiTable);
  if (EFI_ERROR(Status)) {
     DEBUG ((DEBUG_ERROR, "Failed to add SSDT table\n"));
  }

  return EFI_SUCCESS;
}
