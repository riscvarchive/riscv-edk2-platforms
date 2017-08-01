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

#ifndef OGMA_MISC_INTERNAL_H
#define OGMA_MISC_INTERNAL_H

#include "netsec_for_uefi/netsec_sdk/include/ogma_basic_type.h"
#include "netsec_for_uefi/netsec_sdk/include/ogma_api.h"

#define OGMA_CLK_EN_REG_DOM_ALL 0x24

#define OGMA_PKT_IRQ_ALL ( OGMA_PKT_IRQ_MAC_ER    | OGMA_PKT_IRQ_JUMBO_ER      | \
                           OGMA_PKT_IRQ_CHKSUM_ER | OGMA_PKT_IRQ_HD_INCOMPLETE | \
                           OGMA_PKT_IRQ_HD_ER     | OGMA_PKT_IRQ_DRP_NO_MATCH)

typedef struct ogma_global_s ogma_global_t;

struct ogma_global_s{
    ogma_uint valid_flag:1;

    ogma_uint8 list_entry_num;

    ogma_ctrl_t *list_head_p;
};

#endif /* OGMA_MISC_INTERNAL_H */
