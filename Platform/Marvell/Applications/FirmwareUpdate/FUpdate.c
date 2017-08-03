/*******************************************************************************
Copyright (C) 2016 Marvell International Ltd.

Marvell BSD License Option

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File under the following licensing terms.
Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.

* Neither the name of Marvell nor the names of its contributors may be
  used to endorse or promote products derived from this software without
  specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/
#include <ShellBase.h>
#include <Uefi.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/FileHandleLib.h>
#include <Library/HiiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PrintLib.h>
#include <Library/ShellCEntryLib.h>
#include <Library/ShellCommandLib.h>
#include <Library/ShellLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>

#include <Protocol/Spi.h>
#include <Protocol/SpiFlash.h>

#define CMD_NAME_STRING       L"fupdate"

#define MAIN_HDR_MAGIC        0xB105B002

STATIC MARVELL_SPI_FLASH_PROTOCOL *SpiFlashProtocol;
STATIC MARVELL_SPI_MASTER_PROTOCOL *SpiMasterProtocol;

STATIC CONST CHAR16 gShellFUpdateFileName[] = L"ShellCommands";
STATIC EFI_HANDLE gShellFUpdateHiiHandle = NULL;

STATIC CONST SHELL_PARAM_ITEM ParamList[] = {
  {L"help", TypeFlag},
  {NULL , TypeMax}
  };

typedef struct {              // Bytes
  UINT32  Magic;              //  0-3
  UINT32  PrologSize;         //  4-7
  UINT32  PrologChecksum;     //  8-11
  UINT32  BootImageSize;      // 12-15
  UINT32  BootImageChecksum;  // 16-19
  UINT32  Reserved0;          // 20-23
  UINT32  LoadAddr;           // 24-27
  UINT32  ExecAddr;           // 28-31
  UINT8   UartConfig;         //  32
  UINT8   Baudrate;           //  33
  UINT8   ExtCount;           //  34
  UINT8   AuxFlags;           //  35
  UINT32  IoArg0;             // 36-39
  UINT32  IoArg1;             // 40-43
  UINT32  IoArg2;             // 43-47
  UINT32  IoArg3;             // 48-51
  UINT32  Reserved1;          // 52-55
  UINT32  Reserved2;          // 56-59
  UINT32  Reserved3;          // 60-63
} MV_FIRMWARE_IMAGE_HEADER;

STATIC
EFI_STATUS
SpiFlashProbe (
  IN SPI_DEVICE    *Slave
  )
{
  EFI_STATUS       Status;
  UINT32           IdBuffer, Id, RefId;

  Id = PcdGet32 (PcdSpiFlashId);

  IdBuffer = CMD_READ_ID & 0xff;

  // Read SPI flash ID
  SpiFlashProtocol->ReadId (Slave, sizeof (UINT32), (UINT8 *)&IdBuffer);

  // Swap and extract 3 bytes of the ID
  RefId = SwapBytes32 (IdBuffer) >> 8;

  if (RefId == 0) {
    Print (L"%s: No SPI flash detected");
    return EFI_DEVICE_ERROR;
  } else if (RefId != Id) {
    Print (L"%s: Unsupported SPI flash detected with ID=%2x\n", CMD_NAME_STRING, RefId);
    return EFI_DEVICE_ERROR;
  }

  Print (L"%s: Detected supported SPI flash with ID=%3x\n", CMD_NAME_STRING, RefId);

  Status = SpiFlashProtocol->Init (SpiFlashProtocol, Slave);
  if (EFI_ERROR(Status)) {
    Print (L"%s: Cannot initialize flash device\n", CMD_NAME_STRING);
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
CheckImageHeader (
  IN OUT UINTN *ImageHeader
  )
{
  MV_FIRMWARE_IMAGE_HEADER *Header;
  UINT32 HeaderLength, Checksum, ChecksumBackup;

  Header = (MV_FIRMWARE_IMAGE_HEADER *)ImageHeader;
  HeaderLength = Header->PrologSize;
  ChecksumBackup = Header->PrologChecksum;

  // Compare magic number
  if (Header->Magic != MAIN_HDR_MAGIC) {
    Print (L"%s: Bad Image magic 0x%08x != 0x%08x\n", CMD_NAME_STRING, Header->Magic, MAIN_HDR_MAGIC);
    return EFI_DEVICE_ERROR;
  }

  // The checksum field is discarded from calculation
  Header->PrologChecksum = 0;

  Checksum = CalculateSum32 ((UINT32 *)Header, HeaderLength);
  if (Checksum != ChecksumBackup) {
    Print (L"%s: Bad Image checksum. 0x%x != 0x%x\n", CMD_NAME_STRING, Checksum, ChecksumBackup);
    return EFI_DEVICE_ERROR;
  }

  // Restore checksum backup
  Header->PrologChecksum = ChecksumBackup;

  return 0;
}

STATIC
EFI_STATUS
PrepareFirmwareImage (
  IN LIST_ENTRY             *CheckPackage,
  IN OUT SHELL_FILE_HANDLE  *FileHandle,
  IN OUT UINTN              **FileBuffer,
  IN OUT UINTN              *FileSize
  )
{
  CONST CHAR16         *FileStr;
  EFI_STATUS           Status;
  UINT64               OpenMode;
  UINTN                *Buffer;

  // Parse string from commandline
  FileStr = ShellCommandLineGetRawValue (CheckPackage, 1);
  if (FileStr == NULL) {
    Print (L"%s: No image specified\n", CMD_NAME_STRING);
    return EFI_INVALID_PARAMETER;
  } else {
    Status = ShellIsFile (FileStr);
    if (EFI_ERROR(Status)) {
      Print (L"%s: File not found\n", CMD_NAME_STRING);
      return EFI_INVALID_PARAMETER;
    }
  }

  // Obtain file size
  OpenMode = EFI_FILE_MODE_READ;

  Status = ShellOpenFileByName (FileStr, FileHandle, OpenMode, 0);
    if (EFI_ERROR (Status)) {
      Print (L"%s: Cannot open Image file\n", CMD_NAME_STRING);
      return EFI_DEVICE_ERROR;
    }

  Status = FileHandleGetSize (*FileHandle, FileSize);
    if (EFI_ERROR (Status)) {
      Print (L"%s: Cannot get Image file size\n", CMD_NAME_STRING);
    }

  // Read Image header into buffer
  Buffer = AllocateZeroPool (*FileSize);

  Status = FileHandleRead (*FileHandle, FileSize, Buffer);
  if (EFI_ERROR (Status)) {
    Print (L"%s: Cannot read Image file header\n", CMD_NAME_STRING);
    ShellCloseFile (FileHandle);
    FreePool (Buffer);
    return EFI_DEVICE_ERROR;
  }

  *FileBuffer = Buffer;

  return EFI_SUCCESS;
}

/**
  Return the file name of the help text file if not using HII.

  @return The string pointer to the file name.
**/
STATIC
CONST CHAR16*
EFIAPI
ShellCommandGetManFileNameFUpdate (
  VOID
  )
{
  return gShellFUpdateFileName;
}

STATIC
VOID
FUpdateUsage (
  VOID
  )
{
  Print (L"\nFirmware update command\n"
         "fupdate <LocalFilePath>\n\n"
         "LocalFilePath - path to local firmware image file\n"
         "Example:\n"
         "Update firmware from file fs2:flash-image.bin\n"
         "  fupdate fs2:flash-image.bin\n"
  );
}

STATIC
SHELL_STATUS
EFIAPI
ShellCommandRunFUpdate (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  IN SHELL_FILE_HANDLE    FileHandle;
  SPI_DEVICE              *Slave;
  UINTN                   FileSize;
  UINTN                   *FileBuffer = NULL;
  CHAR16                  *ProblemParam;
  LIST_ENTRY              *CheckPackage;
  EFI_STATUS              Status;

  // Locate SPI protocols
  Status = gBS->LocateProtocol (
                   &gMarvellSpiFlashProtocolGuid,
                   NULL,
                   (VOID **)&SpiFlashProtocol
                 );

  if (EFI_ERROR(Status)) {
    Print (L"%s: Cannot locate SpiFlash protocol\n", CMD_NAME_STRING);
    return SHELL_ABORTED;
  }

  Status = gBS->LocateProtocol (
                   &gMarvellSpiMasterProtocolGuid,
                   NULL,
                   (VOID **)&SpiMasterProtocol
                 );

  if (EFI_ERROR(Status)) {
    Print (L"%s: Cannot locate SpiMaster protocol\n", CMD_NAME_STRING);
    return SHELL_ABORTED;
  }

  // Parse command line
  Status = ShellInitialize ();
  if (EFI_ERROR (Status)) {
    Print (L"%s: Error while initializing Shell\n", CMD_NAME_STRING);
    ASSERT_EFI_ERROR (Status);
    return SHELL_ABORTED;
  }

  Status = ShellCommandLineParse (ParamList, &CheckPackage, &ProblemParam, TRUE);
  if (EFI_ERROR (Status)) {
    Print (L"%s: Invalid parameter\n", CMD_NAME_STRING);
    return SHELL_ABORTED;
  }

  if (ShellCommandLineGetFlag (CheckPackage, L"help")) {
    FUpdateUsage();
    return EFI_SUCCESS;
  }

  // Prepare local file to be burned into flash
  Status = PrepareFirmwareImage (CheckPackage, &FileHandle, &FileBuffer, &FileSize);
  if (EFI_ERROR(Status)) {
    return SHELL_ABORTED;
  }

  // Check image checksum and magic
  Status = CheckImageHeader (FileBuffer);
  if (EFI_ERROR(Status)) {
    goto HeaderError;
  }

  // Setup and probe SPI flash
  Slave = SpiMasterProtocol->SetupDevice (SpiMasterProtocol, 0, 0);
  if (Slave == NULL) {
    Print(L"%s: Cannot allocate SPI device!\n", CMD_NAME_STRING);
    goto HeaderError;
  }

  Status = SpiFlashProbe (Slave);
  if (EFI_ERROR(Status)) {
    Print (L"%s: Error while performing SPI flash probe\n", CMD_NAME_STRING);
    goto FlashProbeError;
  }

  // Update firmware image in flash at offset 0x0
  Status = SpiFlashProtocol->Update (Slave, 0, FileSize, (UINT8 *)FileBuffer);

  // Release resources
  SpiMasterProtocol->FreeDevice(Slave);
  FreePool (FileBuffer);
  ShellCloseFile (&FileHandle);

  if (EFI_ERROR(Status)) {
    Print (L"%s: Error while performing flash update\n", CMD_NAME_STRING);
    return SHELL_ABORTED;
  }

  Print (L"%s: Update %d bytes at offset 0x0 succeeded!\n", CMD_NAME_STRING, FileSize);

  return EFI_SUCCESS;

FlashProbeError:
  SpiMasterProtocol->FreeDevice(Slave);
HeaderError:
  FreePool (FileBuffer);
  ShellCloseFile (&FileHandle);

  return SHELL_ABORTED;
}

EFI_STATUS
EFIAPI
ShellFUpdateCommandConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS Status;

  gShellFUpdateHiiHandle = NULL;

  gShellFUpdateHiiHandle = HiiAddPackages (
                                  &gShellFUpdateHiiGuid,
                                  gImageHandle,
                                  UefiShellFUpdateCommandLibStrings,
                                  NULL
                                );

  if (gShellFUpdateHiiHandle == NULL) {
    Print (L"%s: Cannot add Hii package\n", CMD_NAME_STRING);
    return EFI_DEVICE_ERROR;
  }

  Status = ShellCommandRegisterCommandName (
                           CMD_NAME_STRING,
                           ShellCommandRunFUpdate,
                           ShellCommandGetManFileNameFUpdate,
                           0,
                           CMD_NAME_STRING,
                           TRUE,
                           gShellFUpdateHiiHandle,
                           STRING_TOKEN (STR_GET_HELP_FUPDATE)
                         );

  if (EFI_ERROR(Status)) {
    Print (L"%s: Error while registering command\n", CMD_NAME_STRING);
    return SHELL_ABORTED;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
ShellFUpdateCommandDestructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{

  if (gShellFUpdateHiiHandle != NULL) {
    HiiRemovePackages (gShellFUpdateHiiHandle);
  }

  return EFI_SUCCESS;
}
