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

#ifndef _CPLD_IO_LIB_H_
#define _CPLD_IO_LIB_H_

VOID WriteCpldReg(UINTN ulRegAddr, UINT8 ulValue);
UINT8 ReadCpldReg(UINTN ulRegAddr);

#endif /* _CPLD_IO_LIB_H_ */
