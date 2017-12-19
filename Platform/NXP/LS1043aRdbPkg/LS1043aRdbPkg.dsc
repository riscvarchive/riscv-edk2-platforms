#  LS1043aRdbPkg.dsc
#
#  LS1043ARDB Board package.
#
#  Copyright 2017-2019 NXP
#
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#

################################################################################
#
# Defines Section - statements that will be processed to create a Makefile.
#
################################################################################
[Defines]
  #
  # Defines for default states.  These can be changed on the command line.
  # -D FLAG=VALUE
  #
  PLATFORM_NAME                  = LS1043aRdbPkg
  PLATFORM_GUID                  = 60169ec4-d2b4-44f8-825e-f8684fd42e4f
  OUTPUT_DIRECTORY               = Build/LS1043aRdbPkg
  FLASH_DEFINITION               = Platform/NXP/LS1043aRdbPkg/LS1043aRdbPkg.fdf

!include Platform/NXP/NxpQoriqLs.dsc.inc
!include Silicon/NXP/LS1043A/LS1043A.dsc.inc

[LibraryClasses.common]
  SocLib|Silicon/NXP/Library/SocLib/LS1043aSocLib.inf
  ArmPlatformLib|Platform/NXP/LS1043aRdbPkg/Library/PlatformLib/ArmPlatformLib.inf
  ResetSystemLib|ArmPkg/Library/ArmSmcPsciResetSystemLib/ArmSmcPsciResetSystemLib.inf
  SerialPortLib|Silicon/NXP/Library/DUartPortLib/DUartPortLib.inf
  IoAccessLib|Silicon/NXP/Library/IoAccessLib/IoAccessLib.inf
  RealTimeClockLib|Silicon/Maxim/Library/Ds1307RtcLib/Ds1307RtcLib.inf

[PcdsFixedAtBuild.common]

  #
  # LS1043a board Specific PCDs
  # XX (DRAM - Region 1 2GB)
  # (NOR - IFC Region 1 512MB)
  gArmTokenSpaceGuid.PcdSystemMemoryBase|0x80000000
  gArmTokenSpaceGuid.PcdSystemMemorySize|0x7BE00000
  gArmPlatformTokenSpaceGuid.PcdSystemMemoryUefiRegionSize|0x02000000

  #
  # Board Specific Pcds
  #
  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialRegisterBase|0x021c0500
  gNxpQoriqLsTokenSpaceGuid.PcdSerdes2Enabled|FALSE
  gNxpQoriqLsTokenSpaceGuid.PcdPlatformFreqDiv|0x1

  #
  # RTC Pcds
  #
  gDs1307RtcLibTokenSpaceGuid.PcdI2cSlaveAddress|0x68
  gDs1307RtcLibTokenSpaceGuid.PcdI2cBusFrequency|100000

################################################################################
#
# Components Section - list of all EDK II Modules needed by this Platform
#
################################################################################
[Components.common]
  #
  # Architectural Protocols
  #
  MdeModulePkg/Universal/Variable/RuntimeDxe/VariableRuntimeDxe.inf {
    <PcdsFixedAtBuild>
    gEfiMdeModulePkgTokenSpaceGuid.PcdEmuVariableNvModeEnable|TRUE
  }

  MdeModulePkg/Universal/WatchdogTimerDxe/WatchdogTimer.inf
  Silicon/NXP/Drivers/I2cDxe/I2cDxe.inf
  Platform/NXP/LS1043aRdbPkg/Drivers/PlatformDxe/PlatformDxe.inf

 ##
