/** @file
 *
 *  Copyright (c) 2019 Linaro, Limited. All rights reserved.
 *  Copyright (c) 2019 Andrei Warkentin <andrey.warkentin@gmail.com>
 *
 *  SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 **/

#include <IndustryStandard/Bcm2711.h>

/*
 * The following can be used to remove parenthesis from
 * defined macros that the compiler complains about.
 */
#define ISOLATE_ARGS(...)               __VA_ARGS__
#define REMOVE_PARENTHESES(x)           ISOLATE_ARGS x

#define SANITIZED_PCIE_CPU_MMIO_WINDOW  REMOVE_PARENTHESES(PCIE_CPU_MMIO_WINDOW)
#define SANITIZED_PCIE_REG_BASE         REMOVE_PARENTHESES(PCIE_REG_BASE)

/*
 * According to UEFI boot log for the VLI device on Pi 4.
 */
#define XHCI_REG_LENGTH                 0x1000

Device (SCB0) {
    Name (_HID, "ACPI0004")
    Name (_UID, 0x0)
    Name (_CCA, 0x0)

    Method (_CRS, 0, Serialized) { // _CRS: Current Resource Settings
        /*
         * Container devices with _DMA must have _CRS, meaning SCB0
         * to provide all resources that XHC0 consumes (except
         * interrupts).
         */
        Name (RBUF, ResourceTemplate () {
            QWordMemory (ResourceProducer,
                ,
                MinFixed,
                MaxFixed,
                NonCacheable,
                ReadWrite,
                0x0,
                SANITIZED_PCIE_CPU_MMIO_WINDOW, // MIN
                SANITIZED_PCIE_CPU_MMIO_WINDOW, // MAX
                0x0,
                0x1,                            // LEN
                ,
                ,
                MMIO
                )
        })
        CreateQwordField (RBUF, MMIO._MAX, MMBE)
        CreateQwordField (RBUF, MMIO._LEN, MMLE)
        Add (MMBE, XHCI_REG_LENGTH - 1, MMBE)
        Add (MMLE, XHCI_REG_LENGTH - 1, MMLE)
        Return (RBUF)
    }

    Name (_DMA, ResourceTemplate() {
        /*
         * XHC0 is limited to DMA to first 3GB. Note this
         * only applies to PCIe, not GENET or other devices
         * next to the A72.
         */
        QWordMemory (ResourceConsumer,
            ,
            MinFixed,
            MaxFixed,
            NonCacheable,
            ReadWrite,
            0x0,
            0x0,        // MIN
            0xbfffffff, // MAX
            0x0,        // TRA
            0xc0000000, // LEN
            ,
            ,
            )
    })

    Device (XHC0)
    {
        Name (_HID, "PNP0D10")      // _HID: Hardware ID
        Name (_UID, 0x0)            // _UID: Unique ID
        Name (_CCA, 0x0)            // _CCA: Cache Coherency Attribute

        Method (_CRS, 0, Serialized) { // _CRS: Current Resource Settings
            Name (RBUF, ResourceTemplate () {
                QWordMemory (ResourceConsumer,
                    ,
                    MinFixed,
                    MaxFixed,
                    NonCacheable,
                    ReadWrite,
                    0x0,
                    SANITIZED_PCIE_CPU_MMIO_WINDOW, // MIN
                    SANITIZED_PCIE_CPU_MMIO_WINDOW, // MAX
                    0x0,
                    0x1,                            // LEN
                    ,
                    ,
                    MMIO
                    )
                Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive, ,, ) {
                    175
                }
            })
            CreateQwordField (RBUF, MMIO._MAX, MMBE)
            CreateQwordField (RBUF, MMIO._LEN, MMLE)
            Add (MMBE, XHCI_REG_LENGTH - 1, MMBE)
            Add (MMLE, XHCI_REG_LENGTH - 1, MMLE)
            Return (RBUF)
        }

        Method (_INI, 0, Serialized) {
            OperationRegion (PCFG, SystemMemory, SANITIZED_PCIE_REG_BASE + PCIE_EXT_CFG_DATA, 0x1000)
            Field (PCFG, AnyAcc, NoLock, Preserve) {
                VNID, 16, // Vendor ID
                DVID, 16, // Device ID
                CMND, 16, // Command register
                STAT, 16, // Status register
            }

            // Set command register to:
            // 1) decode MMIO (set bit 1)
            // 2) enable DMA (set bit 2)
            // 3) enable interrupts (clear bit 10)
            Debug = "xHCI enable"
            Store (0x6, CMND)
        }
    }
}
