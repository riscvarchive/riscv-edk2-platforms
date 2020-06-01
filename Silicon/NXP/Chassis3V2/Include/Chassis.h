/** @file

  Copyright 2018-2020 NXP

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef CHASSIS_H__
#define CHASSIS_H__

#include <Uefi.h>

#define  NXP_LAYERSCAPE_CHASSIS3V2_DCFG_ADDRESS  0x1E00000

/**
  The Device Configuration Unit provides general purpose configuration and
  status for the device. These registers only support 32-bit accesses.
**/
#pragma pack(1)
typedef struct {
  UINT8   Reserved0[0x100 - 0x0];
  UINT32  RcwSr[32]; // Reset Control Word Status Register
} NXP_LAYERSCAPE_CHASSIS3V2_DEVICE_CONFIG;
#pragma pack()

#endif // CHASSIS_H__
