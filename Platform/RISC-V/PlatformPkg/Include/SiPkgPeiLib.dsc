## @file
#  RISC-V EFI on RiscvVirt RISC-V platform
#
#  Copyright (c) 2021, Hewlett Packard Enterprise Development LP. All rights reserved.<BR>
#
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

[LibraryClasses.common.SEC]
#
# OpenSBI Platform Library
#
  RiscVOpensbiPlatformLib|Platform/RISC-V/PlatformPkg/Library/OpensbiPlatformLib/OpensbiPlatformLib.inf

[LibraryClasses.common.PEI_CORE]
  PeiServicesTablePointerLib|Silicon/RISC-V/ProcessorPkg/Library/PeiServicesTablePointerLibOpenSbi/PeiServicesTablePointerLibOpenSbi.inf

[LibraryClasses.common.PEIM]
  PeiServicesTablePointerLib|Silicon/RISC-V/ProcessorPkg/Library/PeiServicesTablePointerLibOpenSbi/PeiServicesTablePointerLibOpenSbi.inf

#
# RISC-V core libraries
#
  RiscVCoreplexInfoLib|Platform/RISC-V/PlatformPkg/Library/PeiCoreInfoHobLibNull/PeiCoreInfoHobLib.inf

