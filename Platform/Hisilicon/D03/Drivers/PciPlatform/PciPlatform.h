/** @file
*
*  Copyright (c) 2016, Hisilicon Limited. All rights reserved.
*  Copyright (c) 2016, Linaro Limited. All rights reserved.
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

#ifndef PCI_PLATFORM_H_
#define PCI_PLATFORM_H_

#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Protocol/PciPlatform.h>


//
// Global variables for Option ROMs
//
#define NULL_ROM_FILE_GUID \
  { \
    0x00000000, 0x0000, 0x0000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 \
  }


typedef struct {
  EFI_GUID  FileName;
  UINTN     Segment;
  UINTN     Bus;
  UINTN     Device;
  UINTN     Function;
  UINT16    VendorId;
  UINT16    DeviceId;
} PCI_OPTION_ROM_TABLE;

#define INVALID                     0xBD


typedef struct {
  EFI_HANDLE                      PciPlatformHandle;
  EFI_PCI_PLATFORM_PROTOCOL       PciPlatform;
} PCI_PLATFORM_PRIVATE_DATA;



extern PCI_PLATFORM_PRIVATE_DATA  mPciPrivateData;

EFI_STATUS
EFIAPI
PhaseNotify (
  IN EFI_PCI_PLATFORM_PROTOCOL                        *This,
  IN EFI_HANDLE                                       HostBridge,
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PHASE    Phase,
  IN EFI_PCI_CHIPSET_EXECUTION_PHASE                  ChipsetPhase
  )
/*++

Routine Description:

  Perform initialization by the phase indicated.

Arguments:

  This          -  Pointer to the EFI_PCI_PLATFORM_PROTOCOL instance.
  HostBridge    -  The associated PCI host bridge handle.
  Phase         -  The phase of the PCI controller enumeration.
  ChipsetPhase  -  Defines the execution phase of the PCI chipset driver.

Returns:

  EFI_SUCCESS  -  Must return with success.

--*/
;

EFI_STATUS
EFIAPI
PlatformPrepController (
  IN  EFI_PCI_PLATFORM_PROTOCOL                      *This,
  IN  EFI_HANDLE                                     HostBridge,
  IN  EFI_HANDLE                                     RootBridge,
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_PCI_ADDRESS    PciAddress,
  IN  EFI_PCI_CONTROLLER_RESOURCE_ALLOCATION_PHASE   Phase,
  IN  EFI_PCI_CHIPSET_EXECUTION_PHASE                ChipsetPhase
  )
/*++

Routine Description:

  The PlatformPrepController() function can be used to notify the platform driver so that
  it can perform platform-specific actions. No specific actions are required.
  Several notification points are defined at this time. More synchronization points may be
  added as required in the future. The PCI bus driver calls the platform driver twice for
  every PCI controller-once before the PCI Host Bridge Resource Allocation Protocol driver
  is notified, and once after the PCI Host Bridge Resource Allocation Protocol driver has
  been notified.
  This member function may not perform any error checking on the input parameters. It also
  does not return any error codes. If this member function detects any error condition, it
  needs to handle those errors on its own because there is no way to surface any errors to
  the caller.

Arguments:

  This          -  Pointer to the EFI_PCI_PLATFORM_PROTOCOL instance.
  HostBridge    -  The associated PCI host bridge handle.
  RootBridge    -  The associated PCI root bridge handle.
  PciAddress    -  The address of the PCI device on the PCI bus.
  Phase         -  The phase of the PCI controller enumeration.
  ChipsetPhase  -  Defines the execution phase of the PCI chipset driver.

Returns:

  EFI_SUCCESS  -  The function completed successfully.

--*/
;

EFI_STATUS
EFIAPI
GetPlatformPolicy (
  IN  EFI_PCI_PLATFORM_PROTOCOL            *This,
  OUT EFI_PCI_PLATFORM_POLICY              *PciPolicy
  )
/*++

Routine Description:

  Set the PciPolicy as EFI_RESERVE_ISA_IO_NO_ALIAS | EFI_RESERVE_VGA_IO_NO_ALIAS.

Arguments:

  This       -  The pointer to the Protocol itself.
  PciPolicy  -  the returned Policy.

Returns:

  EFI_UNSUPPORTED        -  Function not supported.
  EFI_INVALID_PARAMETER  -  Invalid PciPolicy value.

--*/
;

EFI_STATUS
EFIAPI
GetPciRom (
  IN   EFI_PCI_PLATFORM_PROTOCOL            *This,
  IN   EFI_HANDLE                           PciHandle,
  OUT  VOID                                 **RomImage,
  OUT  UINTN                                *RomSize
  )
/*++

Routine Description:

  Return a PCI ROM image for the onboard device represented by PciHandle.

Arguments:

  This       -  Protocol instance pointer.
  PciHandle  -  PCI device to return the ROM image for.
  RomImage   -  PCI Rom Image for onboard device.
  RomSize    -  Size of RomImage in bytes.

Returns:

  EFI_SUCCESS    -  RomImage is valid.
  EFI_NOT_FOUND  -  No RomImage.

--*/
;

#endif
