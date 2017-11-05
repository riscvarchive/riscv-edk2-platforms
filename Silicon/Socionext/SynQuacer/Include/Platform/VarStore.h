/** @file

  Copyright (c) 2017, Linaro, Ltd. All rights reserved.<BR>

  This program and the accompanying materials are licensed and made available
  under the terms and conditions of the BSD License which accompanies this
  distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
**/

#ifndef __VARSTORE_H__
#define __VARSTORE_H__

#define SYNQUACER_PLATFORM_VARIABLE_NAME  L"SynQuacerPlatformSettings"

#define EMMC_DISABLED                     0x0
#define EMMC_ENABLED                      0x1

#define PCIE_MAX_SPEED_UNLIMITED          0x0
#define PCIE_MAX_SPEED_GEN1               0x1

typedef struct {
  UINT8         EnableEmmc;
  UINT8         PcieSlot0MaxSpeed;
  UINT8         PcieSlot1MaxSpeed;
  UINT8         PcieSlot2MaxSpeed;
  UINT8         Reserved[4];
} SYNQUACER_PLATFORM_VARSTORE_DATA;

#endif
