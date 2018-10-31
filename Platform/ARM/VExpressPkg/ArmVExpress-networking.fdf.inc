#
#  Copyright (c) 2012-2014, ARM Limited. All rights reserved.
#  
#  This program and the accompanying materials                          
#  are licensed and made available under the terms and conditions
#  of the BSD License which accompanies this distribution.
#  The full text of the license may be found at        
#  http://opensource.org/licenses/bsd-license.php                                            
#
#  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS, 
#  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
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
