/** @file
*
*  Copyright (c) 2016, Hisilicon Limited. All rights reserved.
*  Copyright (c) 2016, Linaro Limited. All rights reserved.
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
  EmHilink0Hccs1X8 = 0,
  EmHilink0Pcie1X8 = 2,
  EmHilink0Pcie1X4Pcie2X4 = 3,
  EmHilink0Sas2X8 = 4,
  EmHilink0Hccs1X8Width16,
  EmHilink0Hccs1X8Width32,
} HILINK0_MODE_TYPE;

typedef enum {
  EmHilink1Sas2X1 = 0,
  EmHilink1Hccs0X8 = 1,
  EmHilink1Pcie0X8 = 2,
  EmHilink1Hccs0X8Width16,
  EmHilink1Hccs0X8Width32,
} HILINK1_MODE_TYPE;

typedef enum {
  EmHilink2Pcie2X8 = 0,
  EmHilink2Sas0X8 = 2,
} HILINK2_MODE_TYPE;

typedef enum {
  EmHilink5Pcie3X4 = 0,
  EmHilink5Pcie2X2Pcie3X2 = 1,
  EmHilink5Sas1X4 = 2,
} HILINK5_MODE_TYPE;

typedef enum {
  Em32coreEvbBoard = 0,
  Em16coreEvbBoard = 1,
  EmV2R1CO5Borad = 2,
  EmOtherBorad
} BOARD_TYPE;


typedef struct {
  HILINK0_MODE_TYPE Hilink0Mode;
  HILINK1_MODE_TYPE Hilink1Mode;
  HILINK2_MODE_TYPE Hilink2Mode;
  UINT32 Hilink3Mode;
  UINT32 Hilink4Mode;
  HILINK5_MODE_TYPE Hilink5Mode;
  UINT32 Hilink6Mode;
  UINT32 UseSsc;
} SERDES_PARAM;


#define SERDES_INVALID_MACRO_ID  0xFFFFFFFF
#define SERDES_INVALID_LANE_NUM  0xFFFFFFFF
#define SERDES_INVALID_RATE_MODE  0xFFFFFFFF

typedef struct {
  UINT32 MacroId;
  UINT32 DsNum;
  UINT32 DsCfg;
} SERDES_POLARITY_INVERT;

EFI_STATUS OemGetSerdesParam (SERDES_PARAM *ParamA, SERDES_PARAM *ParamB, UINT32 SocketId);
extern SERDES_POLARITY_INVERT gSerdesPolarityTxDesc[];
extern SERDES_POLARITY_INVERT gSerdesPolarityRxDesc[];
UINT32 GetEthType(UINT8 EthChannel);

EFI_STATUS
EfiSerdesInitWrap (VOID);

void SRE_SerdesEnableCTLEDFE(UINT32 macro, UINT32 lane, UINT32 ulDsCfg);

//EYE test
UINT32 serdes_eye_test(UINT32 uwMacroId, UINT32 uwDsNum, UINT32 eyemode, UINT32 scanwindowvalue, UINT32 uwRateData);

UINT32 Serdes_ReadBert(UINT32   ulMacroId , UINT32   ulDsNum);

//PRBS test
int serdes_prbs_test(UINT8 macro, UINT8 lane, UINT8 prbstype);

int serdes_prbs_test_cancle(UINT8 macro,UINT8 lane);

//CTLE/DFE
void serdes_ctle_adaptation_close(UINT32 macro,UINT32 lane);

void serdes_ctle_adaptation_open(UINT32 macro,UINT32 lane);

void serdes_dfe_adaptation_close(UINT32 macro,UINT32 lane);

void serdes_dfe_adaptation_open(UINT32 macro,UINT32 lane);

void serdes_ctle_dfe_reset(UINT32 macro,UINT32 lane);
//int serdes_reset(UINT32 macro);
//int serdes_release_reset(UINT32 macro);
void Custom_Wave(UINT32 macro,UINT32 lane,UINT32 mode);
void serdes_ffe_show(UINT32 macro,UINT32 lane);
void serdes_dfe_show(UINT32 macro,UINT32 lane);
int  serdes_read_bert(UINT8 macro, UINT8 lane);
void  serdes_clean_bert(UINT8 macro, UINT8 lane);
int  serdes_get_four_point_eye_diagram(UINT32 macro, UINT32 lane,UINT32 eyemode, UINT32 data_rate);
void serdes_release_mcu(UINT32 macro,UINT32 val);
int hilink_write(UINT32 macro, UINT32 reg, UINT32 value);
int hilink_read(UINT32 macro, UINT32 reg, UINT32 *value);
int serdes_tx_to_rx_parallel_loopback(UINT8 macro,UINT8 lane,UINT8 val);//TXRXPARLPBKEN
int serdes_rx_to_tx_parallel_loopback(UINT8 macro,UINT8 lane,UINT8 val);
int serdes_tx_to_rx_serial_loopback(UINT8 macro,UINT8 lane,UINT8 val);
void serdes_ctle_show(UINT32 macro,UINT32 lane);
int serdes_cs_write(UINT32 macro,UINT32 cs_num,UINT32 reg_num,UINT32 bit_high,UINT32 bit_low,UINT32 value);
UINT32 serdes_cs_read(UINT32 macro,UINT32 cs_num,UINT32 reg_num);
int serdes_ds_write(UINT32 macro,UINT32 ds_num,UINT32 ds_index,UINT32 reg_num,UINT32 bit_high,UINT32 bit_low,UINT32 value);
int serdes_ds_read(UINT32 macro,UINT32 ds_num,UINT32 ds_index,UINT32 reg_num);
int report_serdes_mux(void);
int serdes_key_reg_show(UINT32 macro);
void serdes_state_show(UINT32 macro);
UINT32 Serdes_ReadBert(UINT32   ulMacroId , UINT32   ulDsNum);

#endif
