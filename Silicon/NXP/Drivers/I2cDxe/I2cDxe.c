/** I2cDxe.c
  I2c driver APIs for read, write, initialize, set speed and reset

  Copyright 2017-2019 NXP

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/DevicePathLib.h>
#include <Library/IoLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/TimerLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>

#include "I2cDxe.h"

STATIC CONST EFI_I2C_CONTROLLER_CAPABILITIES mI2cControllerCapabilities = {
  0,
  0,
  0,
  0
};

STATIC CONST CLK_DIV mClkDiv[] = {
  { 20,  0x00 }, { 22, 0x01 }, { 24, 0x02 }, { 26, 0x03 },
  { 28,  0x04 }, { 30, 0x05 }, { 32, 0x09 }, { 34, 0x06 },
  { 36,  0x0A }, { 40, 0x07 }, { 44, 0x0C }, { 48, 0x0D },
  { 52,  0x43 }, { 56, 0x0E }, { 60, 0x45 }, { 64, 0x12 },
  { 68,  0x0F }, { 72, 0x13 }, { 80, 0x14 }, { 88, 0x15 },
  { 96,  0x19 }, { 104, 0x16 }, { 112, 0x1A }, { 128, 0x17 },
  { 136, 0x4F }, { 144, 0x1C }, { 160, 0x1D }, { 176, 0x55 },
  { 192, 0x1E }, { 208, 0x56 }, { 224, 0x22 }, { 228, 0x24 },
  { 240, 0x1F }, { 256, 0x23 }, { 288, 0x5C }, { 320, 0x25 },
  { 384, 0x26 }, { 448, 0x2A }, { 480, 0x27 }, { 512, 0x2B },
  { 576, 0x2C }, { 640, 0x2D }, { 768, 0x31 }, { 896, 0x32 },
  { 960, 0x2F }, { 1024, 0x33 }, { 1152, 0x34 }, { 1280, 0x35 },
  { 1536, 0x36 }, { 1792, 0x3A }, { 1920, 0x37 }, { 2048, 0x3B },
  { 2304, 0x3C }, { 2560, 0x3D }, { 3072, 0x3E }, { 3584, 0x7A },
  { 3840, 0x3F }, { 4096, 0x7B }, { 5120, 0x7D }, { 6144, 0x7E }
};

/**
  Calculate and return proper clock divider

  @param  Rate       desired clock rate

  @retval ClkDiv     Index value used to get Bus Clock Rate

**/
STATIC
UINT8
GetClkDivIndex (
  IN  UINT32         Rate
  )
{
  UINTN              ClkRate;
  UINT32             Div;
  UINT8              Index;

  Index = 0;
  ClkRate = GetBusFrequency ();

  Div = (ClkRate + Rate - 1) / Rate;

  if (Div < mClkDiv[0].SCLDivider) {
    return 0;
  }

  do {
    if (mClkDiv[Index].SCLDivider >= Div ) {
      return Index;
    }
    Index++;
  } while (Index < ARRAY_SIZE (mClkDiv));

  return (ARRAY_SIZE (mClkDiv) - 1);
}

/**
  Function used to check if i2c is in mentioned state or not

  @param   I2cRegs        Pointer to I2C registers
  @param   State          i2c state need to be checked

  @retval  EFI_NOT_READY  Arbitration was lost
  @retval  EFI_TIMEOUT    Timeout occured
  @retval  CurrState      Value of state register

**/
STATIC
EFI_STATUS
WaitForI2cState (
  IN  I2C_REGS            *I2cRegs,
  IN  UINT32              State
  )
{
  UINT8                   CurrState;
  UINT64                  Count;

  for (Count = 0; Count < I2C_STATE_RETRIES; Count++) {
    MemoryFence ();
    CurrState = MmioRead8 ((UINTN)&I2cRegs->I2cSr);
    if (CurrState & I2C_SR_IAL) {
      MmioWrite8 ((UINTN)&I2cRegs->I2cSr, CurrState | I2C_SR_IAL);
      return EFI_NOT_READY;
    }

    if ((CurrState & (State >> 8)) == (UINT8)State) {
      return CurrState;
    }
  }

  return EFI_TIMEOUT;
}

/**
  Function to transfer byte on i2c

  @param   I2cRegs        Pointer to i2c registers
  @param   Byte           Byte to be transferred on i2c bus

  @retval  EFI_NOT_READY  Arbitration was lost
  @retval  EFI_TIMEOUT    Timeout occured
  @retval  EFI_NOT_FOUND  ACK was not recieved
  @retval  EFI_SUCCESS    Data transfer was succesful

**/
STATIC
EFI_STATUS
TransferByte (
  IN  I2C_REGS            *I2cRegs,
  IN  UINT8               Byte
  )
{
  EFI_STATUS              RetVal;

  MmioWrite8 ((UINTN)&I2cRegs->I2cSr, I2C_SR_IIF_CLEAR);
  MmioWrite8 ((UINTN)&I2cRegs->I2cDr, Byte);

  RetVal = WaitForI2cState (I2cRegs, IIF);
  if ((RetVal == EFI_TIMEOUT) || (RetVal == EFI_NOT_READY)) {
    return RetVal;
  }

  if (RetVal & I2C_SR_RX_NO_AK) {
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}

/**
  Function to stop transaction on i2c bus

  @param   I2cRegs          Pointer to i2c registers

  @retval  EFI_NOT_READY    Arbitration was lost
  @retval  EFI_TIMEOUT      Timeout occured
  @retval  EFI_SUCCESS      Stop operation was successful

**/
STATIC
EFI_STATUS
I2cStop (
  IN  I2C_REGS             *I2cRegs
  )
{
  EFI_STATUS               RetVal;
  UINT32                   Temp;

  Temp = MmioRead8 ((UINTN)&I2cRegs->I2cCr);

  Temp &= ~(I2C_CR_MSTA | I2C_CR_MTX);
  MmioWrite8 ((UINTN)&I2cRegs->I2cCr, Temp);

  RetVal = WaitForI2cState (I2cRegs, BUS_IDLE);

  if (RetVal < 0) {
    return RetVal;
  } else {
    return EFI_SUCCESS;
  }
}

/**
  Function to send start signal, Chip Address and
  memory offset

  @param   I2cRegs         Pointer to i2c base registers
  @param   Chip            Chip Address
  @param   Offset          Slave memory's offset
  @param   AddressLength   length of chip address

  @retval  EFI_NOT_READY   Arbitration lost
  @retval  EFI_TIMEOUT     Failed to initialize data transfer in predefined time
  @retval  EFI_NOT_FOUND   ACK was not recieved
  @retval  EFI_SUCCESS     Read was successful

**/
STATIC
EFI_STATUS
InitTransfer (
  IN  I2C_REGS             *I2cRegs,
  IN  UINT8                Chip,
  IN  UINT32               Offset,
  IN  INT32                AddressLength
  )
{
  UINT32                   Temp;
  EFI_STATUS               RetVal;

  // Enable I2C controller
  if (MmioRead8 ((UINTN)&I2cRegs->I2cCr) & I2C_CR_IDIS) {
    MmioWrite8 ((UINTN)&I2cRegs->I2cCr, I2C_CR_IEN);
  }

  if (MmioRead8 ((UINTN)&I2cRegs->I2cAdr) == (Chip << 1)) {
    MmioWrite8 ((UINTN)&I2cRegs->I2cAdr, (Chip << 1) ^ 2);
  }

  MmioWrite8 ((UINTN)&I2cRegs->I2cSr, I2C_SR_IIF_CLEAR);
  RetVal = WaitForI2cState (I2cRegs, BUS_IDLE);
  if ((RetVal == EFI_TIMEOUT) || (RetVal == EFI_NOT_READY)) {
    return RetVal;
  }

  // Start I2C transaction
  Temp = MmioRead8 ((UINTN)&I2cRegs->I2cCr);
  // set to master mode
  Temp |= I2C_CR_MSTA;
  MmioWrite8 ((UINTN)&I2cRegs->I2cCr, Temp);

  RetVal = WaitForI2cState (I2cRegs, BUS_BUSY);
  if ((RetVal == EFI_TIMEOUT) || (RetVal == EFI_NOT_READY)) {
    return RetVal;
  }

  Temp |= I2C_CR_MTX | I2C_CR_TX_NO_AK;
  MmioWrite8 ((UINTN)&I2cRegs->I2cCr, Temp);

  // write slave Address
  RetVal = TransferByte (I2cRegs, Chip << 1);
  if (RetVal != EFI_SUCCESS) {
    return RetVal;
  }

  if (AddressLength >= 0) {
    while (AddressLength--) {
      RetVal = TransferByte (I2cRegs, (Offset >> (AddressLength * 8)) & 0xff);
      if (RetVal != EFI_SUCCESS)
        return RetVal;
    }
  }
  return EFI_SUCCESS;
}

/**
  Function to check if i2c bus is idle

  @param   Base          Pointer to base address of I2c controller

  @retval  EFI_SUCCESS

**/
STATIC
INT32
I2cBusIdle (
  IN  VOID               *Base
  )
{
  return EFI_SUCCESS;
}

/**
  Function to initiate data transfer on i2c bus

  @param   I2cRegs         Pointer to i2c base registers
  @param   Chip            Chip Address
  @param   Offset          Slave memory's offset
  @param   AddressLength   length of chip address

  @retval  EFI_NOT_READY   Arbitration lost
  @retval  EFI_TIMEOUT     Failed to initialize data transfer in predefined time
  @retval  EFI_NOT_FOUND   ACK was not recieved
  @retval  EFI_SUCCESS     Read was successful

**/
STATIC
EFI_STATUS
InitDataTransfer (
  IN  I2C_REGS             *I2cRegs,
  IN  UINT8                Chip,
  IN  UINT32               Offset,
  IN  INT32                AddressLength
  )
{
  EFI_STATUS               RetVal;
  INT32                    Retry;

  for (Retry = 0; Retry < RETRY_COUNT; Retry++) {
    RetVal = InitTransfer (I2cRegs, Chip, Offset, AddressLength);
    if (RetVal == EFI_SUCCESS) {
      return EFI_SUCCESS;
    }

    I2cStop (I2cRegs);

    if (EFI_NOT_FOUND == RetVal) {
      return RetVal;
    }

    // Disable controller
    if (RetVal != EFI_NOT_READY) {
      MmioWrite8 ((UINTN)&I2cRegs->I2cCr, I2C_CR_IDIS);
    }

    if (I2cBusIdle (I2cRegs) < 0) {
      break;
    }
  }
  return RetVal;
}

/**
  Function to read data using i2c bus

  @param   BaseAddr        I2c Controller Base Address
  @param   Chip            Address of slave device from where data to be read
  @param   Offset          Offset of slave memory
  @param   AddressLength   Address length of slave
  @param   Buffer          A pointer to the destination buffer for the data
  @param   Len             Length of data to be read

  @retval  EFI_NOT_READY   Arbitration lost
  @retval  EFI_TIMEOUT     Failed to initialize data transfer in predefined time
  @retval  EFI_NOT_FOUND   ACK was not recieved
  @retval  EFI_SUCCESS     Read was successful

**/
STATIC
EFI_STATUS
I2cDataRead (
  IN  UINTN                BaseAddr,
  IN  UINT8                Chip,
  IN  UINT32               Offset,
  IN  UINT32               AddressLength,
  IN  UINT8                *Buffer,
  IN  UINT32               Len
  )
{
  EFI_STATUS               RetVal;
  UINT32                   Temp;
  INT32                    I;
  I2C_REGS                 *I2cRegs;

  I2cRegs = (I2C_REGS *)(BaseAddr);

  RetVal = InitDataTransfer (I2cRegs, Chip, Offset, AddressLength);
  if (RetVal != EFI_SUCCESS) {
    return RetVal;
  }

  Temp = MmioRead8 ((UINTN)&I2cRegs->I2cCr);
  Temp |= I2C_CR_RSTA;
  MmioWrite8 ((UINTN)&I2cRegs->I2cCr, Temp);

  RetVal = TransferByte (I2cRegs, (Chip << 1) | 1);
  if (RetVal != EFI_SUCCESS) {
    I2cStop (I2cRegs);
    return RetVal;
  }

  // setup bus to read data
  Temp = MmioRead8 ((UINTN)&I2cRegs->I2cCr);
  Temp &= ~(I2C_CR_MTX | I2C_CR_TX_NO_AK);
  if (Len == 1) {
    Temp |= I2C_CR_TX_NO_AK;
  }

  MmioWrite8 ((UINTN)&I2cRegs->I2cCr, Temp);
  MmioWrite8 ((UINTN)&I2cRegs->I2cSr, I2C_SR_IIF_CLEAR);

  // Dummy Read to initiate recieve operation
  MmioRead8 ((UINTN)&I2cRegs->I2cDr);

  for (I = 0; I < Len; I++) {
    RetVal = WaitForI2cState (I2cRegs, IIF);
    if ((RetVal == EFI_TIMEOUT) || (RetVal == EFI_NOT_READY)) {
       I2cStop (I2cRegs);
       return RetVal;
    }
    //
    // It must generate STOP before read I2DR to prevent
    // controller from generating another clock cycle
    //
    if (I == (Len - 1)) {
      I2cStop (I2cRegs);
    } else if (I == (Len - 2)) {
      Temp = MmioRead8 ((UINTN)&I2cRegs->I2cCr);
      Temp |= I2C_CR_TX_NO_AK;
      MmioWrite8 ((UINTN)&I2cRegs->I2cCr, Temp);
    }
    MmioWrite8 ((UINTN)&I2cRegs->I2cSr, I2C_SR_IIF_CLEAR);
    Buffer[I] = MmioRead8 ((UINTN)&I2cRegs->I2cDr);
  }

  I2cStop (I2cRegs);

  return EFI_SUCCESS;
}

/**
  Function to write data using i2c bus

  @param   BaseAddr        I2c Controller Base Address
  @param   Chip            Address of slave device where data to be written
  @param   Offset          Offset of slave memory
  @param   AddressLength   Address length of slave
  @param   Buffer          A pointer to the source buffer for the data
  @param   Len             Length of data to be write

  @retval  EFI_NOT_READY   Arbitration lost
  @retval  EFI_TIMEOUT     Failed to initialize data transfer in predefined time
  @retval  EFI_NOT_FOUND   ACK was not recieved
  @retval  EFI_SUCCESS     Read was successful

**/
STATIC
EFI_STATUS
I2cDataWrite (
  IN  UINTN                BaseAddr,
  IN  UINT8                Chip,
  IN  UINT32               Offset,
  IN  INT32                AddressLength,
  OUT UINT8                *Buffer,
  IN  INT32                Len
  )
{
  EFI_STATUS               RetVal;
  I2C_REGS                 *I2cRegs;
  INT32                    I;

  I2cRegs = (I2C_REGS *)BaseAddr;

  RetVal = InitDataTransfer (I2cRegs, Chip, Offset, AddressLength);
  if (RetVal != EFI_SUCCESS) {
    return RetVal;
  }

  // Write operation
  for (I = 0; I < Len; I++) {
    RetVal = TransferByte (I2cRegs, Buffer[I]);
    if (RetVal != EFI_SUCCESS) {
      break;
    }
  }

  I2cStop (I2cRegs);
  return RetVal;
}

/**
  Function to set i2c bus frequency

  @param   This            Pointer to I2c master protocol
  @param   BusClockHertz   value to be set

  @retval EFI_SUCCESS      Operation successfull
**/
STATIC
EFI_STATUS
EFIAPI
SetBusFrequency (
  IN CONST EFI_I2C_MASTER_PROTOCOL   *This,
  IN OUT UINTN                       *BusClockHertz
 )
{
  I2C_REGS                 *I2cRegs;
  UINT8                    ClkId;
  UINT8                    SpeedId;
  NXP_I2C_MASTER           *I2c;

  I2c = NXP_I2C_FROM_THIS (This);

  I2cRegs = (I2C_REGS *)(I2c->Dev->Resources[0].AddrRangeMin);

  ClkId = GetClkDivIndex (*BusClockHertz);
  SpeedId = mClkDiv[ClkId].BusClockRate;

  // Store divider value
  MmioWrite8 ((UINTN)&I2cRegs->I2cFdr, SpeedId);

  MemoryFence ();

  return EFI_SUCCESS;
}

/**
  Function to reset I2c Controller

  @param  This             Pointer to I2c master protocol

  @return EFI_SUCCESS      Operation successfull
**/
STATIC
EFI_STATUS
EFIAPI
Reset (
  IN CONST EFI_I2C_MASTER_PROTOCOL *This
  )
{
  I2C_REGS                         *I2cRegs;
  NXP_I2C_MASTER                   *I2c;

  I2c = NXP_I2C_FROM_THIS (This);

  I2cRegs = (I2C_REGS *)(I2c->Dev->Resources[0].AddrRangeMin);

  // Reset module
  MmioWrite8 ((UINTN)&I2cRegs->I2cCr, I2C_CR_IDIS);
  MmioWrite8 ((UINTN)&I2cRegs->I2cSr, 0);

  MemoryFence ();

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
EFIAPI
StartRequest (
  IN CONST EFI_I2C_MASTER_PROTOCOL *This,
  IN UINTN                         SlaveAddress,
  IN EFI_I2C_REQUEST_PACKET        *RequestPacket,
  IN EFI_EVENT                     Event            OPTIONAL,
  OUT EFI_STATUS                   *I2cStatus       OPTIONAL
  )
{
  NXP_I2C_MASTER                   *I2c;
  UINT32                           Count;
  INT32                            RetVal;
  UINT32                           Length;
  UINT8                            *Buffer;
  UINT32                           Flag;
  UINT32                           RegAddress;
  UINT32                           OffsetLength;

  RegAddress = 0;

  I2c = NXP_I2C_FROM_THIS (This);

  if (RequestPacket->OperationCount <= 0) {
    DEBUG ((DEBUG_ERROR,"%a: Operation count is not valid %d\n",
           __FUNCTION__, RequestPacket->OperationCount));
    return EFI_INVALID_PARAMETER;
  }

  OffsetLength = RequestPacket->Operation[0].LengthInBytes;
  RegAddress = *RequestPacket->Operation[0].Buffer;

  for (Count = 1; Count < RequestPacket->OperationCount; Count++) {
    Flag = RequestPacket->Operation[Count].Flags;
    Length = RequestPacket->Operation[Count].LengthInBytes;
    Buffer = RequestPacket->Operation[Count].Buffer;

    if (Length <= 0) {
      DEBUG ((DEBUG_ERROR,"%a: Invalid length of buffer %d\n",
             __FUNCTION__, Length));
      return EFI_INVALID_PARAMETER;
    }

    if (Flag == I2C_FLAG_READ) {
      RetVal = I2cDataRead (I2c->Dev->Resources[0].AddrRangeMin, SlaveAddress,
                            RegAddress, OffsetLength, Buffer, Length);
      if (RetVal != EFI_SUCCESS) {
        DEBUG ((DEBUG_ERROR,"%a: I2c read operation failed (error %d)\n",
               __FUNCTION__, RetVal));
        return RetVal;
      }
    } else if (Flag == I2C_FLAG_WRITE) {
      RetVal = I2cDataWrite (I2c->Dev->Resources[0].AddrRangeMin, SlaveAddress,
                             RegAddress, OffsetLength, Buffer, Length);
      if (RetVal != EFI_SUCCESS) {
        DEBUG ((DEBUG_ERROR,"%a: I2c write operation failed (error %d)\n",
               __FUNCTION__, RetVal));
        return RetVal;
      }
    } else {
      DEBUG ((DEBUG_ERROR,"%a: Invalid Flag %d\n", __FUNCTION__, Flag));
      return EFI_INVALID_PARAMETER;
    }
  }

  return EFI_SUCCESS;
}

EFI_STATUS
NxpI2cInit (
  IN EFI_HANDLE             DriverBindingHandle,
  IN EFI_HANDLE             ControllerHandle
  )
{
  EFI_STATUS                RetVal;
  NON_DISCOVERABLE_DEVICE   *Dev;
  NXP_I2C_MASTER            *I2c;

  RetVal = gBS->OpenProtocol (ControllerHandle,
                              &gEdkiiNonDiscoverableDeviceProtocolGuid,
                              (VOID **)&Dev, DriverBindingHandle,
                              ControllerHandle, EFI_OPEN_PROTOCOL_BY_DRIVER);
  if (EFI_ERROR (RetVal)) {
    return RetVal;
  }

  I2c = AllocateZeroPool (sizeof (NXP_I2C_MASTER));

  I2c->Signature                            = NXP_I2C_SIGNATURE;
  I2c->I2cMaster.SetBusFrequency            = SetBusFrequency;
  I2c->I2cMaster.Reset                      = Reset;
  I2c->I2cMaster.StartRequest               = StartRequest;
  I2c->I2cMaster.I2cControllerCapabilities  = &mI2cControllerCapabilities;
  I2c->Dev                                  = Dev;

  CopyGuid (&I2c->DevicePath.Vendor.Guid, &gEfiCallerIdGuid);
  I2c->DevicePath.MmioBase = I2c->Dev->Resources[0].AddrRangeMin;
  SetDevicePathNodeLength (&I2c->DevicePath.Vendor,
    sizeof (I2c->DevicePath) - sizeof (I2c->DevicePath.End));
  SetDevicePathEndNode (&I2c->DevicePath.End);

  RetVal = gBS->InstallMultipleProtocolInterfaces (&ControllerHandle,
                  &gEfiI2cMasterProtocolGuid, (VOID**)&I2c->I2cMaster,
                  &gEfiDevicePathProtocolGuid, &I2c->DevicePath,
                  NULL);

  if (EFI_ERROR (RetVal)) {
    FreePool (I2c);
    gBS->CloseProtocol (ControllerHandle,
                        &gEdkiiNonDiscoverableDeviceProtocolGuid,
                        DriverBindingHandle,
                        ControllerHandle);
  }

  return RetVal;
}

EFI_STATUS
NxpI2cRelease (
  IN EFI_HANDLE                 DriverBindingHandle,
  IN EFI_HANDLE                 ControllerHandle
  )
{
  EFI_I2C_MASTER_PROTOCOL       *I2cMaster;
  EFI_STATUS                    RetVal;
  NXP_I2C_MASTER                *I2c;

  RetVal = gBS->HandleProtocol (ControllerHandle,
                                &gEfiI2cMasterProtocolGuid,
                                (VOID **)&I2cMaster);
  ASSERT_EFI_ERROR (RetVal);
  if (EFI_ERROR (RetVal)) {
    return RetVal;
  }

  I2c = NXP_I2C_FROM_THIS (I2cMaster);

  RetVal = gBS->UninstallMultipleProtocolInterfaces (ControllerHandle,
                  &gEfiI2cMasterProtocolGuid, I2cMaster,
                  &gEfiDevicePathProtocolGuid, &I2c->DevicePath,
                  NULL);
  if (EFI_ERROR (RetVal)) {
    return RetVal;
  }

  RetVal = gBS->CloseProtocol (ControllerHandle,
                               &gEdkiiNonDiscoverableDeviceProtocolGuid,
                               DriverBindingHandle,
                               ControllerHandle);
  ASSERT_EFI_ERROR (RetVal);
  if (EFI_ERROR (RetVal)) {
    return RetVal;
  }

  gBS->FreePool (I2c);

  return EFI_SUCCESS;
}
