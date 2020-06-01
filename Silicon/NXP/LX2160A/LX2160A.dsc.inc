#  LX2160A.dsc
#  LX2160A Soc package.
#
#  Copyright 2018-2020 NXP
#
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#
#

!include Silicon/NXP/Chassis3V2/Chassis3V2.dsc.inc

[LibraryClasses.common]
  SocLib|Silicon/NXP/LX2160A/Library/SocLib/SocLib.inf

  PL011UartLib|ArmPlatformPkg/Library/PL011UartLib/PL011UartLib.inf
  SerialPortLib|ArmPlatformPkg/Library/PL011SerialPortLib/PL011SerialPortLib.inf
  PL011UartClockLib|Silicon/NXP/Library/PL011UartClockLib/PL011UartClockLib.inf

################################################################################
#
# Pcd Section - list of all EDK II PCD Entries defined by this Platform
#
################################################################################
[PcdsDynamicDefault.common]
  #
  # ARM General Interrupt Controller
  gArmTokenSpaceGuid.PcdGicDistributorBase|0x6000000
  gArmTokenSpaceGuid.PcdGicRedistributorsBase|0x6200000
  gArmTokenSpaceGuid.PcdGicInterruptInterfaceBase|0xC0C0000

[PcdsFixedAtBuild.common]
  gArmTokenSpaceGuid.PcdGenericWatchdogControlBase|0x23A0000
  gArmTokenSpaceGuid.PcdGenericWatchdogRefreshBase|0x2390000
  gArmTokenSpaceGuid.PcdGenericWatchdogEl2IntrNum|91

  gEfiMdePkgTokenSpaceGuid.PcdUartDefaultReceiveFifoDepth|0
  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialRegisterBase|0x21C0000

[PcdsFeatureFlag]
  gNxpQoriqLsTokenSpaceGuid.PcdI2cErratumA009203|TRUE

################################################################################
#
# Components Section - list of all EDK II Modules needed by this Platform
#
################################################################################
[Components.common]
  ArmPkg/Drivers/GenericWatchdogDxe/GenericWatchdogDxe.inf

##
