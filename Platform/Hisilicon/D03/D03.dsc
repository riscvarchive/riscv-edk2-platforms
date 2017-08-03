#
#  Copyright (c) 2011-2012, ARM Limited. All rights reserved.
#  Copyright (c) 2015, Hisilicon Limited. All rights reserved.
#  Copyright (c) 2015, Linaro Limited. All rights reserved.
#
#  This program and the accompanying materials
#  are licensed and made available under the terms and conditions of the BSD License
#  which accompanies this distribution.  The full text of the license may be found at
#  http://opensource.org/licenses/bsd-license.php
#
#  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
#  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#
#

################################################################################
#
# Defines Section - statements that will be processed to create a Makefile.
#
################################################################################
[Defines]
  PLATFORM_NAME                  = D03
  PLATFORM_GUID                  = e5003abd-8809-6194-ac3d-a6a99ff52478
  PLATFORM_VERSION               = 0.1
  DSC_SPECIFICATION              = 0x00010005
  OUTPUT_DIRECTORY               = Build/$(PLATFORM_NAME)
  SUPPORTED_ARCHITECTURES        = AARCH64
  BUILD_TARGETS                  = DEBUG|RELEASE
  SKUID_IDENTIFIER               = DEFAULT
  FLASH_DEFINITION               = Platform/Hisilicon/$(PLATFORM_NAME)/$(PLATFORM_NAME).fdf
  DEFINE INCLUDE_TFTP_COMMAND=1

!include Silicon/Hisilicon/Hisilicon.dsc.inc

[LibraryClasses.common]
  ArmLib|ArmPkg/Library/ArmLib/ArmBaseLib.inf
  ArmPlatformLib|Silicon/Hisilicon/Library/ArmPlatformLibHisilicon/ArmPlatformLib.inf

  ArmPlatformSysConfigLib|ArmPlatformPkg/ArmVExpressPkg/Library/ArmVExpressSysConfigLib/ArmVExpressSysConfigLib.inf
  NorFlashPlatformLib|ArmPlatformPkg/ArmVExpressPkg/Library/NorFlashArmVExpressLib/NorFlashArmVExpressLib.inf
  LcdPlatformLib|ArmPlatformPkg/ArmVExpressPkg/Library/PL111LcdArmVExpressLib/PL111LcdArmVExpressLib.inf


  I2CLib|Silicon/Hisilicon/Library/I2CLib/I2CLib.inf
  TimerLib|ArmPkg/Library/ArmArchTimerLib/ArmArchTimerLib.inf

  IpmiCmdLib|Silicon/Hisilicon/Hi1610/Library/IpmiCmdLib/IpmiCmdLib.inf

  NetLib|MdeModulePkg/Library/DxeNetLib/DxeNetLib.inf
  DpcLib|MdeModulePkg/Library/DxeDpcLib/DxeDpcLib.inf
  HiiLib|MdeModulePkg/Library/UefiHiiLib/UefiHiiLib.inf
  UefiHiiServicesLib|MdeModulePkg/Library/UefiHiiServicesLib/UefiHiiServicesLib.inf
  UdpIoLib|MdeModulePkg/Library/DxeUdpIoLib/DxeUdpIoLib.inf
  IpIoLib|MdeModulePkg/Library/DxeIpIoLib/DxeIpIoLib.inf



!ifdef $(FDT_ENABLE)
  #FDTUpdateLib
  FdtUpdateLib|Platform/Hisilicon/D03/Library/FdtUpdateLib/FdtUpdateLib.inf
!endif #$(FDT_ENABLE)

  CpldIoLib|Silicon/Hisilicon/Library/CpldIoLib/CpldIoLib.inf

  SerdesLib|Silicon/Hisilicon/Hi1610/Library/Hi1610Serdes/Hi1610SerdesLib.inf

  TimeBaseLib|EmbeddedPkg/Library/TimeBaseLib/TimeBaseLib.inf
  RealTimeClockLib|Platform/Hisilicon/D03/Library/DS3231RealTimeClockLib/DS3231RealTimeClockLib.inf

  OemMiscLib|Platform/Hisilicon/D03/Library/OemMiscLib2P/OemMiscLib2PHi1610.inf
  OemAddressMapLib|Platform/Hisilicon/D03/Library/OemAddressMap2P/OemAddressMap2PHi1610.inf
  PlatformSysCtrlLib|Silicon/Hisilicon/Hi1610/Library/PlatformSysCtrlLibHi1610/PlatformSysCtrlLibHi1610.inf

  CapsuleLib|MdeModulePkg/Library/DxeCapsuleLibNull/DxeCapsuleLibNull.inf
  GenericBdsLib|IntelFrameworkModulePkg/Library/GenericBdsLib/GenericBdsLib.inf
  PlatformBdsLib|Silicon/Hisilicon/Library/PlatformIntelBdsLib/PlatformIntelBdsLib.inf
  CustomizedDisplayLib|MdeModulePkg/Library/CustomizedDisplayLib/CustomizedDisplayLib.inf

  # USB Requirements
  UefiUsbLib|MdePkg/Library/UefiUsbLib/UefiUsbLib.inf

  LpcLib|Silicon/Hisilicon/Hi1610/Library/LpcLib/LpcLib.inf
  SerialPortLib|Silicon/Hisilicon/Hi1610/Library/Uart/LpcSerialPortLib/LpcSerialPortLib.inf

## GIC on D02/D03 is not fully ARM GIC compatible: IRQ cannot be cancelled when
## input signal is de-asserted, except for virtual timer interrupt IRQ #27.
## So we choose to use virtual timer instead of physical one as a workaround.
## This library instance is to override the original define in LibraryClasses.AARCH64 in Hisilicon.dsc.inc.
[LibraryClasses.AARCH64]
  ArmGenericTimerCounterLib|ArmPkg/Library/ArmGenericTimerVirtCounterLib/ArmGenericTimerVirtCounterLib.inf

[LibraryClasses.common.SEC]
  ArmPlatformLib|Silicon/Hisilicon/Library/ArmPlatformLibHisilicon/ArmPlatformLibSec.inf


[LibraryClasses.common.DXE_RUNTIME_DRIVER]
  I2CLib|Silicon/Hisilicon/Library/I2CLib/I2CLibRuntime.inf
  SerialPortLib|Silicon/Hisilicon/Hi1610/Library/Uart/LpcSerialPortLib/LpcSerialPortLib.inf

[BuildOptions]
  GCC:*_*_AARCH64_PLATFORM_FLAGS == -I$(WORKSPACE)/Silicon/Hisilicon/Hi1610/Include

################################################################################
#
# Pcd Section - list of all EDK II PCD Entries defined by this Platform
#
################################################################################

[PcdsFeatureFlag.common]

  ## If TRUE, Graphics Output Protocol will be installed on virtual handle created by ConsplitterDxe.
  #  It could be set FALSE to save size.
  gEfiMdeModulePkgTokenSpaceGuid.PcdConOutGopSupport|TRUE
  gHisiTokenSpaceGuid.PcdIsItsSupported|TRUE
  gHisiTokenSpaceGuid.PcdIsPciPerfTuningEnable|TRUE

[PcdsFixedAtBuild.common]
  gEmbeddedTokenSpaceGuid.PcdEmbeddedPrompt|"D03"

  gArmPlatformTokenSpaceGuid.PcdCoreCount|8

  gEfiMdeModulePkgTokenSpaceGuid.PcdMaxVariableSize|0x2000

  # Stacks for MPCores in Secure World
  gArmPlatformTokenSpaceGuid.PcdCPUCoresSecStackBase|0x81000000
  gArmPlatformTokenSpaceGuid.PcdCPUCoreSecPrimaryStackSize|0x10000

  # Stacks for MPCores in Monitor Mode
  gArmPlatformTokenSpaceGuid.PcdCPUCoresSecMonStackBase|0x8100FF00
  gArmPlatformTokenSpaceGuid.PcdCPUCoreSecMonStackSize|0x100

  # Stacks for MPCores in Normal World
  gArmPlatformTokenSpaceGuid.PcdCPUCoresStackBase|0x81000000
  gArmPlatformTokenSpaceGuid.PcdCPUCorePrimaryStackSize|0xFF00

  gArmTokenSpaceGuid.PcdSystemMemoryBase|0x00000000


  gArmTokenSpaceGuid.PcdSystemMemorySize|0x3FC00000


  # Size of the region used by UEFI in permanent memory (Reserved 64MB)
  gArmPlatformTokenSpaceGuid.PcdSystemMemoryUefiRegionSize|0x10000000

  gHisiTokenSpaceGuid.PcdSerDesFlowCtrlFlag|1
  gHisiTokenSpaceGuid.PcdSlotPerChannelNum|0x2


  gHisiTokenSpaceGuid.PcdPcieRootBridgeMask|0x7 # bit0:HB0RB0,bit1:HB0RB1,bit2:HB0RB2,bit3:HB0RB3,bit4:HB1RB0,bit5:HB1RB1,bit6:HB1RB2,bit7:HB1RB3

  ## SP805 Watchdog - Motherboard Watchdog
  gArmPlatformTokenSpaceGuid.PcdSP805WatchdogBase|0x601e0000

  ## Serial Terminal
  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialRegisterBase|0x2F8
  gEfiMdePkgTokenSpaceGuid.PcdUartDefaultBaudRate|115200

  gHisiTokenSpaceGuid.PcdUartClkInHz|1846100

  gHisiTokenSpaceGuid.PcdSerialPortSendDelay|10000000

  gEfiMdePkgTokenSpaceGuid.PcdUartDefaultDataBits|8
  gEfiMdePkgTokenSpaceGuid.PcdUartDefaultParity|1
  gEfiMdePkgTokenSpaceGuid.PcdUartDefaultStopBits|1


  gHisiTokenSpaceGuid.PcdM3SmmuBaseAddress|0xa0040000
  gHisiTokenSpaceGuid.PcdPcieSmmuBaseAddress|0xb0040000
  gHisiTokenSpaceGuid.PcdDsaSmmuBaseAddress|0xc0040000
  gHisiTokenSpaceGuid.PcdAlgSmmuBaseAddress|0xd0040000


  gEfiMdeModulePkgTokenSpaceGuid.PcdFirmwareVersionString|L"Hisilicon D03 UEFI 16.12 Release"

  gHisiTokenSpaceGuid.PcdBiosVersionString|L"10.01.01T18"

  gHisiTokenSpaceGuid.PcdBiosVersionForBmc|L"1.12"

  gHisiTokenSpaceGuid.PcdSystemProductName|L"D03"
  gHisiTokenSpaceGuid.PcdSystemVersion|L"Estuary"
  gHisiTokenSpaceGuid.PcdBaseBoardProductName|L"D03"
  gHisiTokenSpaceGuid.PcdBaseBoardVersion|L"Estuary"

  gHisiTokenSpaceGuid.PcdCPUInfo|L"Hi1612"

  #
  # ARM PL390 General Interrupt Controller
  #

  gArmTokenSpaceGuid.PcdGicDistributorBase|0x4D000000
  gArmTokenSpaceGuid.PcdGicInterruptInterfaceBase|0xFE000000
  gArmTokenSpaceGuid.PcdGicRedistributorsBase|0x4D100000

  #
  # ARM OS Loader
  #
  # Versatile Express machine type (ARM VERSATILE EXPRESS = 2272) required for ARM Linux:
  gArmPlatformTokenSpaceGuid.PcdDefaultBootDescription|L"Linux from SATA"
  gArmPlatformTokenSpaceGuid.PcdDefaultBootDevicePath|L"EFI\GRUB2\grubaa64.efi"
  gArmPlatformTokenSpaceGuid.PcdDefaultBootArgument|""

  # Use the serial console (ConIn & ConOut) and the Graphic driver (ConOut)
  gArmPlatformTokenSpaceGuid.PcdDefaultConOutPaths|L"VenHw(D3987D4B-971A-435F-8CAF-4967EB627241)/Uart(115200,8,N,1)/VenPcAnsi();VenHw(407B4008-BF5B-11DF-9547-CF16E0D72085)"
  gArmPlatformTokenSpaceGuid.PcdDefaultConInPaths|L"VenHw(D3987D4B-971A-435F-8CAF-4967EB627241)/Uart(115200,8,N,1)/VenPcAnsi()"

  #
  # ARM Architectual Timer Frequency
  #
  # Set it to 0 so that the code will read frequence from register and be
  # adapted to 66M and 50M boards
  gArmTokenSpaceGuid.PcdArmArchTimerFreqInHz|0


  gEfiMdeModulePkgTokenSpaceGuid.PcdResetOnMemoryTypeInformationChange|FALSE
  gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdShellFile|{ 0x83, 0xA5, 0x04, 0x7C, 0x3E, 0x9E, 0x1C, 0x4F, 0xAD, 0x65, 0xE0, 0x52, 0x68, 0xD0, 0xB4, 0xD1 }

  gHisiTokenSpaceGuid.PcdSysControlBaseAddress|0x40010000
  gHisiTokenSpaceGuid.PcdMailBoxAddress|0x0000FFF8

  gHisiTokenSpaceGuid.PcdCpldBaseAddress|0x78000000

  gHisiTokenSpaceGuid.PcdSFCCFGBaseAddress|0xA6000000
  gHisiTokenSpaceGuid.PcdSFCMEM0BaseAddress|0xA4000000


  gHisiTokenSpaceGuid.PcdPeriSubctrlAddress|0x40000000


  gHisiTokenSpaceGuid.PcdMdioSubctrlAddress|0x60000000

  gHisiTokenSpaceGuid.FdtFileAddress|0xA47C0000

  gHisiTokenSpaceGuid.PcdPlatformDefaultPackageType|0x1

  gHisiTokenSpaceGuid.PcdArmPrimaryCoreTemp|0x80010000

  gHisiTokenSpaceGuid.PcdTopOfLowMemory|0x40000000

  gHisiTokenSpaceGuid.PcdBottomOfHighMemory|0x2000000000

  gHisiTokenSpaceGuid.PcdTrustedFirmwareEnable|0x1
  gHisiTokenSpaceGuid.PcdNumaEnable|0

  gHisiTokenSpaceGuid.PcdHb1BaseAddress|0x40000000000


  gHisiTokenSpaceGuid.PcdHb0Rb0PciConfigurationSpaceBaseAddress|0xB0000000
  gHisiTokenSpaceGuid.PcdHb0Rb0PciConfigurationSpaceSize|0x8000000

  gHisiTokenSpaceGuid.PcdHb0Rb1PciConfigurationSpaceBaseAddress|0xB0000000
  gHisiTokenSpaceGuid.PcdHb0Rb1PciConfigurationSpaceSize|0x10000000

  gHisiTokenSpaceGuid.PcdHb0Rb2PciConfigurationSpaceBaseAddress|0xA0000000
  gHisiTokenSpaceGuid.PcdHb0Rb2PciConfigurationSpaceSize|0x10000000

  gHisiTokenSpaceGuid.PcdHb0Rb3PciConfigurationSpaceBaseAddress|0xAC000000
  gHisiTokenSpaceGuid.PcdHb0Rb3PciConfigurationSpaceSize|0x4000000

  gHisiTokenSpaceGuid.PciHb0Rb0Base|0xa0090000
  gHisiTokenSpaceGuid.PciHb0Rb1Base|0xa0200000
  gHisiTokenSpaceGuid.PciHb0Rb2Base|0xa00a0000
  gHisiTokenSpaceGuid.PciHb0Rb3Base|0xa00b0000

  gHisiTokenSpaceGuid.PcdHb0Rb0PciRegionBaseAddress|0xb2000000
  gHisiTokenSpaceGuid.PcdHb0Rb0PciRegionSize|0x5feffff

  gHisiTokenSpaceGuid.PcdHb0Rb1PciRegionBaseAddress|0xb8000000
  gHisiTokenSpaceGuid.PcdHb0Rb1PciRegionSize|0x5feffff

  gHisiTokenSpaceGuid.PcdHb0Rb2PciRegionBaseAddress|0xaa000000
  gHisiTokenSpaceGuid.PcdHb0Rb2PciRegionSize|0x5feffff

  gHisiTokenSpaceGuid.PcdHb0Rb0CpuMemRegionBase|0xB2000000
  gHisiTokenSpaceGuid.PcdHb0Rb1CpuMemRegionBase|0xB8000000
  gHisiTokenSpaceGuid.PcdHb0Rb2CpuMemRegionBase|0xAA000000

  gHisiTokenSpaceGuid.PcdHb0Rb0CpuIoRegionBase|0xb7ff0000
  gHisiTokenSpaceGuid.PcdHb0Rb1CpuIoRegionBase|0xbdff0000
  gHisiTokenSpaceGuid.PcdHb0Rb2CpuIoRegionBase|0xAfff0000

  gHisiTokenSpaceGuid.PcdHb0Rb0IoBase|0
  gHisiTokenSpaceGuid.PcdHb0Rb0IoSize|0xffff #64K

  gHisiTokenSpaceGuid.PcdHb0Rb1IoBase|0
  gHisiTokenSpaceGuid.PcdHb0Rb1IoSize|0xffff #64K

  gHisiTokenSpaceGuid.PcdHb0Rb2IoBase|0
  gHisiTokenSpaceGuid.PcdHb0Rb2IoSize|0xffff #64K

  gHisiTokenSpaceGuid.Pcdsoctype|0x1610
  ## SP804 DualTimer
  gArmPlatformTokenSpaceGuid.PcdSP804TimerFrequencyInMHz|200
  gArmPlatformTokenSpaceGuid.PcdSP804TimerPeriodicInterruptNum|0xb0
  gArmPlatformTokenSpaceGuid.PcdSP804TimerPeriodicBase|0x40060000
  ## TODO: need to confirm the base for Performance and Metronome base for PV660
  gArmPlatformTokenSpaceGuid.PcdSP804TimerPerformanceBase|0x40060000
  gArmPlatformTokenSpaceGuid.PcdSP804TimerMetronomeBase|0x40060000


################################################################################
#
# Components Section - list of all EDK II Modules needed by this Platform
#
################################################################################
[Components.common]

  #
  # SEC
  #

  #
  # PEI Phase modules
  #
  ArmPlatformPkg/PrePeiCore/PrePeiCoreMPCore.inf
  MdeModulePkg/Core/Pei/PeiMain.inf
  MdeModulePkg/Universal/PCD/Pei/Pcd.inf {
    <LibraryClasses>
      PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  }
  Platform/Hisilicon/D03/Drivers/Ipmi/IpmiInterfacePei/IpmiInterfacePei.inf

  ArmPlatformPkg/PlatformPei/PlatformPeim.inf

  Platform/Hisilicon/D03/MemoryInitPei/MemoryInitPeim.inf
  ArmPkg/Drivers/CpuPei/CpuPei.inf
  IntelFrameworkModulePkg/Universal/StatusCode/Pei/StatusCodePei.inf
  MdeModulePkg/Universal/FaultTolerantWritePei/FaultTolerantWritePei.inf
  MdeModulePkg/Universal/Variable/Pei/VariablePei.inf

  Platform/Hisilicon/D03/EarlyConfigPeim/EarlyConfigPeimD03.inf
  Silicon/Hisilicon/Drivers/VersionInfoPeim/VersionInfoPeim.inf

  MdeModulePkg/Core/DxeIplPeim/DxeIpl.inf {
    <LibraryClasses>
      NULL|IntelFrameworkModulePkg/Library/LzmaCustomDecompressLib/LzmaCustomDecompressLib.inf
  }

  #
  # DXE
  #
  MdeModulePkg/Core/Dxe/DxeMain.inf {
    <LibraryClasses>
      NULL|MdeModulePkg/Library/DxeCrc32GuidedSectionExtractLib/DxeCrc32GuidedSectionExtractLib.inf
  }
  MdeModulePkg/Universal/PCD/Dxe/Pcd.inf {
    <LibraryClasses>
      PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  }

  Silicon/Hisilicon/Hi1610/Drivers/IoInitDxe/IoInitDxe.inf

  #
  # Architectural Protocols
  #
  ArmPkg/Drivers/CpuDxe/CpuDxe.inf
  MdeModulePkg/Core/RuntimeDxe/RuntimeDxe.inf

  Platform/Hisilicon/D03/Drivers/OemNicConfig2PHi1610/OemNicConfig2P.inf

  Platform/Hisilicon/D03/Drivers/SFC/SfcDxeDriver.inf

  MdeModulePkg/Universal/SecurityStubDxe/SecurityStubDxe.inf
  MdeModulePkg/Universal/Variable/EmuRuntimeDxe/EmuVariableRuntimeDxe.inf
  Silicon/Hisilicon/Drivers/FlashFvbDxe/FlashFvbDxe.inf
  MdeModulePkg/Universal/Variable/RuntimeDxe/VariableRuntimeDxe.inf {
    <LibraryClasses>
      NULL|MdeModulePkg/Library/VarCheckUefiLib/VarCheckUefiLib.inf
      BaseMemoryLib|MdePkg/Library/BaseMemoryLib/BaseMemoryLib.inf
  }
  MdeModulePkg/Universal/CapsuleRuntimeDxe/CapsuleRuntimeDxe.inf
  MdeModulePkg/Universal/FaultTolerantWriteDxe/FaultTolerantWriteDxe.inf

  MdeModulePkg/Universal/MonotonicCounterRuntimeDxe/MonotonicCounterRuntimeDxe.inf
  MdeModulePkg/Universal/ResetSystemRuntimeDxe/ResetSystemRuntimeDxe.inf
  EmbeddedPkg/RealTimeClockRuntimeDxe/RealTimeClockRuntimeDxe.inf {
    <LibraryClasses>
      CpldIoLib|Silicon/Hisilicon/Library/CpldIoLib/CpldIoLibRuntime.inf
  }
  EmbeddedPkg/MetronomeDxe/MetronomeDxe.inf

  MdeModulePkg/Universal/Console/ConPlatformDxe/ConPlatformDxe.inf
  MdeModulePkg/Universal/Console/ConSplitterDxe/ConSplitterDxe.inf
  MdeModulePkg/Universal/Console/GraphicsConsoleDxe/GraphicsConsoleDxe.inf
  MdeModulePkg/Universal/Console/TerminalDxe/TerminalDxe.inf
  MdeModulePkg/Universal/SerialDxe/SerialDxe.inf

  # Simple TextIn/TextOut for UEFI Terminal
  EmbeddedPkg/SimpleTextInOutSerial/SimpleTextInOutSerial.inf

  MdeModulePkg/Universal/HiiDatabaseDxe/HiiDatabaseDxe.inf

  ArmPkg/Drivers/ArmGic/ArmGicDxe.inf

  ArmPkg/Drivers/TimerDxe/TimerDxe.inf

  ArmPlatformPkg/Drivers/SP805WatchdogDxe/SP805WatchdogDxe.inf
  IntelFrameworkModulePkg/Universal/StatusCode/RuntimeDxe/StatusCodeRuntimeDxe.inf
  #
  #ACPI
  #
  MdeModulePkg/Universal/Acpi/AcpiPlatformDxe/AcpiPlatformDxe.inf
  MdeModulePkg/Universal/Acpi/AcpiTableDxe/AcpiTableDxe.inf

  Silicon/Hisilicon/Hi1610/Hi1610AcpiTables/AcpiTablesHi1610.inf
  Silicon/Hisilicon/Drivers/AcpiPlatformDxe/AcpiPlatformDxe.inf

  #
  # Usb Support
  #
  Silicon/Hisilicon/Drivers/VirtualEhciPciIo/VirtualEhciPciIo.inf
  MdeModulePkg/Bus/Pci/EhciDxe/EhciDxe.inf
  Platform/Hisilicon/D03/Drivers/OhciDxe/OhciDxe.inf
  MdeModulePkg/Bus/Usb/UsbBusDxe/UsbBusDxe.inf
  MdeModulePkg/Bus/Usb/UsbKbDxe/UsbKbDxe.inf
  MdeModulePkg/Bus/Usb/UsbMouseDxe/UsbMouseDxe.inf
  MdeModulePkg/Bus/Usb/UsbMassStorageDxe/UsbMassStorageDxe.inf

  Platform/Hisilicon/D03/Drivers/Ipmi/ipmiInterfaceDxe/IpmiInterfaceDxe.inf

  #
  #network
  #
  Platform/Hisilicon/D03/Drivers/Net/SnpPV660Dxe/SnpPV600Dxe.inf

  MdeModulePkg/Universal/Network/ArpDxe/ArpDxe.inf
  MdeModulePkg/Universal/Network/Dhcp4Dxe/Dhcp4Dxe.inf
  MdeModulePkg/Universal/Network/DpcDxe/DpcDxe.inf
  MdeModulePkg/Universal/Network/Ip4Dxe/Ip4Dxe.inf
  MdeModulePkg/Universal/Network/MnpDxe/MnpDxe.inf
  MdeModulePkg/Universal/Network/Mtftp4Dxe/Mtftp4Dxe.inf
  MdeModulePkg/Universal/Network/Tcp4Dxe/Tcp4Dxe.inf
  MdeModulePkg/Universal/Network/Udp4Dxe/Udp4Dxe.inf
  MdeModulePkg/Universal/Network/UefiPxeBcDxe/UefiPxeBcDxe.inf

  Platform/Hisilicon/D03/Drivers/Sas/SasDxeDriver.inf

  #
  # FAT filesystem + GPT/MBR partitioning
  #

  MdeModulePkg/Universal/Disk/DiskIoDxe/DiskIoDxe.inf
  MdeModulePkg/Universal/Disk/PartitionDxe/PartitionDxe.inf
  MdeModulePkg/Universal/Disk/UnicodeCollation/EnglishDxe/EnglishDxe.inf

  MdeModulePkg/Application/HelloWorld/HelloWorld.inf
  #
  # Bds
  #
  MdeModulePkg/Universal/DevicePathDxe/DevicePathDxe.inf

  Platform/Hisilicon/D03/Drivers/GetInfoFromBmc/GetInfoFromBmc.inf
  Platform/Hisilicon/D03/Drivers/TransferSmbiosInfo/TransSmbiosInfo.inf
  Platform/Hisilicon/D03/Drivers/IpmiMiscOpDxe/IpmiMiscOpDxe.inf

  Platform/Hisilicon/D03/Drivers/IpmiWatchdogDxe/IpmiWatchdogDxe.inf

  MdeModulePkg/Universal/SmbiosDxe/SmbiosDxe.inf
  Silicon/Hisilicon/Drivers/Smbios/SmbiosMiscDxe/SmbiosMiscDxe.inf

!ifdef $(FDT_ENABLE)
  Silicon/Hisilicon/Drivers/UpdateFdtDxe/UpdateFdtDxe.inf {
    <LibraryClasses>
      BaseMemoryLib|MdePkg/Library/BaseMemoryLib/BaseMemoryLib.inf
  }
!endif #$(FDT_ENABLE)

  #PCIe Support
  Silicon/Hisilicon/Hi1610/Drivers/PcieInit1610/PcieInitDxe.inf {
    <LibraryClasses>
      NULL|Platform/Hisilicon/D03/Library/PlatformPciLib/PlatformPciLib.inf
  }
  Platform/Hisilicon/D03/Drivers/PciPlatform/PciPlatform.inf
  Silicon/Hisilicon/Drivers/PciHostBridgeDxe/PciHostBridgeDxe.inf {
    <LibraryClasses>
      NULL|Platform/Hisilicon/D03/Library/PlatformPciLib/PlatformPciLib.inf
  }

  MdeModulePkg/Bus/Pci/PciBusDxe/PciBusDxe.inf

  Platform/Hisilicon/D03/Drivers/ReportPciePlugDidVidToBmc/ReportPciePlugDidVidToBmc.inf
  Silicon/Hisilicon/Drivers/Smbios/AddSmbiosType9/AddSmbiosType9.inf
  Platform/Hisilicon/D03/Drivers/Sm750Dxe/UefiSmi.inf

  Silicon/Hisilicon/Drivers/Smbios/MemorySubClassDxe/MemorySubClassDxe.inf


  Silicon/Hisilicon/Drivers/Smbios/ProcessorSubClassDxe/ProcessorSubClassDxe.inf

  #
  # Memory test
  #
  MdeModulePkg/Universal/MemoryTest/NullMemoryTestDxe/NullMemoryTestDxe.inf

  MdeModulePkg/Universal/DisplayEngineDxe/DisplayEngineDxe.inf
  MdeModulePkg/Universal/SetupBrowserDxe/SetupBrowserDxe.inf
  IntelFrameworkModulePkg/Universal/BdsDxe/BdsDxe.inf

  #
  # UEFI application (Shell Embedded Boot Loader)
  #
  ShellPkg/Application/Shell/Shell.inf {
    <LibraryClasses>
      ShellCommandLib|ShellPkg/Library/UefiShellCommandLib/UefiShellCommandLib.inf
      NULL|ShellPkg/Library/UefiShellLevel2CommandsLib/UefiShellLevel2CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellLevel1CommandsLib/UefiShellLevel1CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellLevel3CommandsLib/UefiShellLevel3CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellDriver1CommandsLib/UefiShellDriver1CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellDebug1CommandsLib/UefiShellDebug1CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellInstall1CommandsLib/UefiShellInstall1CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellNetwork1CommandsLib/UefiShellNetwork1CommandsLib.inf
      HandleParsingLib|ShellPkg/Library/UefiHandleParsingLib/UefiHandleParsingLib.inf
      PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf
      BcfgCommandLib|ShellPkg/Library/UefiShellBcfgCommandLib/UefiShellBcfgCommandLib.inf
!ifdef $(INCLUDE_DP)
      NULL|ShellPkg/Library/UefiDpLib/UefiDpLib.inf
!endif #$(INCLUDE_DP)
!ifdef $(INCLUDE_TFTP_COMMAND)
      NULL|ShellPkg/Library/UefiShellTftpCommandLib/UefiShellTftpCommandLib.inf
!endif #$(INCLUDE_TFTP_COMMAND)

    <PcdsFixedAtBuild>
      gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0xFF
      gEfiShellPkgTokenSpaceGuid.PcdShellLibAutoInitialize|FALSE
      gEfiMdePkgTokenSpaceGuid.PcdUefiLibMaxPrintBufferSize|8000
  }
