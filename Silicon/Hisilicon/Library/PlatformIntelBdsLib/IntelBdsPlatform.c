/** @file
  Implementation for PlatformBdsLib library class interfaces.

  Copyright (C) 2015, Red Hat, Inc.
  Copyright (c) 2014, ARM Ltd. All rights reserved.<BR>
  Copyright (c) 2004 - 2008, Intel Corporation. All rights reserved.<BR>
  Copyright (c) 2015, Hisilicon Limited. All rights reserved.
  Copyright (c) 2015, Linaro Limited. All rights reserved.

  This program and the accompanying materials are licensed and made available
  under the terms and conditions of the BSD License which accompanies this
  distribution. The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS, WITHOUT
  WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

  Based on the files under ArmVirtPkg/Library/PlatformIntelBdsLib/

**/

#include <IndustryStandard/Pci22.h>
#include <Library/DevicePathLib.h>
#include <Library/GenericBdsLib.h>
#include <Library/IpmiCmdLib.h>
#include <Library/PcdLib.h>
#include <Library/PlatformBdsLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiLib.h>
#include <Protocol/DevicePath.h>
#include <Protocol/DevicePathToText.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/PciIo.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Guid/GlobalVariable.h>

#include "IntelBdsPlatform.h"

GUID gOemBootVaraibleGuid = {0xb7784577, 0x5aaf, 0x4557, {0xa1, 0x99,
  0xd4, 0xa4, 0x2f, 0x45, 0x06, 0xf8} };

//3CEF354A-3B7A-4519-AD70-72A134698311
GUID gEblFileGuid = {0x3CEF354A, 0x3B7A, 0x4519, {0xAD, 0x70,
  0x72, 0xA1, 0x34, 0x69, 0x83, 0x11} };

// Need to keep the same with FlashStartOs.inf
// 282cae50-940e-11e5-b7b8-774201c0f2d8
GUID gFlashStartOsAppGuid = { 0x282cae50, 0x940e, 0x11e5, {0xb7, 0xb8,
  0x77, 0x42, 0x01, 0xc0, 0xf2, 0xd8} };

// Need to keep the same with EslStartOs.inf
// 8880a72c-9411-11e5-b6f0-97310bc151d1
GUID gEslStartOsAppGuid = { 0x8880a72c, 0x9411, 0x11e5, {0xb6, 0xf0,
  0x97, 0x31, 0x0b, 0xc1, 0x51, 0xd1} };

EFI_STATUS
BdsDeleteAllInvalidEfiBootOption (
  VOID
  );

#define DP_NODE_LEN(Type) { (UINT8)sizeof (Type), (UINT8)(sizeof (Type) >> 8) }


#pragma pack (1)
typedef struct {
  VENDOR_DEVICE_PATH         SerialDxe;
  UART_DEVICE_PATH           Uart;
  VENDOR_DEFINED_DEVICE_PATH Vt100;
  EFI_DEVICE_PATH_PROTOCOL   End;
} PLATFORM_SERIAL_CONSOLE;
#pragma pack ()

#define SERIAL_DXE_FILE_GUID { \
          0xD3987D4B, 0x971A, 0x435F, \
          { 0x8C, 0xAF, 0x49, 0x67, 0xEB, 0x62, 0x72, 0x41 } \
          }

STATIC PLATFORM_SERIAL_CONSOLE mSerialConsole = {
  //
  // VENDOR_DEVICE_PATH SerialDxe
  //
  {
    { HARDWARE_DEVICE_PATH, HW_VENDOR_DP, DP_NODE_LEN (VENDOR_DEVICE_PATH) },
    SERIAL_DXE_FILE_GUID
  },

  //
  // UART_DEVICE_PATH Uart
  //
  {
    { MESSAGING_DEVICE_PATH, MSG_UART_DP, DP_NODE_LEN (UART_DEVICE_PATH) },
    0,                                      // Reserved
    FixedPcdGet64 (PcdUartDefaultBaudRate), // BaudRate
    FixedPcdGet8 (PcdUartDefaultDataBits),  // DataBits
    FixedPcdGet8 (PcdUartDefaultParity),    // Parity
    FixedPcdGet8 (PcdUartDefaultStopBits)   // StopBits
  },

  //
  // VENDOR_DEFINED_DEVICE_PATH Vt100
  //
  {
    {
      MESSAGING_DEVICE_PATH, MSG_VENDOR_DP,
      DP_NODE_LEN (VENDOR_DEFINED_DEVICE_PATH)
    },
    EFI_VT_100_GUID
  },

  //
  // EFI_DEVICE_PATH_PROTOCOL End
  //
  {
    END_DEVICE_PATH_TYPE, END_ENTIRE_DEVICE_PATH_SUBTYPE,
    DP_NODE_LEN (EFI_DEVICE_PATH_PROTOCOL)
  }
};


#pragma pack (1)
typedef struct {
  USB_CLASS_DEVICE_PATH    Keyboard;
  EFI_DEVICE_PATH_PROTOCOL End;
} PLATFORM_USB_KEYBOARD;
#pragma pack ()

STATIC PLATFORM_USB_KEYBOARD mUsbKeyboard = {
  //
  // USB_CLASS_DEVICE_PATH Keyboard
  //
  {
    {
      MESSAGING_DEVICE_PATH, MSG_USB_CLASS_DP,
      DP_NODE_LEN (USB_CLASS_DEVICE_PATH)
    },
    0xFFFF, // VendorId: any
    0xFFFF, // ProductId: any
    3,      // DeviceClass: HID
    1,      // DeviceSubClass: boot
    1       // DeviceProtocol: keyboard
  },

  //
  // EFI_DEVICE_PATH_PROTOCOL End
  //
  {
    END_DEVICE_PATH_TYPE, END_ENTIRE_DEVICE_PATH_SUBTYPE,
    DP_NODE_LEN (EFI_DEVICE_PATH_PROTOCOL)
  }
};

STATIC
UINT16
GetBBSTypeFromFileSysPath (
  IN CHAR16                   *UsbPathTxt,
  IN CHAR16                   *FileSysPathTxt,
  IN EFI_DEVICE_PATH_PROTOCOL *FileSysPath
  )
{
  EFI_DEVICE_PATH_PROTOCOL *Node;

  if (StrnCmp (UsbPathTxt, FileSysPathTxt, StrLen (UsbPathTxt)) == 0) {
    Node = FileSysPath;
    while (!IsDevicePathEnd (Node)) {
      if ((DevicePathType (Node) == MEDIA_DEVICE_PATH) &&
          (DevicePathSubType (Node) == MEDIA_CDROM_DP)) {
        return BBS_TYPE_CDROM;
      }
      Node = NextDevicePathNode (Node);
    }
  }

  return BBS_TYPE_UNKNOWN;
}

STATIC
UINT16
GetBBSTypeFromUsbPath (
  IN CONST EFI_DEVICE_PATH_PROTOCOL *UsbPath
  )
{
  EFI_STATUS                        Status;
  EFI_HANDLE                        *FileSystemHandles;
  UINTN                             NumberFileSystemHandles;
  UINTN                             Index;
  EFI_DEVICE_PATH_PROTOCOL          *FileSysPath;
  EFI_DEVICE_PATH_TO_TEXT_PROTOCOL  *DevPathToText;
  CHAR16                            *UsbPathTxt;
  CHAR16                            *FileSysPathTxt;
  UINT16                            Result;

  Status = gBS->LocateProtocol (&gEfiDevicePathToTextProtocolGuid, NULL, (VOID **) &DevPathToText);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Locate DevicePathToTextPro %r\n", Status));
    return BBS_TYPE_UNKNOWN;
  }

  Result = BBS_TYPE_UNKNOWN;
  UsbPathTxt = DevPathToText->ConvertDevicePathToText (UsbPath, TRUE, TRUE);
  if (UsbPathTxt == NULL) {
    return Result;
  }

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiSimpleFileSystemProtocolGuid,
                  NULL,
                  &NumberFileSystemHandles,
                  &FileSystemHandles
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Locate SimpleFileSystemProtocol error(%r)\n", Status));
    FreePool (UsbPathTxt);
    return BBS_TYPE_UNKNOWN;
  }

  for (Index = 0; Index < NumberFileSystemHandles; Index++) {
    FileSysPath = DevicePathFromHandle (FileSystemHandles[Index]);
    FileSysPathTxt = DevPathToText->ConvertDevicePathToText (FileSysPath, TRUE, TRUE);

    if (FileSysPathTxt == NULL) {
      continue;
    }

    Result = GetBBSTypeFromFileSysPath (UsbPathTxt, FileSysPathTxt, FileSysPath);
    FreePool (FileSysPathTxt);

    if (Result != BBS_TYPE_UNKNOWN) {
      break;
    }
  }

  if (NumberFileSystemHandles != 0) {
    FreePool (FileSystemHandles);
  }

  FreePool (UsbPathTxt);

  return Result;
}

STATIC
UINT16
GetBBSTypeFromMessagingDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL *DevicePath,
  IN EFI_DEVICE_PATH_PROTOCOL *Node
  )
{
  VENDOR_DEVICE_PATH       *Vendor;
  UINT16                   Result;

  Result = BBS_TYPE_UNKNOWN;

  switch (DevicePathSubType (Node)) {
  case MSG_MAC_ADDR_DP:
    Result = BBS_TYPE_EMBEDDED_NETWORK;
    break;

  case MSG_USB_DP:
    Result = GetBBSTypeFromUsbPath (DevicePath);
    if (Result == BBS_TYPE_UNKNOWN) {
      Result =  BBS_TYPE_USB;
    }
    break;

  case MSG_SATA_DP:
    Result = BBS_TYPE_HARDDRIVE;
    break;

  case MSG_VENDOR_DP:
    Vendor = (VENDOR_DEVICE_PATH *) (Node);
    if ((&Vendor->Guid) != NULL) {
      if (CompareGuid (&Vendor->Guid, &((EFI_GUID) DEVICE_PATH_MESSAGING_SAS))) {
        Result = BBS_TYPE_HARDDRIVE;
      }
    }
    break;

  default:
    Result = BBS_TYPE_UNKNOWN;
    break;
  }

  return Result;
}

STATIC
UINT16
GetBBSTypeByDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL *DevicePath
  )
{
  EFI_DEVICE_PATH_PROTOCOL *Node;
  UINT16                   Result;

  Result = BBS_TYPE_UNKNOWN;
  if (DevicePath == NULL) {
    return Result;
  }

  Node = DevicePath;
  while (!IsDevicePathEnd (Node)) {
    switch (DevicePathType (Node)) {
    case MEDIA_DEVICE_PATH:
      if (DevicePathSubType (Node) == MEDIA_CDROM_DP) {
        Result = BBS_TYPE_CDROM;
      }
      break;

    case MESSAGING_DEVICE_PATH:
      Result = GetBBSTypeFromMessagingDevicePath (DevicePath, Node);
      break;

    default:
      Result = BBS_TYPE_UNKNOWN;
      break;
    }

    if (Result != BBS_TYPE_UNKNOWN) {
      break;
    }

    Node = NextDevicePathNode (Node);
  }

  return Result;
}

STATIC
EFI_STATUS
GetBmcBootOptionsSetting (
  OUT IPMI_GET_BOOT_OPTION *BmcBootOpt
  )
{
  EFI_STATUS   Status;

  Status = IpmiCmdGetSysBootOptions (BmcBootOpt);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Get iBMC BootOpts %r!\n", Status));
    return Status;
  }

  if (BmcBootOpt->BootFlagsValid != BOOT_OPTION_BOOT_FLAG_VALID) {
    return EFI_NOT_FOUND;
  }

  if (BmcBootOpt->Persistent) {
    BmcBootOpt->BootFlagsValid = BOOT_OPTION_BOOT_FLAG_VALID;
  } else {
    BmcBootOpt->BootFlagsValid = BOOT_OPTION_BOOT_FLAG_INVALID;
  }

  Status = IpmiCmdSetSysBootOptions (BmcBootOpt);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Set iBMC BootOpts %r!\n", Status));
  }

  return Status;
}

STATIC
VOID
RestoreBootOrder (
  VOID
  )
{
  EFI_STATUS                Status;
  UINT16                    *BootOrder;
  UINTN                     BootOrderSize;

  GetVariable2 (L"BootOrderBackup", &gOemBootVaraibleGuid, (VOID **) &BootOrder, &BootOrderSize);
  if (BootOrder == NULL) {
    return ;
  }

  Print (L"Restore BootOrder(%d).\n", BootOrderSize / sizeof (UINT16));

  Status = gRT->SetVariable (
                  L"BootOrder",
                  &gEfiGlobalVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  BootOrderSize,
                  BootOrder
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "SetVariable BootOrder %r!\n", Status));
  }

  Status = gRT->SetVariable (
                  L"BootOrderBackup",
                  &gOemBootVaraibleGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  0,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "SetVariable BootOrderBackup %r!\n", Status));
  }

  FreePool (BootOrder);

  return;
}


VOID
RestoreBootOrderOnReadyToBoot (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  // restore BootOrder variable in normal condition.
  RestoreBootOrder ();
}

STATIC
VOID
UpdateBootOrder (
  IN UINT16  *NewOrder,
  IN UINT16  *BootOrder,
  IN UINTN   BootOrderSize
  )
{
  EFI_STATUS  Status;
  EFI_EVENT   Event;

  Status = gRT->SetVariable (
                  L"BootOrderBackup",
                  &gOemBootVaraibleGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  BootOrderSize,
                  BootOrder
                  );
  if (EFI_ERROR (Status)) {
    return;
  }

  Status = gRT->SetVariable (
                  L"BootOrder",
                  &gEfiGlobalVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  BootOrderSize,
                  NewOrder
                  );
  if (EFI_ERROR (Status)) {
    return;
  }

  // Register notify function to restore BootOrder variable on ReadyToBoot Event.
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  RestoreBootOrderOnReadyToBoot,
                  NULL,
                  &gEfiEventReadyToBootGuid,
                  &Event
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Create ready to boot event %r!\n", Status));
  }

  return;
}

STATIC
VOID
SetBootOrder (
  IN UINT16 BootType
  )
{
  UINT16                       *NewOrder;
  UINT16                       *RemainBoots;
  UINT16                       *BootOrder;
  UINTN                        BootOrderSize;
  CHAR16                       OptionName[sizeof ("Boot####")];
  UINTN                        Index;
  LIST_ENTRY                   BootOptionList;
  BDS_COMMON_OPTION            *Option;
  UINTN                        SelectCnt;
  UINTN                        RemainCnt;

  InitializeListHead (&BootOptionList);

  GetEfiGlobalVariable2 (L"BootOrder", (VOID **) &BootOrder, &BootOrderSize);
  if (BootOrder == NULL) {
    return ;
  }

  NewOrder = AllocatePool (BootOrderSize);
  RemainBoots = AllocatePool (BootOrderSize);
  if ((NewOrder == NULL) || (RemainBoots == NULL)) {
    DEBUG ((DEBUG_ERROR, "Out of resources."));
    goto Exit;
  }

  SelectCnt = 0;
  RemainCnt = 0;

  for (Index = 0; Index < BootOrderSize / sizeof (UINT16); Index++) {
    UnicodeSPrint (OptionName, sizeof (OptionName), L"Boot%04x", BootOrder[Index]);
    Option = BdsLibVariableToOption (&BootOptionList, OptionName);
    if (Option == NULL) {
      DEBUG ((DEBUG_ERROR, "Boot%04x is invalid option!\n", BootOrder[Index]));
      continue;
    }

    if (GetBBSTypeByDevicePath (Option->DevicePath) == BootType) {
      NewOrder[SelectCnt++] = BootOrder[Index];
    } else {
      RemainBoots[RemainCnt++] = BootOrder[Index];
    }
  }

  if (SelectCnt != 0) {
    // append RemainBoots to NewOrder
    for (Index = 0; Index < RemainCnt; Index++) {
      NewOrder[SelectCnt + Index] = RemainBoots[Index];
    }

    if (CompareMem (NewOrder, BootOrder, BootOrderSize) != 0) {
      UpdateBootOrder (NewOrder, BootOrder, BootOrderSize);
    }
  }

Exit:
  FreePool (BootOrder);
  if (NewOrder != NULL) {
    FreePool (NewOrder);
  }
  if (RemainBoots != NULL) {
    FreePool (RemainBoots);
  }

  return ;
}

STATIC
VOID
HandleBmcBootType (
  VOID
  )
{
  EFI_STATUS                Status;
  IPMI_GET_BOOT_OPTION      BmcBootOpt;
  UINT16                    BootType;

  Status = GetBmcBootOptionsSetting (&BmcBootOpt);
  if (EFI_ERROR (Status)) {
    return;
  }

  Print (L"Boot Type from BMC is %x\n", BmcBootOpt.BootDeviceSelector);

  switch (BmcBootOpt.BootDeviceSelector) {
  case ForcePxe:
    BootType = BBS_TYPE_EMBEDDED_NETWORK;
    break;

  case ForcePrimaryRemovableMedia:
    BootType = BBS_TYPE_USB;
    break;

  case ForceDefaultHardDisk:
    BootType = BBS_TYPE_HARDDRIVE;
    break;

  case ForceDefaultCD:
    BootType = BBS_TYPE_CDROM;
    break;

  default:
    return;
  }

  SetBootOrder (BootType);
}

//
// BDS Platform Functions
//
/**
  Platform Bds init. Include the platform firmware vendor, revision
  and so crc check.

**/
VOID
EFIAPI
PlatformBdsInit (
  VOID
  )
{
  //Signal EndofDxe Event
  EfiEventGroupSignal(&gEfiEndOfDxeEventGroupGuid);

  // restore BootOrder variable if previous BMC boot override attempt
  // left it in a modified state
  RestoreBootOrder ();
}


/**
  Check if the handle satisfies a particular condition.

  @param[in] Handle      The handle to check.
  @param[in] ReportText  A caller-allocated string passed in for reporting
                         purposes. It must never be NULL.

  @retval TRUE   The condition is satisfied.
  @retval FALSE  Otherwise. This includes the case when the condition could not
                 be fully evaluated due to an error.
**/
typedef
BOOLEAN
(EFIAPI *FILTER_FUNCTION) (
  IN EFI_HANDLE   Handle,
  IN CONST CHAR16 *ReportText
  );


/**
  Process a handle.

  @param[in] Handle      The handle to process.
  @param[in] ReportText  A caller-allocated string passed in for reporting
                         purposes. It must never be NULL.
**/
typedef
VOID
(EFIAPI *CALLBACK_FUNCTION)  (
  IN EFI_HANDLE   Handle,
  IN CONST CHAR16 *ReportText
  );

/**
  Locate all handles that carry the specified protocol, filter them with a
  callback function, and pass each handle that passes the filter to another
  callback.

  @param[in] ProtocolGuid  The protocol to look for.

  @param[in] Filter        The filter function to pass each handle to. If this
                           parameter is NULL, then all handles are processed.

  @param[in] Process       The callback function to pass each handle to that
                           clears the filter.
**/
STATIC
VOID
FilterAndProcess (
  IN EFI_GUID          *ProtocolGuid,
  IN FILTER_FUNCTION   Filter         OPTIONAL,
  IN CALLBACK_FUNCTION Process
  )
{
  EFI_STATUS Status;
  EFI_HANDLE *Handles;
  UINTN      NoHandles;
  UINTN      Idx;

  Status = gBS->LocateHandleBuffer (ByProtocol, ProtocolGuid,
                  NULL /* SearchKey */, &NoHandles, &Handles);
  if (EFI_ERROR (Status)) {
    //
    // This is not an error, just an informative condition.
    //
    DEBUG ((EFI_D_VERBOSE, "%a: %g: %r\n", __FUNCTION__, ProtocolGuid,
      Status));
    return;
  }

  ASSERT (NoHandles > 0);
  for (Idx = 0; Idx < NoHandles; ++Idx) {
    CHAR16        *DevicePathText;
    STATIC CHAR16 Fallback[] = L"<device path unavailable>";

    //
    // The ConvertDevicePathToText() function handles NULL input transparently.
    //
    DevicePathText = ConvertDevicePathToText (
                       DevicePathFromHandle (Handles[Idx]),
                       FALSE, // DisplayOnly
                       FALSE  // AllowShortcuts
                       );
    if (DevicePathText == NULL) {
      DevicePathText = Fallback;
    }

    if (Filter == NULL || Filter (Handles[Idx], DevicePathText)) {
      Process (Handles[Idx], DevicePathText);
    }

    if (DevicePathText != Fallback) {
      FreePool (DevicePathText);
    }
  }
  gBS->FreePool (Handles);
}


/**
  This FILTER_FUNCTION checks if a handle corresponds to a PCI display device.
**/
STATIC
BOOLEAN
EFIAPI
IsPciDisplay (
  IN EFI_HANDLE   Handle,
  IN CONST CHAR16 *ReportText
  )
{
  EFI_STATUS          Status;
  EFI_PCI_IO_PROTOCOL *PciIo;
  PCI_TYPE00          Pci;

  Status = gBS->HandleProtocol (Handle, &gEfiPciIoProtocolGuid,
                  (VOID**)&PciIo);
  if (EFI_ERROR (Status)) {
    //
    // This is not an error worth reporting.
    //
    return FALSE;
  }

  Status = PciIo->Pci.Read (PciIo, EfiPciIoWidthUint32, 0 /* Offset */,
                        sizeof Pci / sizeof (UINT32), &Pci);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "%a: %s: %r\n", __FUNCTION__, ReportText, Status));
    return FALSE;
  }

  return IS_PCI_DISPLAY (&Pci);
}


/**
  This CALLBACK_FUNCTION attempts to connect a handle non-recursively, asking
  the matching driver to produce all first-level child handles.
**/
STATIC
VOID
EFIAPI
Connect (
  IN EFI_HANDLE   Handle,
  IN CONST CHAR16 *ReportText
  )
{
  EFI_STATUS Status;

  Status = gBS->ConnectController (
                  Handle, // ControllerHandle
                  NULL,   // DriverImageHandle
                  NULL,   // RemainingDevicePath -- produce all children
                  FALSE   // Recursive
                  );
  DEBUG ((EFI_ERROR (Status) ? EFI_D_ERROR : EFI_D_VERBOSE, "%a: %s: %r\n",
    __FUNCTION__, ReportText, Status));
}


/**
  This CALLBACK_FUNCTION retrieves the EFI_DEVICE_PATH_PROTOCOL from the
  handle, and adds it to ConOut and ErrOut.
**/
STATIC
VOID
EFIAPI
AddOutput (
  IN EFI_HANDLE   Handle,
  IN CONST CHAR16 *ReportText
  )
{
  EFI_STATUS               Status;
  EFI_DEVICE_PATH_PROTOCOL *DevicePath;

  DevicePath = DevicePathFromHandle (Handle);
  if (DevicePath == NULL) {
    DEBUG ((EFI_D_ERROR, "%a: %s: handle %p: device path not found\n",
      __FUNCTION__, ReportText, Handle));
    return;
  }

  Status = BdsLibUpdateConsoleVariable (L"ConOut", DevicePath, NULL);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "%a: %s: adding to ConOut: %r\n", __FUNCTION__,
      ReportText, Status));
    return;
  }

  Status = BdsLibUpdateConsoleVariable (L"ErrOut", DevicePath, NULL);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "%a: %s: adding to ErrOut: %r\n", __FUNCTION__,
      ReportText, Status));
    return;
  }

  DEBUG ((EFI_D_VERBOSE, "%a: %s: added to ConOut and ErrOut\n", __FUNCTION__,
    ReportText));
}


/**
  The function will execute with as the platform policy, current policy
  is driven by boot mode. IBV/OEM can customize this code for their specific
  policy action.

  @param  DriverOptionList        The header of the driver option link list
  @param  BootOptionList          The header of the boot option link list
  @param  ProcessCapsules         A pointer to ProcessCapsules()
  @param  BaseMemoryTest          A pointer to BaseMemoryTest()

**/
VOID
EFIAPI
PlatformBdsPolicyBehavior (
  IN LIST_ENTRY                      *DriverOptionList,
  IN LIST_ENTRY                      *BootOptionList,
  IN PROCESS_CAPSULES                ProcessCapsules,
  IN BASEM_MEMORY_TEST               BaseMemoryTest
  )
{
  EFI_STATUS Status;
  //
  // Locate the PCI root bridges and make the PCI bus driver connect each,
  // non-recursively. This will produce a number of child handles with PciIo on
  // them.
  //
  FilterAndProcess (&gEfiPciRootBridgeIoProtocolGuid, NULL, Connect);

  //
  // Find all display class PCI devices (using the handles from the previous
  // step), and connect them non-recursively. This should produce a number of
  // child handles with GOPs on them.
  //
  FilterAndProcess (&gEfiPciIoProtocolGuid, IsPciDisplay, Connect);

  //
  // Now add the device path of all handles with GOP on them to ConOut and
  // ErrOut.
  //
  FilterAndProcess (&gEfiGraphicsOutputProtocolGuid, NULL, AddOutput);

  //
  // Add the hardcoded short-form USB keyboard device path to ConIn.
  //
  BdsLibUpdateConsoleVariable (L"ConIn",
    (EFI_DEVICE_PATH_PROTOCOL *)&mUsbKeyboard, NULL);

  //
  // Add the hardcoded serial console device path to ConIn, ConOut, ErrOut.
  //
  BdsLibUpdateConsoleVariable (L"ConIn",
    (EFI_DEVICE_PATH_PROTOCOL *)&mSerialConsole, NULL);
  BdsLibUpdateConsoleVariable (L"ConOut",
    (EFI_DEVICE_PATH_PROTOCOL *)&mSerialConsole, NULL);
  BdsLibUpdateConsoleVariable (L"ErrOut",
    (EFI_DEVICE_PATH_PROTOCOL *)&mSerialConsole, NULL);

  //
  // Connect the consoles based on the above variables.
  //
  BdsLibConnectAllDefaultConsoles ();

  //
  // Show the splash screen.
  //
  EnableQuietBoot (PcdGetPtr (PcdLogoFile));

  //
  // Connect the rest of the devices.
  //
  BdsLibConnectAll ();

  //
  // Add memory test to convert memory above 4GB to be tested
  //
  Status = BaseMemoryTest (QUICK);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "[%a:%d] - Base memory test failed: %r\n", __FUNCTION__, __LINE__, Status));
  }

  //
  // Process QEMU's -kernel command line option. Note that the kernel booted
  // this way should receive ACPI tables, which is why we connect all devices
  // first (see above) -- PCI enumeration blocks ACPI table installation, if
  // there is a PCI host.
  //
  //TryRunningQemuKernel ();

  BdsLibEnumerateAllBootOption (BootOptionList);

  // Add Flash Start OS and ESL Start OS boot option
  (VOID) HwBdsLibRegisterAppBootOption (BootOptionList, &gFlashStartOsAppGuid, L"Flash Start OS");
  (VOID) HwBdsLibRegisterAppBootOption (BootOptionList, &gEslStartOsAppGuid, L"ESL Start OS");

  // Add EBL as boot option
  (VOID) HwBdsLibRegisterAppBootOption (BootOptionList, &gEblFileGuid, L"Embedded Boot Loader (EBL)");

  // Remove EFI Misc Device Boot Options
  BdsDeleteAllInvalidEfiBootOption ();

  //SetBootOrderFromQemu (BootOptionList);
  //
  // The BootOrder variable may have changed, reload the in-memory list with
  // it.
  //
  BdsLibBuildOptionFromVar (BootOptionList, L"BootOrder");

  //PlatformBdsEnterFrontPage (GetFrontPageTimeoutFromQemu(), TRUE);
  Print (L"Press Enter to boot OS immediately.\n");
  Print (L"Press any other key in %d seconds to stop automatical booting...\n", PcdGet16(PcdPlatformBootTimeOut));
  PlatformBdsEnterFrontPage (PcdGet16(PcdPlatformBootTimeOut), TRUE);
  HandleBmcBootType ();
}

/**
  Hook point after a boot attempt succeeds. We don't expect a boot option to
  return, so the UEFI 2.0 specification defines that you will default to an
  interactive mode and stop processing the BootOrder list in this case. This
  is also a platform implementation and can be customized by IBV/OEM.

  @param  Option                  Pointer to Boot Option that succeeded to boot.

**/
VOID
EFIAPI
PlatformBdsBootSuccess (
  IN  BDS_COMMON_OPTION *Option
  )
{
}

/**
  Hook point after a boot attempt fails.

  @param  Option                  Pointer to Boot Option that failed to boot.
  @param  Status                  Status returned from failed boot.
  @param  ExitData                Exit data returned from failed boot.
  @param  ExitDataSize            Exit data size returned from failed boot.

**/
VOID
EFIAPI
PlatformBdsBootFail (
  IN  BDS_COMMON_OPTION  *Option,
  IN  EFI_STATUS         Status,
  IN  CHAR16             *ExitData,
  IN  UINTN              ExitDataSize
  )
{
}

/**
  This function locks platform flash that is not allowed to be updated during normal boot path.
  The flash layout is platform specific.
**/
VOID
EFIAPI
PlatformBdsLockNonUpdatableFlash (
  VOID
  )
{
  return;
}
