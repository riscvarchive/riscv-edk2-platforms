## @file
#  RISC-V EFI on RiscvVirt RISC-V platform
#
#  Copyright (c) 2021, Hewlett Packard Enterprise Development LP. All rights reserved.<BR>
#
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

[LibraryClasses.common.DXE_RUNTIME_DRIVER]
  ResetSystemLib|Platform/RISC-V/PlatformPkg/Library/ResetSystemLib/ResetSystemLib.inf

[LibraryClasses.common.DXE_CORE,LibraryClasses.common.DXE_RUNTIME_DRIVER,LibraryClasses.common.UEFI_DRIVER,LibraryClasses.common.DXE_DRIVER,LibraryClasses.common.UEFI_APPLICATION]
  TimerLib|Silicon/RISC-V/ProcessorPkg/Library/RiscVTimerLib/BaseRiscVTimerLib.inf
!ifdef $(SOURCE_DEBUG_ENABLE)
  DebugAgentLib|SourceLevelDebugPkg/Library/DebugAgent/DxeDebugAgentLib.inf
!endif

[LibraryClasses.common.DXE_DRIVER]
  PlatformBootManagerLib|Platform/RISC-V/PlatformPkg/Library/PlatformBootManagerLib/PlatformBootManagerLib.inf
  PlatformMemoryTestLib|Platform/RISC-V/PlatformPkg/Library/PlatformMemoryTestLibNull/PlatformMemoryTestLibNull.inf
  PlatformUpdateProgressLib|Platform/RISC-V/PlatformPkg/Library/PlatformUpdateProgressLibNull/PlatformUpdateProgressLibNull.inf
