/** @file
*
*  Copyright (c) 2017, Hisilicon Limited. All rights reserved.
*  Copyright (c) 2017, Linaro Limited. All rights reserved.
*
*  This program and the accompanying materials
*  are licensed and made available under the terms and conditions of the BSD License
*  which accompanies this distribution.  The full text of the license may be found at
*  http://opensource.org/licenses/bsd-license.php
*
*  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
*  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
*
**/

#ifndef _SNP_PLATFORM_PROTOCOL_H_
#define _SNP_PLATFORM_PROTOCOL_H_
#define HISI_SNP_PLATFORM_PROTOCOL_GUID \
  { \
    0x81321f27, 0xff58, 0x4a1d, 0x99, 0x97, 0xd, 0xcc, 0xfa, 0x82, 0xf4, 0x6f  \
  }

typedef struct _HISI_PLATFORM_SNP_PROTOCOL  HISI_PLATFORM_SNP_PROTOCOL;

struct _HISI_PLATFORM_SNP_PROTOCOL {
  UINT32        ControllerId;
  UINT32        Enable;
};

extern EFI_GUID gHisiSnpPlatformProtocolGuid;

#endif
