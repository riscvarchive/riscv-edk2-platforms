/** @file
  Provides data structure information used by SiliconIp MK-TME

  @copyright
  Copyright 2020 - 2021 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

//
// MK-TME
//
// NAK - Not a knob, used just for indication
UINT8  TmeCapability; // TME Capable
UINT8  TmeCrSupport; // Flag used to check if Crystal Ridge is supported in UEFI
UINT8  MktmeCapability; // MK-TME Capable
UINT16 MktmeMaxKeys; // Max number of keys used for encryption
UINT8  MkTmeKeyIdBits; // Used to suppress setup menu key-splits