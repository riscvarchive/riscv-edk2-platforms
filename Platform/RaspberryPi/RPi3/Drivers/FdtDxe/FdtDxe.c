/** @file
 *
 *  Copyright (c) 2017, Andrey Warkentin <andrey.warkentin@gmail.com>
 *  Copyright (c) 2016, Linaro, Ltd. All rights reserved.
 *
 *  SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 **/
#include <PiDxe.h>

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/DxeServicesLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <libfdt.h>

#include <Protocol/RpiFirmware.h>

#include <Guid/Fdt.h>

STATIC VOID                             *mFdtImage;

STATIC RASPBERRY_PI_FIRMWARE_PROTOCOL   *mFwProtocol;

STATIC
EFI_STATUS
FixEthernetAliases (
  VOID
)
{
  INTN          Aliases;
  CONST CHAR8   *Ethernet;
  CONST CHAR8   *Ethernet0;
  CONST CHAR8   *Alias;
  UINTN         CopySize;
  CHAR8         *Copy;
  INTN          Retval;
  EFI_STATUS    Status;

  //
  // Look up the 'ethernet[0]' aliases
  //
  Aliases = fdt_path_offset (mFdtImage, "/aliases");
  if (Aliases < 0) {
    DEBUG ((DEBUG_ERROR, "%a: failed to locate '/aliases'\n", __FUNCTION__));
    return EFI_NOT_FOUND;
  }
  Ethernet = fdt_getprop (mFdtImage, Aliases, "ethernet", NULL);
  Ethernet0 = fdt_getprop (mFdtImage, Aliases, "ethernet0", NULL);
  Alias = Ethernet ? Ethernet : Ethernet0;
  if (!Alias) {
    DEBUG ((DEBUG_ERROR, "%a: failed to locate 'ethernet[0]' alias\n", __FUNCTION__));
    return EFI_NOT_FOUND;
  }

  //
  // Create copy for fdt_setprop
  //
  CopySize = AsciiStrSize (Alias);
  Copy = AllocateCopyPool (CopySize, Alias);
  if (!Copy) {
    DEBUG ((DEBUG_ERROR, "%a: failed to copy '%a'\n", __FUNCTION__, Alias));
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Create missing aliases
  //
  Status = EFI_SUCCESS;
  if (!Ethernet) {
    Retval = fdt_setprop (mFdtImage, Aliases, "ethernet", Copy, CopySize);
    if (Retval != 0) {
      Status = EFI_NOT_FOUND;
      DEBUG ((DEBUG_ERROR, "%a: failed to create 'ethernet' alias (%d)\n",
        __FUNCTION__, Retval));
    }
    DEBUG ((DEBUG_INFO, "%a: created 'ethernet' alias '%a'\n", __FUNCTION__, Copy));
  }
  if (!Ethernet0) {
    Retval = fdt_setprop (mFdtImage, Aliases, "ethernet0", Copy, CopySize);
    if (Retval != 0) {
      Status = EFI_NOT_FOUND;
      DEBUG ((DEBUG_ERROR, "%a: failed to create 'ethernet0' alias (%d)\n",
        __FUNCTION__, Retval));
    }
    DEBUG ((DEBUG_INFO, "%a: created 'ethernet0' alias '%a'\n", __FUNCTION__, Copy));
  }

  FreePool (Copy);
  return Status;
}

STATIC
EFI_STATUS
UpdateMacAddress (
  VOID
  )
{
  INTN          Node;
  INTN          Retval;
  EFI_STATUS    Status;
  UINT8         MacAddress[6];

  //
  // Locate the node that the 'ethernet' alias refers to
  //
  Node = fdt_path_offset (mFdtImage, "ethernet");
  if (Node < 0) {
    DEBUG ((DEBUG_ERROR, "%a: failed to locate 'ethernet' alias\n", __FUNCTION__));
    return EFI_NOT_FOUND;
  }

  //
  // Get the MAC address from the firmware
  //
  Status = mFwProtocol->GetMacAddress (MacAddress);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: failed to retrieve MAC address\n", __FUNCTION__));
    return Status;
  }

  Retval = fdt_setprop (mFdtImage, Node, "mac-address", MacAddress,
    sizeof MacAddress);
  if (Retval != 0) {
    DEBUG ((DEBUG_ERROR, "%a: failed to create 'mac-address' property (%d)\n",
      __FUNCTION__, Retval));
    return EFI_NOT_FOUND;
  }

  DEBUG ((DEBUG_INFO, "%a: setting MAC address to %02x:%02x:%02x:%02x:%02x:%02x\n",
    __FUNCTION__, MacAddress[0], MacAddress[1], MacAddress[2], MacAddress[3],
    MacAddress[4], MacAddress[5]));
  return EFI_SUCCESS;
}

//
// Add "bcm2835-usb" to the USB compatible property list, if not present.
// Required because some Linux kernels can't handle USB devices otherwise.
//
STATIC
EFI_STATUS
AddUsbCompatibleProperty (
  VOID
  )
{
  CONST CHAR8   Prop[]    = "brcm,bcm2708-usb";
  CONST CHAR8   NewProp[] = "brcm,bcm2835-usb";
  CONST CHAR8   *List;
  CHAR8         *NewList;
  INT32         ListSize;
  INTN          Node;
  INTN          Retval;

  // Locate the node that the 'usb' alias refers to
  Node = fdt_path_offset (mFdtImage, "usb");
  if (Node < 0) {
    DEBUG ((DEBUG_ERROR, "%a: failed to locate 'usb' alias\n", __FUNCTION__));
    return EFI_NOT_FOUND;
  }

  // Get the property list. This is a list of NUL terminated strings.
  List = fdt_getprop (mFdtImage, Node, "compatible", &ListSize);
  if (List == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: failed to locate properties\n", __FUNCTION__));
    return EFI_NOT_FOUND;
  }

  // Check if the compatible value we plan to add is already present
  if (fdt_stringlist_contains (List, ListSize, NewProp)) {
    DEBUG ((DEBUG_INFO, "%a: property '%a' is already set.\n",
      __FUNCTION__, NewProp));
    return EFI_SUCCESS;
  }

  // Make sure the compatible device is what we expect
  if (!fdt_stringlist_contains (List, ListSize, Prop)) {
    DEBUG ((DEBUG_ERROR, "%a: property '%a' is missing!\n",
      __FUNCTION__, Prop));
    return EFI_NOT_FOUND;
  }

  // Add the new NUL terminated entry to our list
  DEBUG ((DEBUG_INFO, "%a: adding '%a' to the properties\n",
    __FUNCTION__, NewProp));

  NewList = AllocatePool (ListSize + sizeof (NewProp));
  if (NewList == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: failed to allocate memory\n", __FUNCTION__));
    return EFI_OUT_OF_RESOURCES;;
  }
  CopyMem (NewList, List, ListSize);
  CopyMem (&NewList[ListSize], NewProp, sizeof (NewProp));

  Retval = fdt_setprop (mFdtImage, Node, "compatible", NewList,
             ListSize + sizeof (NewProp));
  FreePool (NewList);
  if (Retval != 0) {
    DEBUG ((DEBUG_ERROR, "%a: failed to update properties (%d)\n",
      __FUNCTION__, Retval));
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
CleanMemoryNodes (
  VOID
  )
{
  INTN Node;
  INT32 Retval;

  Node = fdt_path_offset (mFdtImage, "/memory");
  if (Node < 0) {
    return EFI_SUCCESS;
  }

  /*
   * Remove bogus memory nodes which can make the booted
   * OS go crazy and ignore the UEFI map.
   */
  DEBUG ((DEBUG_INFO, "Removing bogus /memory\n"));
  Retval = fdt_del_node (mFdtImage, Node);
  if (Retval != 0) {
    DEBUG ((DEBUG_ERROR, "Failed to remove /memory\n"));
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
SanitizePSCI (
  VOID
  )
{
  INTN Node;
  INTN Root;
  INT32 Retval;

  Root = fdt_path_offset (mFdtImage, "/");
  ASSERT (Root >= 0);
  if (Root < 0) {
    return EFI_NOT_FOUND;
  }

  Node = fdt_path_offset (mFdtImage, "/psci");
  if (Node < 0) {
    Node = fdt_add_subnode (mFdtImage, Root, "psci");
  }

  ASSERT (Node >= 0);
  if (Node < 0) {
    DEBUG ((DEBUG_ERROR, "Couldn't find/create /psci\n"));
    return EFI_NOT_FOUND;
  }

  Retval = fdt_setprop_string (mFdtImage, Node, "compatible", "arm,psci-1.0");
  if (Retval != 0) {
    DEBUG ((DEBUG_ERROR, "Couldn't set /psci compatible property\n"));
    return EFI_NOT_FOUND;
  }

  Retval = fdt_setprop_string (mFdtImage, Node, "method", "smc");
  if (Retval != 0) {
    DEBUG ((DEBUG_ERROR, "Couldn't set /psci method property\n"));
    return EFI_NOT_FOUND;
  }

  Root = fdt_path_offset (mFdtImage, "/cpus");
  if (Root < 0) {
    DEBUG ((DEBUG_ERROR, "No CPUs to update with PSCI enable-method?\n"));
    return EFI_NOT_FOUND;
  }

  Node = fdt_first_subnode (mFdtImage, Root);
  while (Node >= 0) {
    if (fdt_setprop_string (mFdtImage, Node, "enable-method", "psci") != 0) {
      DEBUG ((DEBUG_ERROR, "Failed to update enable-method for a CPU\n"));
      return EFI_NOT_FOUND;
    }

    fdt_delprop (mFdtImage, Node, "cpu-release-addr");
    Node = fdt_next_subnode (mFdtImage, Node);
  }
  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
CleanSimpleFramebuffer (
  VOID
  )
{
  INTN Node;
  INT32 Retval;

  /*
   * Should look for nodes by kind and remove aliases
   * by matching against device.
   */
  Node = fdt_path_offset (mFdtImage, "display0");
  if (Node < 0) {
    return EFI_SUCCESS;
  }

  /*
   * Remove bogus GPU-injected simple-framebuffer, which
   * doesn't reflect the framebuffer built by UEFI.
   */
  DEBUG ((DEBUG_INFO, "Removing bogus display0\n"));
  Retval = fdt_del_node (mFdtImage, Node);
  if (Retval != 0) {
    DEBUG ((DEBUG_ERROR, "Failed to remove display0\n"));
    return EFI_NOT_FOUND;
  }

  Node = fdt_path_offset (mFdtImage, "/aliases");
  if (Node < 0) {
    DEBUG ((DEBUG_ERROR, "Couldn't find /aliases to remove display0\n"));
    return EFI_NOT_FOUND;
  }

  Retval = fdt_delprop (mFdtImage, Node, "display0");
  if (Retval != 0) {
    DEBUG ((DEBUG_ERROR, "Failed to remove display0 alias\n"));
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}

#define MAX_CMDLINE_SIZE    512

STATIC
EFI_STATUS
UpdateBootArgs (
  VOID
  )
{
  INTN          Node;
  INTN          Retval;
  EFI_STATUS    Status;
  CHAR8         *CommandLine;

  //
  // Locate the /chosen node
  //
  Node = fdt_path_offset (mFdtImage, "/chosen");
  if (Node < 0) {
    DEBUG ((DEBUG_ERROR, "%a: failed to locate /chosen node\n", __FUNCTION__));
    return EFI_NOT_FOUND;
  }

  //
  // If /chosen/bootargs already exists, we want to add a space character
  // before adding the firmware supplied arguments. However, the RpiFirmware
  // protocol expects a 32-bit aligned buffer. So let's allocate 4 bytes of
  // slack, and skip the first 3 when passing this buffer into libfdt.
  //
  CommandLine = AllocatePool (MAX_CMDLINE_SIZE) + 4;
  if (!CommandLine) {
    DEBUG ((DEBUG_ERROR, "%a: failed to allocate memory\n", __FUNCTION__));
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Get the command line from the firmware
  //
  Status = mFwProtocol->GetCommandLine (MAX_CMDLINE_SIZE, CommandLine + 4);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: failed to retrieve command line\n", __FUNCTION__));
    return Status;
  }

  if (AsciiStrLen (CommandLine + 4) == 0) {
    DEBUG ((DEBUG_INFO, "%a: empty command line received\n", __FUNCTION__));
    return EFI_SUCCESS;
  }

  CommandLine[3] = ' ';

  Retval = fdt_appendprop_string (mFdtImage, Node, "bootargs", &CommandLine[3]);
  if (Retval != 0) {
    DEBUG ((DEBUG_ERROR, "%a: failed to set /chosen/bootargs property (%d)\n",
      __FUNCTION__, Retval));
  }

  DEBUG_CODE_BEGIN ();
    CONST CHAR8    *Prop;
    INT32         Length;
    INT32         Index;

    Node = fdt_path_offset (mFdtImage, "/chosen");
    ASSERT (Node >= 0);

    Prop = fdt_getprop (mFdtImage, Node, "bootargs", &Length);
    ASSERT (Prop != NULL);

    DEBUG ((DEBUG_INFO, "Command line set from firmware (length %d):\n'", Length));

    for (Index = 0; Index < Length; Index++, Prop++) {
      if (*Prop == '\0') {
        continue;
      }
      DEBUG ((DEBUG_INFO, "%c", *Prop));
    }

    DEBUG ((DEBUG_INFO, "'\n"));
  DEBUG_CODE_END ();

  FreePool (CommandLine - 4);
  return EFI_SUCCESS;
}


/**
  @param  ImageHandle   of the loaded driver
  @param  SystemTable   Pointer to the System Table

  @retval EFI_SUCCESS           Protocol registered
  @retval EFI_OUT_OF_RESOURCES  Cannot allocate protocol data structure
  @retval EFI_DEVICE_ERROR      Hardware problems

**/
EFI_STATUS
EFIAPI
FdtDxeInitialize (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS Status;
  EFI_GUID   *FdtGuid;
  VOID       *FdtImage;
  UINTN      FdtSize;
  INT32      Retval;
  UINT32     BoardRevision;
  BOOLEAN    Internal;

  Status = gBS->LocateProtocol (&gRaspberryPiFirmwareProtocolGuid, NULL,
                  (VOID**)&mFwProtocol);
  ASSERT_EFI_ERROR (Status);

  Internal = FALSE;
  FdtImage = (VOID*)(UINTN)PcdGet32 (PcdFdtBaseAddress);
  Retval = fdt_check_header (FdtImage);
  if (Retval == 0) {
    /*
     * Have FDT passed via config.txt.
     */
    FdtSize = fdt_totalsize (FdtImage);
    DEBUG ((DEBUG_INFO, "Device Tree passed via config.txt (0x%lx bytes)\n", FdtSize));
    Status = EFI_SUCCESS;
  } else {
    /*
     * Use one of the embedded FDT's.
     */
    Internal = TRUE;
    DEBUG ((DEBUG_INFO, "No/Bad Device Tree found at address 0x%p (%a), "
      "looking up internal one...\n", FdtImage, fdt_strerror (Retval)));
    /*
     * Query the board revision to differentiate between models.
     */
    Status = mFwProtocol->GetModelRevision (&BoardRevision);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Failed to get board type: %r\n", Status));
      DEBUG ((DEBUG_INFO, "Using default internal Device Tree\n"));
      FdtGuid = &gRaspberryPiDefaultFdtGuid;
    } else {
      // www.raspberrypi.org/documentation/hardware/raspberrypi/revision-codes/README.md
      switch ((BoardRevision >> 4) & 0xFF) {
      case 0x08:
        DEBUG ((DEBUG_INFO, "Using Raspberry Pi 3 Model B internal Device Tree\n"));
        FdtGuid = &gRaspberryPi3ModelBFdtGuid;
        break;
      case 0x0D:
        DEBUG ((DEBUG_INFO, "Using Raspberry Pi 3 Model B+ internal Device Tree\n"));
        FdtGuid = &gRaspberryPi3ModelBPlusFdtGuid;
        break;
      case 0x11:
        DEBUG ((DEBUG_INFO, "Using Raspberry Pi 4 Model B internal Device Tree\n"));
        FdtGuid = &gRaspberryPi4ModelBFdtGuid;
        break;
      default:
        DEBUG ((DEBUG_INFO, "Using default internal Device Tree\n"));
        FdtGuid = &gRaspberryPiDefaultFdtGuid;
        break;
      }
    }
    do {
      Status = GetSectionFromAnyFv (FdtGuid, EFI_SECTION_RAW, 0, &FdtImage, &FdtSize);
      if (Status == EFI_SUCCESS) {
        if (fdt_check_header (FdtImage) != 0) {
          Status = EFI_INCOMPATIBLE_VERSION;
        }
      }
      // No retry needed if we are successful or are dealing with the default Fdt.
      if ( (Status == EFI_SUCCESS) ||
           (CompareGuid (FdtGuid, &gRaspberryPiDefaultFdtGuid)) )
        break;
      // Otherwise, try one more time with the default Fdt. An example of this
      // is if we detected a non-default Fdt, that isn't included in the FDF.
      DEBUG ((DEBUG_INFO, "Internal Device Tree was not found for this platform, "
        "falling back to default...\n"));
      FdtGuid = &gRaspberryPiDefaultFdtGuid;
    } while (1);
  }

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to locate Device Tree: %r\n", Status));
    return Status;
  }

  /*
   * Probably overkill.
   */
  FdtSize += EFI_PAGE_SIZE * 2;
  Status = gBS->AllocatePages (AllocateAnyPages, EfiBootServicesData,
                  EFI_SIZE_TO_PAGES (FdtSize), (EFI_PHYSICAL_ADDRESS*)&mFdtImage);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to allocate Device Tree: %r\n", Status));
    return Status;
  }

  Retval = fdt_open_into (FdtImage, mFdtImage, FdtSize);
  ASSERT (Retval == 0);

  Status = SanitizePSCI ();
  if (EFI_ERROR (Status)) {
    Print (L"Failed to sanitize PSCI: %r\n", Status);
  }

  Status = CleanMemoryNodes ();
  if (EFI_ERROR (Status)) {
    Print (L"Failed to clean memory nodes: %r\n", Status);
  }

  Status = CleanSimpleFramebuffer ();
  if (EFI_ERROR (Status)) {
    Print (L"Failed to clean frame buffer: %r\n", Status);
  }

  Status = FixEthernetAliases ();
  if (EFI_ERROR (Status)) {
    Print (L"Failed to fix ethernet aliases: %r\n", Status);
  }

  Status = UpdateMacAddress ();
  if (EFI_ERROR (Status)) {
    Print (L"Failed to update MAC address: %r\n", Status);
  }

  Status = AddUsbCompatibleProperty ();
  if (EFI_ERROR (Status)) {
    Print (L"Failed to update USB compatible properties: %r\n", Status);
  }

  if (Internal) {
    /*
     * A GPU-provided DTB already has the full command line.
     */
    Status = UpdateBootArgs ();
    if (EFI_ERROR (Status)) {
      Print (L"Failed to update boot arguments: %r\n", Status);
    }
  }

  DEBUG ((DEBUG_INFO, "Installed Device Tree at address 0x%p\n", mFdtImage));
  Status = gBS->InstallConfigurationTable (&gFdtTableGuid, mFdtImage);
  ASSERT_EFI_ERROR (Status);

  return Status;
}
