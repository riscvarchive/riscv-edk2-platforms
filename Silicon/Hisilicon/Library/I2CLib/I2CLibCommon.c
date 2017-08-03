/** @file
*
*  Copyright (c) 2015, Hisilicon Limited. All rights reserved.
*  Copyright (c) 2015, Linaro Limited. All rights reserved.
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

#include <PiDxe.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>

#include <Library/PlatformSysCtrlLib.h>
#include "I2CLibInternal.h"

UINTN GetI2cBase (UINT32 Socket, UINT8 Port)
{
  return PlatformGetI2cBase(Socket, Port);
}

EFI_STATUS
I2cLibRuntimeSetup (UINT32 Socket, UINT8 Port)
{
  return EFI_SUCCESS;
}


