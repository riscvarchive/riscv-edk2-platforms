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

#ifndef _I2C_LIB_INTERNAL_H_
#define _I2C_LIB_INTERNAL_H_

#include <PlatformArch.h>
#include <Library/I2CLib.h>

UINTN GetI2cBase (UINT32 Socket, UINT8 Port);

EFI_STATUS
I2cLibRuntimeSetup (UINT32 Socket, UINT8 Port);


#endif

