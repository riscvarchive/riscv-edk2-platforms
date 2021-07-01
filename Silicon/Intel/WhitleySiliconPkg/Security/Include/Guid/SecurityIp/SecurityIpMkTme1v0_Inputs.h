/** @file
  Provides data structure information used by SiliconIp MK-TME

  @copyright
  Copyright 2020 - 2021 Intel Corporation. <BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

//
// TME
//
UINT8  EnableTme;                      // TME Enable
UINT8  EnableTmeCR;                    // Exclude Crystal Ridge memory from encryption.

//
// MK-TME
//
UINT8  EnableMktme;                    // MK-TME Enable

UINT8  ReservedS234;
UINT8  ReservedS235;
UINT64 ReservedS236;
UINT64 ReservedS237;
UINT8  ReservedS238;
