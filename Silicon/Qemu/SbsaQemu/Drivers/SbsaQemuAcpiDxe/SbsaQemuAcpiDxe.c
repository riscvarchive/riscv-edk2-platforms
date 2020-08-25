/** @file
*  This file is an ACPI driver for the Qemu SBSA platform.
*
*  Copyright (c) 2020, Linaro Ltd. All rights reserved.
*
*  SPDX-License-Identifier: BSD-2-Clause-Patent
*
**/
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiLib.h>
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

EFI_STATUS
EFIAPI
InitializeSbsaQemuAcpiDxe (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  // Parse the device tree and get the number of CPUs
  CountCpusFromFdt ();

  return EFI_SUCCESS;
}
