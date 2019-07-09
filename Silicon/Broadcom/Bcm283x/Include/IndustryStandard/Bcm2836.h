/** @file
 *
 *  Copyright (c) 2017, Andrei Warkentin <andrey.warkentin@gmail.com>
 *  Copyright (c) 2016, Linaro Limited. All rights reserved.
 *
 *  SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 **/

#ifndef __BCM2836_H__
#define __BCM2836_H__

/*
 * Both "core" and SoC perpherals (1M each).
 */
#define BCM2836_SOC_REGISTERS                               0x3f000000
#define BCM2836_SOC_REGISTER_LENGTH                         0x02000000

/*
 * Offset between the CPU's view and the VC's view of system memory.
 */
#define BCM2836_DMA_DEVICE_OFFSET                           0xc0000000

/* watchdog constants */
#define BCM2836_WDOG_BASE_ADDRESS                           0x3f100000
#define BCM2836_WDOG_PASSWORD                               0x5a000000
#define BCM2836_WDOG_RSTC_OFFSET                            0x0000001c
#define BCM2836_WDOG_WDOG_OFFSET                            0x00000024
#define BCM2836_WDOG_RSTC_WRCFG_MASK                        0x00000030
#define BCM2836_WDOG_RSTC_WRCFG_FULL_RESET                  0x00000020

/* mailbox interface constants */
#define BCM2836_MBOX_BASE_ADDRESS                           0x3f00b880
#define BCM2836_MBOX_READ_OFFSET                            0x00000000
#define BCM2836_MBOX_STATUS_OFFSET                          0x00000018
#define BCM2836_MBOX_CONFIG_OFFSET                          0x0000001c
#define BCM2836_MBOX_WRITE_OFFSET                           0x00000020

#define BCM2836_MBOX_STATUS_FULL                            0x1f
#define BCM2836_MBOX_STATUS_EMPTY                           0x1e

#define BCM2836_MBOX_NUM_CHANNELS                           16

/* interrupt controller constants */
#define BCM2836_INTC_TIMER_CONTROL_OFFSET                   0x00000040
#define BCM2836_INTC_TIMER_PENDING_OFFSET                   0x00000060

/* random number generator */
#define RNG_BASE_ADDRESS   (BCM2836_SOC_REGISTERS + 0x00104000)

#define RNG_CTRL           (RNG_BASE_ADDRESS + 0x0)
#define RNG_STATUS         (RNG_BASE_ADDRESS + 0x4)
#define RNG_DATA           (RNG_BASE_ADDRESS + 0x8)

#define RNG_CTRL_ENABLE    0x1

#endif /*__BCM2836_H__ */
