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

#ifndef OGMA_BASIC_ACCESS_H
#define OGMA_BASIC_ACCESS_H

#include "ogma_internal.h"

static __inline void ogma_write_reg (
    ogma_ctrl_t *ctrl_p,
    ogma_uint32 reg_addr,
    ogma_uint32 value
    )
{

    pfdep_iomem_write( (void *) ( ( pfdep_cpu_addr_t)ctrl_p->base_addr +
                                  ( reg_addr << 2) ),
                       value);
}

static __inline ogma_uint32 ogma_read_reg (
    ogma_ctrl_t *ctrl_p,
    ogma_uint32 reg_addr
    )
{

    return ( ogma_uint32)pfdep_iomem_read( (void *)( ( pfdep_cpu_addr_t)ctrl_p->base_addr +
                                                     ( reg_addr << 2) ) );
}

void ogma_set_mac_reg (
    ogma_ctrl_t *ctrl_p,
    ogma_uint32 addr,
    ogma_uint32 value);

ogma_uint32 ogma_get_mac_reg (
    ogma_ctrl_t *ctrl_p,
    ogma_uint32 addr);

#endif/* OGMA_BASIC_ACCESS_H */
