/** @file

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef __TCO_WDT_HOB_H__
#define __TCO_WDT_HOB_H__

#define TCO_WDT_HOB_GUID \
  { \
    0x3e405418, 0xd8c, 0x4f1a, { 0xb0, 0x55, 0xbe, 0xf9, 0x8, 0x41, 0x46, 0x8d } \
  }

#ifndef _PEI_HOB_H_
#ifndef __HOB__H__
#ifndef __PI_HOB_H__
typedef struct _EFI_HOB_GENERIC_HEADER {
  UINT16  HobType;
  UINT16  HobLength;
  UINT32  Reserved;
} EFI_HOB_GENERIC_HEADER;

typedef struct _EFI_HOB_GUID_TYPE {
  EFI_HOB_GENERIC_HEADER  Header;
  EFI_GUID                Name;
  //
  // Guid specific data goes here
  //
} EFI_HOB_GUID_TYPE;
#endif
#endif
#endif

typedef struct {
  EFI_HOB_GUID_TYPE Header;
  UINT8             TcoRebootHappened;
} TCO_WDT_HOB;

#endif

