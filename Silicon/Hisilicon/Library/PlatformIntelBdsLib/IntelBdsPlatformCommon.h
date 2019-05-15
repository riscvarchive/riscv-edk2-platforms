/** @file
*
*  Copyright (c) 2015, Hisilicon Limited. All rights reserved.
*  Copyright (c) 2015, Linaro Limited. All rights reserved.
*
*  SPDX-License-Identifier: BSD-2-Clause-Patent
*
**/

#ifndef _HW_BDS_LIB_H_
#define _HW_BDS_LIB_H_

EFI_STATUS
EFIAPI
HwBdsLibRegisterAppBootOption (
  IN OUT LIST_ENTRY              *BdsBootOptionList,
  IN GUID                        *FileGuid,
  IN CHAR16                      *Description
  );

#endif
