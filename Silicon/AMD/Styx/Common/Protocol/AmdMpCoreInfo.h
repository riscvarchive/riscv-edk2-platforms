/** @file

  Copyright (c) 2016, AMD Inc. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _AMD_MP_CORE_INFO_H_
#define _AMD_MP_CORE_INFO_H_

extern EFI_GUID gAmdMpCoreInfoProtocolGuid;

typedef
ARM_CORE_INFO *
(EFIAPI *GET_ARM_CORE_INFO_TABLE) (
  OUT UINTN  *NumEntries
  );

typedef
EFI_STATUS
(EFIAPI *GET_PMU_SPI_FROM_MPID) (
  IN  UINT32  MpId,
  OUT UINT32  *PmuSpi
  );

typedef struct _AMD_MP_CORE_INFO_PROTOCOL {
  GET_ARM_CORE_INFO_TABLE  GetArmCoreInfoTable;
  GET_PMU_SPI_FROM_MPID    GetPmuSpiFromMpId;
} AMD_MP_CORE_INFO_PROTOCOL;

#endif // _AMD_MP_CORE_INFO_H_
