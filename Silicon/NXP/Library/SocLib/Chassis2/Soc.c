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
#include <Library/SocLib.h>
#include <Soc.h>

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

  GurBase = (CCSR_GUR *)CHASSIS2_DCFG_ADDRESS;
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
  Return the input clock frequency to an IP Module.
  This function reads the RCW bits and calculates the  PLL multiplier/divider
  values to be applied to various IP modules.
  If a module is disabled or doesn't exist on platform, then return zero.

  @param[in]  BaseClock  Base clock to which PLL multiplier/divider values is
                         to be applied.
  @param[in]  ClockType  Variable of Type NXP_IP_CLOCK. Indicates which IP clock
                         is to be retrieved.
  @param[in]  Args       Variable argument list which is parsed based on
                         ClockType. e.g. if the ClockType is NXP_I2C_CLOCK, then
                         the second argument will be interpreted as controller
                         number. e.g. if there are four i2c controllers in SOC,
                         then this value can be 0, 1, 2, 3
                         e.g. if ClockType is NXP_CORE_CLOCK, then second
                         argument is interpreted as cluster number and third
                         argument is interpreted as core number (within the
                         cluster)

  @return                Actual Clock Frequency. Return value 0 should be
                         interpreted as clock not being provided to IP.
**/
UINT64
SocGetClock (
  IN  UINT64        BaseClock,
  IN  NXP_IP_CLOCK  ClockType,
  IN  VA_LIST       Args
  )
{
  LS1043A_DEVICE_CONFIG  *Dcfg;
  UINT32                 RcwSr;
  UINT64                 ReturnValue;

  ReturnValue = 0;
  Dcfg = (LS1043A_DEVICE_CONFIG  *)LS1043A_DCFG_ADDRESS;

  switch (ClockType) {
  case NXP_UART_CLOCK:
  case NXP_I2C_CLOCK:
    RcwSr = GurRead ((UINTN)&Dcfg->RcwSr[0]);
    ReturnValue = BaseClock * SYS_PLL_RAT (RcwSr);
    break;
  default:
    break;
  }

  return ReturnValue;
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
