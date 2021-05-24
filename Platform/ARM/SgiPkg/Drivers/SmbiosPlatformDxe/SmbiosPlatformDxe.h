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

/**
  Install SMBIOS System information Table.

  Install the SMBIOS system information (type 1) table for Arm's reference
  design platforms.

  @param[in]  Smbios   SMBIOS protocol.

  @retval EFI_SUCCESS           Record was added.
  @retval EFI_NOT_FOUND         Unknown product id.
  @retval EFI_OUT_OF_RESOURCES  Record was not added.
  @retval EFI_ALREADY_STARTED   The SmbiosHandle passed is already in use.
**/
EFI_STATUS
EFIAPI
InstallType1SystemInformation (
  IN     EFI_SMBIOS_PROTOCOL    *Smbios
  );

/**
  Install SMBIOS System Enclosure Table

  Install the SMBIOS System Enclosure (type 3) table for Arm's Reference Design
  platforms.

  @param[in]  Smbios   SMBIOS protocol.

  @retval EFI_SUCCESS           Record was added.
  @retval EFI_OUT_OF_RESOURCES  Record was not added.
  @retval EFI_ALREADY_STARTED   The SmbiosHandle passed is already in use.
**/
EFI_STATUS
EFIAPI
InstallType3SystemEnclosure (
  IN     EFI_SMBIOS_PROTOCOL    *Smbios
  );

typedef enum {
  SMBIOS_HANDLE_ENCLOSURE = 0x1000,
} SMBIOS_REFRENCE_HANDLES;

#endif // SMBIOS_PLATFORM_DXE_H_
