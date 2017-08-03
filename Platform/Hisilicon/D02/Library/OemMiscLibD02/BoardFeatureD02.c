/** @file
*
*  Copyright (c) 2015, Hisilicon Limited. All rights reserved.
*  Copyright (c) 2015, Linaro Limited. All rights reserved.
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

#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <PlatformArch.h>
#include <Library/OemMiscLib.h>
#include <Library/SerdesLib.h>
#include <Library/CpldIoLib.h>
#include <Library/CpldD02.h>
#include <Library/TimerLib.h>
#include <Library/I2CLib.h>
#include <Library/HiiLib.h>

I2C_DEVICE gDS3231RtcDevice = {
    .Socket = 0,
    .Port = 7,
    .DeviceType = DEVICE_TYPE_SPD,
    .SlaveDeviceAddress = 0x68
};

// Set Tx output polarity. Not inverting data is default. For Phosphor660 D02 Board
//if((1 == ulMacroId) && ((7 == ulDsNum)||(0 == ulDsNum)))
SERDES_POLARITY_INVERT gSerdesPolarityTxDesc[] =
{
    {1, 7},
    {1, 0},
    {SERDES_INVALID_MACRO_ID, SERDES_INVALID_LANE_NUM}
};

// Set Rx data polarity. Not inverting data is default. For Phosphor660 D02 Board
//if((1 == ulMacroId) && ((0 == ulDsNum) || (1 == ulDsNum)))
SERDES_POLARITY_INVERT gSerdesPolarityRxDesc[] =
{
    {1, 0},
    {1, 1},
    {SERDES_INVALID_MACRO_ID, SERDES_INVALID_LANE_NUM}
};

SERDES_PARAM gSerdesParam = {
  .Hilink0Mode = EmHilink0Pcie1X8,
  .Hilink1Mode = EmHilink1Pcie0X8,
  .Hilink2Mode = EmHilink2Pcie2X8,
  .Hilink3Mode = EmHilink3GeX4,
  .Hilink4Mode = EmHilink4XgeX4,
  .Hilink5Mode = EmHilink5Sas1X4,
};

EFI_STATUS OemGetSerdesParam (SERDES_PARAM *ParamA, SERDES_PARAM *ParamB, UINT32 SocketId)
{
  if (ParamA == NULL) {
    DEBUG((DEBUG_ERROR, "[%a]:[%dL] ParamA == NULL!\n", __FUNCTION__, __LINE__));
    return EFI_INVALID_PARAMETER;
  }

  (VOID) CopyMem(ParamA, &gSerdesParam, sizeof(*ParamA));
  return EFI_SUCCESS;
}


VOID OemPcieResetAndOffReset(void)
  {
    WriteCpldReg(CPU0_PCIE1_RESET_REG,0x0);
    WriteCpldReg(CPU0_PCIE2_RESET_REG,0x0);
    WriteCpldReg(CPU1_PCIE1_RESET_REG,0x0);
    WriteCpldReg(CPU1_PCIE2_RESET_REG,0x0);
    MicroSecondDelay(100000);
    WriteCpldReg(CPU0_PCIE1_RESET_REG,0x55);
    WriteCpldReg(CPU0_PCIE2_RESET_REG,0x55);
    WriteCpldReg(CPU1_PCIE1_RESET_REG,0x55);
    WriteCpldReg(CPU1_PCIE2_RESET_REG,0x55);
    return;
  }

EFI_STRING_ID gDimmToDevLocator[MAX_SOCKET][MAX_CHANNEL][MAX_DIMM] = {
  {{STRING_TOKEN(STR_D02_DIMM_000), STRING_TOKEN(STR_D02_DIMM_001), 0xFFFF},
   {STRING_TOKEN(STR_D02_DIMM_010), STRING_TOKEN(STR_D02_DIMM_011), 0xFFFF}}
};

EFI_HII_HANDLE
EFIAPI
OemGetPackages (
  )
{
    return HiiAddPackages (
                            &gEfiCallerIdGuid,
                            NULL,
                            OemMiscLibD02Strings,
                            NULL,
                            NULL
                            );
}
