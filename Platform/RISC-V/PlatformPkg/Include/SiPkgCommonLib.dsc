## @file
#  RISC-V EFI on RiscvVirt RISC-V platform
#
#  Copyright (c) 2021, Hewlett Packard Enterprise Development LP. All rights reserved.<BR>
#
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

[LibraryClasses]
  FdtLib|EmbeddedPkg/Library/FdtLib/FdtLib.inf

# RISC-V Platform Library
  TimeBaseLib|EmbeddedPkg//Library/TimeBaseLib/TimeBaseLib.inf
  RealTimeClockLib|EmbeddedPkg//Library/VirtualRealTimeClockLib/VirtualRealTimeClockLib.inf

# RISC-V Core Library
  RiscVOpensbiLib|Silicon/RISC-V/ProcessorPkg/Library/RiscVOpensbiLib/RiscVOpensbiLib.inf

[LibraryClasses.common]
  RiscVCpuLib|Silicon/RISC-V/ProcessorPkg/Library/RiscVCpuLib/RiscVCpuLib.inf
  RiscVEdk2SbiLib|Silicon/RISC-V/ProcessorPkg/Library/RiscVEdk2SbiLib/RiscVEdk2SbiLib.inf
  RiscVPlatformTimerLib|Platform/SiFive/U5SeriesPkg/Library/RiscVPlatformTimerLib/RiscVPlatformTimerLib.inf
  CpuExceptionHandlerLib|Silicon/RISC-V/ProcessorPkg/Library/RiscVExceptionLib/CpuExceptionHandlerDxeLib.inf

  # Flattened Device Tree (FDT) access library
  FdtLib|EmbeddedPkg/Library/FdtLib/FdtLib.inf
