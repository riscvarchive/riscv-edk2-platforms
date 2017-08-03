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
  EmHilink0Hccs1X8Speed5G,
} HILINK0_MODE_TYPE;

typedef enum {
  EmHilink1Sas2X1 = 0,
  EmHilink1Hccs0X8 = 1,
  EmHilink1Pcie0X8 = 2,
  EmHilink1Hccs0X8Width16,
  EmHilink1Hccs0X8Width32,
  EmHilink1Hccs0X8Speed5G,
} HILINK1_MODE_TYPE;

typedef enum {
  EmHilink2Pcie2X8 = 0,
  EmHilink2Hccs2X8 = 1,
  EmHilink2Sas0X8 = 2,
  EmHilink2Hccs2X8Width16,
  EmHilink2Hccs2X8Width32,
  EmHilink2Hccs2X8Speed5G,
} HILINK2_MODE_TYPE;

typedef enum {
  EmHilink5Pcie3X4 = 0,
  EmHilink5Pcie2X2Pcie3X2 = 1,
  EmHilink5Sas1X4 = 2,
} HILINK5_MODE_TYPE;


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
VOID SerdesEnableCtleDfe(UINT32 NimbusId, UINT32 Macro, UINT32 Lane, UINT32 LaneMode);

EFI_STATUS
EfiSerdesInitWrap (VOID);
INT32 SerdesReset(UINT32 SiclId, UINT32 Macro);
VOID SerdesLoadFirmware(UINT32 SiclId, UINT32 Macro);

#endif
