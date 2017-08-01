/** @file

  Copyright (c) 2016 - 2017, Socionext Inc. All rights reserved.<BR>
  Copyright (c) 2017, Linaro, Ltd. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef OGMA_BASIC_TYPE_H
#define OGMA_BASIC_TYPE_H
#include "netsec_for_uefi/pfdep.h"

/**
 *
 */
#define OGMA_TRUE  PFDEP_TRUE
#define OGMA_FALSE PFDEP_FALSE

/**
 * OGMA SDK BASIC DATA TYPE
 */
typedef pfdep_int8 ogma_int8;
typedef pfdep_uint8 ogma_uint8;
typedef pfdep_int16 ogma_int16;
typedef pfdep_uint16 ogma_uint16;
typedef pfdep_int32 ogma_int32;
typedef pfdep_uint32 ogma_uint32;
typedef int ogma_int;
typedef unsigned int ogma_uint;
typedef pfdep_bool ogma_bool;
typedef pfdep_char ogma_char;

#ifdef PFDEP_INT64_AVAILABLE
typedef signed long long ogma_int64;
typedef unsigned long long ogma_uint64;
#endif /* PFDEP_INT64_AVAILABLE*/

#endif /* OGMA_BASIC_TYPE_H */
