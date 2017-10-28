/** @file
  Data structure for passing DRAM information from lower level firmware

  Copyright (c) 2017, Linaro Ltd. All rights reserved.<BR>

  This program and the accompanying materials are licensed and made available
  under the terms and conditions of the BSD License which accompanies this
  distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS, WITHOUT
  WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SYNQUACER_PLATFORM_DRAM_INFO_H_
#define _SYNQUACER_PLATFORM_DRAM_INFO_H_

typedef struct {
  UINT64            Base;
  UINT64            Size;
} DRAM_INFO_ENTRY;

typedef struct {
  UINT32            NumRegions;
  UINT32            Reserved;
  DRAM_INFO_ENTRY   Entry[3];
} DRAM_INFO;

#endif
