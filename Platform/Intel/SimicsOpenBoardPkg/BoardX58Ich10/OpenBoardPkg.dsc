## @file
#
# Copyright (c) 2019 Intel Corporation. All rights reserved. <BR>
#
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

################################################################################
#
# Defines Section - statements that will be processed to create a Makefile.
#
################################################################################
[Defines]
  DEFINE PLATFORM_PACKAGE      = MinPlatformPkg
  DEFINE BOARD_NAME            = BoardX58Ich10
  DEFINE BOARD_PKG             = SimicsOpenBoardPkg
  DEFINE SKT_PKG               = SimicsX58SktPkg
  DEFINE PCH_PKG               = SimicsIch10Pkg
  DEFINE DXE_ARCH              = X64
  DEFINE PEI_ARCH              = IA32

  PLATFORM_NAME                  = SimicsX58
  PLATFORM_GUID                  = EE8EBB5A-CC95-412f-9987-2AF70F88B69A
  PLATFORM_VERSION               = 0.1
  DSC_SPECIFICATION              = 0x00010005
  OUTPUT_DIRECTORY               = Build/SimicsX58Ia32X64
  SUPPORTED_ARCHITECTURES        = IA32|X64
  BUILD_TARGETS                  = DEBUG|RELEASE|NOOPT
  SKUID_IDENTIFIER               = DEFAULT
  FLASH_DEFINITION               = $(BOARD_PKG)/$(BOARD_NAME)/OpenBoardPkg.fdf

  DEFINE SMM_REQUIRE             = TRUE

  #
  #PLATFORMX64_ENABLE is set to TRUE when PEI is IA32 and DXE is X64 platform
  #
  DEFINE PLATFORMX64_ENABLE             = TRUE
  DEFINE NETWORK_TLS_ENABLE             = FALSE
  DEFINE NETWORK_ISCSI_ENABLE           = FALSE
  DEFINE NETWORK_ALLOW_HTTP_CONNECTIONS = TRUE
  !include NetworkPkg/NetworkDefines.dsc.inc
################################################################################
#
# SKU Identification section - list of all SKU IDs supported by this Platform.
#
################################################################################
[SkuIds]
  0|DEFAULT

################################################################################
#
# Library Class section - list of all Library Classes needed by this Platform.
#
################################################################################

[PcdsFeatureFlag]
  #
  # Platform On/Off features are defined here
  #
  !include $(BOARD_PKG)/$(BOARD_NAME)/OpenBoardPkgConfig.dsc
  !include MinPlatformPkg/Include/Dsc/CoreCommonLib.dsc
  !include $(PCH_PKG)/IchCommonLib.dsc

[LibraryClasses]
  ReportFvLib|$(BOARD_PKG)/Library/PeiReportFvLib/PeiReportFvLib.inf
  BootLogoLib|MdeModulePkg/Library/BootLogoLib/BootLogoLib.inf
  SerialPortLib|PcAtChipsetPkg/Library/SerialIoLib/SerialIoLib.inf
  NvVarsFileLib|$(BOARD_PKG)/Library/NvVarsFileLib/NvVarsFileLib.inf
  SerializeVariablesLib|$(BOARD_PKG)/Library/SerializeVariablesLib/SerializeVariablesLib.inf
  DxeLoadLinuxLib|$(BOARD_PKG)/Library/LoadLinuxLib/DxeLoadLinuxLib.inf
  CpuExceptionHandlerLib|MdeModulePkg/Library/CpuExceptionHandlerLibNull/CpuExceptionHandlerLibNull.inf

  TestPointCheckLib|MinPlatformPkg/Test/Library/TestPointCheckLibNull/TestPointCheckLibNull.inf
  BoardInitLib|MinPlatformPkg/PlatformInit/Library/BoardInitLibNull/BoardInitLibNull.inf
  SiliconPolicyInitLib|$(BOARD_PKG)/Policy/Library/SiliconPolicyInitLib/SiliconPolicyInitLib.inf
  SiliconPolicyUpdateLib|$(BOARD_PKG)/Policy/Library/SiliconPolicyUpdateLib/SiliconPolicyUpdateLib.inf
  PciSegmentInfoLib|MinPlatformPkg/Pci/Library/PciSegmentInfoLibSimple/PciSegmentInfoLibSimple.inf

  !include MinPlatformPkg/Include/Dsc/CorePeiLib.dsc

  S3BootScriptLib|MdeModulePkg/Library/PiDxeS3BootScriptLib/DxeS3BootScriptLib.inf
  AslUpdateLib|MinPlatformPkg/Acpi/Library/DxeAslUpdateLib/DxeAslUpdateLib.inf
  LogoLib|$(BOARD_PKG)/Library/DxeLogoLib/DxeLogoLib.inf
[LibraryClasses.common.SEC]
  ExtractGuidedSectionLib|MdePkg/Library/BaseExtractGuidedSectionLib/BaseExtractGuidedSectionLib.inf

[LibraryClasses.common.PEI_CORE]

[LibraryClasses.common.PEIM]
  PeiResourcePublicationLib|MdePkg/Library/PeiResourcePublicationLib/PeiResourcePublicationLib.inf
  MpInitLib|UefiCpuPkg/Library/MpInitLib/PeiMpInitLib.inf

[LibraryClasses.IA32]
!if $(TARGET) == DEBUG
  TestPointCheckLib|MinPlatformPkg/Test/Library/TestPointCheckLib/PeiTestPointCheckLib.inf
!endif
  TestPointLib|MinPlatformPkg/Test/Library/TestPointLib/PeiTestPointLib.inf

  !include MinPlatformPkg/Include/Dsc/CoreDxeLib.dsc

[LibraryClasses.common.DXE_DRIVER]
  PlatformBootManagerLib|$(BOARD_PKG)/Library/PlatformBootManagerLib/PlatformBootManagerLib.inf

[LibraryClasses.common.DXE_SMM_DRIVER]
  SpiFlashCommonLib|$(PCH_PKG)/Library/SmmSpiFlashCommonLib/SmmSpiFlashCommonLib.inf

  !include $(BOARD_PKG)/$(BOARD_NAME)/OpenBoardPkgPcd.dsc

[Components.IA32]
  $(BOARD_PKG)/SecCore/SecMain.inf {
    <LibraryClasses>
      NULL|MdeModulePkg/Library/LzmaCustomDecompressLib/LzmaCustomDecompressLib.inf
  }
  !include $(SKT_PKG)/SktPkgPei.dsc
  !include MinPlatformPkg/Include/Dsc/CorePeiInclude.dsc

  $(BOARD_PKG)/SimicsPei/SimicsPei.inf {
    <LibraryClasses>
      PcdLib|MdePkg/Library/PeiPcdLib/PeiPcdLib.inf
  }
#  S3 SMM driver
#  UefiCpuPkg/PiSmmCommunication/PiSmmCommunicationPei.inf
  UefiCpuPkg/Universal/Acpi/S3Resume2Pei/S3Resume2Pei.inf {
    <LibraryClasses>
      LockBoxLib|MdeModulePkg/Library/SmmLockBoxLib/SmmLockBoxPeiLib.inf
  }

!if gMinPlatformPkgTokenSpaceGuid.PcdBootToShellOnly == FALSE
  $(SKT_PKG)/Smm/Access/SmmAccessPei.inf {
    <LibraryClasses>
      PcdLib|MdePkg/Library/PeiPcdLib/PeiPcdLib.inf
  }
!endif
  $(PLATFORM_PACKAGE)/PlatformInit/ReportFv/ReportFvPei.inf

  MinPlatformPkg/PlatformInit/PlatformInitPei/PlatformInitPreMem.inf {
    <LibraryClasses>
      BoardInitLib|$(BOARD_PKG)/$(BOARD_NAME)/Library/BoardInitLib/PeiBoardInitPreMemLib.inf
  }
  MinPlatformPkg/PlatformInit/PlatformInitPei/PlatformInitPostMem.inf {
    <LibraryClasses>
      BoardInitLib|$(BOARD_PKG)/$(BOARD_NAME)/Library/BoardInitLib/PeiBoardInitPostMemLib.inf
  }
  MinPlatformPkg/PlatformInit/SiliconPolicyPei/SiliconPolicyPeiPreMem.inf
  MinPlatformPkg/PlatformInit/SiliconPolicyPei/SiliconPolicyPeiPostMem.inf

[Components.X64]
  !include MinPlatformPkg/Include/Dsc/CoreDxeInclude.dsc
  !include AdvancedFeaturePkg/Include/Dsc/CoreAdvancedDxeInclude.dsc

  MdeModulePkg/Universal/EbcDxe/EbcDxe.inf

  MdeModulePkg/Bus/Pci/SataControllerDxe/SataControllerDxe.inf
  #
  # ISA Support
  #
  $(BOARD_PKG)/LegacySioDxe/LegacySioDxe.inf
  MdeModulePkg/Bus/Isa/Ps2KeyboardDxe/Ps2KeyboardDxe.inf

  $(BOARD_PKG)/SmbiosPlatformDxe/SmbiosPlatformDxe.inf
  $(BOARD_PKG)/AcpiTables/AcpiTables.inf
  #
  # Video support
  #
  $(BOARD_PKG)/SimicsVideoDxe/SimicsVideoDxe.inf

  MinPlatformPkg/PlatformInit/PlatformInitDxe/PlatformInitDxe.inf
  MinPlatformPkg/PlatformInit/PlatformInitSmm/PlatformInitSmm.inf
  $(BOARD_PKG)/SimicsDxe/SimicsDxe.inf
  MdeModulePkg/Universal/Acpi/S3SaveStateDxe/S3SaveStateDxe.inf
  MdeModulePkg/Universal/Acpi/BootScriptExecutorDxe/BootScriptExecutorDxe.inf

  SimicsIch10BinPkg/UndiBinary/UndiDxe.inf

  #
  # Shell
  #
  ShellPkg/Application/Shell/Shell.inf {
   <PcdsFixedAtBuild>
     gEfiShellPkgTokenSpaceGuid.PcdShellLibAutoInitialize|FALSE
   <LibraryClasses>
     NULL|ShellPkg/Library/UefiShellLevel2CommandsLib/UefiShellLevel2CommandsLib.inf
     NULL|ShellPkg/Library/UefiShellLevel1CommandsLib/UefiShellLevel1CommandsLib.inf
     NULL|ShellPkg/Library/UefiShellLevel3CommandsLib/UefiShellLevel3CommandsLib.inf
     NULL|ShellPkg/Library/UefiShellDriver1CommandsLib/UefiShellDriver1CommandsLib.inf
     NULL|ShellPkg/Library/UefiShellInstall1CommandsLib/UefiShellInstall1CommandsLib.inf
     NULL|ShellPkg/Library/UefiShellDebug1CommandsLib/UefiShellDebug1CommandsLib.inf
     NULL|ShellPkg/Library/UefiShellNetwork1CommandsLib/UefiShellNetwork1CommandsLib.inf
     NULL|ShellPkg/Library/UefiShellNetwork2CommandsLib/UefiShellNetwork2CommandsLib.inf
     ShellCommandLib|ShellPkg/Library/UefiShellCommandLib/UefiShellCommandLib.inf
     HandleParsingLib|ShellPkg/Library/UefiHandleParsingLib/UefiHandleParsingLib.inf
     BcfgCommandLib|ShellPkg/Library/UefiShellBcfgCommandLib/UefiShellBcfgCommandLib.inf
     ShellCEntryLib|ShellPkg/Library/UefiShellCEntryLib/UefiShellCEntryLib.inf
     ShellLib|ShellPkg/Library/UefiShellLib/UefiShellLib.inf
  }

!if gMinPlatformPkgTokenSpaceGuid.PcdBootToShellOnly == FALSE
  $(SKT_PKG)/Smm/Access/SmmAccess2Dxe.inf
  $(PCH_PKG)/SmmControl/RuntimeDxe/SmmControl2Dxe.inf
  UefiCpuPkg/PiSmmCpuDxeSmm/PiSmmCpuDxeSmm.inf
  $(PCH_PKG)/Spi/Smm/PchSpiSmm.inf
  MinPlatformPkg/Flash/SpiFvbService/SpiFvbServiceSmm.inf
  UefiCpuPkg/CpuS3DataDxe/CpuS3DataDxe.inf
  MdeModulePkg/Universal/LockBox/SmmLockBox/SmmLockBox.inf {
    <LibraryClasses>
      LockBoxLib|MdeModulePkg/Library/SmmLockBoxLib/SmmLockBoxSmmLib.inf
  }
!endif
  MdeModulePkg/Bus/Ata/AtaBusDxe/AtaBusDxe.inf
  MdeModulePkg/Bus/Ata/AtaAtapiPassThru/AtaAtapiPassThru.inf
  MdeModulePkg/Bus/Pci/PciHostBridgeDxe/PciHostBridgeDxe.inf {
    <LibraryClasses>
      PciHostBridgeLib|$(BOARD_PKG)/Library/PciHostBridgeLib/PciHostBridgeLib.inf
  }
  MdeModulePkg/Bus/Pci/NvmExpressDxe/NvmExpressDxe.inf

  UefiCpuPkg/CpuDxe/CpuDxe.inf
  MdeModulePkg/Universal/Console/TerminalDxe/TerminalDxe.inf
  MdeModulePkg/Universal/PrintDxe/PrintDxe.inf
  MdeModulePkg/Bus/Scsi/ScsiBusDxe/ScsiBusDxe.inf
  MdeModulePkg/Bus/Scsi/ScsiDiskDxe/ScsiDiskDxe.inf
  #
  # ACPI Support
  #
  MdeModulePkg/Universal/Acpi/AcpiPlatformDxe/AcpiPlatformDxe.inf
  $(BOARD_PKG)/AcpiTables/MinPlatformAcpiTables/AcpiPlatform.inf

!if gAdvancedFeaturePkgTokenSpaceGuid.PcdSmbiosEnable == TRUE
  AdvancedFeaturePkg/Smbios/SmbiosBasicDxe/SmbiosBasicDxe.inf
!endif

  !include $(BOARD_PKG)/$(BOARD_NAME)/OpenBoardPkgBuildOption.dsc
