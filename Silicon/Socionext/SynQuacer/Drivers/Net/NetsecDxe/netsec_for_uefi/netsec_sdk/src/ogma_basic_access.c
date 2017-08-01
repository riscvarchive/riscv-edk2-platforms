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

#include "ogma_internal.h"
#include "ogma_basic_access.h"

/**********************************************************************
 * Function definitions
 **********************************************************************/

void ogma_set_mac_reg (
    ogma_ctrl_t *ctrl_p,
    ogma_uint32 addr,
    ogma_uint32 value)
{

    ogma_uint32 cmd;

    ogma_check_clk_supply( ctrl_p, OGMA_CLK_EN_REG_DOM_G);

    /*
     * Argument check is omitted because this function is
     * of private use only.
     */

    ogma_write_reg( ctrl_p,
                    OGMA_REG_ADDR_MAC_DATA,
                    value);

    cmd = addr | OGMA_GMAC_CMD_ST_WRITE;

    ogma_write_reg( ctrl_p,
                    OGMA_REG_ADDR_MAC_CMD,
                    cmd);

    /*
     * Waits until BUSY bit is cleared.
     */
    while ( ( ogma_read_reg( ctrl_p,
                             OGMA_REG_ADDR_MAC_CMD)
              & OGMA_GMAC_CMD_ST_BUSY)
            != 0) {
        ;
    }
}

ogma_uint32 ogma_get_mac_reg (
    ogma_ctrl_t *ctrl_p,
    ogma_uint32 addr)
{
    ogma_uint32 cmd;

    ogma_check_clk_supply( ctrl_p, OGMA_CLK_EN_REG_DOM_G);

    /*
     * Argument check is omitted because this function is
     * of private use only.
     */

    cmd = addr | OGMA_GMAC_CMD_ST_READ;

    ogma_write_reg( ctrl_p,
                    OGMA_REG_ADDR_MAC_CMD,
                    cmd);

    /*
     * Waits until BUSY bit is cleared.
     */
    while ( ( ogma_read_reg( ctrl_p,
                             OGMA_REG_ADDR_MAC_CMD)
              & OGMA_GMAC_CMD_ST_BUSY)
            != 0) {
        ;
    }
    return ogma_read_reg( ctrl_p, OGMA_REG_ADDR_MAC_DATA);
}
