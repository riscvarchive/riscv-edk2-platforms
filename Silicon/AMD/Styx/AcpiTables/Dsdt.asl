/** @file

  Differentiated System Description Table Fields (DSDT)

  Copyright (c) 2014, ARM Ltd. All rights reserved.<BR>
  Copyright (c) 2014 - 2016, AMD Inc. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
/**

  Derived from:
   ArmPlatformPkg/ArmJunoPkg/AcpiTables/Dsdt.asl

**/

DefinitionBlock ("DSDT.aml", "DSDT", 2, "AMDINC", "SEATTLE ", 3)
{
    Scope (_SB)
    {
        Device (CPU0)
        {
            Name (_HID, "ACPI0007" /* Processor Device */)  // _HID: Hardware ID
            Name (_UID, 0x000)  // _UID: Unique ID
        }
#if (NUM_CORES > 1)
        Device (CPU1)
        {
            Name (_HID, "ACPI0007" /* Processor Device */)  // _HID: Hardware ID
            Name (_UID, 0x001)  // _UID: Unique ID
        }
#endif
#if (NUM_CORES > 2)
        Device (CPU2)
        {
            Name (_HID, "ACPI0007" /* Processor Device */)  // _HID: Hardware ID
            Name (_UID, 0x100)  // _UID: Unique ID
        }
#endif
#if (NUM_CORES > 3)
        Device (CPU3)
        {
            Name (_HID, "ACPI0007" /* Processor Device */)  // _HID: Hardware ID
            Name (_UID, 0x101)  // _UID: Unique ID
        }
#endif
#if (NUM_CORES > 4)
        Device (CPU4)
        {
            Name (_HID, "ACPI0007" /* Processor Device */)  // _HID: Hardware ID
            Name (_UID, 0x200)  // _UID: Unique ID
        }
#endif
#if (NUM_CORES > 5)
        Device (CPU5)
        {
            Name (_HID, "ACPI0007" /* Processor Device */)  // _HID: Hardware ID
            Name (_UID, 0x201)  // _UID: Unique ID
        }
#endif
#if (NUM_CORES > 6)
        Device (CPU6)
        {
            Name (_HID, "ACPI0007" /* Processor Device */)  // _HID: Hardware ID
            Name (_UID, 0x300)  // _UID: Unique ID
        }
#endif
#if (NUM_CORES > 7)
        Device (CPU7)
        {
            Name (_HID, "ACPI0007" /* Processor Device */)  // _HID: Hardware ID
            Name (_UID, 0x301)  // _UID: Unique ID
        }
#endif

        Device (AHC0)
        {
            Name (_HID, "AMDI0600")  // _HID: Hardware ID
            Name (_UID, 0x00)  // _UID: Unique ID
            Name (_CCA, 0x01)  // _CCA: Cache Coherency Attribute
            Name (_CLS, Package (0x03)  // _CLS: Class Code
            {
                0x01,
                0x06,
                0x01
            })
            Name (_CRS, ResourceTemplate ()  // _CRS: Current Resource Settings
            {
                Memory32Fixed (ReadWrite,
                    0xE0300000,         // Address Base (MMIO)
                    0x00010000,         // Address Length
                    )
                Memory32Fixed (ReadWrite,
                    0xE0000078,         // Address Base (SGPIO)
                    0x00000001,         // Address Length
                    )
                Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive, ,, ) {0x00000183, }
            })
        }

        Device (AHC1)
        {
            Name (_HID, "AMDI0600")  // _HID: Hardware ID
            Name (_UID, 0x01)  // _UID: Unique ID
            Name (_CCA, 0x01)  // _CCA: Cache Coherency Attribute
            Name (_CLS, Package (0x03)  // _CLS: Class Code
            {
                0x01,
                0x06,
                0x01
            })
            Method (_STA)
            {
                 Return (0x0F)
            }
            Name (_CRS, ResourceTemplate ()  // _CRS: Current Resource Settings
            {
                Memory32Fixed (ReadWrite,
                    0xE0D00000,         // Address Base (MMIO)
                    0x00010000,         // Address Length
                    )
                Memory32Fixed (ReadWrite,
                    0xE000007C,         // Address Base (SGPIO)
                    0x00000001,         // Address Length
                    )
                Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive, ,, ) {0x00000182, }
            })
        }

#if DO_XGBE
        Device (ETH0)
        {
            Name (_HID, "AMDI8001")  // _HID: Hardware ID
            Name (_UID, 0x00)  // _UID: Unique ID
            Name (_CCA, 0x01)  // _CCA: Cache Coherency Attribute
            Name (_CRS, ResourceTemplate ()  // _CRS: Current Resource Settings

            {
                Memory32Fixed (ReadWrite,
                    0xE0700000,         // Address Base (XGMAC)
                    0x00010000,         // Address Length
                    )
                Memory32Fixed (ReadWrite,
                    0xE0780000,         // Address Base (XPCS)
                    0x00080000,         // Address Length
                    )
                Memory32Fixed (ReadWrite,
                    0xE1240800,         // Address Base (SERDES_RxTx)
                    0x00000400,         // Address Length
                    )
                Memory32Fixed (ReadWrite,
                    0xE1250000,         // Address Base (SERDES_IR_1)
                    0x00000060,         // Address Length
                    )
                Memory32Fixed (ReadWrite,
                    0xE12500F8,         // Address Base (SERDES_IR_2)
                    0x00000004,         // Address Length
                    )
                Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive, ,, ) {0x00000165, } // XGMAC
                Interrupt (ResourceConsumer, Edge, ActiveHigh, Exclusive, ,, )  {0x0000017A, } // DMA0
                Interrupt (ResourceConsumer, Edge, ActiveHigh, Exclusive, ,, )  {0x0000017B, } // DMA1
                Interrupt (ResourceConsumer, Edge, ActiveHigh, Exclusive, ,, )  {0x0000017C, } // DMA2
                Interrupt (ResourceConsumer, Edge, ActiveHigh, Exclusive, ,, )  {0x0000017D, } // DMA3
                Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive, ,, ) {0x00000163, } // XPCS
            })
            Name (_DSD, Package (0x02)  // _DSD: Device-Specific Data
            {
                ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
                Package ()
                {
                    Package (0x02) {"mac-address", Package (0x06) {0x02, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5}},
                    Package (0x02) {"phy-mode", "xgmii"},
                    Package (0x02) {"amd,speed-set", 0x00},
                    Package (0x02) {"amd,dma-freq", 0x0EE6B280},
                    Package (0x02) {"amd,ptp-freq",  0x0EE6B280},
                    Package (0x02) {"amd,serdes-blwc", Package (0x03) {1, 1, 0}},
                    Package (0x02) {"amd,serdes-cdr-rate", Package (0x03) {2, 2, 7}},
                    Package (0x02) {"amd,serdes-pq-skew", Package (0x03) {10, 10, 18}},
                    Package (0x02) {"amd,serdes-tx-amp", Package (0x03) {15, 15, 10}},
                    Package (0x02) {"amd,serdes-dfe-tap-config", Package (0x03) {3, 3, 1}},
                    Package (0x02) {"amd,serdes-dfe-tap-enable", Package (0x03) {0, 0, 127}},
                    Package (0x02) {"amd,per-channel-interrupt", 0x01}
                }
            })
        }

        Device (ETH1)
        {
            Name (_HID, "AMDI8001")  // _HID: Hardware ID
            Name (_UID, 0x01)  // _UID: Unique ID
            Name (_CCA, 0x01)  // _CCA: Cache Coherency Attribute
            Name (_CRS, ResourceTemplate ()  // _CRS: Current Resource Settings
            {
                Memory32Fixed (ReadWrite,
                    0xE0900000,         // Address Base (XGMAC)
                    0x00010000,         // Address Length
                    )
                Memory32Fixed (ReadWrite,
                    0xE0980000,         // Address Base (XPCS)
                    0x00080000,         // Address Length
                    )
                Memory32Fixed (ReadWrite,
                    0xE1240C00,         // Address Base (SERDES_RxTx)
                    0x00000400,         // Address Length
                    )
                Memory32Fixed (ReadWrite,
                    0xE1250080,         // Address Base (SERDES_IR_1)
                    0x00000060,         // Address Length
                    )
                Memory32Fixed (ReadWrite,
                    0xE12500FC,         // Address Base (SERDES_IR_2)
                    0x00000004,         // Address Length
                    )
                Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive, ,, ) {0x00000164, } // XGMAC
                Interrupt (ResourceConsumer, Edge, ActiveHigh, Exclusive, ,, )  {0x00000175, } // DMA0
                Interrupt (ResourceConsumer, Edge, ActiveHigh, Exclusive, ,, )  {0x00000176, } // DMA1
                Interrupt (ResourceConsumer, Edge, ActiveHigh, Exclusive, ,, )  {0x00000177, } // DMA2
                Interrupt (ResourceConsumer, Edge, ActiveHigh, Exclusive, ,, )  {0x00000178, } // DMA3
                Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive, ,, ) {0x00000162, } // XPCS
            })
            Name (_DSD, Package (0x02)  // _DSD: Device-Specific Data
            {
                ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
                Package ()
                {
                    Package (0x02) {"mac-address", Package (0x06) {0x02, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5}},
                    Package (0x02) {"phy-mode", "xgmii"},
                    Package (0x02) {"amd,speed-set", 0x00},
                    Package (0x02) {"amd,dma-freq", 0x0EE6B280},
                    Package (0x02) {"amd,ptp-freq",  0x0EE6B280},
                    Package (0x02) {"amd,serdes-blwc", Package (0x03) {1, 1, 0}},
                    Package (0x02) {"amd,serdes-cdr-rate", Package (0x03) {2, 2, 7}},
                    Package (0x02) {"amd,serdes-pq-skew", Package (0x03) {10, 10, 18}},
                    Package (0x02) {"amd,serdes-tx-amp", Package (0x03) {15, 15, 10}},
                    Package (0x02) {"amd,serdes-dfe-tap-config", Package (0x03) {3, 3, 1}},
                    Package (0x02) {"amd,serdes-dfe-tap-enable", Package (0x03) {0, 0, 127}},
                    Package (0x02) {"amd,per-channel-interrupt", 0x01}
                }
            })
        }
#endif  // DO_XGBE

        Device (SPI0)
        {
            Name (_HID, "AMDI0500")  // _HID: Hardware ID
            Name (_UID, 0x00)  // _UID: Unique ID
            Name (_CRS, ResourceTemplate ()  // _CRS: Current Resource Settings
            {
                Memory32Fixed (ReadWrite,
                    0xE1020000,         // Address Base
                    0x00001000,         // Address Length
                    )
                Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive, ,, ) {0x0000016A, }
            })
        }

        Device (SPI1)
        {
            Name (_HID, "AMDI0500")  // _HID: Hardware ID
            Name (_UID, 0x01)  // _UID: Unique ID
            Name (_CRS, ResourceTemplate ()  // _CRS: Current Resource Settings
            {
                Memory32Fixed (ReadWrite,
                    0xE1030000,         // Address Base
                    0x00001000,         // Address Length
                    )
                Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive, ,, ) {0x00000169, }
            })

            Device(SDC0)
            {
                Name(_HID, "AMDI0501")  // SD Card/MMC slot
                Name(_CRS, ResourceTemplate()
                {
                    SPISerialBus(1,             // DeviceSelection
                       PolarityLow,             // DeviceSelectionPolarity
                       FourWireMode,            // WireMode
                       8,                       // DataBitLength
                       ControllerInitiated,     // SlaveMode
                       20000000,                // ConnectionSpeed
                       ClockPolarityLow,        // ClockPolarity
                       ClockPhaseFirst,         // ClockPhase
                       "\\SB.SPI1",             // ResourceSource
                       0,                       // ResourceSourceIndex
                       ResourceConsumer,        // ResourceUsage
                    ) // SPISerialBus()

                    // SD Card “Detect” signal
                    GpioInt(Edge, ActiveBoth, ExclusiveAndWake, PullDown, , "\\_SB.GIO1") {6}
                }) // ResourceTemplate()

            } // Device()
        }

        Device (COM1)
        {
            Name (_HID, "AMDI0511")  // _HID: Hardware ID
            Name (_CID, "ARMH0011")  // _CID: Compatible ID
            Name (_ADR, 0xE1010000)  // _ADR: Address
            Name (_CRS, ResourceTemplate ()  // _CRS: Current Resource Settings
            {
                Memory32Fixed (ReadWrite,
                    0xE1010000,         // Address Base
                    0x00001000,         // Address Length
                    )
                Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive, ,, ) {0x00000168, }
            })
        }

        Device (GIO0)
        {
            Name (_HID, "AMDI0400")  // _HID: Hardware ID
            Name (_CID, "ARMH0061")  // _CID: Compatible ID
            Name (_UID, 0x00)  // _UID: Unique ID
            Name (_CRS, ResourceTemplate ()  // _CRS: Current Resource Settings
            {
                Memory32Fixed (ReadWrite,
                    0xE0080000,         // Address Base
                    0x00001000,         // Address Length
                    )
                Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive, ,, ) {0x00000189, }
            })
        }

        Device (GIO1)
        {
            Name (_HID, "AMDI0400")  // _HID: Hardware ID
            Name (_CID, "ARMH0061")  // _CID: Compatible ID
            Name (_UID, 0x01)  // _UID: Unique ID
            Name (_CRS, ResourceTemplate ()  // _CRS: Current Resource Settings
            {
                Memory32Fixed (ReadWrite,
                    0xE1050000,         // Address Base
                    0x00001000,         // Address Length
                    )
                Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive, ,, ) {0x00000186, }
            })
        }

        Device (GIO2)
        {
            Name (_HID, "AMDI0400")  // _HID: Hardware ID
            Name (_CID, "ARMH0061")  // _CID: Compatible ID
            Name (_UID, 0x02)  // _UID: Unique ID
            Method (_STA)
            {
                 Return (0x0F)
            }
            Name (_CRS, ResourceTemplate ()  // _CRS: Current Resource Settings
            {
                Memory32Fixed (ReadWrite,
                    0xE0020000,         // Address Base
                    0x00001000,         // Address Length
                    )
                Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive, ,, ) {0x0000018E, }
            })
        }

        Device (GIO3)
        {
            Name (_HID, "AMDI0400")  // _HID: Hardware ID
            Name (_CID, "ARMH0061")  // _CID: Compatible ID
            Name (_UID, 0x03)  // _UID: Unique ID
            Method (_STA)
            {
                 Return (0x0F)
            }
            Name (_CRS, ResourceTemplate ()  // _CRS: Current Resource Settings
            {
                Memory32Fixed (ReadWrite,
                    0xE0030000,         // Address Base
                    0x00001000,         // Address Length
                    )
                Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive, ,, ) {0x0000018D, }
            })
        }

        Device (I2C0)
        {
            Name (_HID, "AMDI0510")  // _HID: Hardware ID
            Name (_UID, 0x00)  // _UID: Unique ID
            Name (_CRS, ResourceTemplate ()  // _CRS: Current Resource Settings
            {
                Memory32Fixed (ReadWrite,
                    0xE1000000,         // Address Base
                    0x00001000,         // Address Length
                    )
                Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive, ,, ) {0x00000185, }
            })

            Method (SSCN, 0, NotSerialized)
            {
                Return (Package (0x03)
                {
                    0x0430,
                    0x04E1,
                    0x00
                })
            }

            Method (FMCN, 0, NotSerialized)
            {
                Return (Package (0x03)
                {
                    0x00DE,
                    0x018F,
                    0x00
                })
            }
        }

        Device (I2C1)
        {
            Name (_HID, "AMDI0510")  // _HID: Hardware ID
            Name (_UID, 0x01)  // _UID: Unique ID
            Name (_CRS, ResourceTemplate ()  // _CRS: Current Resource Settings
            {
                Memory32Fixed (ReadWrite,
                    0xE0050000,         // Address Base
                    0x00001000,         // Address Length
                    )
                Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive, ,, ) {0x00000174, }
            })

            Method (SSCN, 0, NotSerialized)
            {
                Return (Package (0x03)
                {
                    0x0430,
                    0x04E1,
                    0x00
                })
            }

            Method (FMCN, 0, NotSerialized)
            {
                Return (Package (0x03)
                {
                    0x00DE,
                    0x018F,
                    0x00
                })
            }
        }

        Device (CCP0)
        {
            Name (_HID, "AMDI0C00")  // _HID: Hardware ID
            Name (_CCA, 0x01)  // _CCA: Cache Coherency Attribute
            Name (_CRS, ResourceTemplate ()  // _CRS: Current Resource Settings
            {
                Memory32Fixed (ReadWrite,
                    0xE0100000,         // Address Base
                    0x00010000,         // Address Length
                    )
                Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive, ,, ) {0x00000023, }
            })

            Name (_DSD, Package (0x02)  // _DSD: Device-Specific Data
            {
                ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
                Package ()
                {
                    Package (0x02) {"amd,zlib-support", 1}
                }
            })
        }

#if DO_KCS
        //
        // IPMI/KCS
        //
        Device (KCS0)
        {
            Name (_HID, "AMDI0300")
            Name (_CID, "IPI0001")
            Name (_STR, Unicode("IPMI_KCS"))
            Name (_UID, 0)
            Name (_CRS, ResourceTemplate() {
                Memory32Fixed(ReadWrite, 0xE0010000, 0x1) // KCS Data In/Out
                Memory32Fixed(ReadWrite, 0xE0010004, 0x1) // KCS Control/Status
                Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive,,,) { 421 } // GSIV
            })
            Method (_IFT) {     // Interface Type
                Return ( 0x01)   // IPMI KCS
            }

            Method (_SRV) {     // Spec Revision
                Return (0x200)  // IPMI Spec v2.0
            }
        }
#endif // DO_KCS

        //
        // PCIe Root Bus
        //
        Device (PCI0)
        {
            Name (_HID, "PNP0A08" /* PCI Express Bus */)  // _HID: Hardware ID
            Name (_CID, "PNP0A03" /* PCI Bus */)  // _CID: Compatible ID
            Name (_SEG, 0x00)  // _SEG: PCI Segment
            Name (_BBN, 0x00)  // _BBN: BIOS Bus Number
            Name (_CCA, 0x01)  // _CCA: Cache Coherency Attribute
            Name (_PRT, Package ()  // _PRT: PCI Routing Table
            {
                // INTA of the bridge device itself
                Package () { 0x2FFFF, 0x0, 0x0, 0x140 }
            })

            Device (EXP1)
            {
                Name (_ADR, 0x20001)  // _ADR: Address
                Name (_PRT, Package ()  // _PRT: PCI Routing Table
                {
                    Package () { 0xFFFF, 0x0, 0x0, 0x140 },
                    Package () { 0xFFFF, 0x1, 0x0, 0x141 },
                    Package () { 0xFFFF, 0x2, 0x0, 0x142 },
                    Package () { 0xFFFF, 0x3, 0x0, 0x143 }
                }) // _PRT
            }
            Device (EXP2)
            {
                Name (_ADR, 0x20002)  // _ADR: Address
                Name (_PRT, Package ()  // _PRT: PCI Routing Table
                {
                    Package () { 0xFFFF, 0x0, 0x0, 0x144 },
                    Package () { 0xFFFF, 0x1, 0x0, 0x145 },
                    Package () { 0xFFFF, 0x2, 0x0, 0x146 },
                    Package () { 0xFFFF, 0x3, 0x0, 0x147 }
                }) // _PRT
            }
            Device (EXP3)
            {
                Name (_ADR, 0x20003)  // _ADR: Address
                Name (_PRT, Package ()  // _PRT: PCI Routing Table
                {
                    Package () { 0xFFFF, 0x0, 0x0, 0x148 },
                    Package () { 0xFFFF, 0x1, 0x0, 0x149 },
                    Package () { 0xFFFF, 0x2, 0x0, 0x14A },
                    Package () { 0xFFFF, 0x3, 0x0, 0x14B }
                }) // _PRT
            }

            Method (_CRS, 0, Serialized)  // _CRS: Current Resource Settings
            {
                Name (RBUF, ResourceTemplate ()
                {
                    WordBusNumber (ResourceProducer, MinFixed, MaxFixed, PosDecode,
                        0x0000,             // Granularity
                        0x0000,             // Range Minimum
                        0x007F,             // Range Maximum
                        0x0000,             // Translation Offset
                        0x0080,             // Length
                        )
                    DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed, NonCacheable, ReadWrite,
                        0x00000000,         // Granularity
                        0x40000000,         // Range Minimum
                        0x5FFFFFFF,         // Range Maximum
                        0x00000000,         // Translation Offset
                        0x20000000          // Length
                        )
                    DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed, NonCacheable, ReadWrite,
                        0x00000000,         // Granularity
                        0x60000000,         // Range Minimum
                        0x7FFFFFFF,         // Range Maximum
                        0x00000000,         // Translation Offset
                        0x20000000          // Length
                        )
                    DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed, NonCacheable, ReadWrite,
                        0x00000000,         // Granularity
                        0x80000000,         // Range Minimum
                        0x9FFFFFFF,         // Range Maximum
                        0x00000000,         // Translation Offset
                        0x20000000          // Length
                        )
                    DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed, NonCacheable, ReadWrite,
                        0x00000000,         // Granularity
                        0xA0000000,         // Range Minimum
                        0xBFFFFFFF,         // Range Maximum
                        0x00000000,         // Translation Offset
                        0x20000000          // Length
                        )
                    QWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed, NonCacheable, ReadWrite,
                        0x0000000000000000, // Granularity
                        0x0000000100000000, // Range Minimum
                        0x00000001FFFFFFFF, // Range Maximum
                        0x0000000000000000, // Translation Offset
                        0x0000000100000000  // Length
                        )
                    QWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed, NonCacheable, ReadWrite,
                        0x0000000000000000, // Granularity
                        0x0000000200000000, // Range Minimum
                        0x00000003FFFFFFFF, // Range Maximum
                        0x0000000000000000, // Translation Offset
                        0x0000000200000000  // Length
                        )
                    QWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed, NonCacheable, ReadWrite,
                        0x0000000000000000, // Granularity
                        0x0000000400000000, // Range Minimum
                        0x00000007FFFFFFFF, // Range Maximum
                        0x0000000000000000, // Translation Offset
                        0x0000000400000000  // Length
                        )
                    QWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed, NonCacheable, ReadWrite,
                        0x0000000000000000, // Granularity
                        0x0000000800000000, // Range Minimum
                        0x0000000FFFFFFFFF, // Range Maximum
                        0x0000000000000000, // Translation Offset
                        0x0000000800000000  // Length
                        )
                    QWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed, NonCacheable, ReadWrite,
                        0x0000000000000000, // Granularity
                        0x0000001000000000, // Range Minimum
                        0x0000001FFFFFFFFF, // Range Maximum
                        0x0000000000000000, // Translation Offset
                        0x0000001000000000  // Length
                        )
                    QWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed, NonCacheable, ReadWrite,
                        0x0000000000000000, // Granularity
                        0x0000002000000000, // Range Minimum
                        0x0000003FFFFFFFFF, // Range Maximum
                        0x0000000000000000, // Translation Offset
                        0x0000002000000000  // Length
                        )
                    QWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed, NonCacheable, ReadWrite,
                        0x0000000000000000, // Granularity
                        0x0000004000000000, // Range Minimum
                        0x0000007FFFFFFFFF, // Range Maximum
                        0x0000000000000000, // Translation Offset
                        0x0000004000000000  // Length
                        )
                    DWordIo (ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,
                        0x00000000,         // Granularity
                        0x00000000,         // Range Minimum
                        0x0000FFFF,         // Range Maximum
                        0xEFFF0000,         // Translation Address
                        0x00010000,         // Length
                        ,
                        ,
                        ,
                        TypeTranslation
                        )
                })
                Return (RBUF) /* \_SB_.PCI0._CRS.RBUF */
            } // Method(_CRS)

            Device (RES0)
            {
                Name (_HID, "PNP0C02")
                Name (_CRS, ResourceTemplate ()
                {
                    Memory32Fixed (ReadWrite, 0xF0000000, 0x8000000)
                })
            }
            Name (SUPP, 0x00)
            Name (CTRL, 0x00)
            Method (_OSC, 4, NotSerialized)  // _OSC: Operating System Capabilities
            {
                CreateDWordField (Arg3, 0x00, CDW1)
                If (LEqual (Arg0, ToUUID ("33db4d5b-1ff7-401c-9657-7441c03dd766") /* PCI Host Bridge Device */))
                {
                    CreateDWordField (Arg3, 0x04, CDW2)
                    CreateDWordField (Arg3, 0x08, CDW3)
                    Store (CDW2, SUPP) /* \_SB_.PCI0.SUPP */
                    Store (CDW3, CTRL) /* \_SB_.PCI0.CTRL */
                    If (LNotEqual (And (SUPP, 0x16), 0x16))
                    {
                        And (CTRL, 0x1E, CTRL) /* \_SB_.PCI0.CTRL */
                    }

                    And (CTRL, 0x1D, CTRL) /* \_SB_.PCI0.CTRL */
                    If (LNotEqual (Arg1, One))
                    {
                        Or (CDW1, 0x08, CDW1) /* \_SB_.PCI0._OSC.CDW1 */
                    }

                    If (LNotEqual (CDW3, CTRL))
                    {
                        Or (CDW1, 0x10, CDW1) /* \_SB_.PCI0._OSC.CDW1 */
                    }

                    Store (CTRL, CDW3) /* \_SB_.PCI0._OSC.CDW3 */
                    Return (Arg3)
                }
                Else
                {
                    Or (CDW1, 0x04, CDW1) /* \_SB_.PCI0._OSC.CDW1 */
                    Return (Arg3)
                }
            } // Method(_OSC)

            //
            // Device-Specific Methods
            //
            Method(_DSM, 0x4, NotSerialized) {
              If (LEqual(Arg0, ToUUID("E5C937D0-3553-4d7a-9117-EA4D19C3434D"))) {
                switch (ToInteger(Arg2)) {
                  //
                  // Function 0: Return supported functions
                  //
                  case(0) {
                    Return (Buffer() {0xFF})
                  }

                  //
                  // Function 1: Return PCIe Slot Information
                  //
                  case(1) {
                    Return (Package(2) {
                      One, // Success
                      Package(3) {
                        0x1,  // x1 PCIe link
                        0x1,  // PCI express card slot
                        0x1   // WAKE# signal supported
                      }
                    })
                  }

                  //
                  // Function 2: Return PCIe Slot Number.
                  //
                  case(2) {
                    Return (Package(1) {
                      Package(4) {
                        2,  // Source ID
                        4,  // Token ID: ID refers to a slot
                        0,  // Start bit of the field to use.
                        7   // End bit of the field to use.
                      }
                    })
                  }

                  //
                  // Function 3: Return Vendor-specific Token ID Strings.
                  //
                  case(3) {
                    Return (Package(0) {})
                  }

                  //
                  // Function 4: Return PCI Bus Capabilities
                  //
                  case(4) {
                    Return (Package(2) {
                      One, // Success
                      Buffer() {
                        1,0,            // Version
                        0,0,            // Status, 0:Success
                        24,0,0,0,       // Length
                        1,0,            // PCI
                        16,0,           // Length
                        0,              // Attributes
                        0x0D,           // Current Speed/Mode
                        0x3F,0,         // Supported Speeds/Modes
                        0,              // Voltage
                        0,0,0,0,0,0,0   // Reserved
                      }
                    })
                  }

                  //
                  // Function 5: Return Ignore PCI Boot Configuration
                  //
                  case(5) {
                    Return (Package(1) {1})
                  }

                  //
                  // Function 6: Return LTR Maximum Latency
                  //
                  case(6) {
                    Return (Package(4) {
                      Package(1){0},  // Maximum Snoop Latency Scale
                      Package(1){0},  // Maximum Snoop Latency Value
                      Package(1){0},  // Maximum No-Snoop Latency Scale
                      Package(1){0}   // Maximum No-Snoop Latency Value
                    })
                  }

                  //
                  // Function 7: Return PCI Express Naming
                  //
                  case(7) {
                    Return (Package(2) {
                      Package(1) {0},
                      Package(1) {Unicode("PCI0")}
                    })
                  }

                  //
                  // Not supported
                  //
                  default {
                  }
                }
              }
              Return (Buffer(){0})
            } // Method(_DSM)

            //
            // Root-Complex 0
            //
            Device (RP0)
            {
                Name (_ADR, 0xF0000000)  // _ADR: Bus 0, Dev 0, Func 0
            }
        }
    }
}

