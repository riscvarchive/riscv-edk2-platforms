#
#  Copyright (c) 2018, Linaro Limited. All rights reserved.
#
#  This program and the accompanying materials are licensed and made available
#  under the terms and conditions of the BSD License which accompanies this
#  distribution.  The full text of the license may be found at
#  http://opensource.org/licenses/bsd-license.php
#
#  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
#  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#

################################################################################
#
# Pcd Section - list of all EDK II PCD Entries defined by this Platform
#
################################################################################

[PcdsFeatureFlag]
  gEfiMdeModulePkgTokenSpaceGuid.PcdInstallAcpiSdtProtocol|TRUE

[PcdsFixedAtBuild.common]
  gEfiMdeModulePkgTokenSpaceGuid.PcdAcpiDefaultOemId|"SNI   "
  gEfiMdeModulePkgTokenSpaceGuid.PcdAcpiDefaultOemTableId|0x52434155514e5953 # SYNQUACR
  gEfiMdeModulePkgTokenSpaceGuid.PcdAcpiDefaultOemRevision|0x20180226
  gEfiMdeModulePkgTokenSpaceGuid.PcdAcpiDefaultCreatorId|0x4f524e4c # LNRO
  gEfiMdeModulePkgTokenSpaceGuid.PcdAcpiDefaultCreatorRevision|1

################################################################################
#
# Components Section - list of all EDK II Modules needed by this Platform
#
################################################################################

[Components.common]
  #
  # ACPI support
  #
  MdeModulePkg/Universal/Acpi/AcpiTableDxe/AcpiTableDxe.inf {
    <LibraryClasses>
      NULL|EmbeddedPkg/Library/PlatformHasAcpiLib/PlatformHasAcpiLib.inf

    <PcdsFixedAtBuild>
      # support ACPI v5.0 or later
      gEfiMdeModulePkgTokenSpaceGuid.PcdAcpiExposedTableVersions|0x20
  }
  MdeModulePkg/Universal/Acpi/AcpiPlatformDxe/AcpiPlatformDxe.inf
  Silicon/Socionext/SynQuacer/AcpiTables/AcpiTables.inf
