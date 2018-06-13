/** @file
*
*  Copyright (c) 2018, Hisilicon Limited. All rights reserved.
*  Copyright (c) 2018, Linaro Limited. All rights reserved.
*
*  This program and the accompanying materials
*  are licensed and made available under the terms and conditions of the BSD License
*  which accompanies this distribution.  The full text of the license may be found at
*  http://opensource.org/licenses/bsd-license.php
*
*  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
*  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
*
**/

#include <Uefi.h>
#include <Protocol/PciPlatform.h>


/*++

Routine Description:

  Perform Platform initialization first in PciPlatform.

Arguments:

Returns:

 VOID.

--*/
VOID
EFIAPI
PciInitPlatform (
  VOID
  )
{
  return;
}

/*++

Routine Description:

  Perform Platform initialization by the phase indicated.

Arguments:

  HostBridge    -  The associated PCI host bridge handle.
  Phase         -  The phase of the PCI controller enumeration.
  ChipsetPhase  -  Defines the execution phase of the PCI chipset driver.

Returns:

--*/
VOID
EFIAPI
PhaseNotifyPlatform (
  IN  EFI_HANDLE                                     HostBridge,
  IN  EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PHASE  Phase,
  IN  EFI_PCI_CHIPSET_EXECUTION_PHASE                ChipsetPhase
  )
{
  return;
}

