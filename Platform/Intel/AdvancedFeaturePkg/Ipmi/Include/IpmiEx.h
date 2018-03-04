/** @file
  IPMI 2.0 definitions from the IPMI Specification Version 2.0, Revision 1.1.

  See IPMI specification, Appendix G, Command Assignments
  and Appendix H, Sub-function Assignments.

  Copyright (c) 1999 - 2015, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
**/

#ifndef _IPMI_EX_H_
#define _IPMI_EX_H_

#pragma pack(1)

//
// Generic completion Codes
//
#define IPMI_COMP_CODE_NORMAL                  0x00
#define IPMI_COMP_CODE_NODE_BUSY               0xC0
#define IPMI_COMP_CODE_TIMEOUT                 0xC3
#define IPMI_COMP_CODE_OUT_OF_SPACE            0xC4
#define IPMI_COMP_CODE_OUT_OF_RANGE            0xC9
#define IPMI_COMP_CODE_CMD_RESP_NOT_PROVIDED   0xCE
#define IPMI_COMP_CODE_FAIL_DUP_REQUEST        0xCF
#define IPMI_COMP_CODE_SDR_REP_IN_UPDATE_MODE  0xD0
#define IPMI_COMP_CODE_DEV_IN_FW_UPDATE_MODE   0xD1
#define IPMI_COMP_CODE_BMC_INIT_IN_PROGRESS    0xD2
#define IPMI_COMP_CODE_UNSPECIFIED             0xFF

//
// Watchdog timer 
//
#define IPMI_WATCHDOG_TIMER_BIOS_FRB2  0x1
#define IPMI_WATCHDOG_TIMER_BIOS_POST  0x2
#define IPMI_WATCHDOG_TIMER_OS_LOADER  0x3
#define IPMI_WATCHDOG_TIMER_SMS        0x4

#define IPMI_WATCHDOG_TIMER_ACTION_NO_ACTION   0x0
#define IPMI_WATCHDOG_TIMER_ACTION_HARD_RESET  0x1
#define IPMI_WATCHDOG_TIMER_ACTION_POWER_DONW  0x2
#define IPMI_WATCHDOG_TIMER_ACTION_POWER_CYCLE 0x3

//
// Device and Messaging
//

typedef struct {
  UINT8  EnableReceiveMessageQueueInterrupt : 1;
  UINT8  EnableEventMessageBufferFullInterrupt : 1;
  UINT8  EnableEventMessageBuffer : 1;
  UINT8  EnableSystemEventLogging : 1;
  UINT8  Reserved : 1;
  UINT8  Oem0Enable : 1;
  UINT8  Oem1Enable : 1;
  UINT8  Oem2Enable : 1;
} IPMI_SET_BMC_GLOBAL_ENABLES_REQUEST;

typedef struct {
  UINT8  CompletionCode;
  UINT8  ReceiveMessageQueueInterrupt : 1;
  UINT8  EventMessageBufferFullInterrupt : 1;
  UINT8  EventMessageBuffer : 1;
  UINT8  SystemEventLogging : 1;
  UINT8  Reserved : 1;
  UINT8  Oem0Enable : 1;
  UINT8  Oem1Enable : 1;
  UINT8  Oem2Enable : 1;
} IPMI_GET_BMC_GLOBAL_ENABLES_RESPONSE;

typedef struct {
  UINT8  ClearReceiveMessageQueue : 1;
  UINT8  ClearEventMessageBuffer : 1;
  UINT8  Reserved0 : 1;
  UINT8  ClearWatchdogPerTimeoutInterruptFlag : 1;
  UINT8  Reserved : 1;
  UINT8  ClearOem0Enable : 1;
  UINT8  ClearOem1Enable : 1;
  UINT8  ClearOem2Enable : 1;
} IPMI_CLEAR_MESSAGE_FLAGS_REQUEST;

typedef struct {
  UINT8  CompletionCode;
  UINT8  ReceiveMessageAvailable : 1;
  UINT8  EventMessageBufferFull : 1;
  UINT8  Reserved0 : 1;
  UINT8  WatchdogPerTimeoutInterruptOccurred : 1;
  UINT8  Reserved : 1;
  UINT8  Oem0DataAvailable : 1;
  UINT8  Oem1DataAvailable : 1;
  UINT8  Oem2DataAvailable : 1;
} IPMI_GET_MESSAGE_FLAGS_RESPONSE;

typedef struct {
  UINT8  CompletionCode;
  UINT8  ChannelNumber : 4;
  UINT8  InferredPrivilegeLevel : 4;
  UINT8  MessageData[];
} IPMI_GET_MESSAGE_RESPONSE;

typedef struct {
  UINT8  CompletionCode;
  UINT8  ChannelNumber : 4;
  UINT8  Authentication : 1;
  UINT8  Encryption : 1;
  UINT8  Tracking : 2;
  UINT8  MessageData[];
} IPMI_SEND_MESSAGE_REQUEST;

typedef struct {
  UINT8  CompletionCode;
  UINT8  ResponseData[];
} IPMI_SEND_MESSAGE_RESPONSE;

//
// SOL
//

typedef struct {
  UINT8  SessionState : 4;
  UINT8  Reserved : 4;
  UINT8  PayloadInstance;
  UINT8  FormatVersionMajor; // 1
  UINT8  FormatVersionMinor; // 0
} IPMI_SOL_ACTIVATING_REQUEST;

typedef struct {
  UINT8  ChannelNumber : 4;
  UINT8  Reserved : 4;
  UINT8  ParameterSelector;
  UINT8  ParameterData[1];
} IPMI_SET_SOL_CONFIGURATION_PARAMETERS_REQUEST;

typedef struct {
  UINT8  ChannelNumber : 4;
  UINT8  Reserved : 3;
  UINT8  GetParameter : 1;
  UINT8  ParameterSelector;
  UINT8  SetSelector;
  UINT8  BlockSelector;
} IPMI_GET_SOL_CONFIGURATION_PARAMETERS_REQUEST;

typedef struct {
  UINT8  CompletionCode;
  UINT8  ParameterRevision;
  UINT8  ParameterData[1];
} IPMI_GET_SOL_CONFIGURATION_PARAMETERS_RESPONSE;

#define IPMI_SOL_CONFIGURATION_PARAMETER_SET_IN_PROGRESS       0
#define IPMI_SOL_CONFIGURATION_PARAMETER_SOL_ENABLE            1
#define IPMI_SOL_CONFIGURATION_PARAMETER_SOL_AUTHENTICATION    2
#define IPMI_SOL_CONFIGURATION_PARAMETER_SOL_CHARACTER_PARAM   3
#define IPMI_SOL_CONFIGURATION_PARAMETER_SOL_RETRY             4
#define IPMI_SOL_CONFIGURATION_PARAMETER_SOL_NV_BIT_RATE       5
#define IPMI_SOL_CONFIGURATION_PARAMETER_SOL_VOLATILE_BIT_RATE 6
#define IPMI_SOL_CONFIGURATION_PARAMETER_SOL_PAYLOAD_CHANNEL   7
#define IPMI_SOL_CONFIGURATION_PARAMETER_SOL_PAYLOAD_PORT      8

//
// Chasis
//

typedef struct {
  UINT8   CompletionCode;
  UINT8   CapabilitiesFlags;
  UINT8   ChassisFruInfoDeviceAddress;
  UINT8   ChassisSDRDeviceAddress;
  UINT8   ChassisSELDeviceAddress;
  UINT8   ChassisSystemManagementDeviceAddress;
  UINT8   ChassisBridgeDeviceAddress;
} IPMI_GET_CHASSIS_CAPABILITIES_RESPONSE;

typedef struct {
  UINT8   CompletionCode;
  UINT8   CurrentPowerState;
  UINT8   LastPowerEvent;
  UINT8   MiscChassisState;
  UINT8   FrontPanelButtonCapabilities;
} IPMI_GET_CHASSIS_STATUS_RESPONSE;

typedef struct {
  UINT8   ChassisControl:4;
  UINT8   Reserved:4;
} IPMI_CHASSIS_CONTROL_REQUEST;

typedef struct {
  UINT8   PowerRestorePolicy:3;
  UINT8   Reserved:5;
} IPMI_SET_POWER_RESTORE_POLICY_REQUEST;

typedef struct {
  UINT8   CompletionCode;
  UINT8   PowerRestorePolicySupport;
} IPMI_SET_POWER_RESTORE_POLICY_RESPONSE;

//
// FRU
//

typedef struct {
  UINT8  DeviceId;
} IPMI_GET_FRU_INVENTORY_AREA_INFO_REQUEST;

typedef struct {
  UINT8   CompletionCode;
  UINT16  InventoryAreaSize;
  UINT8   AccessType;
} IPMI_GET_FRU_INVENTORY_AREA_INFO_RESPONSE;

typedef struct {
  UINT8   DeviceId;
  UINT16  InventoryOffset;
  UINT8   CountToRead;
} IPMI_READ_FRU_DATA_REQUEST;

typedef struct {
  UINT8   CompletionCode;
  UINT8   CountReturned;
  UINT8   Data[];
} IPMI_READ_FRU_DATA_RESPONSE;

typedef struct {
  UINT8   DeviceId;
  UINT16  InventoryOffset;
  UINT8   Data[];
} IPMI_WRITE_FRU_DATA_REQUEST;

typedef struct {
  UINT8   CompletionCode;
  UINT8   CountWritten;
} IPMI_WRITE_FRU_DATA_RESPONSE;

//
// SEL
//

typedef struct {
  UINT8   CompletionCode;
  UINT16  NextSelRecord;
  UINT8   RecordData[];
} IPMI_GET_SEL_ENTRY_RESPONSE;

typedef struct {
  UINT8   RecordData[16];
} IPMI_ADD_SEL_ENTRY_REQUEST;

typedef struct {
  UINT8   CompletionCode;
  UINT16  RecordId;
} IPMI_ADD_SEL_ENTRY_RESPONSE;

typedef struct {
  UINT16  ReservationId;
  UINT16  RecordId;
  UINT8   OffsetIntoRecord;
  UINT8   InProgress;
  UINT8   RecordData[];
} IPMI_PARTIAL_ADD_SEL_ENTRY_REQUEST;

typedef struct {
  UINT8   CompletionCode;
  UINT16  RecordId;
} IPMI_PARTIAL_ADD_SEL_ENTRY_RESPONSE;

typedef struct {
  UINT8   CompletionCode;
  UINT8   ErasureProgress;
} IPMI_CLEAR_SEL_RESPONSE;

typedef struct {
  UINT8   CompletionCode;
  UINT32  Timestamp;
} IPMI_GET_SEL_TIME_RESPONSE;

typedef struct {
  UINT32  Timestamp;
} IPMI_SET_SEL_TIME_REQUEST;

//
// SDR
//
typedef struct {
  UINT8   CompletionCode;
  UINT16  RecordId;
  UINT8   RecordData[];
} IPMI_GET_SDR_RESPONSE;

#pragma pack()

#endif