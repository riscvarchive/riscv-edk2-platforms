## @file
#  Component description file for the CN9130 Development Board (variant A)
#
#  Copyright (c) 2019 Marvell International Ltd.<BR>
#
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

################################################################################
#
# Defines Section - statements that will be processed to create a Makefile.
#
################################################################################
[Defines]
!if $(CN9130)
  PLATFORM_NAME                  = Cn9130DbA
!endif
  PLATFORM_GUID                  = 087305a1-8ddd-4027-89ca-68a3ef78fcc7
  PLATFORM_VERSION               = 0.1
  DSC_SPECIFICATION              = 0x0001001B
  OUTPUT_DIRECTORY               = Build/$(PLATFORM_NAME)-$(ARCH)
  SUPPORTED_ARCHITECTURES        = AARCH64|ARM
  BUILD_TARGETS                  = DEBUG|RELEASE|NOOPT
  SKUID_IDENTIFIER               = DEFAULT
  FLASH_DEFINITION               = Silicon/Marvell/Armada7k8k/Armada7k8k.fdf
  BOARD_DXE_FV_COMPONENTS        = Platform/Marvell/Cn913xDb/Cn913xDbA.fdf.inc

  #
  # Network definition
  #
  DEFINE NETWORK_IP6_ENABLE             = FALSE
  DEFINE NETWORK_TLS_ENABLE             = FALSE
  DEFINE NETWORK_HTTP_BOOT_ENABLE       = FALSE
  DEFINE NETWORK_ISCSI_ENABLE           = FALSE

!include Silicon/Marvell/Armada7k8k/Armada7k8k.dsc.inc
!include Platform/Marvell/Cn913xDb/Cn9130DbA.dsc.inc

[Components.common]
  Silicon/Marvell/OcteonTx/DeviceTree/T91/$(PLATFORM_NAME).inf

[Components.AARCH64]
  Silicon/Marvell/OcteonTx/AcpiTables/T91/$(PLATFORM_NAME).inf

[LibraryClasses.common]
  ArmadaBoardDescLib|Platform/Marvell/Cn913xDb/BoardDescriptionLib/Cn9130DbABoardDescLib.inf
  NonDiscoverableInitLib|Platform/Marvell/Cn913xDb/NonDiscoverableInitLib/NonDiscoverableInitLib.inf
