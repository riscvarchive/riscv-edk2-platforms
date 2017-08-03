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

#include "VirtualEhciPciIo.h"
#include <Protocol/PciRootBridgeIo.h>

UINT32 mUsbMemBase;
UINTN mSegmentNumber  = 0;
// Use 0xFF for the virtual PCI devices
UINTN mBusNumber      = 0xFF;
UINTN mDeviceNumber   = 0;
UINTN mFunctionNumber = 0;

typedef struct {
  EFI_PHYSICAL_ADDRESS              HostAddress;
  EFI_PHYSICAL_ADDRESS              DeviceAddress;
  UINTN                             NumberOfBytes;
  EFI_PCI_IO_PROTOCOL_OPERATION     Operation;
  BOOLEAN                           DoubleBuffer;
} MEM_MAP_INFO_INSTANCE;


EFI_CPU_ARCH_PROTOCOL      *gCpu;


EHCI_PCI_CONFIG mEhciPciConfig = {
  {
    0x00,//UINT16  VendorId;
    0x00,//UINT16  DeviceId;
    0x00,//UINT16  Command;
    0x0010,//UINT16  Status;
    0x00,//UINT8   RevisionID;
    {
      PCI_IF_EHCI,//UINT8   ClassCode[3];
      PCI_CLASS_SERIAL_USB,
      PCI_CLASS_SERIAL
    },
    0x00,//UINT8   CacheLineSize;
    0x00,//UINT8   LatencyTimer;
    0x00,//UINT8   HeaderType;
    0x00//UINT8   BIST;
  },
  {
    {
      0x00,//UINT32  Bar[6];
      0x00,
      0x00,
      0x00,
      0x00,
      0x00
    },
    0x00,//UINT32  CISPtr;
    0x00,//UINT16  SubsystemVendorID;
    0x00,//UINT16  SubsystemID;
    0x00,//UINT32  ExpansionRomBar;
    0x40,//UINT8   CapabilityPtr;
    {
      0x00,//UINT8   Reserved1[3];
      0x00,
      0x00
    },
    0x00,//UINT32  Reserved2;
    0x00,//UINT8   InterruptLine;
    0x00,//UINT8   InterruptPin;
    0x00,//UINT8   MinGnt;
    0x00//UINT8   MaxLat;
  },
    0x0A,//  UINT8 CapabilityID offset 0x40
    0x00,// UINT8 NextItemPtr
    0x2000 //UINT16 DebugPort
};



EFI_STATUS
EhciPciIoPollMem (
  IN EFI_PCI_IO_PROTOCOL           *This,
  IN  EFI_PCI_IO_PROTOCOL_WIDTH    Width,
  IN  UINT8                        BarIndex,
  IN  UINT64                       Offset,
  IN  UINT64                       Mask,
  IN  UINT64                       Value,
  IN  UINT64                       Delay,
  OUT UINT64                       *Result
  )
{
  ASSERT (FALSE);
  return EFI_UNSUPPORTED;
}

EFI_STATUS
EhciPciIoPollIo (
  IN EFI_PCI_IO_PROTOCOL           *This,
  IN  EFI_PCI_IO_PROTOCOL_WIDTH    Width,
  IN  UINT8                        BarIndex,
  IN  UINT64                       Offset,
  IN  UINT64                       Mask,
  IN  UINT64                       Value,
  IN  UINT64                       Delay,
  OUT UINT64                       *Result
  )
{
  ASSERT (FALSE);
  return EFI_UNSUPPORTED;
}

EFI_STATUS
EhciPciIoMemRead (
  IN EFI_PCI_IO_PROTOCOL              *This,
  IN     EFI_PCI_IO_PROTOCOL_WIDTH    Width,
  IN     UINT8                        BarIndex,
  IN     UINT64                       Offset,
  IN     UINTN                        Count,
  IN OUT VOID                         *Buffer
  )
{
  UINT32 i;

  if ((UINT32)Width >= EfiPciIoWidthMaximum) {
    return EFI_INVALID_PARAMETER;
  }

  if (Buffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (BarIndex != 0) {
    return EFI_INVALID_PARAMETER;
  }

  Width     = Width & 0x03;

  //
  // Loop for each iteration and move the data
  //
  switch (Width) {
  case EfiPciWidthUint8:
    for (i = 0; i < Count; i++){
      *((UINT8 *)Buffer + i)= MmioRead8(mUsbMemBase + Offset + i);
    }
    break;
  case EfiPciWidthUint16:
    for (i = 0; i < Count; i++){
      *((UINT16 *)Buffer + i)= MmioRead16(mUsbMemBase + Offset + i * 2);
    }
    break;
  case EfiPciWidthUint32:
    for (i = 0; i < Count; i++){
      *((UINT32 *)Buffer + i)= MmioRead32(mUsbMemBase + Offset + i * 4);
    }
    break;
  case EfiPciWidthUint64:
    for (i = 0; i < Count; i++){
      *((UINT64 *)Buffer + i)= MmioRead64(mUsbMemBase + Offset + i * 8);
    }
    break;
  default:
    return EFI_INVALID_PARAMETER;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EhciPciIoMemWrite (
  IN EFI_PCI_IO_PROTOCOL              *This,
  IN     EFI_PCI_IO_PROTOCOL_WIDTH    Width,
  IN     UINT8                        BarIndex,
  IN     UINT64                       Offset,
  IN     UINTN                        Count,
  IN OUT VOID                         *Buffer
  )
{
  UINT32 i;

  if ((UINT32)Width >= EfiPciIoWidthMaximum) {
    return EFI_INVALID_PARAMETER;
  }

  if (Buffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Width     = Width & 0x03;

  //
  // Loop for each iteration and move the data
  //
  switch (Width) {
  case EfiPciWidthUint8:
    for (i = 0; i < Count; i++){
      MmioWrite8(mUsbMemBase + Offset + i, *((UINT8 *)Buffer + i));
    }
    break;
  case EfiPciWidthUint16:
    for (i = 0; i < Count; i++){
      MmioWrite16(mUsbMemBase + Offset + i * 2, *((UINT16 *)Buffer + i));
    }
    break;
  case EfiPciWidthUint32:
    for (i = 0; i < Count; i++){
      MmioWrite32(mUsbMemBase + Offset + i * 4, *((UINT32 *)Buffer + i));
    }
    break;
  case EfiPciWidthUint64:
    for (i = 0; i < Count; i++){
      MmioWrite64(mUsbMemBase + Offset + i * 8, *((UINT64 *)Buffer + i));
    }
    break;
  default:
    return EFI_INVALID_PARAMETER;
  }

  return EFI_SUCCESS;

}

EFI_STATUS
EhciPciIoIoRead (
  IN EFI_PCI_IO_PROTOCOL              *This,
  IN     EFI_PCI_IO_PROTOCOL_WIDTH    Width,
  IN     UINT8                        BarIndex,
  IN     UINT64                       Offset,
  IN     UINTN                        Count,
  IN OUT VOID                         *Buffer
  )
{
  ASSERT (FALSE);
  return EFI_UNSUPPORTED;
}

EFI_STATUS
EhciPciIoIoWrite (
  IN EFI_PCI_IO_PROTOCOL              *This,
  IN     EFI_PCI_IO_PROTOCOL_WIDTH    Width,
  IN     UINT8                        BarIndex,
  IN     UINT64                       Offset,
  IN     UINTN                        Count,
  IN OUT VOID                         *Buffer
  )
{
  ASSERT (FALSE);
  return EFI_UNSUPPORTED;
}

EFI_STATUS
EhciPciIoPciRead (
  IN     EFI_PCI_IO_PROTOCOL       *This,
  IN     EFI_PCI_IO_PROTOCOL_WIDTH  Width,
  IN     UINT32                     Offset,
  IN     UINTN                      Count,
  IN OUT VOID                      *Buffer
  )
{
  UINT32 i;
  UINT8 *DataPtr;

  Width     = Width & 0x03;

  if (Offset < sizeof (EHCI_PCI_CONFIG) / sizeof (UINT8)){

    DataPtr = (UINT8 *)(&mEhciPciConfig) + Offset;

    switch (Width) {
    case EfiPciWidthUint8:
      for (i = 0; i < Count; i++){
        *((UINT8 *)Buffer + i)= *(DataPtr + i);
      }
      break;
    case EfiPciWidthUint16:
      for (i = 0; i < Count; i++){
        *((UINT16 *)Buffer + i)= *((UINT16 *)DataPtr + i);
      }
      break;
    case EfiPciWidthUint32:
      for (i = 0; i < Count; i++){
        *(UINT32 *)(Buffer + i)= *((UINT32 *)DataPtr + i);
      }
      break;
    case EfiPciWidthUint64:
      for (i = 0; i < Count; i++){
        *(UINT64 *)(Buffer + i)= *((UINT64 *)DataPtr + i);
      }
      break;
    default:
      return EFI_INVALID_PARAMETER;
    }

  } else {
    switch (Width) {
    case EfiPciWidthUint8:
        *(UINT8 *)Buffer = 0xFF;
      break;
    case EfiPciWidthUint16:
        *(UINT16 *)Buffer = 0xFFFF;
      break;
    case EfiPciWidthUint32:
        *(UINT32 *)Buffer = 0xFFFFFFFF;
      break;
    case EfiPciWidthUint64:
        *(UINT64 *)Buffer = 0xFFFFFFFFFFFFFFFF;
      break;
    default:
      return EFI_INVALID_PARAMETER;
    }
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EhciPciIoPciWrite (
  IN EFI_PCI_IO_PROTOCOL              *This,
  IN     EFI_PCI_IO_PROTOCOL_WIDTH    Width,
  IN     UINT32                       Offset,
  IN     UINTN                        Count,
  IN OUT VOID                         *Buffer
  )
{

  return EFI_SUCCESS;
}

EFI_STATUS
EhciPciIoCopyMem (
  IN EFI_PCI_IO_PROTOCOL              *This,
  IN     EFI_PCI_IO_PROTOCOL_WIDTH    Width,
  IN     UINT8                        DestBarIndex,
  IN     UINT64                       DestOffset,
  IN     UINT8                        SrcBarIndex,
  IN     UINT64                       SrcOffset,
  IN     UINTN                        Count
  )
{
  ASSERT (FALSE);
  return EFI_UNSUPPORTED;
}

EFI_STATUS
EhciPciIoMap (
  IN EFI_PCI_IO_PROTOCOL                *This,
  IN     EFI_PCI_IO_PROTOCOL_OPERATION  Operation,
  IN     VOID                           *HostAddress,
  IN OUT UINTN                          *NumberOfBytes,
  OUT    EFI_PHYSICAL_ADDRESS           *DeviceAddress,
  OUT    VOID                           **Mapping
  )
{
  EFI_STATUS                      Status;
  MEM_MAP_INFO_INSTANCE           *Map;
  VOID                            *Buffer;
  EFI_GCD_MEMORY_SPACE_DESCRIPTOR GcdDescriptor;

  if (HostAddress == NULL || NumberOfBytes == NULL || DeviceAddress == NULL || Mapping == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if ((UINT32)Operation >= EfiPciIoOperationMaximum) {
    return EFI_INVALID_PARAMETER;
  }

  *DeviceAddress = ConvertToPhysicalAddress (HostAddress);

  // Remember range so we can flush on the other side
  Map = AllocatePool (sizeof (MEM_MAP_INFO_INSTANCE));
  if (Map == NULL) {
    return  EFI_OUT_OF_RESOURCES;
  }

  *Mapping = Map;

  if ((((UINTN)HostAddress & (EFI_PAGE_SIZE - 1)) != 0) ||
      ((*NumberOfBytes % EFI_PAGE_SIZE) != 0)) {

    // Get the cacheability of the region
    Status = gDS->GetMemorySpaceDescriptor (*DeviceAddress, &GcdDescriptor);
    if (EFI_ERROR(Status)) {
      return Status;
    }

    // If the mapped buffer is not an uncached buffer
    if ( (GcdDescriptor.Attributes != EFI_MEMORY_WC) &&
         (GcdDescriptor.Attributes != EFI_MEMORY_UC) )
    {
      //
      // If the buffer does not fill entire cache lines we must double buffer into
      // uncached memory. Device (PCI) address becomes uncached page.
      //
      Map->DoubleBuffer  = TRUE;
      Buffer = UncachedAllocatePages(EFI_SIZE_TO_PAGES (*NumberOfBytes));

      if (Buffer == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }

    CopyMem (Buffer,  HostAddress, *NumberOfBytes);
      *DeviceAddress = (EFI_PHYSICAL_ADDRESS)(UINTN)Buffer;
    } else {
      Map->DoubleBuffer  = FALSE;
    }
  } else {
    Map->DoubleBuffer  = FALSE;

    // Flush the Data Cache (should not have any effect if the memory region is uncached)
    gCpu->FlushDataCache (gCpu, *DeviceAddress, *NumberOfBytes, EfiCpuFlushTypeWriteBackInvalidate);

    Status = gDS->SetMemorySpaceAttributes (*DeviceAddress & ~(BASE_4KB - 1), ALIGN_VALUE (*NumberOfBytes, BASE_4KB), EFI_MEMORY_WC);
    if (EFI_ERROR (Status)) {
      DEBUG((EFI_D_ERROR, "[%a]:[%dL] SetMemorySpaceAttributes Fail. %r\n", __FUNCTION__, __LINE__, Status));
    }
  }

  Map->HostAddress   = (UINTN)HostAddress;
  Map->DeviceAddress = *DeviceAddress;
  Map->NumberOfBytes = *NumberOfBytes;
  Map->Operation     = Operation;

  return EFI_SUCCESS;
}

EFI_STATUS
EhciPciIoUnmap (
  IN EFI_PCI_IO_PROTOCOL           *This,
  IN  VOID                         *Mapping
  )
{
  MEM_MAP_INFO_INSTANCE *Map;

  if (Mapping == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Map = (MEM_MAP_INFO_INSTANCE *)Mapping;

  if (Map->DoubleBuffer) {
    if ((Map->Operation == EfiPciIoOperationBusMasterWrite) || (Map->Operation == EfiPciIoOperationBusMasterCommonBuffer)) {
      CopyMem ((VOID *)(UINTN)Map->HostAddress, (VOID *)(UINTN)Map->DeviceAddress, Map->NumberOfBytes);
    }

    if((VOID *)(UINTN)Map->DeviceAddress != NULL) {
      UncachedFreePages ((VOID *)(UINTN)Map->DeviceAddress, EFI_SIZE_TO_PAGES (Map->NumberOfBytes));
    }


  } else {
    if (Map->Operation == EfiPciIoOperationBusMasterWrite) {
      //
      // Make sure we read buffer from uncached memory and not the cache
      //
      gCpu->FlushDataCache (gCpu, Map->HostAddress, Map->NumberOfBytes, EfiCpuFlushTypeInvalidate);
    }
  }

  FreePool (Map);

  return EFI_SUCCESS;
}



EFI_STATUS
EhciPciIoAllocateBuffer (
  IN EFI_PCI_IO_PROTOCOL  *This,
  IN  EFI_ALLOCATE_TYPE   Type,
  IN  EFI_MEMORY_TYPE     MemoryType,
  IN  UINTN               Pages,
  OUT VOID                **HostAddress,
  IN  UINT64              Attributes
  )
{
  UINT32 HcCapParams;

  if (Attributes &
      (~(EFI_PCI_ATTRIBUTE_MEMORY_WRITE_COMBINE |
         EFI_PCI_ATTRIBUTE_MEMORY_CACHED         ))) {
    return EFI_UNSUPPORTED;
  }

  if (HostAddress == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (MemoryType == EfiBootServicesData) {
    HcCapParams  = MmioRead32(mUsbMemBase + EHC_HCCPARAMS_OFFSET);
    if ((BOOLEAN)(((HcCapParams) & (HCCP_64BIT)) == (HCCP_64BIT))){
      *HostAddress = UncachedAllocatePages(Pages);
    } else {
      // TODO: We need support allocating UC memory below 4GB strictly
      *HostAddress = UncachedAllocatePages(Pages);
    }

  }else{
    return EFI_INVALID_PARAMETER;
  }

  return EFI_SUCCESS;
}


EFI_STATUS
EhciPciIoFreeBuffer (
  IN EFI_PCI_IO_PROTOCOL           *This,
  IN  UINTN                        Pages,
  IN  VOID                         *HostAddress
  )
{
  UncachedFreePages (HostAddress, Pages);
  return EFI_SUCCESS;
}

EFI_STATUS
EhciPciIoFlush (
  IN EFI_PCI_IO_PROTOCOL  *This
  )
{
  return EFI_SUCCESS;
}

EFI_STATUS
EhciPciIoGetLocation (
  IN   EFI_PCI_IO_PROTOCOL  *This,
  OUT  UINTN                *SegmentNumber,
  OUT  UINTN                *BusNumber,
  OUT  UINTN                *DeviceNumber,
  OUT  UINTN                *FunctionNumber
  )
{

  *SegmentNumber  = mSegmentNumber;
  *BusNumber      = mBusNumber;
  *DeviceNumber   = mDeviceNumber;
  *FunctionNumber = mFunctionNumber;

  return EFI_SUCCESS;
}


EFI_STATUS
EhciPciIoAttributes (
  IN EFI_PCI_IO_PROTOCOL                       *This,
  IN  EFI_PCI_IO_PROTOCOL_ATTRIBUTE_OPERATION  Operation,
  IN  UINT64                                   Attributes,
  OUT UINT64                                   *Result OPTIONAL
  )
{
  if (Result != NULL) {
     *Result = 0;
  }
  return EFI_SUCCESS;
}

EFI_STATUS
EhciPciIoGetBarAttributes (
  IN EFI_PCI_IO_PROTOCOL             *This,
  IN  UINT8                          BarIndex,
  OUT UINT64                         *Supports, OPTIONAL
  OUT VOID                           **Resources OPTIONAL
  )
{
  ASSERT (FALSE);
  return EFI_UNSUPPORTED;
}

EFI_STATUS
EhciPciIoSetBarAttributes (
  IN EFI_PCI_IO_PROTOCOL              *This,
  IN     UINT64                       Attributes,
  IN     UINT8                        BarIndex,
  IN OUT UINT64                       *Offset,
  IN OUT UINT64                       *Length
  )
{
  ASSERT (FALSE);
  return EFI_UNSUPPORTED;
}

//
// Pci Io Protocol Interface
//
EFI_PCI_IO_PROTOCOL  mEhciPciIoInterface = {
  EhciPciIoPollMem,
  EhciPciIoPollIo,
  {
    EhciPciIoMemRead,
    EhciPciIoMemWrite
  },
  {
    EhciPciIoIoRead,
    EhciPciIoIoWrite
  },
  {
    EhciPciIoPciRead,
    EhciPciIoPciWrite
  },
  EhciPciIoCopyMem,
  EhciPciIoMap,
  EhciPciIoUnmap,
  EhciPciIoAllocateBuffer,
  EhciPciIoFreeBuffer,
  EhciPciIoFlush,
  EhciPciIoGetLocation,
  EhciPciIoAttributes,
  EhciPciIoGetBarAttributes,
  EhciPciIoSetBarAttributes,
  0,
  NULL
};


EFI_STATUS
EFIAPI
EhciVirtualPciIoInitialize (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS          Status;
  EFI_HANDLE                    Handle;
  EFI_DEV_PATH              EndNode;
  EFI_DEV_PATH              Node;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath = NULL;

  mUsbMemBase            = PlatformGetEhciBase ();

    DEBUG ((EFI_D_ERROR, "mUsbMemBase: 0x%x\n", mUsbMemBase));

  // Get the Cpu protocol for later use
  Status = gBS->LocateProtocol (&gEfiCpuArchProtocolGuid, NULL, (VOID **)&gCpu);

  //
  // Install the pciio protocol, device path protocol
  //
  Handle = NULL;

  Status = gBS->InstallMultipleProtocolInterfaces (
                  &Handle,
                  &gEfiPciIoProtocolGuid,
                  &mEhciPciIoInterface,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  (void)ZeroMem (&Node, sizeof (Node));
  Node.DevPath.Type = HARDWARE_DEVICE_PATH;
  Node.DevPath.SubType = HW_PCI_DP;
  (void)SetDevicePathNodeLength (&Node.DevPath, sizeof (PCI_DEVICE_PATH));
  // Make USB controller device path different from built-in SATA controller
  Node.Pci.Function = 1;
  Node.Pci.Device = 0;

  SetDevicePathEndNode (&EndNode.DevPath);

  DevicePath = AppendDevicePathNode (&EndNode.DevPath, &Node.DevPath);

  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gEfiDevicePathProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  DevicePath
                  );
  if(EFI_ERROR(Status))
  {
      DEBUG((EFI_D_ERROR, "[%a]:[%dL] InstallProtocolInterface fail. %r\n", __FUNCTION__, __LINE__, Status));
  }


  return EFI_SUCCESS;
}
