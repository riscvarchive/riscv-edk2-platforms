#  LS1043A.dsc
#  LS1043A Soc package.
#
#  Copyright 2017-2019 NXP
#
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#
#

################################################################################
#
# Pcd Section - list of all EDK II PCD Entries defined by this Platform
#
################################################################################
[PcdsDynamicDefault.common]

  #
  # ARM General Interrupt Controller
  gArmTokenSpaceGuid.PcdGicDistributorBase|0x01401000
  gArmTokenSpaceGuid.PcdGicInterruptInterfaceBase|0x01402000

[PcdsFixedAtBuild.common]

  #
  # CCSR Address Space and other attached Memories
  #
  gNxpQoriqLsTokenSpaceGuid.PcdCcsrBaseAddr|0x01000000
  gNxpQoriqLsTokenSpaceGuid.PcdCcsrSize|0x0F000000
  gNxpQoriqLsTokenSpaceGuid.PcdClkBaseAddr|0x01EE1000
  gNxpQoriqLsTokenSpaceGuid.PcdIfcRegion1BaseAddr|0x60000000
  gNxpQoriqLsTokenSpaceGuid.PcdIfcRegion1Size|0x20000000
  gNxpQoriqLsTokenSpaceGuid.PcdIfcRegion2BaseAddr|0x0620000000
  gNxpQoriqLsTokenSpaceGuid.PcdIfcRegion2Size|0x00E0000000
  gNxpQoriqLsTokenSpaceGuid.PcdIfcNandReservedSize|0x2EA
  gNxpQoriqLsTokenSpaceGuid.PcdQmanSwpBaseAddr|0x0500000000
  gNxpQoriqLsTokenSpaceGuid.PcdQmanSwpSize|0x0080000000
  gNxpQoriqLsTokenSpaceGuid.PcdBmanSwpBaseAddr|0x0508000000
  gNxpQoriqLsTokenSpaceGuid.PcdBmanSwpSize|0x0080000000
  gNxpQoriqLsTokenSpaceGuid.PcdPciExp1BaseAddr|0x4000000000
  gNxpQoriqLsTokenSpaceGuid.PcdPciExp1BaseSize|0x800000000
  gNxpQoriqLsTokenSpaceGuid.PcdPciExp2BaseAddr|0x4800000000
  gNxpQoriqLsTokenSpaceGuid.PcdPciExp2BaseSize|0x800000000
  gNxpQoriqLsTokenSpaceGuid.PcdPciExp3BaseAddr|0x5000000000
  gNxpQoriqLsTokenSpaceGuid.PcdPciExp3BaseSize|0x800000000
  gNxpQoriqLsTokenSpaceGuid.PcdScfgBaseAddr|0x1570000
  gNxpQoriqLsTokenSpaceGuid.PcdGutsBaseAddr|0x01EE0000
  gNxpQoriqLsTokenSpaceGuid.PcdWatchdog1BaseAddr|0x02AD0000
  gNxpQoriqLsTokenSpaceGuid.PcdSdxcBaseAddr|0x01560000
  gNxpQoriqLsTokenSpaceGuid.PcdI2c0BaseAddr|0x02180000
  gNxpQoriqLsTokenSpaceGuid.PcdI2cSize|0x10000
  gNxpQoriqLsTokenSpaceGuid.PcdNumI2cController|4
  gNxpQoriqLsTokenSpaceGuid.PcdQspiRegionBaseAddr|0x40000000
  gNxpQoriqLsTokenSpaceGuid.PcdQspiRegionSize|0x20000000

  #
  # Big Endian IPs
  #
  gNxpQoriqLsTokenSpaceGuid.PcdGurBigEndian|TRUE
  gNxpQoriqLsTokenSpaceGuid.PcdWatchdogBigEndian|TRUE

##
