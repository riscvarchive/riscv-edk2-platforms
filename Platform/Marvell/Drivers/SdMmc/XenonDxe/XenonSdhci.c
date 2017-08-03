/*******************************************************************************
Copyright (C) 2016 Marvell International Ltd.

Marvell BSD License Option

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File under the following licensing terms.
Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.

* Neither the name of Marvell nor the names of its contributors may be
  used to endorse or promote products derived from this software without
  specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/

#include "XenonSdhci.h"

STATIC
VOID
XenonReadVersion (
  IN  EFI_PCI_IO_PROTOCOL   *PciIo,
  OUT UINT32 *ControllerVersion
  )
{
  SdMmcHcRwMmio (PciIo, SD_BAR_INDEX, SD_MMC_HC_CTRL_VER, TRUE, SDHC_REG_SIZE_2B, ControllerVersion);
}

STATIC
VOID
XenonSetFifo (
  IN EFI_PCI_IO_PROTOCOL   *PciIo
  )
{
  UINTN Data;

  // Set FIFO_RTC, FIFO_WTC, FIFO_CS and FIFO_PDLVMC
  Data = SDHC_SLOT_FIFO_DEFAULT_CONFIG;

  SdMmcHcRwMmio (PciIo, SD_BAR_INDEX, SDHC_SLOT_FIFO_CTRL, FALSE, SDHC_REG_SIZE_4B, &Data);
}

// Auto Clock Gating
STATIC
VOID
XenonSetAcg (
  IN EFI_PCI_IO_PROTOCOL   *PciIo,
  IN BOOLEAN Enable
  )
{
  UINT32 Var;

  SdMmcHcRwMmio (PciIo, SD_BAR_INDEX, SDHC_SYS_OP_CTRL, TRUE, SDHC_REG_SIZE_4B, &Var);

  if (Enable) {
    Var &= ~AUTO_CLKGATE_DISABLE_MASK;
  } else {
    Var |= AUTO_CLKGATE_DISABLE_MASK;
  }

  SdMmcHcRwMmio (PciIo, SD_BAR_INDEX, SDHC_SYS_OP_CTRL, FALSE, SDHC_REG_SIZE_4B, &Var);
}

STATIC
VOID
XenonSetSlot (
  IN EFI_PCI_IO_PROTOCOL *PciIo,
  IN UINT8 Slot,
  IN BOOLEAN Enable
  )
{
  UINT32 Var;

  SdMmcHcRwMmio (PciIo, SD_BAR_INDEX, SDHC_SYS_OP_CTRL, TRUE, SDHC_REG_SIZE_4B, &Var);
  if (Enable) {
    Var |= ((0x1 << Slot) << SLOT_ENABLE_SHIFT);
  } else {
    Var &= ~((0x1 << Slot) << SLOT_ENABLE_SHIFT);
  }

  SdMmcHcRwMmio (PciIo, SD_BAR_INDEX, SDHC_SYS_OP_CTRL, FALSE, SDHC_REG_SIZE_4B, &Var);
}

//
// Stub function, which will in future be responsible for
// setting SDIO controller in either HIGH (if Voltage parameter
// is equal 1) or LOW (if Voltage is equal 0)
//
STATIC
VOID
XenonSetSdio (
  IN EFI_PCI_IO_PROTOCOL *PciIo,
  IN UINTN Voltage
  )
{
  // Currently SDIO isn't supported
  return;
}

STATIC
VOID
XenonSetPower (
  IN EFI_PCI_IO_PROTOCOL *PciIo,
  IN UINT32 Vcc,
  IN UINT32 Vccq,
  IN UINT8 Mode
  )
{
  UINT8 Pwr = 0;
  UINT32 Ctrl = 0;

  // Below statement calls routine to set voltage for SDIO devices in either HIGH (1) or LOW (0) mode
  switch (Vcc) {
  case MMC_VDD_165_195:
    Pwr = SDHCI_POWER_180;
    if (Mode == XENON_MMC_MODE_SD_SDIO) {
      XenonSetSdio (PciIo, 0);
    }
    break;
  case MMC_VDD_29_30:
  case MMC_VDD_30_31:
    Pwr = SDHCI_POWER_300;
    if (Mode == XENON_MMC_MODE_SD_SDIO) {
      XenonSetSdio (PciIo, 1);
    }
    break;
  case MMC_VDD_32_33:
  case MMC_VDD_33_34:
    Pwr = SDHCI_POWER_330;
    if (Mode == XENON_MMC_MODE_SD_SDIO) {
      XenonSetSdio (PciIo, 1);
    }
    break;
  default:
    DEBUG((DEBUG_ERROR, "SD/MMC: Does not support power mode(0x%X)\n", Vcc));
    break;
  }

  if (Pwr == 0) {
    SdMmcHcRwMmio (PciIo, SD_BAR_INDEX, SD_MMC_HC_POWER_CTRL, FALSE, SDHC_REG_SIZE_1B, &Pwr);
    return;
  }

  Pwr |= SDHCI_POWER_ON;

  SdMmcHcRwMmio (PciIo, SD_BAR_INDEX,SD_MMC_HC_POWER_CTRL, FALSE, SDHC_REG_SIZE_1B, &Pwr);

  // Set VCCQ
  SdMmcHcRwMmio (PciIo, SD_BAR_INDEX, SDHC_SLOT_eMMC_CTRL, TRUE, SDHC_REG_SIZE_4B, &Ctrl);
  Ctrl |= Vccq;
  SdMmcHcRwMmio (PciIo, SD_BAR_INDEX, SDHC_SLOT_eMMC_CTRL, FALSE, SDHC_REG_SIZE_4B, &Ctrl);
}

UINTN
XenonSetClk (
  IN EFI_PCI_IO_PROTOCOL   *PciIo,
  IN SD_MMC_HC_PRIVATE_DATA *Private,
  IN UINT32 Clock
  )
{
  UINT32 Div;
  UINT32 Clk;
  UINT32 Retry;
  UINT16 Value = 0;

  SdMmcHcRwMmio (PciIo, SD_BAR_INDEX, SD_MMC_HC_CLOCK_CTRL, FALSE, SDHC_REG_SIZE_2B, &Value);

  if (Clock == 0) {
    return 0;
  }

  if (Private->ControllerVersion >= SDHCI_SPEC_300) {
    // Version 3.00 Divisors must be a multiple of 2
    if (XENON_MMC_MAX_CLK <= Clock) {
      Div = 1;
    } else {
      for (Div = 2; Div < SDHCI_MAX_DIV_SPEC_300; Div += 2) {
        if ((XENON_MMC_MAX_CLK / Div) <= Clock)
          break;
      }
    }
  } else {
    // Version 2.00 Divisors must be a power of 2
    for (Div = 1; Div < SDHCI_MAX_DIV_SPEC_200; Div *= 2) {
      if ((XENON_MMC_MAX_CLK / Div) <= Clock)
        break;
    }
  }
  Div >>= 1;

  Clk = (Div & SDHCI_DIV_MASK) << SDHCI_DIVIDER_SHIFT;
  Clk |= ((Div & SDHCI_DIV_HI_MASK) >> SDHCI_DIV_MASK_LEN) << SDHCI_DIVIDER_HI_SHIFT;
  Clk |= SDHCI_CLOCK_INT_EN;

  SdMmcHcRwMmio (PciIo, SD_BAR_INDEX, SD_MMC_HC_CLOCK_CTRL, FALSE, SDHC_REG_SIZE_2B, &Clk);

  //
  // Poll for internal controller clock to be stabilised
  // Wait up to 200us for this to occur
  //
  Retry = 200;

  do {
    SdMmcHcRwMmio (PciIo, SD_BAR_INDEX, SD_MMC_HC_CLOCK_CTRL, TRUE, SDHC_REG_SIZE_2B, &Clk);
    if (Retry == 0) {
      DEBUG((DEBUG_ERROR, "SD/MMC: Internal Clock never stabilised\n"));
      return -1;
    }

    Retry--;

    // Wait for internal clock to be stabilised
    gBS->Stall (1);

  } while (!(Clk & SDHCI_CLOCK_INT_STABLE));

  Clk |= SDHCI_CLOCK_CARD_EN;
  SdMmcHcRwMmio (PciIo, SD_BAR_INDEX, SD_MMC_HC_CLOCK_CTRL, FALSE, SDHC_REG_SIZE_2B, &Clk);

  return 0;
}

VOID
XenonPhyInit (
  IN EFI_PCI_IO_PROTOCOL   *PciIo
  )
{
  UINT32 Var, Wait, Time;
  UINT32 Clock = XENON_MMC_MAX_CLK;
  UINT16 ClkCtrl;

  // Need to disable the clock to set EMMC_PHY_TIMING_ADJUST register
  SdMmcHcRwMmio (PciIo, SD_BAR_INDEX, SD_MMC_HC_CLOCK_CTRL, TRUE, SDHC_REG_SIZE_2B, &ClkCtrl);
  ClkCtrl &= ~(SDHCI_CLOCK_CARD_EN | SDHCI_CLOCK_INT_EN);
  SdMmcHcRwMmio (PciIo, SD_BAR_INDEX, SD_MMC_HC_CLOCK_CTRL, FALSE, SDHC_REG_SIZE_2B, &ClkCtrl);

  // Enable QSP PHASE SELECT
  SdMmcHcRwMmio (PciIo, SD_BAR_INDEX, EMMC_PHY_TIMING_ADJUST, TRUE, SDHC_REG_SIZE_4B, &Var);
  Var |= SAMPL_INV_QSP_PHASE_SELECT;
  SdMmcHcRwMmio (PciIo, SD_BAR_INDEX, EMMC_PHY_TIMING_ADJUST, FALSE, SDHC_REG_SIZE_4B, &Var);

  // Enable internal clock
  SdMmcHcRwMmio (PciIo, SD_BAR_INDEX, SD_MMC_HC_CLOCK_CTRL, TRUE, SDHC_REG_SIZE_2B, &ClkCtrl);
  ClkCtrl |= SDHCI_CLOCK_INT_EN;
  SdMmcHcRwMmio (PciIo, SD_BAR_INDEX, SD_MMC_HC_CLOCK_CTRL, FALSE, SDHC_REG_SIZE_2B, &ClkCtrl);

  //
  // Poll for host MMC PHY clock init to be stable
  // Wait up to 100us
  //
  Time = 100;
  while (Time--) {
    SdMmcHcRwMmio (PciIo, SD_BAR_INDEX, EMMC_PHY_TIMING_ADJUST, TRUE, SDHC_REG_SIZE_4B, &Var);
    if (Var & SDHCI_CLOCK_INT_STABLE) {
      break;
    }

    // Poll interval for MMC PHY clock to be stable is 1us
    gBS->Stall (1);
  }
  if (Time <= 0) {
    DEBUG((DEBUG_ERROR, "SD/MMC: Failed to enable MMC internal clock in Time\n"));
    return;
  }

  // Enable bus clock
  SdMmcHcRwMmio (PciIo, SD_BAR_INDEX, SD_MMC_HC_CLOCK_CTRL, TRUE, SDHC_REG_SIZE_2B, &ClkCtrl);
  ClkCtrl |= SDHCI_CLOCK_CARD_EN;
  SdMmcHcRwMmio (PciIo, SD_BAR_INDEX, SD_MMC_HC_CLOCK_CTRL, FALSE, SDHC_REG_SIZE_2B, &ClkCtrl);

  // Delay 200us to wait for the completion of bus clock
  gBS->Stall (200);

  // Init PHY
  SdMmcHcRwMmio (PciIo, SD_BAR_INDEX, EMMC_PHY_TIMING_ADJUST, TRUE, SDHC_REG_SIZE_4B, &Var);
  Var |= PHY_INITIALIZAION;
  SdMmcHcRwMmio (PciIo, SD_BAR_INDEX, EMMC_PHY_TIMING_ADJUST, FALSE, SDHC_REG_SIZE_4B, &Var);

  // Add duration of FC_SYNC_RST
  Wait = ((Var >> FC_SYNC_RST_DURATION_SHIFT) & FC_SYNC_RST_DURATION_MASK);

  // Add interval between FC_SYNC_EN and FC_SYNC_RST
  Wait += ((Var >> FC_SYNC_RST_EN_DURATION_SHIFT) & FC_SYNC_RST_EN_DURATION_MASK);

  // Add duration of asserting FC_SYNC_EN
  Wait += ((Var >> FC_SYNC_EN_DURATION_SHIFT) & FC_SYNC_EN_DURATION_MASK);

  // Add duration of Waiting for PHY
  Wait += ((Var >> WAIT_CYCLE_BEFORE_USING_SHIFT) & WAIT_CYCLE_BEFORE_USING_MASK);

  // 4 addtional bus clock and 4 AXI bus clock are required left shift 20 bits
  Wait += 8;
  Wait <<= 20;

  // Use the possibly slowest bus frequency value
  if (Clock == 0) {
    Clock = XENON_MMC_MIN_CLK;
  }

  // Get the Wait Time in unit of ms
  Wait = Wait / Clock;
  Wait++;

  // Poll for host eMMC PHY init to complete, wait up to 100us
  Time = 100;
  while (Time--) {
    Var = SdMmcHcRwMmio (PciIo, SD_BAR_INDEX, EMMC_PHY_TIMING_ADJUST, TRUE, SDHC_REG_SIZE_4B, &Var);
    Var &= PHY_INITIALIZAION;
    if (!Var) {
      break;
    }

    // Wait for host eMMC PHY init to complete
    gBS->Stall (1);
  }

  if (Time <= 0) {
    DEBUG((DEBUG_ERROR, "SD/MMC: Failed to init MMC PHY in Time\n"));
    return;
  }

  return;
}

STATIC
VOID
XenonSetPhy (
  IN EFI_PCI_IO_PROTOCOL   *PciIo,
  UINT8 Timing
  )
{
  UINT32 Var = 0;

  // Setup pad, set bit[30], bit[28] and bits[26:24]
  SdMmcHcRwMmio (PciIo, SD_BAR_INDEX, EMMC_PHY_PAD_CONTROL, TRUE, SDHC_REG_SIZE_4B, &Var);
  Var |= (AUTO_RECEN_CTRL | OEN_QSN | FC_QSP_RECEN | FC_CMD_RECEN | FC_DQ_RECEN);
  SdMmcHcRwMmio (PciIo, SD_BAR_INDEX, EMMC_PHY_PAD_CONTROL, FALSE, SDHC_REG_SIZE_4B, &Var);

  //
  // If Timing belongs to high speed, set bit[17] of
  // EMMC_PHY_TIMING_ADJUST register
  //
  if ((Timing == MMC_TIMING_MMC_HS400) ||
      (Timing == MMC_TIMING_MMC_HS200) ||
      (Timing == MMC_TIMING_UHS_SDR50) ||
      (Timing == MMC_TIMING_UHS_SDR104) ||
      (Timing == MMC_TIMING_UHS_DDR50) ||
      (Timing == MMC_TIMING_UHS_SDR25)) {

    SdMmcHcRwMmio(PciIo, SD_BAR_INDEX, EMMC_PHY_TIMING_ADJUST, TRUE, SDHC_REG_SIZE_4B, &Var);

    // Set SLOW_MODE for PHY
    Var |= OUTPUT_QSN_PHASE_SELECT | QSN_PHASE_SLOW_MODE_BIT;
    SdMmcHcRwMmio(PciIo, SD_BAR_INDEX, EMMC_PHY_TIMING_ADJUST, FALSE, SDHC_REG_SIZE_4B, &Var);
  }

  SdMmcHcRwMmio(PciIo, SD_BAR_INDEX, EMMC_PHY_FUNC_CONTROL, TRUE, SDHC_REG_SIZE_4B, &Var);
  Var |= (DQ_DDR_MODE_MASK << DQ_DDR_MODE_SHIFT) | CMD_DDR_MODE;
  SdMmcHcRwMmio(PciIo, SD_BAR_INDEX, EMMC_PHY_FUNC_CONTROL, FALSE, SDHC_REG_SIZE_4B, &Var);

  if (Timing == MMC_TIMING_MMC_HS400) {
    SdMmcHcRwMmio(PciIo, SD_BAR_INDEX, EMMC_PHY_FUNC_CONTROL, TRUE, SDHC_REG_SIZE_4B, &Var);
    Var &= ~DQ_ASYNC_MODE;
    SdMmcHcRwMmio(PciIo, SD_BAR_INDEX, EMMC_PHY_FUNC_CONTROL, FALSE, SDHC_REG_SIZE_4B, &Var);

    Var = LOGIC_TIMING_VALUE;
    SdMmcHcRwMmio(PciIo, SD_BAR_INDEX, EMMC_LOGIC_TIMING_ADJUST, FALSE, SDHC_REG_SIZE_4B, &Var);
  }

  XenonPhyInit (PciIo);
}

STATIC
VOID
XenonConfigureInterrupts (
  IN EFI_PCI_IO_PROTOCOL *PciIo
  )
{
  UINT32 Var;

  // Clear interrupt status
  Var = SDHC_CLR_ALL_IRQ_MASK;
  SdMmcHcRwMmio (PciIo, SD_BAR_INDEX, SD_MMC_HC_NOR_INT_STS, FALSE, SDHC_REG_SIZE_4B, &Var);
  SdMmcHcRwMmio (PciIo, SD_BAR_INDEX, SD_MMC_HC_NOR_INT_STS, FALSE, SDHC_REG_SIZE_4B, &Var);

  // Enable only interrupts served by the SD controller
  Var = SDHC_CLR_ALL_IRQ_MASK & ~(NOR_INT_STS_CARD_INS | NOR_INT_STS_CARD_INT);
  SdMmcHcRwMmio (PciIo, SD_BAR_INDEX, SD_MMC_HC_NOR_INT_STS_EN, FALSE, SDHC_REG_SIZE_4B, &Var);

  // Mask all sdhci interrupt sources
  Var = SDHC_CLR_ALL_IRQ_MASK & ~NOR_INT_SIG_EN_CARD_INT;
  SdMmcHcRwMmio (PciIo, SD_BAR_INDEX, SD_MMC_HC_NOR_INT_SIG_EN, FALSE, SDHC_REG_SIZE_4B, &Var);
}

// Enable Parallel Transfer Mode
STATIC
VOID
XenonSetParallelTransfer (
  IN EFI_PCI_IO_PROTOCOL *PciIo,
  IN UINT8 Slot,
  IN BOOLEAN Enable
  )
{
  UINT32 Var;

  SdMmcHcRwMmio(PciIo, SD_BAR_INDEX, SDHC_SYS_EXT_OP_CTRL, TRUE, SDHC_REG_SIZE_4B, &Var);

  if (Enable) {
    Var |= (0x1 << Slot);
  } else {
    Var &= ~(0x1 << Slot);
  }

  SdMmcHcRwMmio(PciIo, SD_BAR_INDEX, SDHC_SYS_EXT_OP_CTRL, FALSE, SDHC_REG_SIZE_4B, &Var);
}

STATIC
VOID
XenonSetTuning (
  IN EFI_PCI_IO_PROTOCOL   *PciIo,
  IN UINT8 Slot,
  IN BOOLEAN Enable
  )
{
  UINT32 Var;

  // Set the Re-Tuning Request functionality
  SdMmcHcRwMmio(PciIo, SD_BAR_INDEX, SDHC_SLOT_RETUNING_REQ_CTRL, TRUE, SDHC_REG_SIZE_4B, &Var);

  if (Enable) {
    Var |= RETUNING_COMPATIBLE;
  } else {
    Var &= ~RETUNING_COMPATIBLE;
  }

  SdMmcHcRwMmio(PciIo, SD_BAR_INDEX, SDHC_SLOT_RETUNING_REQ_CTRL, FALSE, SDHC_REG_SIZE_4B, &Var);

  // Set the Re-tuning Event Signal Enable
  SdMmcHcRwMmio(PciIo, SD_BAR_INDEX, SDHCI_SIGNAL_ENABLE, TRUE, SDHC_REG_SIZE_4B, &Var);

  if (Enable) {
    Var |= SDHCI_RETUNE_EVT_INTSIG;
  } else {
    Var &= ~SDHCI_RETUNE_EVT_INTSIG;
  }

  SdMmcHcRwMmio(PciIo, SD_BAR_INDEX, SDHCI_SIGNAL_ENABLE, FALSE, SDHC_REG_SIZE_4B, &Var);
}

VOID
XenonReset (
  IN SD_MMC_HC_PRIVATE_DATA *Private,
  IN UINT8 Slot,
  IN UINT8 Mask
  )
{
  UINT32 Retry = 1000;
  UINT8 SwReset;

  SwReset = Mask;

  SdMmcHcRwMmio (
          Private->PciIo,
          Slot,
          SD_MMC_HC_SW_RST,
          FALSE,
          sizeof (SwReset),
          &SwReset
        );

  SdMmcHcRwMmio (
          Private->PciIo,
          Slot,
          SD_MMC_HC_SW_RST,
          TRUE,
          sizeof (SwReset),
          &SwReset
        );

  while (SwReset & Mask) {
    if (Retry == 0) {
      DEBUG((DEBUG_ERROR, "SD/MMC: Reset never completed\n"));
      return;
    }

    Retry--;

    // Poll interval for SwReset is 100us according to SDHCI spec
    gBS-> Stall (100);
    SdMmcHcRwMmio (
            Private->PciIo,
            Slot,
            SD_MMC_HC_SW_RST,
            TRUE,
            sizeof (SwReset),
            &SwReset
          );
  }
}

STATIC
VOID
XenonTransferPio (
  IN SD_MMC_HC_PRIVATE_DATA *Private,
  IN UINT8 Slot,
  IN OUT VOID *Buffer,
  IN UINT16 BlockSize,
  IN BOOLEAN Read
  )
{
  UINTN Index;
  UINT8 *Offs;

  //
  // SD stack's intrinsic functions cannot perform properly reading/writing from
  // buffer register, that is why MmioRead/MmioWrite are used. It is temporary
  // solution.
  //
  for (Index = 0; Index < BlockSize; Index += 4) {
    Offs = Buffer + Index;
    if (Read) {
      *(UINT32 *)Offs = MmioRead32 (SDHC_DAT_BUF_PORT_ADDR);
    } else {
      MmioWrite32 (SDHC_DAT_BUF_PORT_ADDR, *(UINT32 *)Offs);
    }
  }
}

EFI_STATUS
XenonTransferData (
  IN SD_MMC_HC_PRIVATE_DATA *Private,
  IN UINT8 Slot,
  IN OUT VOID *Buffer,
  IN UINT32 DataLen,
  IN UINT16 BlockSize,
  IN UINT16 Blocks,
  IN BOOLEAN Read
  )
{
  UINT32 IntStatus, PresentState, Rdy, Mask, Retry, Block = 0;

  if (Buffer == NULL) {
    return EFI_DEVICE_ERROR;
  }

  Retry = SDHC_INT_STATUS_POLL_RETRY_DATA_TRAN;
  Rdy = NOR_INT_STS_TX_RDY | NOR_INT_STS_RX_RDY;
  Mask = PRESENT_STATE_BUFFER_RD_EN | PRESENT_STATE_BUFFER_WR_EN;

  do {
    SdMmcHcRwMmio (
            Private->PciIo,
            Slot,
            SD_MMC_HC_NOR_INT_STS,
            TRUE,
            sizeof (IntStatus),
            &IntStatus
          );

    if (IntStatus & NOR_INT_STS_ERR_INT) {
      DEBUG((DEBUG_INFO, "SD/MMC: Error detected in status %0x\n", IntStatus));
      return EFI_DEVICE_ERROR;
    }

    if (IntStatus & Rdy) {
      SdMmcHcRwMmio (
              Private->PciIo,
              Slot,
              SD_MMC_HC_PRESENT_STATE,
              TRUE,
              sizeof (PresentState),
              &PresentState
            );

      if (!(PresentState & Mask)) {
        continue;
      }

      SdMmcHcRwMmio (
              Private->PciIo,
              Slot,
              SD_MMC_HC_NOR_INT_STS,
              FALSE,
              sizeof (Rdy),
              &Rdy
            );

      XenonTransferPio (Private, Slot, Buffer, BlockSize, Read);

      Buffer += BlockSize;
      if (++Block >= Blocks) {
        break;
      }
    }

    if (Retry-- > 0) {

      // Poll interval for data transfer complete bit in NOR_INT_STS register is 10us
      gBS->Stall (10);
    } else {
      DEBUG((DEBUG_INFO, "SD/MMC: Transfer data timeout\n"));
      return EFI_TIMEOUT;
    }
  } while (!(IntStatus & NOR_INT_STS_XFER_COMPLETE));

  return EFI_SUCCESS;
}

EFI_STATUS
XenonInit (
  IN SD_MMC_HC_PRIVATE_DATA *Private
  )
{
  EFI_PCI_IO_PROTOCOL *PciIo = Private->PciIo;

  // Read XENON version
  XenonReadVersion (PciIo, &Private->ControllerVersion);

  XenonSetFifo (PciIo);

  // Disable auto clock generator
  XenonSetAcg (PciIo, FALSE);

  // XENON has only one port
  XenonSetSlot (PciIo, XENON_MMC_SLOT_ID, TRUE);

  XenonSetPower (PciIo, MMC_VDD_165_195, eMMC_VCCQ_1_8V, XENON_MMC_MODE_SD_SDIO);

  // Set MAX_CLOCK for configuring PHY
  XenonSetClk (PciIo, Private, XENON_MMC_MAX_CLK);
  XenonSetPhy (PciIo, MMC_TIMING_UHS_SDR50);

  XenonConfigureInterrupts (PciIo);

  // Enable parallel transfer
  XenonSetParallelTransfer (PciIo, XENON_MMC_SLOT_ID, TRUE);
  XenonSetTuning (PciIo, XENON_MMC_SLOT_ID, FALSE);

  // Enable auto clock generator
  XenonSetAcg (PciIo, TRUE);

  // Set proper clock for PHY configuration
  XenonSetClk (PciIo, Private, XENON_MMC_BASE_CLK);
  XenonPhyInit (PciIo);

  return EFI_SUCCESS;
}
