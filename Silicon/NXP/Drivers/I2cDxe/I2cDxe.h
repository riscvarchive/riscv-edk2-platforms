/** I2cDxe.h
  Header defining the constant, base address amd function for I2C controller

  Copyright 2017-2019 NXP

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef I2C_DXE_H_
#define I2C_DXE_H_

#include <Library/UefiLib.h>
#include <Uefi.h>

#include <Protocol/I2cMaster.h>
#include <Protocol/NonDiscoverableDevice.h>

#define I2C_CR_IIEN               (1 << 6)
#define I2C_CR_MSTA               (1 << 5)
#define I2C_CR_MTX                (1 << 4)
#define I2C_CR_TX_NO_AK           (1 << 3)
#define I2C_CR_RSTA               (1 << 2)

#define I2C_SR_ICF                (1 << 7)
#define I2C_SR_IBB                (1 << 5)
#define I2C_SR_IAL                (1 << 4)
#define I2C_SR_IIF                (1 << 1)
#define I2C_SR_RX_NO_AK           (1 << 0)

#define I2C_CR_IEN                (0 << 7)
#define I2C_CR_IDIS               (1 << 7)
#define I2C_SR_IIF_CLEAR          (1 << 1)

#define BUS_IDLE                  (0 | (I2C_SR_IBB << 8))
#define BUS_BUSY                  (I2C_SR_IBB | (I2C_SR_IBB << 8))
#define IIF                       (I2C_SR_IIF | (I2C_SR_IIF << 8))

#define I2C_FLAG_WRITE            0x0

#define I2C_STATE_RETRIES         50000

#define RETRY_COUNT               3

#define NXP_I2C_SIGNATURE         SIGNATURE_32 ('N', 'I', '2', 'C')
#define NXP_I2C_FROM_THIS(a)      CR ((a), NXP_I2C_MASTER, \
                                    I2cMaster, NXP_I2C_SIGNATURE)

extern EFI_COMPONENT_NAME2_PROTOCOL gNxpI2cDriverComponentName2;

#pragma pack(1)
typedef struct {
  VENDOR_DEVICE_PATH              Vendor;
  UINT64                          MmioBase;
  EFI_DEVICE_PATH_PROTOCOL        End;
} NXP_I2C_DEVICE_PATH;
#pragma pack()

typedef struct {
  UINT32                          Signature;
  EFI_I2C_MASTER_PROTOCOL         I2cMaster;
  NXP_I2C_DEVICE_PATH             DevicePath;
  NON_DISCOVERABLE_DEVICE         *Dev;
} NXP_I2C_MASTER;

/**
  Record defining i2c registers
**/
typedef struct {
  UINT8     I2cAdr;
  UINT8     I2cFdr;
  UINT8     I2cCr;
  UINT8     I2cSr;
  UINT8     I2cDr;
} I2C_REGS;

typedef struct {
  UINT16   SCLDivider;
  UINT16   BusClockRate;
} CLK_DIV;

extern
UINT64
GetBusFrequency (
  VOID
  );

EFI_STATUS
NxpI2cInit (
  IN EFI_HANDLE  DriverBindingHandle,
  IN EFI_HANDLE  ControllerHandle
  );

EFI_STATUS
NxpI2cRelease (
  IN EFI_HANDLE  DriverBindingHandle,
  IN EFI_HANDLE  ControllerHandle
  );

#endif //I2C_DXE_H_
