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



#include <PiDxe.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/ArmLib.h>
#include <Library/BaseLib.h>
#include <Library/I2CLib.h>
#include <Library/TimerLib.h>

#include <Library/PlatformSysCtrlLib.h>

#include "I2CLibInternal.h"
#include "I2CHw.h"

VOID I2C_Delay(UINT32 ulCount)
{
    MicroSecondDelay(ulCount);
    return;
}


EFI_STATUS
EFIAPI
I2C_Disable(UINT32 Socket,UINT8 Port)
{
    UINT32                  ulTimeCnt = I2C_READ_TIMEOUT;
    I2C0_STATUS_U           I2cStatusReg;
    I2C0_ENABLE_U           I2cEnableReg;
    I2C0_ENABLE_STATUS_U    I2cEnableStatusReg;

    UINTN Base = GetI2cBase(Socket, Port);

    I2C_REG_READ((Base + I2C_STATUS_OFFSET), I2cStatusReg.Val32);

    while (I2cStatusReg.bits.activity)
    {
        I2C_Delay(10000);

        ulTimeCnt--;
        I2C_REG_READ(Base + I2C_STATUS_OFFSET, I2cStatusReg.Val32);
        if (0 == ulTimeCnt)
        {
            return EFI_DEVICE_ERROR;
        }
    }


    I2C_REG_READ(Base + I2C_ENABLE_OFFSET, I2cEnableReg.Val32);
    I2cEnableReg.bits.enable = 0;
    I2C_REG_WRITE(Base + I2C_ENABLE_OFFSET,I2cEnableReg.Val32);

    I2C_REG_READ(Base + I2C_ENABLE_OFFSET,I2cEnableStatusReg.Val32);
    if (0 == I2cEnableStatusReg.bits.ic_en)
    {
        return EFI_SUCCESS;
    }
    else
    {
        return EFI_DEVICE_ERROR;
    }
}


EFI_STATUS
EFIAPI
I2C_Enable(UINT32 Socket,UINT8 Port)
{
    I2C0_ENABLE_U           I2cEnableReg;
    I2C0_ENABLE_STATUS_U    I2cEnableStatusReg;

    UINTN Base = GetI2cBase(Socket, Port);


    I2C_REG_READ(Base + I2C_ENABLE_OFFSET, I2cEnableReg.Val32);
    I2cEnableReg.bits.enable = 1;
    I2C_REG_WRITE(Base + I2C_ENABLE_OFFSET, I2cEnableReg.Val32);


    I2C_REG_READ(Base + I2C_ENABLE_STATUS_OFFSET, I2cEnableStatusReg.Val32);
    if (1 == I2cEnableStatusReg.bits.ic_en)
    {
        return EFI_SUCCESS;
    }
    else
    {
        return EFI_DEVICE_ERROR;
    }
}

void I2C_SetTarget(UINT32 Socket,UINT8 Port,UINT32 I2cDeviceAddr)
{
    I2C0_TAR_U    I2cTargetReg;
    UINTN Base = GetI2cBase(Socket, Port);


    I2C_REG_READ(Base + I2C_TAR_OFFSET, I2cTargetReg.Val32);
    I2cTargetReg.bits.ic_tar = I2cDeviceAddr;
    I2C_REG_WRITE(Base + I2C_TAR_OFFSET, I2cTargetReg.Val32);

    return;
}


EFI_STATUS
EFIAPI
I2CInit(UINT32 Socket, UINT32 Port, SPEED_MODE SpeedMode)
{
    I2C0_CON_U              I2cControlReg;
    I2C0_SS_SCL_HCNT_U      I2cStandardSpeedSclHighCount;
    I2C0_SS_SCL_LCNT_U      I2cStandardSpeedSclLowCount;
    I2C0_RX_TL_U            I2cRxFifoReg;
    I2C0_TX_TL_U            I2cTxFifoReg;
    I2C0_INTR_MASK_U        I2cIntrMask;
    EFI_STATUS              Status;

    UINTN Base = GetI2cBase(Socket, Port);

    if((Socket >= MAX_SOCKET) || (Port >= I2C_PORT_MAX) || (SpeedMode >= SPEED_MODE_MAX)){
        return EFI_INVALID_PARAMETER;
    }


    Status = I2C_Disable(Socket,Port);
    if(EFI_ERROR(Status))
    {
        return EFI_DEVICE_ERROR;
    }


    I2C_REG_READ(Base + I2C_CON_OFFSET, I2cControlReg.Val32);
    I2cControlReg.bits.master = 1;
    I2cControlReg.bits.spedd = 0x1;
    I2cControlReg.bits.restart_en = 1;
    I2cControlReg.bits.slave_disable = 1;
    I2C_REG_WRITE(Base + I2C_CON_OFFSET,I2cControlReg.Val32);


    if(Normal == SpeedMode)
    {
        I2C_REG_READ(Base + I2C_SS_SCL_HCNT_OFFSET,I2cStandardSpeedSclHighCount.Val32);
        I2cStandardSpeedSclHighCount.bits.ic_ss_scl_hcnt = I2C_SS_SCLHCNT;
        I2C_REG_WRITE(Base + I2C_SS_SCL_HCNT_OFFSET, I2cStandardSpeedSclHighCount.Val32);
        I2C_REG_READ(Base + I2C_SS_SCL_LCNT_OFFSET, I2cStandardSpeedSclLowCount.Val32);
        I2cStandardSpeedSclLowCount.bits.ic_ss_scl_lcnt = I2C_SS_SCLLCNT;
        I2C_REG_WRITE(Base + I2C_SS_SCL_LCNT_OFFSET, I2cStandardSpeedSclLowCount.Val32);
    }
    else
    {
        I2C_REG_READ(Base + I2C_FS_SCL_HCNT_OFFSET,I2cStandardSpeedSclHighCount.Val32);
        I2cStandardSpeedSclHighCount.bits.ic_ss_scl_hcnt = I2C_SS_SCLHCNT;
        I2C_REG_WRITE(Base + I2C_FS_SCL_HCNT_OFFSET, I2cStandardSpeedSclHighCount.Val32);
        I2C_REG_READ(Base + I2C_FS_SCL_LCNT_OFFSET, I2cStandardSpeedSclLowCount.Val32);
        I2cStandardSpeedSclLowCount.bits.ic_ss_scl_lcnt = I2C_SS_SCLLCNT;
        I2C_REG_WRITE(Base + I2C_FS_SCL_LCNT_OFFSET, I2cStandardSpeedSclLowCount.Val32);
    }


    I2C_REG_READ(Base + I2C_RX_TL_OFFSET, I2cRxFifoReg.Val32);
    I2cRxFifoReg.bits.rx_tl = I2C_TXRX_THRESHOLD;
    I2C_REG_WRITE(Base + I2C_RX_TL_OFFSET, I2cRxFifoReg.Val32);
    I2C_REG_READ(Base + I2C_TX_TL_OFFSET,I2cTxFifoReg.Val32);
    I2cTxFifoReg.bits.tx_tl = I2C_TXRX_THRESHOLD;
    I2C_REG_WRITE(Base + I2C_TX_TL_OFFSET, I2cTxFifoReg.Val32);


    I2C_REG_READ(Base + I2C_INTR_MASK_OFFSET, I2cIntrMask.Val32);
    I2cIntrMask.Val32 = 0x0;
    I2C_REG_WRITE(Base + I2C_INTR_MASK_OFFSET, I2cIntrMask.Val32);


    Status = I2C_Enable(Socket,Port);
    if(EFI_ERROR(Status))
    {
        return EFI_DEVICE_ERROR;
    }

    return I2cLibRuntimeSetup (Socket, Port);
}

EFI_STATUS
EFIAPI
I2CSdaConfig(UINT32 Socket, UINT32 Port)
{

    UINTN Base = GetI2cBase(Socket, Port);

    if((Socket >= MAX_SOCKET) || (Port >= I2C_PORT_MAX)){
        return EFI_INVALID_PARAMETER;
    }

    I2C_REG_WRITE(Base + I2C_SDA_HOLD, 0x14);

    return EFI_SUCCESS;
}



UINT32 I2C_GetTxStatus(UINT32 Socket,UINT8 Port)
{
    I2C0_TXFLR_U ulFifo;
    UINTN Base = GetI2cBase(Socket, Port);

    I2C_REG_READ(Base + I2C_TXFLR_OFFSET, ulFifo.Val32);
    return ulFifo.bits.txflr;
}

UINT32
I2C_GetRxStatus(UINT32 Socket,UINT8 Port)
{
    I2C0_RXFLR_U ulFifo;
    UINTN Base = GetI2cBase(Socket, Port);

    I2C_REG_READ(Base + I2C_RXFLR_OFFSET, ulFifo.Val32);
    return ulFifo.bits.rxflr;
}

EFI_STATUS
EFIAPI
WriteBeforeRead(I2C_DEVICE *I2cInfo, UINT32 ulLength, UINT8 *pBuf)
{
    UINT32 ulFifo;
    UINT32 ulCnt;
    UINT32 ulTimes = 0;

    UINTN  Base = GetI2cBase(I2cInfo->Socket, I2cInfo->Port);


    I2C_SetTarget(I2cInfo->Socket,I2cInfo->Port,I2cInfo->SlaveDeviceAddress);

    ulFifo = I2C_GetTxStatus(I2cInfo->Socket,I2cInfo->Port);
    while(0 != ulFifo)
    {
        I2C_Delay(2);
        if(++ulTimes > I2C_READ_TIMEOUT)
        {
            return EFI_TIMEOUT;
        }
        ulFifo = I2C_GetTxStatus(I2cInfo->Socket,I2cInfo->Port);
    }

    for(ulCnt = 0; ulCnt < ulLength; ulCnt++)
    {
        ulTimes = 0;
        while(ulFifo > I2C_TXRX_THRESHOLD)
        {
            I2C_Delay(2);
            if(++ulTimes > I2C_READ_TIMEOUT)
            {
                return EFI_TIMEOUT;
            }
            ulFifo = I2C_GetTxStatus(I2cInfo->Socket,I2cInfo->Port);
        }

        I2C_REG_WRITE(Base + I2C_DATA_CMD_OFFSET, *pBuf++);
        ulFifo = I2C_GetTxStatus(I2cInfo->Socket,I2cInfo->Port);
    }

    ulFifo = I2C_GetTxStatus(I2cInfo->Socket,I2cInfo->Port);
    ulTimes = 0;
    while(0 != ulFifo)
    {
        I2C_Delay(2);

        if(++ulTimes > I2C_READ_TIMEOUT)
        {
            return EFI_TIMEOUT;
        }
        ulFifo = I2C_GetTxStatus(I2cInfo->Socket,I2cInfo->Port);
    }

    return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
I2CWrite(I2C_DEVICE *I2cInfo, UINT16 InfoOffset, UINT32 ulLength, UINT8 *pBuf)
{
    UINT32 ulFifo;
    UINT32 ulTimes = 0;
    UINT32  Idx;
    UINTN  Base;


    if(I2cInfo->Port >= I2C_PORT_MAX)
    {
        return EFI_INVALID_PARAMETER;
    }

    Base = GetI2cBase(I2cInfo->Socket, I2cInfo->Port);

    (VOID)I2C_Enable(I2cInfo->Socket, I2cInfo->Port);

    I2C_SetTarget(I2cInfo->Socket,I2cInfo->Port,I2cInfo->SlaveDeviceAddress);

    ulFifo = I2C_GetTxStatus(I2cInfo->Socket,I2cInfo->Port);
    while(0 != ulFifo)
    {
        I2C_Delay(2);
        if(++ulTimes > I2C_READ_TIMEOUT)
        {
            (VOID)I2C_Disable(I2cInfo->Socket, I2cInfo->Port);
            return EFI_TIMEOUT;
        }
        ulFifo = I2C_GetTxStatus(I2cInfo->Socket,I2cInfo->Port);
    }


    if(I2cInfo->DeviceType)
    {
        I2C_REG_WRITE(Base + I2C_DATA_CMD_OFFSET, (InfoOffset >> 8) & 0xff);
        I2C_REG_WRITE(Base + I2C_DATA_CMD_OFFSET, InfoOffset & 0xff);
    }
    else
    {
        I2C_REG_WRITE(Base + I2C_DATA_CMD_OFFSET, InfoOffset & 0xff);
    }

    ulFifo = I2C_GetTxStatus(I2cInfo->Socket,I2cInfo->Port);
    ulTimes = 0;
    while(0 != ulFifo)
    {
        I2C_Delay(2);
        if(++ulTimes > I2C_READ_TIMEOUT)
        {
            (VOID)I2C_Disable(I2cInfo->Socket, I2cInfo->Port);
            return EFI_TIMEOUT;
        }
        ulFifo = I2C_GetTxStatus(I2cInfo->Socket,I2cInfo->Port);
    }

    for(Idx = 0; Idx < ulLength; Idx++)
    {
        ulTimes = 0;
        ulFifo = I2C_GetTxStatus(I2cInfo->Socket,I2cInfo->Port);
        while(ulFifo > I2C_TXRX_THRESHOLD)
        {
            I2C_Delay(2);
            if(++ulTimes > I2C_READ_TIMEOUT)
            {
                (VOID)I2C_Disable(I2cInfo->Socket, I2cInfo->Port);
                return EFI_TIMEOUT;
            }
            ulFifo = I2C_GetTxStatus(I2cInfo->Socket,I2cInfo->Port);
        }

        if (Idx < ulLength - 1) {
            I2C_REG_WRITE(Base + I2C_DATA_CMD_OFFSET, (*pBuf++));
        } else {
            //Send command stop bit for the last transfer
            I2C_REG_WRITE(Base + I2C_DATA_CMD_OFFSET, (*pBuf++) | I2C_CMD_STOP_BIT);
        }
    }

    ulFifo = I2C_GetTxStatus(I2cInfo->Socket,I2cInfo->Port);
    ulTimes = 0;
    while(0 != ulFifo)
    {
        I2C_Delay(2);

        if(++ulTimes > I2C_READ_TIMEOUT)
        {
            DEBUG ((EFI_D_ERROR, "I2C Write try to finished,time out!\n"));
            (VOID)I2C_Disable(I2cInfo->Socket, I2cInfo->Port);
            return EFI_TIMEOUT;
        }
        ulFifo = I2C_GetTxStatus(I2cInfo->Socket,I2cInfo->Port);
    }
    (VOID)I2C_Disable(I2cInfo->Socket, I2cInfo->Port);

    return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
I2CRead(I2C_DEVICE *I2cInfo, UINT16 InfoOffset,UINT32 ulRxLen,UINT8 *pBuf)
{
    UINT32 ulFifo;
    UINT32 ulTimes = 0;
    UINT8  I2CWAddr[2];
    EFI_STATUS  Status;
    UINT32  Idx = 0;
    UINTN Base;


    if(I2cInfo->Port >= I2C_PORT_MAX)
    {
        return EFI_INVALID_PARAMETER;
    }

    (VOID)I2C_Enable(I2cInfo->Socket, I2cInfo->Port);
    Base = GetI2cBase(I2cInfo->Socket, I2cInfo->Port);
    if(I2cInfo->DeviceType)
    {
        I2CWAddr[0] = (InfoOffset >> 8) & 0xff;
        I2CWAddr[1] = (InfoOffset & 0xff);
        Status = WriteBeforeRead(I2cInfo, 2,I2CWAddr);
        if(EFI_ERROR(Status))
        {
            (VOID)I2C_Disable(I2cInfo->Socket, I2cInfo->Port);
            return EFI_ABORTED;
        }
    }
    else
    {
        I2CWAddr[0] = (InfoOffset & 0xff);
        Status = WriteBeforeRead(I2cInfo, 1,I2CWAddr);
        if(EFI_ERROR(Status))
        {
            (VOID)I2C_Disable(I2cInfo->Socket, I2cInfo->Port);
            return EFI_ABORTED;
        }
    }

    I2C_SetTarget(I2cInfo->Socket,I2cInfo->Port,I2cInfo->SlaveDeviceAddress);

    ulFifo = I2C_GetTxStatus(I2cInfo->Socket,I2cInfo->Port);
    while(0 != ulFifo)
    {
        I2C_Delay(2);

        while(++ulTimes > I2C_READ_TIMEOUT)
        {
            (VOID)I2C_Disable(I2cInfo->Socket, I2cInfo->Port);
            return EFI_TIMEOUT;
        }
        ulFifo = I2C_GetTxStatus(I2cInfo->Socket,I2cInfo->Port);
    }

    while (ulRxLen > 0) {
        if (ulRxLen > 1) {
            I2C_REG_WRITE(Base + I2C_DATA_CMD_OFFSET, I2C_READ_SIGNAL);
        } else {
            //Send command stop bit for the last transfer
            I2C_REG_WRITE(Base + I2C_DATA_CMD_OFFSET, I2C_READ_SIGNAL | I2C_CMD_STOP_BIT);
        }

        ulTimes = 0;
        do {
            I2C_Delay(2);

            while(++ulTimes > I2C_READ_TIMEOUT) {
                (VOID)I2C_Disable(I2cInfo->Socket, I2cInfo->Port);
                return EFI_TIMEOUT;
            }
            ulFifo = I2C_GetRxStatus(I2cInfo->Socket,I2cInfo->Port);
        }while(0 == ulFifo);

        I2C_REG_READ(Base + I2C_DATA_CMD_OFFSET, pBuf[Idx++]);

        ulRxLen --;
    }
    (VOID)I2C_Disable(I2cInfo->Socket, I2cInfo->Port);

    return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
I2CReadMultiByte(I2C_DEVICE *I2cInfo, UINT32 InfoOffset,UINT32 ulRxLen,UINT8 *pBuf)
{
    UINT32 ulCnt;
    UINT16 usTotalLen = 0;
    UINT32 ulFifo;
    UINT32 ulTimes = 0;
    UINT8  I2CWAddr[4];
    EFI_STATUS  Status;
    UINT32  BytesLeft;
    UINT32  Idx = 0;
    UINTN Base;


    if(I2cInfo->Port >= I2C_PORT_MAX)
    {
        return EFI_INVALID_PARAMETER;
    }

    (VOID)I2C_Enable(I2cInfo->Socket, I2cInfo->Port);
    Base = GetI2cBase(I2cInfo->Socket, I2cInfo->Port);
    if(I2cInfo->DeviceType == DEVICE_TYPE_E2PROM)
    {
        I2CWAddr[0] = (InfoOffset >> 8) & 0xff;
        I2CWAddr[1] = (InfoOffset & 0xff);
        Status = WriteBeforeRead(I2cInfo, 2,I2CWAddr);
        if(EFI_ERROR(Status))
        {
            (VOID)I2C_Disable(I2cInfo->Socket, I2cInfo->Port);
            return EFI_ABORTED;
        }
    }

    else if(I2cInfo->DeviceType == DEVICE_TYPE_CPLD_3BYTE_OPERANDS)
    {
        I2CWAddr[0] = (InfoOffset >> 16) & 0xff;
        I2CWAddr[1] = (InfoOffset >> 8) & 0xff;
        I2CWAddr[2] = (InfoOffset & 0xff);
        Status = WriteBeforeRead(I2cInfo, 3,I2CWAddr);
        if(EFI_ERROR(Status))
        {
            (VOID)I2C_Disable(I2cInfo->Socket, I2cInfo->Port);
            return EFI_ABORTED;
        }
    }

    else if(I2cInfo->DeviceType == DEVICE_TYPE_CPLD_4BYTE_OPERANDS)
    {
        I2CWAddr[0] = (InfoOffset >> 24) & 0xff;
        I2CWAddr[1] = (InfoOffset >> 16) & 0xff;
        I2CWAddr[2] = (InfoOffset >> 8) & 0xff;
        I2CWAddr[3] = (InfoOffset & 0xff);
        Status = WriteBeforeRead(I2cInfo, 4,I2CWAddr);
        if(EFI_ERROR(Status))
        {
            (VOID)I2C_Disable(I2cInfo->Socket, I2cInfo->Port);
            return EFI_ABORTED;
        }
    }

    else
    {
        I2CWAddr[0] = (InfoOffset & 0xff);
        Status = WriteBeforeRead(I2cInfo, 1,I2CWAddr);
        if(EFI_ERROR(Status))
        {
            (VOID)I2C_Disable(I2cInfo->Socket, I2cInfo->Port);
            return EFI_ABORTED;
        }
    }


    I2C_SetTarget(I2cInfo->Socket,I2cInfo->Port,I2cInfo->SlaveDeviceAddress);
    usTotalLen = ulRxLen;
    BytesLeft = usTotalLen;

    for(ulCnt = 0; ulCnt < BytesLeft; ulCnt++) {
        I2C_REG_WRITE(Base + I2C_DATA_CMD_OFFSET, I2C_READ_SIGNAL);
    }


    for(ulCnt = 0; ulCnt < BytesLeft; ulCnt++) {
        ulTimes = 0;
        do {
            I2C_Delay(2);

            while(++ulTimes > I2C_READ_TIMEOUT) {
                (VOID)I2C_Disable(I2cInfo->Socket, I2cInfo->Port);
                return EFI_TIMEOUT;
            }
            ulFifo = I2C_GetRxStatus(I2cInfo->Socket,I2cInfo->Port);
        }while(0 == ulFifo);

        I2C_REG_READ(Base + I2C_DATA_CMD_OFFSET, pBuf[Idx++]);
    }
    (VOID)I2C_Disable(I2cInfo->Socket, I2cInfo->Port);

    return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
I2CWriteMultiByte(I2C_DEVICE *I2cInfo, UINT32 InfoOffset, UINT32 ulLength, UINT8 *pBuf)
{
    UINT32 ulFifo;
    UINT32 ulTimes = 0;
    UINT32  Idx;
    UINTN  Base;


    if(I2cInfo->Port >= I2C_PORT_MAX)
    {
        return EFI_INVALID_PARAMETER;
    }

    Base = GetI2cBase(I2cInfo->Socket, I2cInfo->Port);

    (VOID)I2C_Enable(I2cInfo->Socket, I2cInfo->Port);

    I2C_SetTarget(I2cInfo->Socket,I2cInfo->Port,I2cInfo->SlaveDeviceAddress);

    ulFifo = I2C_GetTxStatus(I2cInfo->Socket,I2cInfo->Port);
    while(0 != ulFifo)
    {
        I2C_Delay(2);
        if(++ulTimes > I2C_READ_TIMEOUT)
        {
            (VOID)I2C_Disable(I2cInfo->Socket, I2cInfo->Port);
            return EFI_TIMEOUT;
        }
        ulFifo = I2C_GetTxStatus(I2cInfo->Socket,I2cInfo->Port);
    }


    if(I2cInfo->DeviceType == DEVICE_TYPE_CPLD_3BYTE_OPERANDS)
    {
        I2C_REG_WRITE(Base + I2C_DATA_CMD_OFFSET, (InfoOffset >> 16) & 0xff);
        I2C_REG_WRITE(Base + I2C_DATA_CMD_OFFSET, (InfoOffset >> 8) & 0xff);
        I2C_REG_WRITE(Base + I2C_DATA_CMD_OFFSET, InfoOffset & 0xff);
    }

    else if(I2cInfo->DeviceType == DEVICE_TYPE_CPLD_4BYTE_OPERANDS)
    {
        I2C_REG_WRITE(Base + I2C_DATA_CMD_OFFSET, (InfoOffset >> 24) & 0xff);
        I2C_REG_WRITE(Base + I2C_DATA_CMD_OFFSET, (InfoOffset >> 16) & 0xff);
        I2C_REG_WRITE(Base + I2C_DATA_CMD_OFFSET, (InfoOffset >> 8) & 0xff);
        I2C_REG_WRITE(Base + I2C_DATA_CMD_OFFSET, InfoOffset & 0xff);
    }

    else
    {

    }

    ulTimes = 0;
    for(Idx = 0; Idx < ulLength; Idx++)
    {

       I2C_REG_WRITE(Base + I2C_DATA_CMD_OFFSET, *pBuf++);

    }

    ulFifo = I2C_GetTxStatus(I2cInfo->Socket,I2cInfo->Port);
    ulTimes = 0;
    while(0 != ulFifo)
    {
        I2C_Delay(2);

        if(++ulTimes > I2C_READ_TIMEOUT)
        {
            DEBUG ((EFI_D_ERROR, "I2C Write try to finished,time out!\n"));
            (VOID)I2C_Disable(I2cInfo->Socket, I2cInfo->Port);
            return EFI_TIMEOUT;
        }
        ulFifo = I2C_GetTxStatus(I2cInfo->Socket,I2cInfo->Port);
    }
    (VOID)I2C_Disable(I2cInfo->Socket, I2cInfo->Port);

    return EFI_SUCCESS;
}

