/** @file
 *
 *  Copyright 2017-2019 NXP
 *
 * SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 **/

#ifndef IO_ACCESS_LIB_H_
#define IO_ACCESS_LIB_H_

#include <Base.h>

///
///  Structure to have pointer to R/W
///  Mmio operations for 16 bits.
///
typedef struct _MMIO_OPERATIONS_16 {
  UINT16 (*Read16) (UINTN Address);
  UINT16 (*Write16) (UINTN Address, UINT16 Value);
  UINT16 (*Or16) (UINTN Address, UINT16 OrData);
  UINT16 (*And16) (UINTN Address, UINT16 AndData);
  UINT16 (*AndThenOr16) (UINTN Address, UINT16 AndData, UINT16 OrData);
} MMIO_OPERATIONS_16;

///
///  Structure to have pointer to R/W
///  Mmio operations for 32 bits.
///
typedef struct _MMIO_OPERATIONS_32 {
  UINT32 (*Read32) (UINTN Address);
  UINT32 (*Write32) (UINTN Address, UINT32 Value);
  UINT32 (*Or32) (UINTN Address, UINT32 OrData);
  UINT32 (*And32) (UINTN Address, UINT32 AndData);
  UINT32 (*AndThenOr32) (UINTN Address, UINT32 AndData, UINT32 OrData);
} MMIO_OPERATIONS_32;

///
///  Structure to have pointer to R/W
///  Mmio operations for 64 bits.
///
typedef struct _MMIO_OPERATIONS_64 {
  UINT64 (*Read64) (UINTN Address);
  UINT64 (*Write64) (UINTN Address, UINT64 Value);
  UINT64 (*Or64) (UINTN Address, UINT64 OrData);
  UINT64 (*And64) (UINTN Address, UINT64 AndData);
  UINT64 (*AndThenOr64) (UINTN Address, UINT64 AndData, UINT64 OrData);
} MMIO_OPERATIONS_64;

/**
  Function to return pointer to 16 bit Mmio operations.

  @param  Swap  Flag to tell if Swap is needed or not
                on Mmio Operations.

  @return       Pointer to Mmio Operations.

**/
MMIO_OPERATIONS_16 *
GetMmioOperations16  (
  IN  BOOLEAN  Swap
  );

/**
  Function to return pointer to 32 bit Mmio operations.

  @param  Swap  Flag to tell if Swap is needed or not
                on Mmio Operations.

  @return       Pointer to Mmio Operations.

**/
MMIO_OPERATIONS_32 *
GetMmioOperations32  (
  IN  BOOLEAN  Swap
  );

/**
  Function to return pointer to 64 bit Mmio operations.

  @param  Swap  Flag to tell if Swap is needed or not
                on Mmio Operations.

  @return       Pointer to Mmio Operations.

**/
MMIO_OPERATIONS_64 *
GetMmioOperations64  (
  IN  BOOLEAN  Swap
  );

/**
  MmioRead16 for Big-Endian modules.

  @param  Address The MMIO register to read.

  @return The value read.

**/
UINT16
EFIAPI
SwapMmioRead16 (
  IN  UINTN     Address
  );

/**
  MmioRead32 for Big-Endian modules.

  @param  Address The MMIO register to read.

  @return The value read.

**/
UINT32
EFIAPI
SwapMmioRead32 (
  IN  UINTN     Address
  );

/**
  MmioRead64 for Big-Endian modules.

  @param  Address The MMIO register to read.

  @return The value read.

**/
UINT64
EFIAPI
SwapMmioRead64 (
  IN  UINTN     Address
  );

/**
  MmioWrite16 for Big-Endian modules.

  @param  Address The MMIO register to write.
  @param  Value   The value to write to the MMIO register.

**/
UINT16
EFIAPI
SwapMmioWrite16 (
  IN  UINTN     Address,
  IN  UINT16    Value
  );

/**
  MmioWrite32 for Big-Endian modules.

  @param  Address The MMIO register to write.
  @param  Value   The value to write to the MMIO register.

**/
UINT32
EFIAPI
SwapMmioWrite32 (
  IN  UINTN     Address,
  IN  UINT32    Value
  );

/**
  MmioWrite64 for Big-Endian modules.

  @param  Address The MMIO register to write.
  @param  Value   The value to write to the MMIO register.

**/
UINT64
EFIAPI
SwapMmioWrite64 (
  IN  UINTN     Address,
  IN  UINT64    Value
  );

/**
  MmioAndThenOr16 for Big-Endian modules.

  @param  Address The MMIO register to write.
  @param  AndData The value to AND with the read value from the MMIO register.
  @param  OrData  The value to OR with the result of the AND operation.

  @return The value written back to the MMIO register.

**/
UINT16
EFIAPI
SwapMmioAndThenOr16 (
  IN  UINTN     Address,
  IN  UINT16    AndData,
  IN  UINT16    OrData
  );

/**
  MmioAndThenOr32 for Big-Endian modules.

  @param  Address The MMIO register to write.
  @param  AndData The value to AND with the read value from the MMIO register.
  @param  OrData  The value to OR with the result of the AND operation.

  @return The value written back to the MMIO register.

**/
UINT32
EFIAPI
SwapMmioAndThenOr32 (
  IN  UINTN     Address,
  IN  UINT32    AndData,
  IN  UINT32    OrData
  );

/**
  MmioAndThenOr64 for Big-Endian modules.

  @param  Address The MMIO register to write.
  @param  AndData The value to AND with the read value from the MMIO register.
  @param  OrData  The value to OR with the result of the AND operation.

  @return The value written back to the MMIO register.

**/
UINT64
EFIAPI
SwapMmioAndThenOr64 (
  IN  UINTN     Address,
  IN  UINT64    AndData,
  IN  UINT64    OrData
  );

/**
  MmioOr16 for Big-Endian modules.

  @param  Address The MMIO register to write.
  @param  OrData  The value to OR with the read value from the MMIO register.

  @return The value written back to the MMIO register.

**/
UINT16
EFIAPI
SwapMmioOr16 (
  IN  UINTN     Address,
  IN  UINT16    OrData
  );

/**
  MmioOr32 for Big-Endian modules.

  @param  Address The MMIO register to write.
  @param  OrData  The value to OR with the read value from the MMIO register.

  @return The value written back to the MMIO register.

**/
UINT32
EFIAPI
SwapMmioOr32 (
  IN  UINTN     Address,
  IN  UINT32    OrData
  );

/**
  MmioOr64 for Big-Endian modules.

  @param  Address The MMIO register to write.
  @param  OrData  The value to OR with the read value from the MMIO register.

  @return The value written back to the MMIO register.

**/
UINT64
EFIAPI
SwapMmioOr64 (
  IN  UINTN     Address,
  IN  UINT64    OrData
  );

/**
  MmioAnd16 for Big-Endian modules.

  @param  Address The MMIO register to write.
  @param  AndData The value to AND with the read value from the MMIO register.

  @return The value written back to the MMIO register.

**/
UINT16
EFIAPI
SwapMmioAnd16 (
  IN  UINTN     Address,
  IN  UINT16    AndData
  );

/**
  MmioAnd32 for Big-Endian modules.

  @param  Address The MMIO register to write.
  @param  AndData The value to AND with the read value from the MMIO register.

  @return The value written back to the MMIO register.

**/
UINT32
EFIAPI
SwapMmioAnd32 (
  IN  UINTN     Address,
  IN  UINT32    AndData
  );

/**
  MmioAnd64 for Big-Endian modules.

  @param  Address The MMIO register to write.
  @param  AndData The value to AND with the read value from the MMIO register.

  @return The value written back to the MMIO register.

**/
UINT64
EFIAPI
SwapMmioAnd64 (
  IN  UINTN     Address,
  IN  UINT64    AndData
  );

#endif /* IO_ACCESS_LIB_H_ */
