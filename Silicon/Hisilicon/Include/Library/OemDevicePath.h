/** @file
*
*  Copyright (c) 2015 - 2017, Hisilicon Limited. All rights reserved.
*  Copyright (c) 2015 - 2017, Linaro Limited. All rights reserved.
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

#ifndef _OEM_DEVICE_PATH_H_
#define _OEM_DEVICE_PATH_H_
#include <Protocol/DevicePath.h>

typedef enum
{
    C_NIC  = 1,
    C_SATA = 2,
    C_SAS  = 3,
    C_USB  = 4,
} CONTROLLER_TYPE;

typedef struct{
  VENDOR_DEVICE_PATH Vender;
  UINT8 ControllerType;
  UINT8 Socket;
  UINT8 Port;
} EXT_VENDOR_DEVICE_PATH;

typedef struct{
  UINT16  BootIndex;
  UINT16  Port;
} SATADES;

typedef struct{
  UINT16  BootIndex;
  UINT16  ParentPortNumber;
  UINT16  InterfaceNumber;
} USBDES;

typedef struct{
  UINT16  BootIndex;
  UINT16  Port;
} PXEDES;

#endif

