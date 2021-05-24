## @file
#  StandaloneMM platform description file for RD-N2 platforms.
#
#  Copyright (c) 2021, ARM Limited. All rights reserved.
#
#  SPDX-License-Identifier: BSD-2-Clause-Patent
##

################################################################################
#
# Defines Section - statements that will be processed to create a Makefile.
#
################################################################################
[Defines]
  PLATFORM_NAME                  = SgiMmStandalone
  PLATFORM_GUID                  = 67309f8a-d278-4df5-86ee-a1826cf481ed
  PLATFORM_VERSION               = 1.0
  DSC_SPECIFICATION              = 0x0001001B
  OUTPUT_DIRECTORY               = Build/$(PLATFORM_NAME)
  SUPPORTED_ARCHITECTURES        = AARCH64
  BUILD_TARGETS                  = DEBUG|RELEASE|NOOPT
  SKUID_IDENTIFIER               = DEFAULT
  FLASH_DEFINITION               = Platform/ARM/SgiPkg/PlatformStandaloneMm.fdf
  DEFINE DEBUG_MESSAGE           = TRUE

  # LzmaF86
  DEFINE COMPRESSION_TOOL_GUID   = D42AE6BD-1352-4bfb-909A-CA72A6EAE889

# include common definitions.
!include MdePkg/MdeLibs.dsc.inc
!include Platform/ARM/SgiPkg/SgiPlatformMm.dsc.inc

################################################################################
#
# Pcd Section - list of all EDK II PCD Entries defined by this Platform
#
################################################################################
[PcdsFixedAtBuild]
  ## PL011 - Serial Terminal
  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialRegisterBase|0x0EF80000
