/** @file

  Copyright (c) 2018, Linaro Limited. All rights reserved.

  This program and the accompanying materials are licensed and made available
  under the terms and conditions of the BSD License which accompanies this
  distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __LSCONNECTOR_DXE_H__
#define __LSCONNECTOR_DXE_H__

#include <Guid/HiiPlatformSetupFormset.h>
#include <Guid/FormSet.h>

#define MEZZANINE_NONE                            0x0
#define MEZZANINE_SECURE96                        0x1
#define MEZZANINE_MAX                             0x2

#define NINETY_SIX_BOARDS_CONFIG_VARIABLE_NAME    L"NinetySixBoardsConfig"

typedef struct {
  UINT8         MezzanineType;
  UINT8         Reserved[7];
} NINETY_SIX_BOARDS_CONFIG_DATA;

#endif // __LSCONNECTOR_DXE_H__
