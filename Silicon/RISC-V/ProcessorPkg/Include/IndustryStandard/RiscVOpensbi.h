/** @file
  SBI inline function calls.

  Copyright (c) 2020, Hewlett Packard Enterprise Development LP. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef EDK2_SBI_H_
#define EDK2_SBI_H_

#include <RiscVImpl.h>
#include <sbi/riscv_asm.h>
#include <sbi/sbi_ecall_interface.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_platform.h>
#include <sbi/sbi_scratch.h>
#include <sbi/sbi_types.h>

// Translation from OpenSBI constants to SBI names
#define SBI_SUCCESS               SBI_OK
#define SBI_ERR_FAILED            SBI_EFAIL
#define SBI_ERR_NOT_SUPPORTED     SBI_ENOTSUPP
#define SBI_ERR_INVALID_PARAM     SBI_EINVAL
#define SBI_ERR_DENIED            SBI_DENIED
#define SBI_ERR_INVALID_ADDRESS   SBI_INVALID_ADDR
// Currently improperly defined in upstream OpenSBI
#define SBI_ERR_ALREADY_AVAILABLE -6

// Included in OpenSBI 0.7
// Can be removed, once we upgrade
#define SBI_EXT_HSM               0x48534D
#define SBI_EXT_HSM_HART_START    0x0
#define SBI_EXT_HSM_HART_STOP     0x1
#define SBI_EXT_HSM_HART_GET_STATUS 0x2

//
// Below two definitions should be defined in OpenSBI.
// Submitted to upstream, waiting for merge and release.
//
#define SBI_EXT_FIRMWARE_CODE_BASE_START 0x0A000000
#define SBI_EXT_FIRMWARE_CODE_BASE_END   0x0AFFFFFF

#define RISC_V_MAX_HART_SUPPORTED 16

typedef
VOID
(EFIAPI *RISCV_HART_SWITCH_MODE)(
  IN  UINTN   FuncArg0,
  IN  UINTN   FuncArg1,
  IN  UINTN   NextAddr,
  IN  UINTN   NextMode,
  IN  BOOLEAN NextVirt
  );

//
// Keep the structure member in 64-bit alignment.
//
typedef struct {
    UINT64                 IsaExtensionSupported;  // The ISA extension this core supported.
    RISCV_UINT128          MachineVendorId;        // Machine vendor ID
    RISCV_UINT128          MachineArchId;          // Machine Architecture ID
    RISCV_UINT128          MachineImplId;          // Machine Implementation ID
    RISCV_HART_SWITCH_MODE HartSwitchMode;         // OpenSBI's function to switch the mode of a hart
} EFI_RISCV_FIRMWARE_CONTEXT_HART_SPECIFIC;
#define FIRMWARE_CONTEXT_HART_SPECIFIC_SIZE  (64 * 8) // This is the size of EFI_RISCV_FIRMWARE_CONTEXT_HART_SPECIFIC
                                                      // structure. Referred by both C code and assembly code.

typedef struct {
  VOID            *PeiServiceTable;       // PEI Service table
  EFI_RISCV_FIRMWARE_CONTEXT_HART_SPECIFIC  *HartSpecific[RISC_V_MAX_HART_SUPPORTED];
} EFI_RISCV_OPENSBI_FIRMWARE_CONTEXT;

//
// Typedefs of OpenSBI type to make them conform to EDK2 coding guidelines
//
typedef struct sbi_scratch SBI_SCRATCH;
typedef struct sbi_platform SBI_PLATFORM;

#endif
