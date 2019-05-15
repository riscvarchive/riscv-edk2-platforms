#
#  Copyright (c) 2012-2014, ARM Limited. All rights reserved.
#  
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#

  #
  # Networking stack
  #
  INF MdeModulePkg/Universal/Network/DpcDxe/DpcDxe.inf
  INF MdeModulePkg/Universal/Network/ArpDxe/ArpDxe.inf
  INF MdeModulePkg/Universal/Network/Dhcp4Dxe/Dhcp4Dxe.inf
  INF MdeModulePkg/Universal/Network/Ip4Dxe/Ip4Dxe.inf
  INF MdeModulePkg/Universal/Network/MnpDxe/MnpDxe.inf
  INF MdeModulePkg/Universal/Network/VlanConfigDxe/VlanConfigDxe.inf
  INF MdeModulePkg/Universal/Network/Mtftp4Dxe/Mtftp4Dxe.inf
  INF NetworkPkg/UefiPxeBcDxe/UefiPxeBcDxe.inf
  INF NetworkPkg/TcpDxe/TcpDxe.inf
  INF NetworkPkg/IScsiDxe/IScsiDxe.inf
  INF MdeModulePkg/Universal/Network/Udp4Dxe/Udp4Dxe.inf

!if $(INCLUDE_TFTP_COMMAND) == TRUE
  #
  # TFTP Shell command
  #
  INF ShellPkg/DynamicCommand/TftpDynamicCommand/TftpDynamicCommand.inf
!endif
