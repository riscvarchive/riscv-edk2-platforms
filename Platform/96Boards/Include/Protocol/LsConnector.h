/** @file

  Copyright (c) 2018, Linaro, Ltd. All rights reserved.<BR>

  This program and the accompanying materials are licensed and made available
  under the terms and conditions of the BSD License which accompanies this
  distribution. The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _LS_CONNECTOR_H_
#define _LS_CONNECTOR_H_

#define LS_CONNECTOR_PROTOCOL_GUID \
  { 0xae548d4c, 0x9062, 0x4eed, { 0x83, 0x5f, 0xf5, 0x10, 0xf8, 0xfc, 0x48, 0xaf } }

typedef struct _LS_CONNECTOR_PROTOCOL LS_CONNECTOR_PROTOCOL;

typedef enum {
  MezzanineUnknown,
  MezzanineSecure96,
  MezzanineMax
} MEZZANINE_TYPE;

struct _LS_CONNECTOR_PROTOCOL {
  MEZZANINE_TYPE      MezzanineType;
};

extern EFI_GUID g96BoardsLsConnectorProtocolGuid;

#endif // _LS_CONNECTOR_H_
