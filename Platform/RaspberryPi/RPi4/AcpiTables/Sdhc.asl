/** @file
 *
 *  [DSDT] SD controller/card definition (SDHC)
 *
 *  Copyright (c) 2020, Pete Batard <pete@akeo.ie>
 *  Copyright (c) 2018, Andrey Warkentin <andrey.warkentin@gmail.com>
 *  Copyright (c) Microsoft Corporation. All rights reserved.
 *
 *  SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 **/

#include <IndustryStandard/Bcm2836SdHost.h>
#include <IndustryStandard/Bcm2836Sdio.h>

#include "AcpiTables.h"

//
// Note: UEFI can use either SDHost or Arasan. We expose both to the OS.
//

// ArasanSD 3.0 SD Host Controller.
Device (SDC1)
{
  Name (_HID, "BCM2847")
  Name (_CID, "ARASAN")
  Name (_UID, 0x0)
  Name (_CCA, 0x0)
  Name (_S1D, 0x1)
  Name (_S2D, 0x1)
  Name (_S3D, 0x1)
  Name (_S4D, 0x1)
  Method (_STA)
  {
    Return(0xf)
  }
  Name (RBUF, ResourceTemplate ()
  {
    MEMORY32FIXED (ReadWrite, 0, MMCHS1_LENGTH, RMEM)
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { BCM2836_MMCHS1_INTERRUPT }
  })
  Method (_CRS, 0x0, Serialized)
  {
    MEMORY32SETBASE (RBUF, RMEM, RBAS, MMCHS1_OFFSET)
    Return (^RBUF)
  }

  //
  // A child device that represents the
  // sd card, which is marked as non-removable.
  //
  Device (SDMM)
  {
    Method (_ADR)
    {
      Return (0)
    }
    Method (_RMV) // Is removable
    {
      Return (0) // 0 - fixed
    }
  }
}


// Broadcom SDHost 2.0 SD Host Controller
Device (SDC2)
{
  Name (_HID, "BCM2855")
  Name (_CID, "SDHST")
  Name (_UID, 0x0)
  Name (_CCA, 0x0)
  Name (_S1D, 0x1)
  Name (_S2D, 0x1)
  Name (_S3D, 0x1)
  Name (_S4D, 0x1)
  Method (_STA)
  {
    Return (0xf)
  }
  Name (RBUF, ResourceTemplate ()
  {
    MEMORY32FIXED (ReadWrite, 0, SDHOST_LENGTH, RMEM)
    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { BCM2836_SDHOST_INTERRUPT }
  })
  Method (_CRS, 0x0, Serialized)
  {
    MEMORY32SETBASE (RBUF, RMEM, RBAS, SDHOST_OFFSET)
    Return (^RBUF)
  }

  //
  // A child device that represents the
  // sd card, which is marked as non-removable.
  //
  Device (SDMM)
  {
    Method (_ADR)
    {
      Return (0)
    }
    Method (_RMV) // Is removable
    {
      Return (0) // 0 - fixed
    }
  }
}
