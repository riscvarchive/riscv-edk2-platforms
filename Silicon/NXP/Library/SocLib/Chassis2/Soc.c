/** @Soc.c
  SoC specific Library containg functions to initialize various SoC components

  Copyright 2017-2020 NXP

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
  UINTN        SysClk;

  GurBase = (VOID *)PcdGet64 (PcdGutsBaseAddr);
  SysClk = CLK_FREQ;

  SetMem (PtrSysInfo, sizeof (SYS_INFO), 0);

  PtrSysInfo->FreqSystemBus = SysClk;

  //
  // selects the platform clock:SYSCLK ratio and calculate
  // system frequency
  //
  PtrSysInfo->FreqSystemBus *= (GurRead ((UINTN)&GurBase->RcwSr[0]) >>
                CHASSIS2_RCWSR0_SYS_PLL_RAT_SHIFT) &
                CHASSIS2_RCWSR0_SYS_PLL_RAT_MASK;
}

/**
  Function to initialize SoC specific constructs
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

  return;
}
