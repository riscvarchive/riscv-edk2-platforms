## @file
#  Platform configuration file.
#
# Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
#
# This program and the accompanying materials are licensed and made available under
# the terms and conditions of the BSD License which accompanies this distribution.
# The full text of the license may be found at
# http://opensource.org/licenses/bsd-license.php
#
# THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
# WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#
##

  #
  # Please select BootStage here.
  # Stage 1 - enable debug (system deadloop after debug init)
  # Stage 2 - mem init (system deadloop after mem init)
  # Stage 3 - boot to shell only
  # Stage 4 - boot to OS
  # Stage 5 - boot to OS with security boot enabled
  #
  gMinPlatformModuleTokenSpaceGuid.PcdBootStage|4
  
  gMinPlatformModuleTokenSpaceGuid.PcdStopAfterDebugInit|FALSE
  gMinPlatformModuleTokenSpaceGuid.PcdStopAfterMemInit|FALSE
  gMinPlatformModuleTokenSpaceGuid.PcdBootToShellOnly|FALSE
  gMinPlatformModuleTokenSpaceGuid.PcdUefiSecureBootEnable|FALSE
  gMinPlatformModuleTokenSpaceGuid.PcdTpm2Enable|FALSE

!if gMinPlatformModuleTokenSpaceGuid.PcdBootStage >= 1
  gMinPlatformModuleTokenSpaceGuid.PcdStopAfterDebugInit|TRUE
!endif

!if gMinPlatformModuleTokenSpaceGuid.PcdBootStage >= 2
  gMinPlatformModuleTokenSpaceGuid.PcdStopAfterDebugInit|FALSE
  gMinPlatformModuleTokenSpaceGuid.PcdStopAfterMemInit|TRUE
!endif

!if gMinPlatformModuleTokenSpaceGuid.PcdBootStage >= 3
  gMinPlatformModuleTokenSpaceGuid.PcdStopAfterMemInit|FALSE
  gMinPlatformModuleTokenSpaceGuid.PcdBootToShellOnly|TRUE
!endif

!if gMinPlatformModuleTokenSpaceGuid.PcdBootStage >= 4
  gMinPlatformModuleTokenSpaceGuid.PcdBootToShellOnly|FALSE
!endif

!if gMinPlatformModuleTokenSpaceGuid.PcdBootStage >= 5
  gMinPlatformModuleTokenSpaceGuid.PcdUefiSecureBootEnable|TRUE
  gMinPlatformModuleTokenSpaceGuid.PcdTpm2Enable|TRUE
!endif
  
  #
  # More fine granularity control below:
  #
  
  
  gBoardModuleTokenSpaceGuid.PcdMultiBoardSupport|TRUE



#
# TRUE is ENABLE. FALSE is DISABLE.
#

#
# BIOS build switches configuration
#
  gSiPkgTokenSpaceGuid.PcdOptimizeCompilerEnable|TRUE

# CPU
  gSiPkgTokenSpaceGuid.PcdSourceDebugEnable|FALSE
  gSiPkgTokenSpaceGuid.PcdTxtEnable|TRUE  #Set to FALSE for GCC Build @todo Convert TXT ASM to NASM
  gSiPkgTokenSpaceGuid.PcdBiosGuardEnable|TRUE

# SA
  gSiPkgTokenSpaceGuid.PcdIgdEnable|TRUE
  gSiPkgTokenSpaceGuid.PcdPegEnable|TRUE
  gSiPkgTokenSpaceGuid.PcdSgEnable|TRUE
  gSiPkgTokenSpaceGuid.PcdSaDmiEnable|TRUE
  gSiPkgTokenSpaceGuid.PcdSkycamEnable|TRUE
  gSiPkgTokenSpaceGuid.PcdGmmEnable|TRUE
  gSiPkgTokenSpaceGuid.PcdSaOcEnable|TRUE
  gSiPkgTokenSpaceGuid.PcdVtdEnable|TRUE
  gSiPkgTokenSpaceGuid.PcdPeiDisplayEnable|TRUE

# ME
  gSiPkgTokenSpaceGuid.PcdAmtEnable|TRUE
  gSiPkgTokenSpaceGuid.PcdAtaEnable|TRUE
  gSiPkgTokenSpaceGuid.PcdPttEnable|TRUE
  gSiPkgTokenSpaceGuid.PcdJhiEnable|TRUE

  gSiPkgTokenSpaceGuid.PcdAcpiEnable|TRUE
  gSiPkgTokenSpaceGuid.PcdBdatEnable|TRUE
  gSiPkgTokenSpaceGuid.PcdBootGuardEnable|TRUE
  gSiPkgTokenSpaceGuid.PcdIntegratedTouchEnable|TRUE
  gSiPkgTokenSpaceGuid.PcdCpuPowerOnConfigEnable|TRUE
  gSiPkgTokenSpaceGuid.PcdSiCsmEnable|FALSE
  gSiPkgTokenSpaceGuid.PcdEvLoaderEnable|FALSE
  gSiPkgTokenSpaceGuid.PcdTraceHubEnable|TRUE
  gSiPkgTokenSpaceGuid.PcdOverclockEnable|TRUE
  gSiPkgTokenSpaceGuid.PcdPpmEnable|TRUE
  gSiPkgTokenSpaceGuid.PcdS3Enable|TRUE
  gSiPkgTokenSpaceGuid.PcdSerialGpioEnable|TRUE
  gSiPkgTokenSpaceGuid.PcdSmbiosEnable|TRUE
  gSiPkgTokenSpaceGuid.PcdSmmVariableEnable|TRUE
  gSiPkgTokenSpaceGuid.PcdSoftwareGuardEnable|TRUE
  gSiPkgTokenSpaceGuid.PcdSsaFlagEnable|FALSE
  gSiPkgTokenSpaceGuid.PcdUseHpetTimer|TRUE                       # TRUE - HPET / FALSE - 8254 timer is used.
  gSiPkgTokenSpaceGuid.PcdOcWdtEnable|TRUE
  gSiPkgTokenSpaceGuid.PcdSiCatalogDebugEnable|FALSE

#
# Override some PCDs for specific build requirements.
#
  #
  # Disable USB debug message when Source Level Debug is enabled
  # because they cannot be enabled at the same time.
  #

    gSiPkgTokenSpaceGuid.PcdPttEnable|FALSE
    gSiPkgTokenSpaceGuid.PcdTxtEnable|FALSE
    gSiPkgTokenSpaceGuid.PcdTxtEnable|FALSE



  !if $(TARGET) == DEBUG
    gSiPkgTokenSpaceGuid.PcdOptimizeCompilerEnable|TRUE
  !else
    gSiPkgTokenSpaceGuid.PcdOptimizeCompilerEnable|TRUE
  !endif

  !if $(TARGET) == DEBUG
    gMinPlatformModuleTokenSpaceGuid.PcdSmiHandlerProfileEnable|TRUE
  !else
    gMinPlatformModuleTokenSpaceGuid.PcdSmiHandlerProfileEnable|FALSE
  !endif

    gMinPlatformModuleTokenSpaceGuid.PcdPerformanceEnable|FALSE
