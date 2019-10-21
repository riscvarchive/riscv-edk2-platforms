## @file
# Build description file for a temporary build workaround.
#
# The feature enable PCD for advanced features must be referenced in an INF
# to be referenced in DSC/FDF files. This DSC only exists in the build to
# allow the PCDs to be referenced. This workaround does not affect the final
# flash image or boot in any way.
#
# The request to update BaseTools to allow a PCD to be referenced in DSC/FDF
# files without requiring the PCD to be referenced in an INF file is tracked
# here: https://bugzilla.tianocore.org/show_bug.cgi?id=2270
#
# When the BaseTools update is complete, this file can entirely be removed
# from this package.
#
# Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
#
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

#
# BEGIN:Temporary Build Workaround (resolution: https://bugzilla.tianocore.org/show_bug.cgi?id=2270)
#

# THIS FILE IS TEMPORARY. PLEASE TRY TO LOOK PAST THE "HACKS" ASSOCIATED WITH IT.
#
# With the BaseTools change requested, the changes needed to move to the end state are simply:
#  1. Remove the !include for this file in AdvancedFeatures.dsc
#  2. Remove this directory

!if $(PLATFORM_NAME) != AdvancedFeaturePkg
#
# AdvancedFeaturePkg initializes all FeaturePCDs to TRUE so they can conveniently be built in one package.
# Board packages will normally only enable (and therefore reference) a small subset of advanced features
# relative to the board. If an INF does not reference a package DEC file (which will be the case if the
# feature is not enabled) then the DSC must set ("define") the PCD for the conditional statements based
# on the PCD to work.
#
# AdvancedFeaturePkg has no problem as it naturally has a need to set al PCDs to TRUE for build.
# The section below sets all PCDs to FALSE in the DSC file so if the feature is not enabled by a board,
# the build will still be successful.
#
[PcdsFeatureFlag]
  gAcpiDebugFeaturePkgTokenSpaceGuid.PcdAcpiDebugFeatureEnable            |FALSE
  gIpmiFeaturePkgTokenSpaceGuid.PcdIpmiFeatureEnable                      |FALSE
  gNetworkFeaturePkgTokenSpaceGuid.PcdNetworkFeatureEnable                |FALSE
  gS3FeaturePkgTokenSpaceGuid.PcdS3FeatureEnable                          |FALSE
  gSmbiosFeaturePkgTokenSpaceGuid.PcdSmbiosFeatureEnable                  |FALSE
  gUsb3DebugFeaturePkgTokenSpaceGuid.PcdUsb3DebugFeatureEnable            |FALSE
  gUserAuthFeaturePkgTokenSpaceGuid.PcdUserAuthenticationFeatureEnable    |FALSE
!endif

#
# The LibraryClasses required to build TemporaryBuildWorkaround.inf
# (mostly libraries requiring other libraries)
#
[LibraryClasses]
  BaseLib|MdePkg/Library/BaseLib/BaseLib.inf
  BaseMemoryLib|MdePkg/Library/BaseMemoryLibRepStr/BaseMemoryLibRepStr.inf
  DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  PeimEntryPoint|MdePkg/Library/PeimEntryPoint/PeimEntryPoint.inf
  PeiServicesLib|MdePkg/Library/PeiServicesLib/PeiServicesLib.inf
  PeiServicesTablePointerLib|MdePkg/Library/PeiServicesTablePointerLibIdt/PeiServicesTablePointerLibIdt.inf

#
# The driver that references all feature PCDs to satsify current build limitations
#
[Components]
  AdvancedFeaturePkg/TemporaryBuildWorkaround/TemporaryBuildWorkaround.inf
#
# END:Temporary Build Workaround (resolution: https://bugzilla.tianocore.org/show_bug.cgi?id=2270)
#
