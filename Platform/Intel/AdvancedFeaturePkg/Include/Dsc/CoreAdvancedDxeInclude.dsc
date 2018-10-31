## @file
#  Platform description.
#
# Copyright (c) 2017 - 2018, Intel Corporation. All rights reserved.<BR>
#
# This program and the accompanying materials are licensed and made available under
# the terms and conditions of the BSD License which accompanies this distribution.
# The full text of the license may be found at
# http://opensource.org/licenses/bsd-license.php
#
# THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
# WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#
##


  #
  # UEFI network modules
  #
!if gAdvancedFeaturePkgTokenSpaceGuid.PcdNetworkEnable == TRUE
  MdeModulePkg/Universal/Network/DpcDxe/DpcDxe.inf
  MdeModulePkg/Universal/Network/SnpDxe/SnpDxe.inf
  MdeModulePkg/Universal/Network/MnpDxe/MnpDxe.inf
  MdeModulePkg/Universal/Network/VlanConfigDxe/VlanConfigDxe.inf
  MdeModulePkg/Universal/Network/ArpDxe/ArpDxe.inf
  MdeModulePkg/Universal/Network/Dhcp4Dxe/Dhcp4Dxe.inf
  MdeModulePkg/Universal/Network/Ip4Dxe/Ip4Dxe.inf
  MdeModulePkg/Universal/Network/Mtftp4Dxe/Mtftp4Dxe.inf
  MdeModulePkg/Universal/Network/Udp4Dxe/Udp4Dxe.inf

  NetworkPkg/Ip6Dxe/Ip6Dxe.inf
  NetworkPkg/TcpDxe/TcpDxe.inf
  NetworkPkg/Udp6Dxe/Udp6Dxe.inf
  NetworkPkg/Dhcp6Dxe/Dhcp6Dxe.inf
  NetworkPkg/Mtftp6Dxe/Mtftp6Dxe.inf

  NetworkPkg/DnsDxe/DnsDxe.inf
  NetworkPkg/HttpDxe/HttpDxe.inf
  NetworkPkg/HttpUtilitiesDxe/HttpUtilitiesDxe.inf
  NetworkPkg/HttpBootDxe/HttpBootDxe.inf

  NetworkPkg/IScsiDxe/IScsiDxe.inf
  NetworkPkg/UefiPxeBcDxe/UefiPxeBcDxe.inf
!endif

!if gAdvancedFeaturePkgTokenSpaceGuid.PcdSmbiosEnable == TRUE
  MdeModulePkg/Universal/SmbiosDxe/SmbiosDxe.inf
!endif

