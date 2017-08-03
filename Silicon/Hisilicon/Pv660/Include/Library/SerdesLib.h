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


#ifndef _SERDES_LIB_H_
#define _SERDES_LIB_H_


typedef enum {
  EmHilink0Pcie1X8 = 0,
  EmHilink0Pcie1X4Pcie2X4 = 1,
} HILINK0_MODE_TYPE;

typedef enum {
  EmHilink1Pcie0X8 = 0,
  EmHilink1HccsX8 = 1,
} HILINK1_MODE_TYPE;

typedef enum {
  EmHilink2Pcie2X8 = 0,
  EmHilink2Sas0X8 = 1,
} HILINK2_MODE_TYPE;

typedef enum {
  EmHilink3GeX4 = 0,
  EmHilink3GeX2XgeX2 = 1, //lane0,lane1-ge,lane2,lane3 xge
} HILINK3_MODE_TYPE;


typedef enum {
  EmHilink4GeX4 = 0,
  EmHilink4XgeX4 = 1,
} HILINK4_MODE_TYPE;

typedef enum {
  EmHilink5Sas1X4 = 0,
  EmHilink5Pcie3X4 = 1,
} HILINK5_MODE_TYPE;


typedef struct {
  HILINK0_MODE_TYPE Hilink0Mode;
  HILINK1_MODE_TYPE Hilink1Mode;
  HILINK2_MODE_TYPE Hilink2Mode;
  HILINK3_MODE_TYPE Hilink3Mode;
  HILINK4_MODE_TYPE Hilink4Mode;
  HILINK5_MODE_TYPE Hilink5Mode;
} SERDES_PARAM;


#define SERDES_INVALID_MACRO_ID  0xFFFFFFFF
#define SERDES_INVALID_LANE_NUM  0xFFFFFFFF

typedef struct {
    UINT32 MacroId;
    UINT32 DsNum;
} SERDES_POLARITY_INVERT;


EFI_STATUS OemGetSerdesParam (SERDES_PARAM *ParamA, SERDES_PARAM *ParamB, UINT32 SocketId);
extern SERDES_POLARITY_INVERT gSerdesPolarityTxDesc[];
extern SERDES_POLARITY_INVERT gSerdesPolarityRxDesc[];
UINT32 GetEthType(UINT8 EthChannel);

EFI_STATUS
EfiSerdesInitWrap (VOID);

void serdes_state_show(UINT32 macro1);
//uniBIOS__l00228991_start DTS2015042210118 2015-4-22 20:06:34

void SRE_SerdesEnableCTLEDFE(UINT32 macro, UINT32 lane, UINT32 ulDsCfg);
//uniBIOS__l00228991_end DTS2015042210118 2015-4-22 20:06:34

//uniBIOS_l00306713_000_start 2015-3-19 17:37:06

//EYE test
UINT32 serdes_eye_test(UINT32 uwMacroId, UINT32 uwDsNum, UINT32 eyemode, UINT32 scanwindowvalue, UINT32 uwRateData);

UINT32 Serdes_ReadBert(UINT32   ulMacroId , UINT32   ulDsNum);

//PRBS test
int serdes_prbs_test(UINT8   ulMacroId , UINT8   ulDsNum,UINT8 PrbsType);

int serdes_prbs_test_cancle(UINT8  ulMacroId,UINT8 ulDsNum);

//CTLE/DFE
void serdes_ctle_adaptation_close(UINT32 macro,UINT32 lane);

void serdes_ctle_adaptation_open(UINT32 macro,UINT32 lane);

void serdes_dfe_adaptation_close(UINT32 macro,UINT32 lane);

void serdes_dfe_adaptation_open(UINT32 macro,UINT32 lane);

void serdes_ctle_dfe_reset(UINT32 macro,UINT32 lane);
//uniBIOS_l00306713_000_end   2015-3-19 17:37:06


//uniBIOS_l00306713_000_start 2015-7-15 9:13:55

int serdes_tx_to_rx_serial_loopback(UINT8 macro,UINT8 lane,UINT8 val);

int serdes_tx_to_rx_parallel_loopback(UINT8 macro,UINT8 lane,UINT8 val);

int serdes_rx_to_tx_parallel_loopback(UINT8 macro,UINT8 lane,UINT8 val);
//uniBIOS_l00306713_000_end   2015-7-15 9:13:55

#endif
