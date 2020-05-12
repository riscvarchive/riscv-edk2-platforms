/** @file
 *
 *  Differentiated System Definition Table (DSDT)
 *
 *  Copyright (c) 2020, Pete Batard <pete@akeo.ie>
 *  Copyright (c) 2018-2020, Andrey Warkentin <andrey.warkentin@gmail.com>
 *  Copyright (c) Microsoft Corporation. All rights reserved.
 *
 *  SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 **/

#include <IndustryStandard/Bcm2711.h>
#include <IndustryStandard/Bcm2836.h>
#include <IndustryStandard/Bcm2836Gpio.h>
#include <IndustryStandard/Bcm2836Gpu.h>
#include <IndustryStandard/Bcm2836Pwm.h>
#include <IndustryStandard/Bcm2836Sdio.h>
#include <IndustryStandard/Bcm2836SdHost.h>

#include "AcpiTables.h"

#define BCM_ALT0 0x4
#define BCM_ALT1 0x5
#define BCM_ALT2 0x6
#define BCM_ALT3 0x7
#define BCM_ALT4 0x3
#define BCM_ALT5 0x2

//
// The ASL compiler does not support argument arithmetic in functions
// like QWordMemory (). So we need to instantiate dummy qword regions
// that we can then update the Min, Max and Length attributes of.
// The three macros below help accomplish this.
//
// QWORDMEMORYSET specifies a CPU memory range (whose base address is
// BCM2836_SOC_REGISTERS + Offset), and QWORDBUSMEMORYSET specifies
// a VPU memory range (whose base address is provided directly).
//
#define QWORDMEMORYBUF(Index)                                   \
  QWordMemory (ResourceProducer,,                               \
    MinFixed, MaxFixed, NonCacheable, ReadWrite,                \
    0x0, 0x0, 0x0, 0x0, 0x1,,, RB ## Index)

#define QWORDMEMORYSET(Index, Offset, Length)                   \
  CreateQwordField (RBUF, RB ## Index._MIN, MI ## Index)        \
  CreateQwordField (RBUF, RB ## Index._MAX, MA ## Index)        \
  CreateQwordField (RBUF, RB ## Index._LEN, LE ## Index)        \
  Store (Length, LE ## Index)                                   \
  Add (BCM2836_SOC_REGISTERS, Offset, MI ## Index)              \
  Add (MI ## Index, LE ## Index - 1, MA ## Index)

#define QWORDBUSMEMORYSET(Index, Base, Length)                  \
  CreateQwordField (RBUF, RB ## Index._MIN, MI ## Index)        \
  CreateQwordField (RBUF, RB ## Index._MAX, MA ## Index)        \
  CreateQwordField (RBUF, RB ## Index._LEN, LE ## Index)        \
  Store (Base, MI ## Index)                                     \
  Store (Length, LE ## Index)                                   \
  Add (MI ## Index, LE ## Index - 1, MA ## Index)

DefinitionBlock ("Dsdt.aml", "DSDT", 5, "RPIFDN", "RPI", 2)
{
  Scope (\_SB_)
  {
    include ("Pep.asl")
#if (RPI_MODEL == 4)
    include ("Xhci.asl")
#endif

    Device (CPU0)
    {
      Name (_HID, "ACPI0007")
      Name (_UID, 0x0)
      Method (_STA)
      {
        Return (0xf)
      }
    }

    Device (CPU1)
    {
      Name (_HID, "ACPI0007")
      Name (_UID, 0x1)
      Method (_STA)
      {
        Return (0xf)
      }
    }

    Device (CPU2)
    {
      Name (_HID, "ACPI0007")
      Name (_UID, 0x2)
      Method (_STA)
      {
        Return (0xf)
      }
    }

    Device (CPU3)
    {
      Name (_HID, "ACPI0007")
      Name (_UID, 0x3)
      Method (_STA)
      {
        Return (0xf)
      }
    }

    //
    // GPU device container describes the DMA translation required
    // when a device behind the GPU wants to access Arm memory.
    // Only the first GB can be addressed.
    //
    Device (GDV0)
    {
      Name (_HID, "ACPI0004")
      Name (_UID, 0x1)
      Name (_CCA, 0x0)

      Method (_CRS, 0, Serialized) {
        //
        // Container devices with _DMA must have _CRS, meaning GDV0
        // to provide all resources that GpuDevs.asl consume (except
        // interrupts).
        //
        Name (RBUF, ResourceTemplate () {
          QWORDMEMORYBUF(01)
          QWORDMEMORYBUF(02)
          QWORDMEMORYBUF(03)
          // QWORDMEMORYBUF(04)
          // QWORDMEMORYBUF(05)
          QWORDMEMORYBUF(06)
          QWORDMEMORYBUF(07)
          QWORDMEMORYBUF(08)
          QWORDMEMORYBUF(09)
          QWORDMEMORYBUF(10)
          QWORDMEMORYBUF(11)
          QWORDMEMORYBUF(12)
          QWORDMEMORYBUF(13)
          QWORDMEMORYBUF(14)
          QWORDMEMORYBUF(15)
          // QWORDMEMORYBUF(16)
          QWORDMEMORYBUF(17)
          QWORDMEMORYBUF(18)
          QWORDMEMORYBUF(19)
          QWORDMEMORYBUF(20)
          QWORDMEMORYBUF(21)
          QWORDMEMORYBUF(22)
          QWORDMEMORYBUF(23)
          QWORDMEMORYBUF(24)
          QWORDMEMORYBUF(25)
        })

        // USB
        QWORDMEMORYSET(01, BCM2836_USB_OFFSET, BCM2836_USB_LENGTH)

        // GPU
        QWORDMEMORYSET(02, BCM2836_V3D_BUS_OFFSET, BCM2836_V3D_BUS_LENGTH)
        QWORDMEMORYSET(03, BCM2836_HVS_OFFSET, BCM2836_HVS_LENGTH)
        // QWORDMEMORYSET(04, BCM2836_PV0_OFFSET, BCM2836_PV0_LENGTH)
        // QWORDMEMORYSET(05, BCM2836_PV1_OFFSET, BCM2836_PV1_LENGTH)
        QWORDMEMORYSET(06, BCM2836_PV2_OFFSET, BCM2836_PV2_LENGTH)
        QWORDMEMORYSET(07, BCM2836_HDMI0_OFFSET, BCM2836_HDMI0_LENGTH)
        QWORDMEMORYSET(08, BCM2836_HDMI1_OFFSET, BCM2836_HDMI1_LENGTH)

        // Mailbox
        QWORDMEMORYSET(09, BCM2836_MBOX_OFFSET, BCM2836_MBOX_LENGTH)

        // VCHIQ
        QWORDMEMORYSET(10, BCM2836_VCHIQ_OFFSET, BCM2836_VCHIQ_LENGTH)

        // GPIO
        QWORDMEMORYSET(11, GPIO_OFFSET, GPIO_LENGTH)

        // I2C
        QWORDMEMORYSET(12, BCM2836_I2C1_OFFSET, BCM2836_I2C1_LENGTH)
        QWORDMEMORYSET(13, BCM2836_I2C2_OFFSET, BCM2836_I2C2_LENGTH)

        // SPI
        QWORDMEMORYSET(14, BCM2836_SPI0_OFFSET, BCM2836_SPI0_LENGTH)
        QWORDMEMORYSET(15, BCM2836_SPI1_OFFSET, BCM2836_SPI1_LENGTH)
        // QWORDMEMORYSET(16, BCM2836_SPI2_OFFSET, BCM2836_SPI2_LENGTH)

        // PWM
        QWORDMEMORYSET(17, BCM2836_PWM_DMA_OFFSET, BCM2836_PWM_DMA_LENGTH)
        QWORDMEMORYSET(18, BCM2836_PWM_CTRL_OFFSET, BCM2836_PWM_CTRL_LENGTH)
        QWORDBUSMEMORYSET(19, BCM2836_PWM_BUS_BASE_ADDRESS, BCM2836_PWM_BUS_LENGTH)
        QWORDBUSMEMORYSET(20, BCM2836_PWM_CTRL_UNCACHED_BASE_ADDRESS, BCM2836_PWM_CTRL_UNCACHED_LENGTH)
        QWORDMEMORYSET(21, BCM2836_PWM_CLK_OFFSET, BCM2836_PWM_CLK_LENGTH)

        // UART
        QWORDMEMORYSET(22, BCM2836_PL011_UART_OFFSET, BCM2836_PL011_UART_LENGTH)
        QWORDMEMORYSET(23, BCM2836_MINI_UART_OFFSET, BCM2836_MINI_UART_LENGTH)

        // SDC
        QWORDMEMORYSET(24, MMCHS1_OFFSET, MMCHS1_LENGTH)
        QWORDMEMORYSET(25, SDHOST_OFFSET, SDHOST_LENGTH)

        Return (RBUF)
      }

      Name (_DMA, ResourceTemplate() {
        //
        // Only the first GB is available.
        // Bus 0xC0000000 -> CPU 0x00000000.
        //
        QWordMemory (ResourceConsumer,
          ,
          MinFixed,
          MaxFixed,
          NonCacheable,
          ReadWrite,
          0x0,
          0x00000000C0000000, // MIN
          0x00000000FFFFFFFF, // MAX
          0xFFFFFFFF40000000, // TRA
          0x0000000040000000, // LEN
          ,
          ,
          )
      })
      include ("GpuDevs.asl")
    }

#if (RPI_MODEL == 4)
    Device (ETH0)
    {
      Name (_HID, "BCM6E4E")
      Name (_CID, "BCM6E4E")
      Name (_UID, 0x0)
      Name (_CCA, 0x0)
      Method (_STA)
      {
        Return (0xf)
      }
      Method (_CRS, 0x0, Serialized)
      {
        Name (RBUF, ResourceTemplate ()
        {
          // No need for MEMORY32SETBASE on Genet as we have a straight base address constant
          MEMORY32FIXED (ReadWrite, GENET_BASE_ADDRESS, GENET_LENGTH, )
          Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { GENET_INTERRUPT0, GENET_INTERRUPT1 }
        })
        Return (RBUF)
      }
      Name (_DSD, Package () {
        ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
        Package () {
          Package () { "brcm,max-dma-burst-size", 0x08 },
          Package () { "phy-mode", "rgmii-rxid" },
        }
      })
    }
#endif

  }
}
