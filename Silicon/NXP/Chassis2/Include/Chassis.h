/** @file

  Copyright 2020 NXP

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef CHASSIS_H__
#define CHASSIS_H__

#include <Uefi.h>

#define  NXP_LAYERSCAPE_CHASSIS2_DCFG_ADDRESS  0x1EE0000

#define SVR_SOC_VER(svr)            (((svr) >> 8) & 0xFFFFFE)
#define SVR_MAJOR(svr)              (((svr) >> 4) & 0xf)
#define SVR_MINOR(svr)              (((svr) >> 0) & 0xf)

/* SMMU Defintions */
#define SMMU_BASE_ADDR             0x09000000
#define SMMU_REG_SCR0              (SMMU_BASE_ADDR + 0x0)
#define SMMU_REG_SACR              (SMMU_BASE_ADDR + 0x10)
#define SMMU_REG_NSCR0             (SMMU_BASE_ADDR + 0x400)

#define SCR0_USFCFG_MASK           0x00000400
#define SCR0_CLIENTPD_MASK         0x00000001
#define SACR_PAGESIZE_MASK         0x00010000

/**
  The Device Configuration Unit provides general purpose configuration and
  status for the device. These registers only support 32-bit accesses.
**/
#pragma pack(1)
typedef struct {
  UINT8   Reserved0[0x70 - 0x0];
  UINT32  DeviceDisableRegister1;  // Device Disable Register 1
  UINT32  DeviceDisableRegister2;  // Device Disable Register 2
  UINT32  DeviceDisableRegister3;  // Device Disable Register 3
  UINT32  DeviceDisableRegister4;  // Device Disable Register 4
  UINT32  DeviceDisableRegister5;  // Device Disable Register 5
  UINT8   Reserved1[0xa4 - 0x84];
  UINT32  Svr;                     // System Version Register
  UINT8   Reserved2[0x100 - 0xa8];
  UINT32  RcwSr[16]; // Reset Control Word Status Register
} NXP_LAYERSCAPE_CHASSIS2_DEVICE_CONFIG;
#pragma pack()

#endif // CHASSIS_H__
