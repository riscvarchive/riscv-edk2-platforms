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



#ifndef _FDTUPDATELIB_H_
#define _FDTUPDATELIB_H_

#define ADD_FILE_LENGTH  0x400

typedef struct
{
  UINT32 BaseHigh;
  UINT32 BaseLow;
  UINT32 LengthHigh;
  UINT32 LengthLow;
}PHY_MEM_REGION;

typedef struct
{
  UINT8 data0;
  UINT8 data1;
  UINT8 data2;
  UINT8 data3;
  UINT8 data4;
  UINT8 data5;
}MAC_ADDRESS;

extern  EFI_STATUS EFIFdtUpdate(UINTN FdtFileAddr);

#endif


