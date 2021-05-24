/** @file
  Declarations required for SMBIOS DXE driver.

  Functions declarations and data type declarations required for SMBIOS DXE
  driver of the Arm Reference Design platforms.

  Copyright (c) 2021, ARM Limited. All rights reserved.
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef SMBIOS_PLATFORM_DXE_H_
#define SMBIOS_PLATFORM_DXE_H_

/**
  Install SMBIOS BIOS information Table.

  Install the SMBIOS BIOS information (type 0) table for Arm's reference design
  platforms.

  @param[in] Smbios   SMBIOS protocol.

  @retval EFI_SUCCESS           Record was added.
  @retval EFI_OUT_OF_RESOURCES  Record was not added.
  @retval EFI_ALREADY_STARTED   The SmbiosHandle passed is already in use.
**/
EFI_STATUS
EFIAPI
InstallType0BiosInformation (
  IN     EFI_SMBIOS_PROTOCOL    *Smbios
  );

#endif // SMBIOS_PLATFORM_DXE_H_
