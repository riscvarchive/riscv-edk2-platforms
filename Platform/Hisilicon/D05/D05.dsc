#
#  Copyright (c) 2011-2012, ARM Limited. All rights reserved.
#  Copyright (c) 2015-2016, Hisilicon Limited. All rights reserved.
#  Copyright (c) 2015-2016, Linaro Limited. All rights reserved.
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
  PLATFORM_NAME                  = D05
  PLATFORM_GUID                  = D0D445F1-B2CA-4101-9986-1B23525CBEA6
  PLATFORM_VERSION               = 0.1
  DSC_SPECIFICATION              = 0x00010019
  OUTPUT_DIRECTORY               = Build/$(PLATFORM_NAME)
  SUPPORTED_ARCHITECTURES        = AARCH64
  BUILD_TARGETS                  = DEBUG|RELEASE
  SKUID_IDENTIFIER               = DEFAULT
  FLASH_DEFINITION               = Platform/Hisilicon/$(PLATFORM_NAME)/$(PLATFORM_NAME).fdf
  DEFINE EDK2_SKIP_PEICORE=0
  DEFINE INCLUDE_TFTP_COMMAND=1
  DEFINE NETWORK_IP6_ENABLE      = FALSE
  DEFINE HTTP_BOOT_ENABLE        = FALSE

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
  OrderedCollectionLib|MdePkg/Library/BaseOrderedCollectionRedBlackTreeLib/BaseOrderedCollectionRedBlackTreeLib.inf
  ReportStatusCodeLib|MdeModulePkg/Library/DxeReportStatusCodeLib/DxeReportStatusCodeLib.inf
  DxeServicesLib|MdePkg/Library/DxeServicesLib/DxeServicesLib.inf
  FileExplorerLib|MdeModulePkg/Library/FileExplorerLib/FileExplorerLib.inf
  BootLogoLib|MdeModulePkg/Library/BootLogoLib/BootLogoLib.inf
  SortLib|MdeModulePkg/Library/UefiSortLib/UefiSortLib.inf

!if $(NETWORK_IP6_ENABLE) == TRUE
  TcpIoLib|MdeModulePkg/Library/DxeTcpIoLib/DxeTcpIoLib.inf
!endif

!if $(HTTP_BOOT_ENABLE) == TRUE
  HttpLib|MdeModulePkg/Library/DxeHttpLib/DxeHttpLib.inf
!endif

!ifdef $(FDT_ENABLE)
  #FDTUpdateLib
  FdtUpdateLib|Platform/Hisilicon/D05/Library/FdtUpdateLib/FdtUpdateLib.inf
!endif #$(FDT_ENABLE)

  CpldIoLib|Silicon/Hisilicon/Library/CpldIoLib/CpldIoLib.inf

  SerdesLib|Silicon/Hisilicon/Hi1616/Library/Hi1616Serdes/Hi1616SerdesLib.inf

  TimeBaseLib|EmbeddedPkg/Library/TimeBaseLib/TimeBaseLib.inf
  #D05 RTC hardware is same as D03
  RealTimeClockLib|Platform/Hisilicon/D03/Library/DS3231RealTimeClockLib/DS3231RealTimeClockLib.inf

  OemMiscLib|Platform/Hisilicon/D05/Library/OemMiscLibD05/OemMiscLibD05.inf
  OemAddressMapLib|Platform/Hisilicon/D05/Library/OemAddressMapD05/OemAddressMapD05.inf
  PlatformSysCtrlLib|Silicon/Hisilicon/Hi1616/Library/PlatformSysCtrlLibHi1616/PlatformSysCtrlLibHi1616.inf

  CapsuleLib|MdeModulePkg/Library/DxeCapsuleLibNull/DxeCapsuleLibNull.inf
  GenericBdsLib|IntelFrameworkModulePkg/Library/GenericBdsLib/GenericBdsLib.inf
  PlatformBdsLib|Silicon/Hisilicon/Library/PlatformIntelBdsLib/PlatformIntelBdsLib.inf
  CustomizedDisplayLib|MdeModulePkg/Library/CustomizedDisplayLib/CustomizedDisplayLib.inf

  # USB Requirements
  UefiUsbLib|MdePkg/Library/UefiUsbLib/UefiUsbLib.inf

  LpcLib|Silicon/Hisilicon/Hi1610/Library/LpcLib/LpcLib.inf
  SerialPortLib|ArmPlatformPkg/Library/PL011SerialPortLib/PL011SerialPortLib.inf

[LibraryClasses.common.SEC]
  ArmPlatformLib|Silicon/Hisilicon/Library/ArmPlatformLibHisilicon/ArmPlatformLibSec.inf


[LibraryClasses.common.DXE_RUNTIME_DRIVER]
  I2CLib|Silicon/Hisilicon/Library/I2CLib/I2CLibRuntime.inf
  SerialPortLib|ArmPlatformPkg/Library/PL011SerialPortLib/PL011SerialPortLib.inf

[BuildOptions]
  GCC:*_*_AARCH64_PLATFORM_FLAGS == -I$(WORKSPACE)/Silicon/Hisilicon/Hi1616/Include

################################################################################
#
# Pcd Section - list of all EDK II PCD Entries defined by this Platform
#
################################################################################

[PcdsFeatureFlag.common]

!if $(EDK2_SKIP_PEICORE) == 1
  gArmPlatformTokenSpaceGuid.PcdSystemMemoryInitializeInSec|TRUE
  gArmPlatformTokenSpaceGuid.PcdSendSgiToBringUpSecondaryCores|TRUE
!endif

  ## If TRUE, Graphics Output Protocol will be installed on virtual handle created by ConsplitterDxe.
  #  It could be set FALSE to save size.
  gEfiMdeModulePkgTokenSpaceGuid.PcdConOutGopSupport|TRUE
  gHisiTokenSpaceGuid.PcdIsItsSupported|TRUE

[PcdsFixedAtBuild.common]
  gEmbeddedTokenSpaceGuid.PcdEmbeddedPrompt|"D05"

  gArmPlatformTokenSpaceGuid.PcdCoreCount|8

  gEfiMdeModulePkgTokenSpaceGuid.PcdMaxVariableSize|0x2000

  # Stacks for MPCores in Secure World
  gArmPlatformTokenSpaceGuid.PcdCPUCoresSecStackBase|0xE1000000
  gArmPlatformTokenSpaceGuid.PcdCPUCoreSecPrimaryStackSize|0x10000

  # Stacks for MPCores in Monitor Mode
  gArmPlatformTokenSpaceGuid.PcdCPUCoresSecMonStackBase|0xE100FF00
  gArmPlatformTokenSpaceGuid.PcdCPUCoreSecMonStackSize|0x100

  # Stacks for MPCores in Normal World
  gArmPlatformTokenSpaceGuid.PcdCPUCoresStackBase|0xE1000000
  gArmPlatformTokenSpaceGuid.PcdCPUCorePrimaryStackSize|0xFF00

  gArmTokenSpaceGuid.PcdSystemMemoryBase|0x00000000


  gArmTokenSpaceGuid.PcdSystemMemorySize|0x3FC00000


  # Size of the region used by UEFI in permanent memory (Reserved 64MB)
  gArmPlatformTokenSpaceGuid.PcdSystemMemoryUefiRegionSize|0x10000000

  gHisiTokenSpaceGuid.PcdSerDesFlowCtrlFlag|1
  gHisiTokenSpaceGuid.PcdSlotPerChannelNum|0x2


  gHisiTokenSpaceGuid.PcdPcieRootBridgeMask|0x94 # bit0:HB0RB0,bit1:HB0RB1,bit2:HB0RB2,bit3:HB0RB3,bit4:HB0RB4,bit5:HB0RB5,bit6:HB0RB6,bit7:HB0RB7
                                                # bit8:HB1RB0,bit9:HB1RB1,bit10:HB1RB2,bit11:HB1RB3,bit12:HB1RB4,bit13:HB1RB5,bit14:HB1RB6,bit14:HB1RB15
  ## enable all the pcie device, because it is ok for bios
  gHisiTokenSpaceGuid.PcdPcieRootBridgeMask2P|0x34F4 # bit0:HB0RB0,bit1:HB0RB1,bit2:HB0RB2,bit3:HB0RB3,bit4:HB0RB4,bit5:HB0RB5,bit6:HB0RB6,bit7:HB0RB7
                                                # bit8:HB1RB0,bit9:HB1RB1,bit10:HB1RB2,bit11:HB1RB3,bit12:HB1RB4,bit13:HB1RB5,bit14:HB1RB6,bit14:HB1RB15

  ## SP805 Watchdog - Motherboard Watchdog
  gArmPlatformTokenSpaceGuid.PcdSP805WatchdogBase|0x601e0000

  ## Serial Terminal
  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialRegisterBase|0x602B0000
  gEfiMdePkgTokenSpaceGuid.PcdUartDefaultBaudRate|115200

  gArmPlatformTokenSpaceGuid.PL011UartClkInHz|200000000

  gEfiMdePkgTokenSpaceGuid.PcdUartDefaultDataBits|8
  gEfiMdePkgTokenSpaceGuid.PcdUartDefaultParity|1
  gEfiMdePkgTokenSpaceGuid.PcdUartDefaultStopBits|1
  # use the TTY terminal type (which has a working backspace)
  gEfiMdePkgTokenSpaceGuid.PcdDefaultTerminalType|4


  gHisiTokenSpaceGuid.PcdM3SmmuBaseAddress|0xa0040000
  gHisiTokenSpaceGuid.PcdPcieSmmuBaseAddress|0xb0040000
  gHisiTokenSpaceGuid.PcdDsaSmmuBaseAddress|0xc0040000
  gHisiTokenSpaceGuid.PcdAlgSmmuBaseAddress|0xd0040000


  gHisiTokenSpaceGuid.PcdIsMPBoot|1
  gHisiTokenSpaceGuid.PcdSocketMask|0x3
  gEfiMdeModulePkgTokenSpaceGuid.PcdFirmwareVersionString|L"Hisilicon D05 UEFI 16.12 Release"

  gHisiTokenSpaceGuid.PcdBiosVersionString|L"10.01.01T18"

  gHisiTokenSpaceGuid.PcdBiosVersionForBmc|L"1.12"

  gHisiTokenSpaceGuid.PcdSystemProductName|L"D05"
  gHisiTokenSpaceGuid.PcdSystemVersion|L"Estuary"
  gHisiTokenSpaceGuid.PcdBaseBoardProductName|L"D05"
  gHisiTokenSpaceGuid.PcdBaseBoardVersion|L"Estuary"

  gHisiTokenSpaceGuid.PcdCPUInfo|L"Hi1616"


  gArmTokenSpaceGuid.PcdGicDistributorBase|0x4D000000
  gArmTokenSpaceGuid.PcdGicRedistributorsBase|0x4D100000
  gArmTokenSpaceGuid.PcdGicInterruptInterfaceBase|0xFE000000


  # Use the serial console (ConIn & ConOut) and the Graphic driver (ConOut)
  gArmPlatformTokenSpaceGuid.PcdDefaultConOutPaths|L"VenHw(D3987D4B-971A-435F-8CAF-4967EB627241)/Uart(115200,8,N,1)/VenPcAnsi();VenHw(407B4008-BF5B-11DF-9547-CF16E0D72085)"
  gArmPlatformTokenSpaceGuid.PcdDefaultConInPaths|L"VenHw(D3987D4B-971A-435F-8CAF-4967EB627241)/Uart(115200,8,N,1)/VenPcAnsi()"

  #
  # ARM Architectual Timer Frequency
  #
  gArmTokenSpaceGuid.PcdArmArchTimerFreqInHz|50000000


  gEfiMdeModulePkgTokenSpaceGuid.PcdResetOnMemoryTypeInformationChange|FALSE
  gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdShellFile|{ 0x83, 0xA5, 0x04, 0x7C, 0x3E, 0x9E, 0x1C, 0x4F, 0xAD, 0x65, 0xE0, 0x52, 0x68, 0xD0, 0xB4, 0xD1 }

  gHisiTokenSpaceGuid.PcdSysControlBaseAddress|0x40010000
  gHisiTokenSpaceGuid.PcdMailBoxAddress|0x0000FFF8

  gHisiTokenSpaceGuid.PcdCpldBaseAddress|0x78000000

  gHisiTokenSpaceGuid.PcdSFCCFGBaseAddress|0xA6000000
  gHisiTokenSpaceGuid.PcdSFCMEM0BaseAddress|0xA4000000


  gHisiTokenSpaceGuid.PcdPeriSubctrlAddress|0x40000000


  gHisiTokenSpaceGuid.PcdMdioSubctrlAddress|0x60000000

  ## DTB address at spi flash
  gHisiTokenSpaceGuid.FdtFileAddress|0xA47A0000

  gHisiTokenSpaceGuid.PcdPlatformDefaultPackageType|0x1

  gHisiTokenSpaceGuid.PcdArmPrimaryCoreTemp|0x80010000

  gHisiTokenSpaceGuid.PcdTopOfLowMemory|0x40000000

  gHisiTokenSpaceGuid.PcdBottomOfHighMemory|0x1000000000

  gHisiTokenSpaceGuid.PcdNORFlashBase|0x70000000
  gHisiTokenSpaceGuid.PcdNORFlashCachableSize|0x8000000

  gHisiTokenSpaceGuid.PcdTrustedFirmwareEnable|0x1
  gHisiTokenSpaceGuid.PcdNumaEnable|1
  gHisiTokenSpaceGuid.PcdMacAddress|0xA47E0000

  gHisiTokenSpaceGuid.PcdHb1BaseAddress|0x40000000000


  gHisiTokenSpaceGuid.PcdHb0Rb0PciConfigurationSpaceBaseAddress|0xA0000000
  gHisiTokenSpaceGuid.PcdHb0Rb0PciConfigurationSpaceSize|0x10000000
  gHisiTokenSpaceGuid.PcdHb0Rb1PciConfigurationSpaceBaseAddress|0xA0000000
  gHisiTokenSpaceGuid.PcdHb0Rb1PciConfigurationSpaceSize|0x10000000
  gHisiTokenSpaceGuid.PcdHb0Rb2PciConfigurationSpaceBaseAddress|0xA0000000
  gHisiTokenSpaceGuid.PcdHb0Rb2PciConfigurationSpaceSize|0x10000000
  gHisiTokenSpaceGuid.PcdHb0Rb3PciConfigurationSpaceBaseAddress|0xA0000000
  gHisiTokenSpaceGuid.PcdHb0Rb3PciConfigurationSpaceSize|0x10000000
  gHisiTokenSpaceGuid.PcdHb0Rb4PciConfigurationSpaceBaseAddress|0x8A0000000
  gHisiTokenSpaceGuid.PcdHb0Rb4PciConfigurationSpaceSize|0x10000000
  gHisiTokenSpaceGuid.PcdHb0Rb5PciConfigurationSpaceBaseAddress|0x8B0000000
  gHisiTokenSpaceGuid.PcdHb0Rb5PciConfigurationSpaceSize|0x8000000
  gHisiTokenSpaceGuid.PcdHb0Rb6PciConfigurationSpaceBaseAddress|0x8A0000000
  gHisiTokenSpaceGuid.PcdHb0Rb6PciConfigurationSpaceSize|0x10000000
  gHisiTokenSpaceGuid.PcdHb0Rb7PciConfigurationSpaceBaseAddress|0x8B0000000
  gHisiTokenSpaceGuid.PcdHb0Rb7PciConfigurationSpaceSize|0x10000000
  gHisiTokenSpaceGuid.PcdHb1Rb0PciConfigurationSpaceBaseAddress|0x400A0000000
  gHisiTokenSpaceGuid.PcdHb1Rb0PciConfigurationSpaceSize|0x10000000
  gHisiTokenSpaceGuid.PcdHb1Rb1PciConfigurationSpaceBaseAddress|0x400A0000000
  gHisiTokenSpaceGuid.PcdHb1Rb1PciConfigurationSpaceSize|0x10000000
  gHisiTokenSpaceGuid.PcdHb1Rb2PciConfigurationSpaceBaseAddress|0x64000000000
  gHisiTokenSpaceGuid.PcdHb1Rb2PciConfigurationSpaceSize|0x400000000
  gHisiTokenSpaceGuid.PcdHb1Rb3PciConfigurationSpaceBaseAddress|0x400A0000000
  gHisiTokenSpaceGuid.PcdHb1Rb3PciConfigurationSpaceSize|0x10000000
  gHisiTokenSpaceGuid.PcdHb1Rb4PciConfigurationSpaceBaseAddress|0x74000000000
  gHisiTokenSpaceGuid.PcdHb1Rb4PciConfigurationSpaceSize|0x400000000
  gHisiTokenSpaceGuid.PcdHb1Rb5PciConfigurationSpaceBaseAddress|0x78000000000
  gHisiTokenSpaceGuid.PcdHb1Rb5PciConfigurationSpaceSize|0x400000000
  gHisiTokenSpaceGuid.PcdHb1Rb6PciConfigurationSpaceBaseAddress|0x408A0000000
  gHisiTokenSpaceGuid.PcdHb1Rb6PciConfigurationSpaceSize|0x10000000
  gHisiTokenSpaceGuid.PcdHb1Rb7PciConfigurationSpaceBaseAddress|0x408A0000000
  gHisiTokenSpaceGuid.PcdHb1Rb7PciConfigurationSpaceSize|0x10000000

  gHisiTokenSpaceGuid.PciHb0Rb0Base|0xa0090000
  gHisiTokenSpaceGuid.PciHb0Rb1Base|0xa0200000
  gHisiTokenSpaceGuid.PciHb0Rb2Base|0xa00a0000
  gHisiTokenSpaceGuid.PciHb0Rb3Base|0xa00b0000
  gHisiTokenSpaceGuid.PciHb0Rb4Base|0x8a0090000
  gHisiTokenSpaceGuid.PciHb0Rb5Base|0x8a0200000
  gHisiTokenSpaceGuid.PciHb0Rb6Base|0x8a00a0000
  gHisiTokenSpaceGuid.PciHb0Rb7Base|0x8a00b0000
  gHisiTokenSpaceGuid.PciHb1Rb0Base|0x600a0090000
  gHisiTokenSpaceGuid.PciHb1Rb1Base|0x600a0200000
  gHisiTokenSpaceGuid.PciHb1Rb2Base|0x600a00a0000
  gHisiTokenSpaceGuid.PciHb1Rb3Base|0x600a00b0000
  gHisiTokenSpaceGuid.PciHb1Rb4Base|0x700a0090000
  gHisiTokenSpaceGuid.PciHb1Rb5Base|0x700a0200000
  gHisiTokenSpaceGuid.PciHb1Rb6Base|0x700a00a0000
  gHisiTokenSpaceGuid.PciHb1Rb7Base|0x700a00b0000

  gHisiTokenSpaceGuid.PcdHb0Rb0PciRegionBaseAddress|0xa8400000
  gHisiTokenSpaceGuid.PcdHb0Rb0PciRegionSize|0xbeffff
  gHisiTokenSpaceGuid.PcdHb0Rb1PciRegionBaseAddress|0xa9400000
  gHisiTokenSpaceGuid.PcdHb0Rb1PciRegionSize|0xbeffff
  gHisiTokenSpaceGuid.PcdHb0Rb2PciRegionBaseAddress|0xa8800000
  gHisiTokenSpaceGuid.PcdHb0Rb2PciRegionSize|0x77effff
  gHisiTokenSpaceGuid.PcdHb0Rb3PciRegionBaseAddress|0xab400000
  gHisiTokenSpaceGuid.PcdHb0Rb3PciRegionSize|0xbeffff
  gHisiTokenSpaceGuid.PcdHb0Rb4PciRegionBaseAddress|0xa9000000
  gHisiTokenSpaceGuid.PcdHb0Rb4PciRegionSize|0x2feffff
  gHisiTokenSpaceGuid.PcdHb0Rb5PciRegionBaseAddress|0xb0800000
  gHisiTokenSpaceGuid.PcdHb0Rb5PciRegionSize|0x77effff
  gHisiTokenSpaceGuid.PcdHb0Rb6PciRegionBaseAddress|0xac900000
  gHisiTokenSpaceGuid.PcdHb0Rb6PciRegionSize|0x36effff
  gHisiTokenSpaceGuid.PcdHb0Rb7PciRegionBaseAddress|0xb9800000
  gHisiTokenSpaceGuid.PcdHb0Rb7PciRegionSize|0x67effff
  gHisiTokenSpaceGuid.PcdHb1Rb0PciRegionBaseAddress|0x400a8400000
  gHisiTokenSpaceGuid.PcdHb1Rb0PciRegionSize|0xbeffff
  gHisiTokenSpaceGuid.PcdHb1Rb1PciRegionBaseAddress|0x400a9400000
  gHisiTokenSpaceGuid.PcdHb1Rb1PciRegionSize|0xbeffff
  gHisiTokenSpaceGuid.PcdHb1Rb2PciRegionBaseAddress|0x20000000
  gHisiTokenSpaceGuid.PcdHb1Rb2PciRegionSize|0xcfffffff
  gHisiTokenSpaceGuid.PcdHb1Rb3PciRegionBaseAddress|0x400ab400000
  gHisiTokenSpaceGuid.PcdHb1Rb3PciRegionSize|0xbeffff
  gHisiTokenSpaceGuid.PcdHb1Rb4PciRegionBaseAddress|0x30000000
  gHisiTokenSpaceGuid.PcdHb1Rb4PciRegionSize|0xbfffffff
  gHisiTokenSpaceGuid.PcdHb1Rb5PciRegionBaseAddress|0x40000000
  gHisiTokenSpaceGuid.PcdHb1Rb5PciRegionSize|0xafffffff
  gHisiTokenSpaceGuid.PcdHb1Rb6PciRegionBaseAddress|0x408aa400000
  gHisiTokenSpaceGuid.PcdHb1Rb6PciRegionSize|0xbeffff
  gHisiTokenSpaceGuid.PcdHb1Rb7PciRegionBaseAddress|0x408ab400000
  gHisiTokenSpaceGuid.PcdHb1Rb7PciRegionSize|0xbeffff

  gHisiTokenSpaceGuid.PcdHb0Rb0CpuMemRegionBase|0xA8400000
  gHisiTokenSpaceGuid.PcdHb0Rb1CpuMemRegionBase|0xA9400000
  gHisiTokenSpaceGuid.PcdHb0Rb2CpuMemRegionBase|0xA8800000
  gHisiTokenSpaceGuid.PcdHb0Rb3CpuMemRegionBase|0xAB400000
  gHisiTokenSpaceGuid.PcdHb0Rb4CpuMemRegionBase|0x8A9000000
  gHisiTokenSpaceGuid.PcdHb0Rb5CpuMemRegionBase|0x8B0800000
  gHisiTokenSpaceGuid.PcdHb0Rb6CpuMemRegionBase|0x8AC900000
  gHisiTokenSpaceGuid.PcdHb0Rb7CpuMemRegionBase|0x8B9800000
  gHisiTokenSpaceGuid.PcdHb1Rb0CpuMemRegionBase|0x400A8400000
  gHisiTokenSpaceGuid.PcdHb1Rb1CpuMemRegionBase|0x400A9400000
  gHisiTokenSpaceGuid.PcdHb1Rb2CpuMemRegionBase|0x65020000000
  gHisiTokenSpaceGuid.PcdHb1Rb3CpuMemRegionBase|0x400AB400000
  gHisiTokenSpaceGuid.PcdHb1Rb4CpuMemRegionBase|0x75030000000
  gHisiTokenSpaceGuid.PcdHb1Rb5CpuMemRegionBase|0x79040000000
  gHisiTokenSpaceGuid.PcdHb1Rb6CpuMemRegionBase|0x408AA400000
  gHisiTokenSpaceGuid.PcdHb1Rb7CpuMemRegionBase|0x408AB400000

  gHisiTokenSpaceGuid.PcdHb0Rb0CpuIoRegionBase|0xa8ff0000
  gHisiTokenSpaceGuid.PcdHb0Rb1CpuIoRegionBase|0xa9ff0000
  gHisiTokenSpaceGuid.PcdHb0Rb2CpuIoRegionBase|0xafff0000
  gHisiTokenSpaceGuid.PcdHb0Rb3CpuIoRegionBase|0xabff0000
  gHisiTokenSpaceGuid.PcdHb0Rb4CpuIoRegionBase|0x8abff0000
  gHisiTokenSpaceGuid.PcdHb0Rb5CpuIoRegionBase|0x8b7ff0000
  gHisiTokenSpaceGuid.PcdHb0Rb6CpuIoRegionBase|0x8afff0000
  gHisiTokenSpaceGuid.PcdHb0Rb7CpuIoRegionBase|0x8bfff0000
  gHisiTokenSpaceGuid.PcdHb1Rb0CpuIoRegionBase|0x400a8ff0000
  gHisiTokenSpaceGuid.PcdHb1Rb1CpuIoRegionBase|0x400a9ff0000
  gHisiTokenSpaceGuid.PcdHb1Rb2CpuIoRegionBase|0x67fffff0000
  gHisiTokenSpaceGuid.PcdHb1Rb3CpuIoRegionBase|0x400abff0000
  gHisiTokenSpaceGuid.PcdHb1Rb4CpuIoRegionBase|0x77fffff0000
  gHisiTokenSpaceGuid.PcdHb1Rb5CpuIoRegionBase|0x7bfffff0000
  gHisiTokenSpaceGuid.PcdHb1Rb6CpuIoRegionBase|0x408aaff0000
  gHisiTokenSpaceGuid.PcdHb1Rb7CpuIoRegionBase|0x408abff0000

  gHisiTokenSpaceGuid.PcdHb0Rb0IoBase|0
  gHisiTokenSpaceGuid.PcdHb0Rb0IoSize|0xffff #64K

  gHisiTokenSpaceGuid.PcdHb0Rb1IoBase|0
  gHisiTokenSpaceGuid.PcdHb0Rb1IoSize|0xffff #64K

  gHisiTokenSpaceGuid.PcdHb0Rb2IoBase|0
  gHisiTokenSpaceGuid.PcdHb0Rb2IoSize|0xffff #64K

  gHisiTokenSpaceGuid.PcdHb0Rb3IoBase|0
  gHisiTokenSpaceGuid.PcdHb0Rb3IoSize|0xffff #64K

  gHisiTokenSpaceGuid.PcdHb0Rb4IoBase|0
  gHisiTokenSpaceGuid.PcdHb0Rb4IoSize|0xffff #64K

  gHisiTokenSpaceGuid.PcdHb0Rb5IoBase|0
  gHisiTokenSpaceGuid.PcdHb0Rb5IoSize|0xffff #64K

  gHisiTokenSpaceGuid.PcdHb0Rb6IoBase|0
  gHisiTokenSpaceGuid.PcdHb0Rb6IoSize|0xffff #64K

  gHisiTokenSpaceGuid.PcdHb0Rb7IoBase|0
  gHisiTokenSpaceGuid.PcdHb0Rb7IoSize|0xffff #64K

  gHisiTokenSpaceGuid.PcdHb1Rb0IoBase|0
  gHisiTokenSpaceGuid.PcdHb1Rb0IoSize|0xffff #64K

  gHisiTokenSpaceGuid.PcdHb1Rb1IoBase|0
  gHisiTokenSpaceGuid.PcdHb1Rb1IoSize|0xffff #64K

  gHisiTokenSpaceGuid.PcdHb1Rb2IoBase|0
  gHisiTokenSpaceGuid.PcdHb1Rb2IoSize|0xffff #64K

  gHisiTokenSpaceGuid.PcdHb1Rb3IoBase|0
  gHisiTokenSpaceGuid.PcdHb1Rb3IoSize|0xffff #64K

  gHisiTokenSpaceGuid.PcdHb1Rb4IoBase|0
  gHisiTokenSpaceGuid.PcdHb1Rb4IoSize|0xffff #64K

  gHisiTokenSpaceGuid.PcdHb1Rb5IoBase|0
  gHisiTokenSpaceGuid.PcdHb1Rb5IoSize|0xffff #64K

  gHisiTokenSpaceGuid.PcdHb1Rb6IoBase|0
  gHisiTokenSpaceGuid.PcdHb1Rb6IoSize|0xffff #64K

  gHisiTokenSpaceGuid.PcdHb1Rb7IoBase|0
  gHisiTokenSpaceGuid.PcdHb1Rb7IoSize|0xffff #64K

  gHisiTokenSpaceGuid.Pcdsoctype|0x1610

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
  MdeModulePkg/Universal/PCD/Pei/Pcd.inf
  Platform/Hisilicon/D05/Drivers/Ipmi/IpmiInterfacePei/IpmiInterfacePei.inf

  ArmPlatformPkg/PlatformPei/PlatformPeim.inf

  Platform/Hisilicon/D05/MemoryInitPei/MemoryInitPeim.inf
  ArmPkg/Drivers/CpuPei/CpuPei.inf
  IntelFrameworkModulePkg/Universal/StatusCode/Pei/StatusCodePei.inf
  MdeModulePkg/Universal/FaultTolerantWritePei/FaultTolerantWritePei.inf
  MdeModulePkg/Universal/Variable/Pei/VariablePei.inf

  Platform/Hisilicon/D05/EarlyConfigPeim/EarlyConfigPeimD05.inf
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
  MdeModulePkg/Universal/PCD/Dxe/Pcd.inf

  Silicon/Hisilicon/Hi1610/Drivers/IoInitDxe/IoInitDxe.inf

  #
  # Architectural Protocols
  #
  ArmPkg/Drivers/CpuDxe/CpuDxe.inf
  MdeModulePkg/Core/RuntimeDxe/RuntimeDxe.inf

  Platform/Hisilicon/D03/Drivers/OemNicConfig2PHi1610/OemNicConfig2P.inf

  Platform/Hisilicon/D05/Drivers/SFC/SfcDxeDriver.inf

  MdeModulePkg/Universal/SecurityStubDxe/SecurityStubDxe.inf
  # Sometimes we can use EmuVariableRuntimeDxe instead of real flash variable store for debug.
  #MdeModulePkg/Universal/Variable/EmuRuntimeDxe/EmuVariableRuntimeDxe.inf
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
  Silicon/Hisilicon/Drivers/HisiAcpiPlatformDxe/AcpiPlatformDxe.inf
  MdeModulePkg/Universal/Acpi/AcpiTableDxe/AcpiTableDxe.inf

  Silicon/Hisilicon/Hi1616/D05AcpiTables/AcpiTablesHi1616.inf
  Silicon/Hisilicon/Drivers/AcpiPlatformDxe/AcpiPlatformDxe.inf

  #
  # Usb Support
  #
  Silicon/Hisilicon/Drivers/VirtualEhciPciIo/VirtualEhciPciIo.inf
  MdeModulePkg/Bus/Pci/EhciDxe/EhciDxe.inf
  MdeModulePkg/Bus/Pci/XhciDxe/XhciDxe.inf
  MdeModulePkg/Bus/Usb/UsbBusDxe/UsbBusDxe.inf
  MdeModulePkg/Bus/Usb/UsbKbDxe/UsbKbDxe.inf
  MdeModulePkg/Bus/Usb/UsbMouseDxe/UsbMouseDxe.inf
  MdeModulePkg/Bus/Usb/UsbMassStorageDxe/UsbMassStorageDxe.inf

  Platform/Hisilicon/D05/Drivers/Ipmi/IpmiInterfaceDxe/IpmiInterfaceDxe.inf

  #
  #network
  #
  Platform/Hisilicon/D05/Drivers/Net/SnpPV660Dxe/SnpPV600Dxe.inf

  MdeModulePkg/Universal/Network/ArpDxe/ArpDxe.inf
  MdeModulePkg/Universal/Network/Dhcp4Dxe/Dhcp4Dxe.inf
  MdeModulePkg/Universal/Network/DpcDxe/DpcDxe.inf
  MdeModulePkg/Universal/Network/Ip4Dxe/Ip4Dxe.inf
  MdeModulePkg/Universal/Network/MnpDxe/MnpDxe.inf
  MdeModulePkg/Universal/Network/Mtftp4Dxe/Mtftp4Dxe.inf
  MdeModulePkg/Universal/Network/Udp4Dxe/Udp4Dxe.inf
!if $(NETWORK_IP6_ENABLE) == TRUE
  NetworkPkg/Ip6Dxe/Ip6Dxe.inf
  NetworkPkg/TcpDxe/TcpDxe.inf
  NetworkPkg/Udp6Dxe/Udp6Dxe.inf
  NetworkPkg/Dhcp6Dxe/Dhcp6Dxe.inf
  NetworkPkg/Mtftp6Dxe/Mtftp6Dxe.inf
  NetworkPkg/UefiPxeBcDxe/UefiPxeBcDxe.inf
!else
  MdeModulePkg/Universal/Network/Tcp4Dxe/Tcp4Dxe.inf
  MdeModulePkg/Universal/Network/UefiPxeBcDxe/UefiPxeBcDxe.inf
!endif
  MdeModulePkg/Universal/Network/IScsiDxe/IScsiDxe.inf
!if $(HTTP_BOOT_ENABLE) == TRUE
  NetworkPkg/DnsDxe/DnsDxe.inf
  NetworkPkg/HttpUtilitiesDxe/HttpUtilitiesDxe.inf
  NetworkPkg/HttpDxe/HttpDxe.inf
  NetworkPkg/HttpBootDxe/HttpBootDxe.inf
!endif


  Platform/Hisilicon/D05/Drivers/Sas/SasDxeDriver.inf

  #
  # FAT filesystem + GPT/MBR partitioning
  #

  MdeModulePkg/Universal/Disk/DiskIoDxe/DiskIoDxe.inf
  MdeModulePkg/Universal/Disk/PartitionDxe/PartitionDxe.inf
  MdeModulePkg/Universal/Disk/UnicodeCollation/EnglishDxe/EnglishDxe.inf

  #
  # Bds
  #
  MdeModulePkg/Universal/DevicePathDxe/DevicePathDxe.inf

  Platform/Hisilicon/D05/Drivers/GetInfoFromBmc/GetInfoFromBmc.inf
  Platform/Hisilicon/D05/Drivers/TransferSmbiosInfo/TransSmbiosInfo.inf
  Platform/Hisilicon/D05/Drivers/IpmiMiscOpDxe/IpmiMiscOpDxe.inf

  Platform/Hisilicon/D05/Drivers/IpmiWatchdogDxe/IpmiWatchdogDxe.inf

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
      NULL|Platform/Hisilicon/D05/Library/PlatformPciLib/PlatformPciLib.inf
  }
  Platform/Hisilicon/D03/Drivers/PciPlatform/PciPlatform.inf
  Silicon/Hisilicon/Drivers/PciHostBridgeDxe/PciHostBridgeDxe.inf {
    <LibraryClasses>
      NULL|Platform/Hisilicon/D05/Library/PlatformPciLib/PlatformPciLib.inf
  }

  MdeModulePkg/Bus/Pci/PciBusDxe/PciBusDxe.inf

  Platform/Hisilicon/D05/Drivers/ReportPciePlugDidVidToBmc/ReportPciePlugDidVidToBmc.inf
  Silicon/Hisilicon/Drivers/Smbios/AddSmbiosType9/AddSmbiosType9.inf
  Platform/Hisilicon/D05/Drivers/Sm750Dxe/UefiSmi.inf
  MdeModulePkg/Bus/Pci/NvmExpressDxe/NvmExpressDxe.inf
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
!if $(NETWORK_IP6_ENABLE) == TRUE
      NULL|ShellPkg/Library/UefiShellNetwork2CommandsLib/UefiShellNetwork2CommandsLib.inf
!endif

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
