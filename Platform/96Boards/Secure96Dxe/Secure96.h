/** @file

  Copyright (c) 2018, Linaro, Ltd. All rights reserved.<BR>

  This program and the accompanying materials are licensed and made available
  under the terms and conditions of the BSD License which accompanies this
  distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
**/

#ifndef _SECURE96_H_
#define _SECURE96_H_

#define ATSHA204A_SLAVE_ADDRESS         0x60
#define ATSHA204A_DT_NODENAME           atsha204a@60

#define ATECC508A_SLAVE_ADDRESS         0x51
#define ATECC508A_DT_NODENAME           atecc508a@51

#define INFINEON_SLB9670_SPI_CS         0x0
#define INFINEON_SLB9670_DT_NODENAME    tpm@0

#endif // _SECURE96_H_
