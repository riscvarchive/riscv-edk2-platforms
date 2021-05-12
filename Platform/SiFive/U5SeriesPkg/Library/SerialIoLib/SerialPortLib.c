/** @file
  UART Serial Port library functions

  Copyright (c) 2019, Hewlett Packard Enterprise Development LP. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <Library/IoLib.h>
#include <Library/SerialPortLib.h>
//#include <Include/SifiveU5Uart.h>
#include <sbi/riscv_io.h>
//#include <sbi/sbi_console.h>
#include <sbi_utils/serial/sifive-uart.h>

/* clang-format off */

#define UART_REG_TXFIFO		0
#define UART_REG_RXFIFO		1
#define UART_REG_TXCTRL		2
#define UART_REG_RXCTRL		3
#define UART_REG_IE		4
#define UART_REG_IP		5
#define UART_REG_DIV		6

#define UART_TXFIFO_FULL	0x80000000
#define UART_RXFIFO_EMPTY	0x80000000
#define UART_RXFIFO_DATA	0x000000ff
#define UART_TXCTRL_TXEN	0x1
#define UART_RXCTRL_RXEN	0x1

/* clang-format on */

static volatile void *uart_base;
static u32 uart_in_freq;
static u32 uart_baudrate;

/**
 * Find minimum divisor divides in_freq to max_target_hz;
 * Based on uart driver n SiFive FSBL.
 *
 * f_baud = f_in / (div + 1) => div = (f_in / f_baud) - 1
 * The nearest integer solution requires rounding up as to not exceed max_target_hz.
 * div  = ceil(f_in / f_baud) - 1
 *	= floor((f_in - 1 + f_baud) / f_baud) - 1
 * This should not overflow as long as (f_in - 1 + f_baud) does not exceed
 * 2^32 - 1, which is unlikely since we represent frequencies in kHz.
 */
static inline unsigned int uart_min_clk_divisor(uint64_t in_freq,
						uint64_t max_target_hz)
{
	uint64_t quotient = (in_freq + max_target_hz - 1) / (max_target_hz);
	/* Avoid underflow */
	if (quotient == 0) {
		return 0;
	} else {
		return quotient - 1;
	}
}

static u32 get_reg(u32 num)
{
	return readl(uart_base + (num * 0x4));
}

static void set_reg(u32 num, u32 val)
{
	writel(val, uart_base + (num * 0x4));
}

static void sifive_uart_putc(char ch)
{
	while (get_reg(UART_REG_TXFIFO) & UART_TXFIFO_FULL)
		;

	set_reg(UART_REG_TXFIFO, ch);
}

static int sifive_uart_getc(void)
{
	u32 ret = get_reg(UART_REG_RXFIFO);
	if (!(ret & UART_RXFIFO_EMPTY))
		return ret & UART_RXFIFO_DATA;
	return -1;
}

int edk2_sifive_uart_init(unsigned long base, u32 in_freq, u32 baudrate)
{
	uart_base     = (volatile void *)base;
	uart_in_freq  = in_freq;
	uart_baudrate = baudrate;

	/* Configure baudrate */
	if (in_freq)
		set_reg(UART_REG_DIV, uart_min_clk_divisor(in_freq, baudrate));
	/* Disable interrupts */
	set_reg(UART_REG_IE, 0);
	/* Enable TX */
	set_reg(UART_REG_TXCTRL, UART_TXCTRL_TXEN);
	/* Enable Rx */
	set_reg(UART_REG_RXCTRL, UART_RXCTRL_RXEN);

	//sbi_console_set_device(&sifive_console);

	return 0;
}

//---------------------------------------------
// UART Register Offsets
//---------------------------------------------

#define UART_REG_IP     5
#define UART_IP_RXWM    0x02

//---------------------------------------------
// UART Settings
//---------------------------------------------

#define UART_BAUDRATE  115200
#define SYS_CLK        FixedPcdGet32(PcdU5PlatformSystemClock)

BOOLEAN Initiated = FALSE;

/**
  Initialize the serial device hardware.

  If no initialization is required, then return RETURN_SUCCESS.
  If the serial device was successfuly initialized, then return RETURN_SUCCESS.
  If the serial device could not be initialized, then return RETURN_DEVICE_ERROR.

  @retval RETURN_SUCCESS        The serial device was initialized.
  @retval RETURN_DEVICE_ERROR   The serail device could not be initialized.

**/
RETURN_STATUS
EFIAPI
SerialPortInitialize (
  VOID
  )
{
  if (Initiated) {
    return RETURN_SUCCESS;
  }
  if (edk2_sifive_uart_init (FixedPcdGet32(PcdU5UartBase), SYS_CLK / 2, UART_BAUDRATE) != 0) {
      return EFI_DEVICE_ERROR;
  }
  Initiated = TRUE;
  return RETURN_SUCCESS;
}

/**
  Write data from buffer to serial device.

  Writes NumberOfBytes data bytes from Buffer to the serial device.
  The number of bytes actually written to the serial device is returned.
  If the return value is less than NumberOfBytes, then the write operation failed.

  If Buffer is NULL, then ASSERT().

  If NumberOfBytes is zero, then return 0.

  @param  Buffer           Pointer to the data buffer to be written.
  @param  NumberOfBytes    Number of bytes to written to the serial device.

  @retval 0                NumberOfBytes is 0.
  @retval >0               The number of bytes written to the serial device.
                           If this value is less than NumberOfBytes, then the write operation failed.

**/
UINTN
EFIAPI
SerialPortWrite (
  IN UINT8     *Buffer,
  IN UINTN     NumberOfBytes
  )
{
  UINTN Index;

  if (Buffer == NULL || Initiated == FALSE) {
    return 0;
  }

  for(Index = 0; Index < NumberOfBytes; Index ++) {
    sifive_uart_putc (Buffer [Index]);
  }

  return Index;
}

/**
  Reads data from a serial device into a buffer.

  @param  Buffer           Pointer to the data buffer to store the data read from the serial device.
  @param  NumberOfBytes    Number of bytes to read from the serial device.

  @retval 0                NumberOfBytes is 0.
  @retval >0               The number of bytes read from the serial device.
                           If this value is less than NumberOfBytes, then the read operation failed.

**/
UINTN
EFIAPI
SerialPortRead (
  OUT UINT8     *Buffer,
  IN  UINTN     NumberOfBytes
  )
{
  UINTN Index;

  if (NULL == Buffer || Initiated == FALSE) {
    return 0;
  }

  for (Index = 0; Index < NumberOfBytes; Index ++) {
    Buffer [Index] = (UINT8)sifive_uart_getc ();
  }

  return Index;
}

/**
  Polls a serial device to see if there is any data waiting to be read.

  Polls aserial device to see if there is any data waiting to be read.
  If there is data waiting to be read from the serial device, then TRUE is returned.
  If there is no data waiting to be read from the serial device, then FALSE is returned.

  @retval TRUE             Data is waiting to be read from the serial device.
  @retval FALSE            There is no data waiting to be read from the serial device.

**/
BOOLEAN
EFIAPI
SerialPortPoll (
  VOID
  )
{
  STATIC volatile UINT32 * const uart = (UINT32 *)FixedPcdGet32(PcdU5UartBase);
  UINT32 IP;

  if (Initiated == FALSE) {
    return FALSE;
  }
  IP = MmioRead32 ((UINTN)(uart + UART_REG_IP));
  if(IP & UART_IP_RXWM) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**
  Sets the control bits on a serial device.

  @param Control                Sets the bits of Control that are settable.

  @retval RETURN_SUCCESS        The new control bits were set on the serial device.
  @retval RETURN_UNSUPPORTED    The serial device does not support this operation.
  @retval RETURN_DEVICE_ERROR   The serial device is not functioning correctly.

**/
RETURN_STATUS
EFIAPI
SerialPortSetControl (
  IN UINT32 Control
  )
{
  if (Initiated == FALSE) {
    return EFI_NOT_READY;
  }
  return RETURN_SUCCESS;
}

/**
  Retrieve the status of the control bits on a serial device.

  @param Control                A pointer to return the current control signals from the serial device.

  @retval RETURN_SUCCESS        The control bits were read from the serial device.
  @retval RETURN_UNSUPPORTED    The serial device does not support this operation.
  @retval RETURN_DEVICE_ERROR   The serial device is not functioning correctly.

**/
RETURN_STATUS
EFIAPI
SerialPortGetControl (
  OUT UINT32 *Control
  )
{
  if (Initiated == FALSE) {
    return EFI_NOT_READY;
  }
  *Control = 0;
  return RETURN_SUCCESS;
}

/**
  Sets the baud rate, receive FIFO depth, transmit/receice time out, parity,
  data bits, and stop bits on a serial device.

  @param BaudRate           The requested baud rate. A BaudRate value of 0 will use the
                            device's default interface speed.
                            On output, the value actually set.
  @param ReveiveFifoDepth   The requested depth of the FIFO on the receive side of the
                            serial interface. A ReceiveFifoDepth value of 0 will use
                            the device's default FIFO depth.
                            On output, the value actually set.
  @param Timeout            The requested time out for a single character in microseconds.
                            This timeout applies to both the transmit and receive side of the
                            interface. A Timeout value of 0 will use the device's default time
                            out value.
                            On output, the value actually set.
  @param Parity             The type of parity to use on this serial device. A Parity value of
                            DefaultParity will use the device's default parity value.
                            On output, the value actually set.
  @param DataBits           The number of data bits to use on the serial device. A DataBits
                            vaule of 0 will use the device's default data bit setting.
                            On output, the value actually set.
  @param StopBits           The number of stop bits to use on this serial device. A StopBits
                            value of DefaultStopBits will use the device's default number of
                            stop bits.
                            On output, the value actually set.

  @retval RETURN_SUCCESS            The new attributes were set on the serial device.
  @retval RETURN_UNSUPPORTED        The serial device does not support this operation.
  @retval RETURN_INVALID_PARAMETER  One or more of the attributes has an unsupported value.
  @retval RETURN_DEVICE_ERROR       The serial device is not functioning correctly.

**/
RETURN_STATUS
EFIAPI
SerialPortSetAttributes (
  IN OUT UINT64             *BaudRate,
  IN OUT UINT32             *ReceiveFifoDepth,
  IN OUT UINT32             *Timeout,
  IN OUT EFI_PARITY_TYPE    *Parity,
  IN OUT UINT8              *DataBits,
  IN OUT EFI_STOP_BITS_TYPE *StopBits
  )
{
  if (Initiated == FALSE) {
    return EFI_NOT_READY;
  }
  return RETURN_SUCCESS;
}
