/** @Soc.c
  SoC specific Library containg functions to initialize various SoC components

  Copyright 2017-2019 NXP

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <NxpChassis.h>
#include <Chassis2/NxpSoc.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/IoAccessLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/PrintLib.h>
#include <Library/SerialPortLib.h>

/**
  Calculate the frequency of various controllers and
  populate the passed structure with frequuencies.

  @param  PtrSysInfo            Input structure to populate with
                                frequencies.
**/
VOID
GetSysInfo (
  OUT SYS_INFO *PtrSysInfo
  )
{
  CCSR_GUR     *GurBase;
  CCSR_CLOCK   *ClkBase;
  UINTN        CpuIndex;
  UINT32       TempRcw;
  UINT32       CPllSel;
  UINT32       CplxPll;
  CONST UINT8  CoreCplxPll[8] = {
    [0] = 0,    /* CC1 PPL / 1 */
    [1] = 0,    /* CC1 PPL / 2 */
    [4] = 1,    /* CC2 PPL / 1 */
    [5] = 1,    /* CC2 PPL / 2 */
  };

  CONST UINT8  CoreCplxPllDivisor[8] = {
    [0] = 1,    /* CC1 PPL / 1 */
    [1] = 2,    /* CC1 PPL / 2 */
    [4] = 1,    /* CC2 PPL / 1 */
    [5] = 2,    /* CC2 PPL / 2 */
  };

  UINTN        PllCount;
  UINTN        FreqCPll[NUM_CC_PLLS];
  UINTN        PllRatio[NUM_CC_PLLS];
  UINTN        SysClk;

  GurBase = (VOID *)PcdGet64 (PcdGutsBaseAddr);
  ClkBase = (VOID *)PcdGet64 (PcdClkBaseAddr);
  SysClk = CLK_FREQ;

  SetMem (PtrSysInfo, sizeof (SYS_INFO), 0);

  PtrSysInfo->FreqSystemBus = SysClk;
  PtrSysInfo->FreqDdrBus = SysClk;

  //
  // selects the platform clock:SYSCLK ratio and calculate
  // system frequency
  //
  PtrSysInfo->FreqSystemBus *= (GurRead ((UINTN)&GurBase->RcwSr[0]) >>
                CHASSIS2_RCWSR0_SYS_PLL_RAT_SHIFT) &
                CHASSIS2_RCWSR0_SYS_PLL_RAT_MASK;
  //
  // selects the DDR PLL:SYSCLK Ratio and calculate DDR frequency
  //
  PtrSysInfo->FreqDdrBus *= (GurRead ((UINTN)&GurBase->RcwSr[0]) >>
                CHASSIS2_RCWSR0_MEM_PLL_RAT_SHIFT) &
                CHASSIS2_RCWSR0_MEM_PLL_RAT_MASK;

  for (PllCount = 0; PllCount < NUM_CC_PLLS; PllCount++) {
    PllRatio[PllCount] = (GurRead ((UINTN)&ClkBase->PllCgSr[PllCount].PllCnGSr) >> 1) & 0xff;
    if (PllRatio[PllCount] > 4) {
      FreqCPll[PllCount] = SysClk * PllRatio[PllCount];
    } else {
      FreqCPll[PllCount] = PtrSysInfo->FreqSystemBus * PllRatio[PllCount];
    }
  }

  //
  // Calculate Core frequency
  //
  for (CpuIndex = 0; CpuIndex < MAX_CPUS; CpuIndex++) {
    CPllSel = (GurRead ((UINTN)&ClkBase->ClkcSr[CpuIndex].ClkCnCSr) >> 27) & 0xf;
    CplxPll = CoreCplxPll[CPllSel];

    PtrSysInfo->FreqProcessor[CpuIndex] = FreqCPll[CplxPll] / CoreCplxPllDivisor[CPllSel];
  }

  //
  // Calculate FMAN frequency
  //
  TempRcw = GurRead ((UINTN)&GurBase->RcwSr[7]);
  switch ((TempRcw & HWA_CGA_M1_CLK_SEL) >> HWA_CGA_M1_CLK_SHIFT) {
  case 2:
    PtrSysInfo->FreqFman[0] = FreqCPll[0] / 2;
    break;
  case 3:
    PtrSysInfo->FreqFman[0] = FreqCPll[0] / 3;
    break;
  case 4:
    PtrSysInfo->FreqFman[0] = FreqCPll[0] / 4;
    break;
  case 5:
    PtrSysInfo->FreqFman[0] = PtrSysInfo->FreqSystemBus;
    break;
  case 6:
    PtrSysInfo->FreqFman[0] = FreqCPll[1] / 2;
    break;
  case 7:
    PtrSysInfo->FreqFman[0] = FreqCPll[1] / 3;
    break;
  default:
    DEBUG ((DEBUG_WARN, "Error: Unknown FMan1 clock select!\n"));
    break;
  }
  PtrSysInfo->FreqSdhc = PtrSysInfo->FreqSystemBus/PcdGet32 (PcdPlatformFreqDiv);
  PtrSysInfo->FreqQman = PtrSysInfo->FreqSystemBus/PcdGet32 (PcdPlatformFreqDiv);
}

/**
  Function to initialize SoC specific constructs
  CPU Info
  SoC Personality
  Board Personality
  RCW prints
 **/
VOID
SocInit (
  VOID
  )
{
  SmmuInit ();

  //
  // Early init serial Port to get board information.
  //
  SerialPortInitialize ();
  DEBUG ((DEBUG_INIT, "\nUEFI firmware (version %s built at %a on %a)\n",
          (CHAR16*)PcdGetPtr (PcdFirmwareVersionString), __TIME__, __DATE__));

  PrintCpuInfo ();

  //
  // Print Reset control Word
  //
  PrintRCW ();
  PrintSoc ();

  return;
}
