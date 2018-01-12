/** @file
*
*  Copyright (c) 2018, Hisilicon Limited. All rights reserved.
*  Copyright (c) 2018, Linaro Limited. All rights reserved.
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

#ifndef _HISI_PLATFORM_SAS_PROTOCOL_H_
#define _HISI_PLATFORM_SAS_PROTOCOL_H_

typedef struct _HISI_PLATFORM_SAS_PROTOCOL HISI_PLATFORM_SAS_PROTOCOL;

struct _HISI_PLATFORM_SAS_PROTOCOL {
  UINT32        ControllerId;
  BOOLEAN       Enable;
  UINT64        BaseAddr;
  UINT64        ResetAddr;
};

extern EFI_GUID gHisiPlatformSasProtocolGuid;

#endif
