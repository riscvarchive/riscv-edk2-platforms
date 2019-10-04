## @file
#
# Copyright (c) 2019 Intel Corporation. All rights reserved. <BR>
#
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

################################################################################
#
# Pcd Section - list of all EDK II PCD Entries defined by this Platform
#
################################################################################
[PcdsFixedAtBuild]
  gMinPlatformPkgTokenSpaceGuid.PcdBootStage|4

[PcdsFeatureFlag.common]
  gMinPlatformPkgTokenSpaceGuid.PcdStopAfterDebugInit|FALSE
  gMinPlatformPkgTokenSpaceGuid.PcdStopAfterMemInit|FALSE
  gMinPlatformPkgTokenSpaceGuid.PcdBootToShellOnly|FALSE
  gMinPlatformPkgTokenSpaceGuid.PcdUefiSecureBootEnable|FALSE
  gMinPlatformPkgTokenSpaceGuid.PcdTpm2Enable|FALSE

!if gMinPlatformPkgTokenSpaceGuid.PcdBootStage >= 1
  gMinPlatformPkgTokenSpaceGuid.PcdStopAfterDebugInit|TRUE
!endif

!if gMinPlatformPkgTokenSpaceGuid.PcdBootStage >= 2
  gMinPlatformPkgTokenSpaceGuid.PcdStopAfterDebugInit|FALSE
  gMinPlatformPkgTokenSpaceGuid.PcdStopAfterMemInit|TRUE
!endif

!if gMinPlatformPkgTokenSpaceGuid.PcdBootStage >= 3
  gMinPlatformPkgTokenSpaceGuid.PcdStopAfterMemInit|FALSE
  gMinPlatformPkgTokenSpaceGuid.PcdBootToShellOnly|TRUE
!endif

!if gMinPlatformPkgTokenSpaceGuid.PcdBootStage >= 4
  gMinPlatformPkgTokenSpaceGuid.PcdBootToShellOnly|FALSE
!endif

!if gMinPlatformPkgTokenSpaceGuid.PcdBootStage >= 5
  gMinPlatformPkgTokenSpaceGuid.PcdUefiSecureBootEnable|TRUE
  gMinPlatformPkgTokenSpaceGuid.PcdTpm2Enable|TRUE
!endif

  !if $(TARGET) == DEBUG
    gMinPlatformPkgTokenSpaceGuid.PcdSmiHandlerProfileEnable|TRUE
  !else
    gMinPlatformPkgTokenSpaceGuid.PcdSmiHandlerProfileEnable|FALSE
  !endif

  gMinPlatformPkgTokenSpaceGuid.PcdPerformanceEnable|FALSE

  gAdvancedFeaturePkgTokenSpaceGuid.PcdNetworkEnable|TRUE
  gAdvancedFeaturePkgTokenSpaceGuid.PcdSmbiosEnable|TRUE

!if $(TARGET) == RELEASE
  gEfiMdeModulePkgTokenSpaceGuid.PcdStatusCodeUseSerial|FALSE
!else
  gEfiMdeModulePkgTokenSpaceGuid.PcdStatusCodeUseSerial|TRUE
!endif
  # Server doesn't support capsle update on Reset.
  gEfiMdeModulePkgTokenSpaceGuid.PcdSupportUpdateCapsuleReset|FALSE
  gUefiCpuPkgTokenSpaceGuid.PcdCpuSmmEnableBspElection|FALSE
  gUefiCpuPkgTokenSpaceGuid.PcdCpuHotPlugSupport|FALSE
  gEfiMdeModulePkgTokenSpaceGuid.PcdPciBusHotplugDeviceSupport|FALSE


#S3 add
  gEfiMdeModulePkgTokenSpaceGuid.PcdDxeIplSwitchToLongMode|TRUE
#S3 add

  ## This PCD specified whether ACPI SDT protocol is installed.
  gEfiMdeModulePkgTokenSpaceGuid.PcdInstallAcpiSdtProtocol|TRUE

[PcdsFeatureFlag.X64]
  gUefiCpuPkgTokenSpaceGuid.PcdCpuSmmStackGuard|FALSE

[PcdsFeatureFlag]
  gEfiMdeModulePkgTokenSpaceGuid.PcdBrowerGrayOutReadOnlyMenu|TRUE

[PcdsDynamicExDefault]

[PcdsFixedAtBuild.common]
  gEfiMdeModulePkgTokenSpaceGuid.PcdResetOnMemoryTypeInformationChange|TRUE
!if $(TARGET) == "RELEASE"
  gEfiMdePkgTokenSpaceGuid.PcdReportStatusCodePropertyMask|0x03
!else
  gEfiMdePkgTokenSpaceGuid.PcdReportStatusCodePropertyMask|0x07
!endif
  gEfiMdePkgTokenSpaceGuid.PcdReportStatusCodePropertyMask|0x07
  gEfiMdeModulePkgTokenSpaceGuid.PcdLoadModuleAtFixAddressEnable|0
  gEfiMdeModulePkgTokenSpaceGuid.PcdHwErrStorageSize|0x0
#S3 modified
  gEfiMdeModulePkgTokenSpaceGuid.PcdShadowPeimOnS3Boot|TRUE
#S3 modified

  gEfiMdeModulePkgTokenSpaceGuid.PcdHwErrStorageSize|0x0
  gEfiMdePkgTokenSpaceGuid.PcdPerformanceLibraryPropertyMask|0x0
  gEfiMdePkgTokenSpaceGuid.PcdFSBClock|133333333
  gEfiMdeModulePkgTokenSpaceGuid.PcdPeiCoreMaxPeiStackSize|0x100000
  gEfiMdeModulePkgTokenSpaceGuid.PcdMaxSizeNonPopulateCapsule|0x1700000

  gUefiCpuPkgTokenSpaceGuid.PcdCpuSmmApSyncTimeout|10000
  gUefiCpuPkgTokenSpaceGuid.PcdCpuMaxLogicalProcessorNumber|512
  gUefiCpuPkgTokenSpaceGuid.PcdCpuSmmStackSize|0x4000

  ## Specifies delay value in microseconds after sending out an INIT IPI.
  # @Prompt Configure delay value after send an INIT IPI
  gUefiCpuPkgTokenSpaceGuid.PcdCpuInitIpiDelayInMicroSeconds|10

  ## Specifies max supported number of Logical Processors.
  # @Prompt Configure max supported number of Logical Processorss
  gUefiCpuPkgTokenSpaceGuid.PcdCpuMaxLogicalProcessorNumber|512
  gUefiCpuPkgTokenSpaceGuid.PcdCpuApStackSize|0x1000
!if gMinPlatformPkgTokenSpaceGuid.PcdPerformanceEnable == TRUE
  gEfiMdePkgTokenSpaceGuid.PcdPerformanceLibraryPropertyMask|0x1
!endif

  ## Defines the ACPI register set base address.
  #  The invalid 0xFFFF is as its default value. It must be configured to the real value.
  # @Prompt ACPI Timer IO Port Address
  gPcAtChipsetPkgTokenSpaceGuid.PcdAcpiIoPortBaseAddress         |  0x0400

  ## Defines the PCI Bus Number of the PCI device that contains the BAR and Enable for ACPI hardware registers.
  # @Prompt ACPI Hardware PCI Bus Number
  gPcAtChipsetPkgTokenSpaceGuid.PcdAcpiIoPciBusNumber            |  0x00

  gEfiMdeModulePkgTokenSpaceGuid.PcdAcpiDefaultOemRevision|0x00000002
  gEfiMdeModulePkgTokenSpaceGuid.PcdAcpiDefaultCreatorId|0x4C544E49
  gEfiMdeModulePkgTokenSpaceGuid.PcdAcpiDefaultCreatorRevision|0x20091013

  ## Defines the PCI Device Number of the PCI device that contains the BAR and Enable for ACPI hardware registers.
  #  The invalid 0xFF is as its default value. It must be configured to the real value.
  # @Prompt ACPI Hardware PCI Device Number
  gPcAtChipsetPkgTokenSpaceGuid.PcdAcpiIoPciDeviceNumber         |  0x1F

  ## Defines the PCI Function Number of the PCI device that contains the BAR and Enable for ACPI hardware registers.
  #  The invalid 0xFF is as its default value. It must be configured to the real value.
  # @Prompt ACPI Hardware PCI Function Number
  gPcAtChipsetPkgTokenSpaceGuid.PcdAcpiIoPciFunctionNumber       |  0x00

  ## Defines the PCI Register Offset of the PCI device that contains the Enable for ACPI hardware registers.
  #  The invalid 0xFFFF is as its default value. It must be configured to the real value.
  # @Prompt ACPI Hardware PCI Register Offset
  gPcAtChipsetPkgTokenSpaceGuid.PcdAcpiIoPciEnableRegisterOffset |0x0044

  ## Defines the bit mask that must be set to enable the APIC hardware register BAR.
  # @Prompt ACPI Hardware PCI Bar Enable BitMask
  gPcAtChipsetPkgTokenSpaceGuid.PcdAcpiIoBarEnableMask           |  0x80

  ## Defines the PCI Register Offset of the PCI device that contains the BAR for ACPI hardware registers.
  #  The invalid 0xFFFF is as its default value. It must be configured to the real value.
  # @Prompt ACPI Hardware PCI Bar Register Offset
  gPcAtChipsetPkgTokenSpaceGuid.PcdAcpiIoPciBarRegisterOffset    |0x0040

  ## Defines the offset to the 32-bit Timer Value register that resides within the ACPI BAR.
  # @Prompt Offset to 32-bit Timer register in ACPI BAR
  gPcAtChipsetPkgTokenSpaceGuid.PcdAcpiPm1TmrOffset              |0x0008

  ## Defines the bit mask to retrieve ACPI IO Port Base Address
  # @Prompt ACPI IO Port Base Address Mask
  gPcAtChipsetPkgTokenSpaceGuid.PcdAcpiIoPortBaseAddressMask     |0xFFFC

  gEfiMdeModulePkgTokenSpaceGuid.PcdResetOnMemoryTypeInformationChange|FALSE

  gMinPlatformPkgTokenSpaceGuid.PcdMaxCpuThreadCount|4
  gMinPlatformPkgTokenSpaceGuid.PcdMaxCpuCoreCount|128
  gMinPlatformPkgTokenSpaceGuid.PcdMaxCpuSocketCount|4
  gMinPlatformPkgTokenSpaceGuid.PcdLocalApicAddress|0xFEE00000
  gMinPlatformPkgTokenSpaceGuid.PcdPcIoApicAddressBase|0xFEC01000
  gMinPlatformPkgTokenSpaceGuid.PcdFadtPreferredPmProfile|0x0
  gMinPlatformPkgTokenSpaceGuid.PcdFadtIaPcBootArch|0x0003
  gMinPlatformPkgTokenSpaceGuid.PcdFadtFlags|0x000004A5
  gMinPlatformPkgTokenSpaceGuid.PcdAcpiPm1AEventBlockAddress|0x400
  gMinPlatformPkgTokenSpaceGuid.PcdAcpiPm1BEventBlockAddress|0
  gMinPlatformPkgTokenSpaceGuid.PcdAcpiPm1AControlBlockAddress|0x404
  gMinPlatformPkgTokenSpaceGuid.PcdAcpiPm1BControlBlockAddress|0
  gMinPlatformPkgTokenSpaceGuid.PcdAcpiPm2ControlBlockAddress|0x450
  gMinPlatformPkgTokenSpaceGuid.PcdAcpiPmTimerBlockAddress|0x408
  gMinPlatformPkgTokenSpaceGuid.PcdAcpiGpe0BlockAddress|0x420
  gMinPlatformPkgTokenSpaceGuid.PcdAcpiGpe1BlockAddress|0

[PcdsFixedAtBuild.X64]
  gPcAtChipsetPkgTokenSpaceGuid.Pcd8259LegacyModeMask|0x0eB8
  gPcAtChipsetPkgTokenSpaceGuid.PcdMinimalValidYear|2015
  gPcAtChipsetPkgTokenSpaceGuid.PcdMaximalValidYear|2099
  # Change PcdBootManagerMenuFile to UiApp
##

  gEfiMdeModulePkgTokenSpaceGuid.PcdBootManagerMenuFile|{ 0x21, 0xaa, 0x2c, 0x46, 0x14, 0x76, 0x03, 0x45, 0x83, 0x6e, 0x8a, 0xb6, 0xf4, 0x66, 0x23, 0x31 }

  gUefiCpuPkgTokenSpaceGuid.PcdCpuSmmCodeAccessCheckEnable |TRUE

 [PcdsPatchableInModule.common]

!if gMinPlatformPkgTokenSpaceGuid.PcdSmiHandlerProfileEnable == TRUE
  gEfiMdeModulePkgTokenSpaceGuid.PcdSmiHandlerProfilePropertyMask|0x1
!endif

  gPcAtChipsetPkgTokenSpaceGuid.PcdHpetBaseAddress|0xFED00000

  gEfiMdeModulePkgTokenSpaceGuid.PcdVideoHorizontalResolution|1024
  gEfiMdeModulePkgTokenSpaceGuid.PcdVideoVerticalResolution|600

  gEfiMdeModulePkgTokenSpaceGuid.PcdUse1GPageTable|TRUE

  gMinPlatformPkgTokenSpaceGuid.PcdFlashFvMicrocodeBase|0x0
  gMinPlatformPkgTokenSpaceGuid.PcdFlashFvMicrocodeSize|0x0

[PcdsDynamicExDefault.common.DEFAULT]
  gUefiCpuPkgTokenSpaceGuid.PcdCpuApInitTimeOutInMicroSeconds|30000
  gUefiCpuPkgTokenSpaceGuid.PcdCpuS3DataAddress|0

[PcdsDynamicExHii.common.DEFAULT]
  gEfiMdePkgTokenSpaceGuid.PcdPlatformBootTimeOut|L"Timeout"|gEfiGlobalVariableGuid|0x0|50 # Variable: L"Timeout"
  gEfiMdePkgTokenSpaceGuid.PcdHardwareErrorRecordLevel|L"HwErrRecSupport"|gEfiGlobalVariableGuid|0x0|1 # Variable: L"HwErrRecSupport"


[PcdsDynamicExDefault]

  gEfiMdeModulePkgTokenSpaceGuid.PcdMrIovSupport|FALSE
  gEfiMdeModulePkgTokenSpaceGuid.PcdSrIovSystemPageSize|0x1F

  gEfiMdeModulePkgTokenSpaceGuid.PcdFirmwareVersionString|L""|VOID*|36

  gEfiMdeModulePkgTokenSpaceGuid.PcdAcpiDefaultOemId|{0x49, 0x4E, 0x54, 0x45, 0x4C, 0x20}
  gEfiMdeModulePkgTokenSpaceGuid.PcdAcpiDefaultOemTableId|0x2046573030363253

[PcdsDynamicExDefault.X64]

  gEfiMdePkgTokenSpaceGuid.PcdUartDefaultBaudRate|115200
  gEfiMdePkgTokenSpaceGuid.PcdUartDefaultDataBits|8
  gEfiMdePkgTokenSpaceGuid.PcdUartDefaultParity|1
  gEfiMdePkgTokenSpaceGuid.PcdUartDefaultStopBits|1
  gEfiMdePkgTokenSpaceGuid.PcdDefaultTerminalType|0

  gEfiMdeModulePkgTokenSpaceGuid.PcdConOutRow|31
  gEfiMdeModulePkgTokenSpaceGuid.PcdConOutColumn|100

  gEfiMdeModulePkgTokenSpaceGuid.PcdSetupVideoHorizontalResolution|800
  gEfiMdeModulePkgTokenSpaceGuid.PcdSetupVideoVerticalResolution|600

  gUefiCpuPkgTokenSpaceGuid.PcdCpuHotPlugDataAddress|0

[PcdsFeatureFlag]
  gEfiMdeModulePkgTokenSpaceGuid.PcdStatusCodeUseMemory|TRUE
  gEfiMdeModulePkgTokenSpaceGuid.PcdDxeIplSupportUefiDecompress|FALSE
  gEfiMdeModulePkgTokenSpaceGuid.PcdDxeIplSwitchToLongMode|TRUE
  gEfiMdeModulePkgTokenSpaceGuid.PcdConOutGopSupport|TRUE
  gEfiMdeModulePkgTokenSpaceGuid.PcdConOutUgaSupport|FALSE
  #gOptionRomPkgTokenSpaceGuid.PcdSupportGop|TRUE
  #gOptionRomPkgTokenSpaceGuid.PcdSupportUga|FALSE

!if gMinPlatformPkgTokenSpaceGuid.PcdBootToShellOnly == FALSE
  gSimicsX58PkgTokenSpaceGuid.PcdSmmSmramRequire|TRUE
!endif

[PcdsFixedAtBuild]
  gEfiMdeModulePkgTokenSpaceGuid.PcdStatusCodeMemorySize|1
  gEfiMdePkgTokenSpaceGuid.PcdMaximumGuidedExtractHandler|0x10
  gEfiMdeModulePkgTokenSpaceGuid.PcdMaxVariableSize|0x400
  gEfiMdeModulePkgTokenSpaceGuid.PcdMaxHardwareErrorVariableSize|0x8000
  gEfiMdeModulePkgTokenSpaceGuid.PcdVariableStoreSize|0xc000
  gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageVariableSize|0xc000
  gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageFtwWorkingSize|0x2000
  gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageFtwSpareSize|0x10000

  gEfiMdeModulePkgTokenSpaceGuid.PcdVpdBaseAddress|0x0

  # DEBUG_INIT      0x00000001  // Initialization
  # DEBUG_WARN      0x00000002  // Warnings
  # DEBUG_LOAD      0x00000004  // Load events
  # DEBUG_FS        0x00000008  // EFI File system
  # DEBUG_POOL      0x00000010  // Alloc & Free (pool)
  # DEBUG_PAGE      0x00000020  // Alloc & Free (page)
  # DEBUG_INFO      0x00000040  // Informational debug messages
  # DEBUG_DISPATCH  0x00000080  // PEI/DXE/SMM Dispatchers
  # DEBUG_VARIABLE  0x00000100  // Variable
  # DEBUG_BM        0x00000400  // Boot Manager
  # DEBUG_BLKIO     0x00001000  // BlkIo Driver
  # DEBUG_NET       0x00004000  // SNP Driver
  # DEBUG_UNDI      0x00010000  // UNDI Driver
  # DEBUG_LOADFILE  0x00020000  // LoadFile
  # DEBUG_EVENT     0x00080000  // Event messages
  # DEBUG_GCD       0x00100000  // Global Coherency Database changes
  # DEBUG_CACHE     0x00200000  // Memory range cachability changes
  # DEBUG_VERBOSE   0x00400000  // Detailed debug messages that may
  #                             // significantly impact boot performance
  # DEBUG_ERROR     0x80000000  // Error
  gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x8000004F
  gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x2F

  #
  # PCI feature overrides.
  #
  gEfiMdeModulePkgTokenSpaceGuid.PcdAriSupport|FALSE
  gEfiMdeModulePkgTokenSpaceGuid.PcdSrIovSupport|FALSE

################################################################################
#
# Pcd Dynamic Section - list of all EDK II PCD Entries defined by this Platform
#
################################################################################

[PcdsDynamicDefault]
  gEfiMdeModulePkgTokenSpaceGuid.PcdEmuVariableNvStoreReserved|0
  gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageVariableBase64|0

  gBoardModuleTokenSpaceGuid.PcdSimicsX58HostBridgePciDevId|0
  gBoardModuleTokenSpaceGuid.PcdPciIoBase|0x0
  gBoardModuleTokenSpaceGuid.PcdPciIoSize|0x0
  gBoardModuleTokenSpaceGuid.PcdPciMmio32Base|0x0
  gBoardModuleTokenSpaceGuid.PcdPciMmio32Size|0x0
  gBoardModuleTokenSpaceGuid.PcdPciMmio64Base|0x0
  gBoardModuleTokenSpaceGuid.PcdPciMmio64Size|0x800000000

  gAdvancedFeaturePkgTokenSpaceGuid.PcdSmbiosType0StringBiosVersion|"Ver.1.0.0"
  gAdvancedFeaturePkgTokenSpaceGuid.PcdSmbiosType1StringProductName|"QSP UEFI BIOS"
  gAdvancedFeaturePkgTokenSpaceGuid.PcdSmbiosType2StringProductName|"QSP UEFI BIOS"
  gAdvancedFeaturePkgTokenSpaceGuid.PcdSmbiosType0StringBiosReleaseDate|"2019-08-09"

  gEfiMdeModulePkgTokenSpaceGuid.PcdAcpiS3Enable|FALSE
