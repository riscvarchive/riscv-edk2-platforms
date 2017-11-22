/** @file
*  Header defining the structure for Dram Information
*
*  Copyright 2019 NXP
*
*  SPDX-License-Identifier: BSD-2-Clause-Patent
*
**/

#ifndef DRAM_INFO_H_
#define DRAM_INFO_H_

#include <Uefi/UefiBaseType.h>

#define SMC_DRAM_BANK_INFO          (0xC200FF12)

typedef struct {
  UINTN            BaseAddress;
  UINTN            Size;
} DRAM_REGION_INFO;

typedef struct {
  UINT32            NumOfDrams;
  UINT32            Reserved;
  DRAM_REGION_INFO  DramRegion[3];
} DRAM_INFO;

EFI_STATUS
GetDramBankInfo (
  IN OUT DRAM_INFO *DramInfo
  );

VOID
UpdateDpaaDram (
  IN OUT DRAM_INFO *DramInfo
  );

#endif /* DRAM_INFO_H_ */
