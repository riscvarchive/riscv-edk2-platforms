/** @file
*
*  Copyright (c) 2018, Linaro Ltd. All rights reserved.
*
*  This program and the accompanying materials
*  are licensed and made available under the terms and conditions of the BSD License
*  which accompanies this distribution.  The full text of the license may be found at
*  http://opensource.org/licenses/bsd-license.php
*
*  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
*  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
*
**/

#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/UefiLib.h>

#include <Hi6220.h>
#include <Hi6220RegsPeri.h>

#include "HiKeyDxe.h"

STATIC
VOID
UartInit (
  IN VOID
  )
{
  UINT32     Val;

  /* make UART1 out of reset */
  MmioWrite32 (PERI_CTRL_BASE + SC_PERIPH_RSTDIS3, PERIPH_RST3_UART1);
  MmioWrite32 (PERI_CTRL_BASE + SC_PERIPH_CLKEN3, PERIPH_RST3_UART1);
  /* make UART2 out of reset */
  MmioWrite32 (PERI_CTRL_BASE + SC_PERIPH_RSTDIS3, PERIPH_RST3_UART2);
  MmioWrite32 (PERI_CTRL_BASE + SC_PERIPH_CLKEN3, PERIPH_RST3_UART2);
  /* make UART3 out of reset */
  MmioWrite32 (PERI_CTRL_BASE + SC_PERIPH_RSTDIS3, PERIPH_RST3_UART3);
  MmioWrite32 (PERI_CTRL_BASE + SC_PERIPH_CLKEN3, PERIPH_RST3_UART3);
  /* make UART4 out of reset */
  MmioWrite32 (PERI_CTRL_BASE + SC_PERIPH_RSTDIS3, PERIPH_RST3_UART4);
  MmioWrite32 (PERI_CTRL_BASE + SC_PERIPH_CLKEN3, PERIPH_RST3_UART4);

  /* make DW_MMC2 out of reset */
  MmioWrite32 (PERI_CTRL_BASE + SC_PERIPH_RSTDIS0, PERIPH_RST0_MMC2);

  /* enable clock for BT/WIFI */
  Val = MmioRead32 (PMUSSI_ONOFF8_REG) | PMUSSI_ONOFF8_EN_32KB;
  MmioWrite32 (PMUSSI_ONOFF8_REG, Val);
}

STATIC
VOID
MtcmosInit (
  IN VOID
  )
{
  UINT32     Data;

  /* enable MTCMOS for GPU */
  MmioWrite32 (AO_CTRL_BASE + SC_PW_MTCMOS_EN0, PW_EN0_G3D);
  do {
    Data = MmioRead32 (AO_CTRL_BASE + SC_PW_MTCMOS_ACK_STAT0);
  } while ((Data & PW_EN0_G3D) == 0);
}

EFI_STATUS
HiKeyInitPeripherals (
  IN VOID
  )
{
  UINT32     Data, Bits;

  /* make I2C0/I2C1/I2C2/SPI0 out of reset */
  Bits = PERIPH_RST3_I2C0 | PERIPH_RST3_I2C1 | PERIPH_RST3_I2C2 | \
         PERIPH_RST3_SSP;
  MmioWrite32 (PERI_CTRL_BASE + SC_PERIPH_RSTDIS3, Bits);

  do {
    Data = MmioRead32 (PERI_CTRL_BASE + SC_PERIPH_RSTSTAT3);
  } while (Data & Bits);

  UartInit ();
  /* MTCMOS -- Multi-threshold CMOS */
  MtcmosInit ();

  /* Set DETECT_J15_FASTBOOT (GPIO24) pin as GPIO function */
  MmioWrite32 (IOCG_084_REG, 0);        /* configure GPIO24 as nopull */
  MmioWrite32 (IOMG_080_REG, 0);        /* configure GPIO24 as GPIO */

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
HiKeyEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS            Status;

  Status = HiKeyInitPeripherals ();
  if (EFI_ERROR (Status)) {
    return Status;
  }
  return Status;
}
