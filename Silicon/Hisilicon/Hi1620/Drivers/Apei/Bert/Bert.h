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

#ifndef _BERT_H_
#define _BERT_H_

#include "Apei.h"
#include <Guid/Cper.h>

typedef struct _BERT_CONTEXT {
  EFI_ACPI_6_0_BOOT_ERROR_RECORD_TABLE_HEADER   *BertHeader;
  VOID                                          *Block;
  UINT32                                        BlockSize;
} BERT_CONTEXT;

EFI_STATUS
OemInitBertTable (
  IN EFI_HANDLE    ImageHandle
);
VOID
BertSetAcpiTable (
  IN BERT_CONTEXT *Context
);
EFI_STATUS
BertHeaderCreator (
  BERT_CONTEXT  *Context,
  UINT32        ErrorBlockSize
);


#endif    // _BERT_H_
