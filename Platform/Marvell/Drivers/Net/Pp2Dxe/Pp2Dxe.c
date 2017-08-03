/********************************************************************************
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

#include <Protocol/DevicePath.h>
#include <Protocol/DriverBinding.h>
#include <Protocol/SimpleNetwork.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/CacheMaintenanceLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/NetLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>

#include "Mvpp2LibHw.h"
#include "Mvpp2Lib.h"
#include "Pp2Dxe.h"

#define ReturnUnlock(tpl, status) do { gBS->RestoreTPL (tpl); return (status); } while(0)

STATIC MVPP2_SHARED *Mvpp2Shared;
STATIC BUFFER_LOCATION BufferLocation;
STATIC PP2_DEVICE_PATH Pp2DevicePathTemplate = {
  {
    {
      MESSAGING_DEVICE_PATH,
      MSG_MAC_ADDR_DP,
      {
        (UINT8) (sizeof(MAC_ADDR_DEVICE_PATH)),
        (UINT8) ((sizeof(MAC_ADDR_DEVICE_PATH)) >> 8)
      }
    },
    { { 0 } },
    0
  },
  {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    { sizeof(EFI_DEVICE_PATH_PROTOCOL), 0 }
  }
};

EFI_SIMPLE_NETWORK_PROTOCOL Pp2SnpTemplate = {
  EFI_SIMPLE_NETWORK_PROTOCOL_REVISION,                 // Revision
  Pp2SnpStart,                                          // Start
  Pp2SnpStop,                                           // Stop
  Pp2DxeSnpInitialize,                                  // Initialize
  Pp2SnpReset,                                          // Reset
  Pp2SnpShutdown,                                       // Shutdown
  Pp2SnpReceiveFilters,                                 // ReceiveFilters
  Pp2SnpStationAddress,                                 // StationAddress
  Pp2SnpNetStat,                                        // Statistics
  Pp2SnpIpToMac,                                        // MCastIpToMac
  NULL,                                                 // NvData
  Pp2SnpGetStatus,                                      // GetStatus
  Pp2SnpTransmit,                                       // Transmit
  Pp2SnpReceive,                                        // Receive
  NULL,                                                 // WaitForPacket
  NULL                                                  // Mode
};

EFI_SIMPLE_NETWORK_MODE Pp2SnpModeTemplate = {
  EfiSimpleNetworkStopped,                              // State
  NET_ETHER_ADDR_LEN,                                   // HwAddressSize
  sizeof (ETHER_HEAD),                                  // MediaHeaderSize
  EFI_PAGE_SIZE,                                        // MaxPacketSize
  0,                                                    // NvRamSize
  0,                                                    // MvRamAccessSize
  EFI_SIMPLE_NETWORK_RECEIVE_UNICAST |
  EFI_SIMPLE_NETWORK_RECEIVE_MULTICAST |
  EFI_SIMPLE_NETWORK_RECEIVE_BROADCAST |
  EFI_SIMPLE_NETWORK_RECEIVE_PROMISCUOUS |
  EFI_SIMPLE_NETWORK_RECEIVE_PROMISCUOUS_MULTICAST,     // ReceiveFilterMask
  EFI_SIMPLE_NETWORK_RECEIVE_UNICAST |
  EFI_SIMPLE_NETWORK_RECEIVE_MULTICAST |
  EFI_SIMPLE_NETWORK_RECEIVE_BROADCAST,                 // ReceiveFilterSetting
  MAX_MCAST_FILTER_CNT,                                 // MacMCastFilterCount
  0,                                                    // MCastFilterCount
  {
    { { 0 } }
  },                                                    // MCastFilter
  {
    { 0 }
  },                                                    // CurrentAddress
  {
    { 0 }
  },                                                    // BroadcastAddress
  {
    { 0 }
  },                                                    // Permanent Address
  NET_IFTYPE_ETHERNET,                                  // IfType
  TRUE,                                                 // MacAddressChangeable
  FALSE,                                                // MultipleTxSupported
  TRUE,                                                 // MediaPresentSupported
  FALSE                                                 // MediaPresent
};

#define QueueNext(off)  ((((off) + 1) >= QUEUE_DEPTH) ? 0 : ((off) + 1))

STATIC
EFI_STATUS
QueueInsert (
  IN PP2DXE_CONTEXT *Pp2Context,
  IN VOID *Buffer
  )
{

  if (QueueNext (Pp2Context->CompletionQueueTail) == Pp2Context->CompletionQueueHead) {
    return EFI_OUT_OF_RESOURCES;
  }

  Pp2Context->CompletionQueue[Pp2Context->CompletionQueueTail] = Buffer;
  Pp2Context->CompletionQueueTail = QueueNext (Pp2Context->CompletionQueueTail);

  return EFI_SUCCESS;
}

STATIC
VOID *
QueueRemove (
  IN PP2DXE_CONTEXT *Pp2Context
  )
{
  VOID *Buffer;

  if (Pp2Context->CompletionQueueTail == Pp2Context->CompletionQueueHead) {
    return NULL;
  }

  Buffer = Pp2Context->CompletionQueue[Pp2Context->CompletionQueueHead];
  Pp2Context->CompletionQueue[Pp2Context->CompletionQueueHead] = NULL;
  Pp2Context->CompletionQueueHead = QueueNext (Pp2Context->CompletionQueueHead);

  return Buffer;
}

STATIC
EFI_STATUS
Pp2DxeBmPoolInit (
  VOID
  )
{
  INTN Index;
  UINT8 *PoolAddr;
  UINT32 PoolSize = (sizeof(VOID *) * MVPP2_BM_SIZE) * 2 + MVPP2_BM_POOL_PTR_ALIGN;

  ASSERT(MVPP2_BM_POOL_PTR_ALIGN >= sizeof(UINTN));

  PoolSize = (sizeof(VOID *) * MVPP2_BM_SIZE) * 2 + MVPP2_BM_POOL_PTR_ALIGN;

  for (Index = 0; Index < MVPP2_BM_POOLS_NUM; Index++) {
    /* BmIrqClear */
    Mvpp2BmIrqClear(Mvpp2Shared, Index);
  }

  Mvpp2Shared->BmPools = AllocateZeroPool (sizeof(MVPP2_BMS_POOL));

  if (Mvpp2Shared->BmPools == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  PoolAddr = UncachedAllocateAlignedZeroPool (PoolSize, MVPP2_BM_POOL_PTR_ALIGN);
  if (PoolAddr == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Mvpp2Shared->BmPools->Id = MVPP2_BM_POOL;
  Mvpp2Shared->BmPools->VirtAddr = (UINT32 *)PoolAddr;
  Mvpp2Shared->BmPools->PhysAddr = (UINTN)PoolAddr;

  Mvpp2BmPoolHwCreate(Mvpp2Shared, Mvpp2Shared->BmPools, MVPP2_BM_SIZE);

  return EFI_SUCCESS;
}

/* Enable and fill BM pool */
STATIC
EFI_STATUS
Pp2DxeBmStart (
  VOID
  )
{
  UINT8 *Buff, *BuffPhys;
  INTN Index;

  ASSERT(BM_ALIGN >= sizeof(UINTN));

  Mvpp2BmPoolCtrl(Mvpp2Shared, MVPP2_BM_POOL, MVPP2_START);
  Mvpp2BmPoolBufsizeSet(Mvpp2Shared, Mvpp2Shared->BmPools, RX_BUFFER_SIZE);

  /* Fill BM pool with Buffers */
  for (Index = 0; Index < MVPP2_BM_SIZE; Index++) {
    Buff = (UINT8 *)(BufferLocation.RxBuffers + (Index * RX_BUFFER_SIZE));
    if (Buff == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    BuffPhys = ALIGN_POINTER(Buff, BM_ALIGN);
    Mvpp2BmPoolPut(Mvpp2Shared, MVPP2_BM_POOL, (UINTN)BuffPhys, (UINTN)BuffPhys);
  }

  return EFI_SUCCESS;
}

STATIC
VOID
Pp2DxeStartDev (
  IN PP2DXE_CONTEXT *Pp2Context
  )
{
  PP2DXE_PORT *Port = &Pp2Context->Port;

  /* Config classifier decoding table */
  Mvpp2ClsPortConfig(Port);
  Mvpp2ClsOversizeRxqSet(Port);
  MvGop110PortEventsMask(Port);
  MvGop110PortEnable(Port);

  /* Enable transmit and receive */
  Mvpp2EgressEnable(Port);
  Mvpp2IngressEnable(Port);
}

STATIC
EFI_STATUS
Pp2DxeSetupRxqs (
  IN PP2DXE_CONTEXT *Pp2Context
  )
{
  INTN Queue;
  EFI_STATUS Status;
  MVPP2_RX_QUEUE *Rxq;

  for (Queue = 0; Queue < RxqNumber; Queue++) {
    Rxq = &Pp2Context->Port.Rxqs[Queue];
    Rxq->DescsPhys = (DmaAddrT)Rxq->Descs;
    if (Rxq->Descs == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto ErrCleanup;
    }

    Mvpp2RxqHwInit(&Pp2Context->Port, Rxq);
  }

  return EFI_SUCCESS;

ErrCleanup:
  Mvpp2CleanupRxqs(&Pp2Context->Port);
  return Status;
}

STATIC
EFI_STATUS
Pp2DxeSetupTxqs (
  IN PP2DXE_CONTEXT *Pp2Context
  )
{
  INTN Queue;
  MVPP2_TX_QUEUE *Txq;
  EFI_STATUS Status;

  for (Queue = 0; Queue < TxqNumber; Queue++) {
    Txq = &Pp2Context->Port.Txqs[Queue];
    Txq->DescsPhys = (DmaAddrT)Txq->Descs;
    if (Txq->Descs == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto ErrCleanup;
    }

    Mvpp2TxqHwInit(&Pp2Context->Port, Txq);
  }

  return EFI_SUCCESS;

ErrCleanup:
  Mvpp2CleanupTxqs(&Pp2Context->Port);
  return Status;
}

STATIC
EFI_STATUS
Pp2DxeSetupAggrTxqs (
  IN PP2DXE_CONTEXT *Pp2Context
  )
{
  MVPP2_TX_QUEUE *AggrTxq;

  AggrTxq = Mvpp2Shared->AggrTxqs;
  AggrTxq->DescsPhys = (DmaAddrT)AggrTxq->Descs;
  if (AggrTxq->Descs == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Mvpp2AggrTxqHwInit(AggrTxq, AggrTxq->Size, 0, Mvpp2Shared);

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
Pp2DxeOpen (
  IN PP2DXE_CONTEXT *Pp2Context
  )
{
  PP2DXE_PORT *Port = &Pp2Context->Port;
  UINT8 MacBcast[NET_ETHER_ADDR_LEN] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
  UINT8 DevAddr[NET_ETHER_ADDR_LEN];
  INTN Ret;
  EFI_STATUS Status;

  CopyMem (DevAddr, Pp2Context->Snp.Mode->CurrentAddress.Addr, NET_ETHER_ADDR_LEN);

  Ret = Mvpp2PrsMacDaAccept(Mvpp2Shared, Port->Id, MacBcast, TRUE);
  if (Ret != 0) {
    return EFI_DEVICE_ERROR;
  }
  Ret = Mvpp2PrsMacDaAccept(Mvpp2Shared, Port->Id, DevAddr, TRUE);
  if (Ret != 0) {
    return EFI_DEVICE_ERROR;
  }
  Ret = Mvpp2PrsTagModeSet(Mvpp2Shared, Port->Id, MVPP2_TAG_TYPE_MH);
  if (Ret != 0) {
    return EFI_DEVICE_ERROR;
  }
  Ret = Mvpp2PrsDefFlow(Port);
  if (Ret != 0) {
    return EFI_DEVICE_ERROR;
  }

  Status = Pp2DxeSetupRxqs(Pp2Context);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = Pp2DxeSetupTxqs(Pp2Context);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = Pp2DxeSetupAggrTxqs(Pp2Context);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Pp2DxeStartDev(Pp2Context);

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
Pp2DxeLatePortInitialize (
  IN PP2DXE_CONTEXT *Pp2Context
  )
{
  PP2DXE_PORT *Port = &Pp2Context->Port;
  INTN Queue;

  Port->TxRingSize = MVPP2_MAX_TXD;
  Port->RxRingSize = MVPP2_MAX_RXD;

  Mvpp2EgressDisable(Port);
  MvGop110PortEventsMask(Port);
  MvGop110PortDisable(Port);

  Port->Txqs = AllocateZeroPool (sizeof(MVPP2_TX_QUEUE) * TxqNumber);
  if (Port->Txqs == NULL) {
    DEBUG((DEBUG_ERROR, "Failed to allocate Txqs\n"));
    return EFI_OUT_OF_RESOURCES;
  }

  /* Use preallocated area */
  Port->Txqs[0].Descs = BufferLocation.TxDescs;

  for (Queue = 0; Queue < TxqNumber; Queue++) {
    MVPP2_TX_QUEUE *Txq = &Port->Txqs[Queue];

    Txq->Id = Mvpp2TxqPhys(Port->Id, Queue);
    Txq->LogId = Queue;
    Txq->Size = Port->TxRingSize;
  }

  Port->Rxqs = AllocateZeroPool (sizeof(MVPP2_RX_QUEUE) * RxqNumber);
  if (Port->Rxqs == NULL) {
    DEBUG((DEBUG_ERROR, "Failed to allocate Rxqs\n"));
    return EFI_OUT_OF_RESOURCES;
  }

  Port->Rxqs[0].Descs = BufferLocation.RxDescs;

  for (Queue = 0; Queue < TxqNumber; Queue++) {
    MVPP2_RX_QUEUE *Rxq = &Port->Rxqs[Queue];

    Rxq->Id = Queue + Port->FirstRxq;
    Rxq->Size = Port->RxRingSize;
  }

  Mvpp2IngressDisable(Port);

  Mvpp2DefaultsSet(Port);

  return Pp2DxeOpen(Pp2Context);
}

STATIC
EFI_STATUS
Pp2DxeLateInitialize (
  IN PP2DXE_CONTEXT *Pp2Context
  )
{
  PP2DXE_PORT *Port = &Pp2Context->Port;
  EFI_STATUS Status;

  if (!Pp2Context->LateInitialized) {
    /* Full init on first call */
    Status = Pp2DxeLatePortInitialize(Pp2Context);
    if (EFI_ERROR(Status)) {
      DEBUG((DEBUG_ERROR, "Pp2Dxe: late initialization failed\n"));
      return Status;
    }

    /* Attach pool to Rxq */
    Mvpp2RxqLongPoolSet(Port, 0, MVPP2_BM_POOL);
    Mvpp2RxqShortPoolSet(Port, 0, MVPP2_BM_POOL);

    /*
     * Mark this port being fully initialized,
     * otherwise it will be inited again
     * during next networking transaction,
     * including memory allocatation for
     * TX/RX queue, PHY connect/configuration
     * and address decode configuration.
     */
    Pp2Context->LateInitialized = TRUE;
  } else {
    /* Upon all following calls, this is enough */
    MvGop110PortEventsMask(Port);
    MvGop110PortEnable(Port);
  }
  return 0;
}

EFI_STATUS
Pp2DxePhyInitialize (
  PP2DXE_CONTEXT *Pp2Context
  )
{
  EFI_STATUS Status;
  UINT8 *PhyAddresses;

  PhyAddresses = PcdGetPtr (PcdPhySmiAddresses);
  Status = gBS->LocateProtocol (
               &gMarvellPhyProtocolGuid,
               NULL,
               (VOID **) &Pp2Context->Phy
             );

  if (EFI_ERROR(Status)) {
    return Status;
  }

  if (PhyAddresses[Pp2Context->Instance] == 0xff) {
    /* PHY iniitalization not required */
    return EFI_SUCCESS;
  }

  Status = Pp2Context->Phy->Init(
               Pp2Context->Phy,
               PhyAddresses[Pp2Context->Instance],
               Pp2Context->Port.PhyInterface,
               &Pp2Context->PhyDev
             );

  if (EFI_ERROR(Status) && Status != EFI_TIMEOUT) {
    return Status;
  }

  Pp2Context->Phy->Status(Pp2Context->Phy, Pp2Context->PhyDev);
  Mvpp2SmiPhyAddrCfg(&Pp2Context->Port, Pp2Context->Port.GopIndex, Pp2Context->PhyDev->Addr);

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
Pp2DxeSnpInitialize (
  IN EFI_SIMPLE_NETWORK_PROTOCOL *This,
  IN UINTN                       ExtraRxBufferSize  OPTIONAL,
  IN UINTN                       ExtraTxBufferSize  OPTIONAL
  )
{
  EFI_STATUS Status;
  PP2DXE_CONTEXT *Pp2Context;
  Pp2Context = INSTANCE_FROM_SNP(This);
  UINT32 State = This->Mode->State;
  EFI_TPL SavedTpl;

  if (ExtraRxBufferSize != 0 || ExtraTxBufferSize != 0) {
    DEBUG((DEBUG_ERROR, "Pp2Dxe%d: non-zero buffer requests\n", Pp2Context->Instance));
    return EFI_UNSUPPORTED;
  }

  SavedTpl = gBS->RaiseTPL (TPL_CALLBACK);

  if (State != EfiSimpleNetworkStarted) {
    switch (State) {
    case EfiSimpleNetworkInitialized:
      DEBUG((DEBUG_WARN, "Pp2Dxe%d: already initialized\n", Pp2Context->Instance));
      ReturnUnlock (SavedTpl, EFI_SUCCESS);
    case EfiSimpleNetworkStopped:
      DEBUG((DEBUG_WARN, "Pp2Dxe%d: network stopped\n", Pp2Context->Instance));
      ReturnUnlock (SavedTpl, EFI_NOT_STARTED);
    default:
      DEBUG((DEBUG_ERROR, "Pp2Dxe%d: wrong state\n", Pp2Context->Instance));
      ReturnUnlock (SavedTpl, EFI_DEVICE_ERROR);
    }
  }

  /* Successfully started, change state to Initialized */
  This->Mode->State = EfiSimpleNetworkInitialized;

  if (Pp2Context->Initialized) {
    ReturnUnlock(SavedTpl, EFI_SUCCESS);
  }

  Pp2Context->Initialized = TRUE;

  Status = Pp2DxePhyInitialize(Pp2Context);
  if (EFI_ERROR(Status)) {
    ReturnUnlock (SavedTpl, Status);
  }

  Status = Pp2DxeLateInitialize(Pp2Context);
  ReturnUnlock (SavedTpl, Status);
}

EFI_STATUS
EFIAPI
Pp2SnpStart (
  IN EFI_SIMPLE_NETWORK_PROTOCOL  *This
  )
{
  PP2DXE_CONTEXT *Pp2Context;
  UINT32 State = This->Mode->State;
  EFI_TPL SavedTpl;

  SavedTpl = gBS->RaiseTPL (TPL_CALLBACK);
  Pp2Context = INSTANCE_FROM_SNP(This);

  if (State != EfiSimpleNetworkStopped) {
    switch (State) {
    case EfiSimpleNetworkStarted:
    case EfiSimpleNetworkInitialized:
      DEBUG((DEBUG_WARN, "Pp2Dxe%d: already initialized\n", Pp2Context->Instance));
      ReturnUnlock (SavedTpl, EFI_ALREADY_STARTED);
    default:
      DEBUG((DEBUG_ERROR, "Pp2Dxe%d: wrong state\n", Pp2Context->Instance));
      ReturnUnlock (SavedTpl, EFI_DEVICE_ERROR);
    }
  }

  This->Mode->State = EfiSimpleNetworkStarted;
  ReturnUnlock (SavedTpl, EFI_SUCCESS);
}

EFI_STATUS
EFIAPI
Pp2SnpStop (
  IN EFI_SIMPLE_NETWORK_PROTOCOL  *This
  )
{
  EFI_TPL SavedTpl;
  SavedTpl = gBS->RaiseTPL (TPL_CALLBACK);
  PP2DXE_CONTEXT *Pp2Context = INSTANCE_FROM_SNP(This);
  UINT32 State = This->Mode->State;

  if (State != EfiSimpleNetworkStarted && State != EfiSimpleNetworkInitialized) {
    switch (State) {
    case EfiSimpleNetworkStopped:
      DEBUG((DEBUG_WARN, "Pp2Dxe%d: not started\n", Pp2Context->Instance));
      ReturnUnlock (SavedTpl, EFI_NOT_STARTED);
    default:
      DEBUG((DEBUG_ERROR, "Pp2Dxe%d: wrong state\n", Pp2Context->Instance));
      ReturnUnlock (SavedTpl, EFI_DEVICE_ERROR);
    }
  }

  This->Mode->State = EfiSimpleNetworkStopped;
  ReturnUnlock (SavedTpl, EFI_SUCCESS);
}

EFI_STATUS
EFIAPI
Pp2SnpReset (
  IN EFI_SIMPLE_NETWORK_PROTOCOL *This,
  IN BOOLEAN                     ExtendedVerification
  )
{
  return EFI_SUCCESS;
}

VOID
EFIAPI
Pp2DxeHalt (
  IN EFI_EVENT Event,
  IN VOID *Context
  )
{
  PP2DXE_CONTEXT *Pp2Context = Context;
  PP2DXE_PORT *Port = &Pp2Context->Port;
  STATIC BOOLEAN CommonPartHalted = FALSE;

  if (!CommonPartHalted) {
    Mvpp2BmStop(Mvpp2Shared, MVPP2_BM_POOL);
    CommonPartHalted = TRUE;
  }

  Mvpp2TxqDrainSet(Port, 0, TRUE);
  Mvpp2IngressDisable(Port);
  Mvpp2EgressDisable(Port);

  MvGop110PortEventsMask(Port);
  MvGop110PortDisable(Port);
}

EFI_STATUS
EFIAPI
Pp2SnpShutdown (
  IN EFI_SIMPLE_NETWORK_PROTOCOL *This
  )
{
  EFI_TPL SavedTpl;
  SavedTpl = gBS->RaiseTPL (TPL_CALLBACK);
  PP2DXE_CONTEXT *Pp2Context = INSTANCE_FROM_SNP(This);
  UINT32 State = This->Mode->State;

  if (State != EfiSimpleNetworkInitialized) {
    switch (State) {
    case EfiSimpleNetworkStopped:
      DEBUG((DEBUG_WARN, "Pp2Dxe%d: not started\n", Pp2Context->Instance));
      ReturnUnlock (SavedTpl, EFI_NOT_STARTED);
    case EfiSimpleNetworkStarted:
    /* Fall through */
    default:
      DEBUG((DEBUG_ERROR, "Pp2Dxe%d: wrong state\n", Pp2Context->Instance));
      ReturnUnlock (SavedTpl, EFI_DEVICE_ERROR);
    }
  }

  ReturnUnlock (SavedTpl, EFI_SUCCESS);
}

EFI_STATUS
EFIAPI
Pp2SnpReceiveFilters (
  IN EFI_SIMPLE_NETWORK_PROTOCOL *This,
  IN UINT32                      Enable,
  IN UINT32                      Disable,
  IN BOOLEAN                     ResetMCastFilter,
  IN UINTN                       MCastFilterCnt     OPTIONAL,
  IN EFI_MAC_ADDRESS             *MCastFilter OPTIONAL
  )
{
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
Pp2SnpStationAddress (
  IN EFI_SIMPLE_NETWORK_PROTOCOL *Snp,
  IN BOOLEAN Reset,
  IN EFI_MAC_ADDRESS *NewMac
)
{
  PP2DXE_CONTEXT *Pp2Context = INSTANCE_FROM_SNP(Snp);
  PP2_DEVICE_PATH *Pp2DevicePath = Pp2Context->DevicePath;
  PP2DXE_PORT *Port = &Pp2Context->Port;
  UINT32 State = Snp->Mode->State;
  EFI_TPL SavedTpl;
  INTN Ret;

  /* Check Snp instance */
  ASSERT(Snp != NULL);

  /* Serialize access to data and registers */
  SavedTpl = gBS->RaiseTPL (TPL_CALLBACK);

  /* Check that driver was started and initialised */
  if (State != EfiSimpleNetworkInitialized) {
    switch (State) {
    case EfiSimpleNetworkStopped:
      DEBUG((DEBUG_WARN, "Pp2Dxe%d: not started\n", Pp2Context->Instance));
      ReturnUnlock (SavedTpl, EFI_NOT_STARTED);
    case EfiSimpleNetworkStarted:
    /* Fall through */
    default:
      DEBUG((DEBUG_ERROR, "Pp2Dxe%d: wrong state\n", Pp2Context->Instance));
      ReturnUnlock (SavedTpl, EFI_DEVICE_ERROR);
    }
  }

  /* Invalidate old unicast address in parser */
  Ret = Mvpp2PrsMacDaAccept(Mvpp2Shared, Port->Id, Snp->Mode->CurrentAddress.Addr, FALSE);
  if (Ret != 0) {
    DEBUG((DEBUG_ERROR, "Pp2SnpStationAddress - Fail\n"));
    return EFI_DEVICE_ERROR;
  }

  if (Reset) {
    CopyMem (Snp->Mode->CurrentAddress.Addr, Snp->Mode->PermanentAddress.Addr, NET_ETHER_ADDR_LEN);
    CopyMem (NewMac->Addr, Snp->Mode->PermanentAddress.Addr, NET_ETHER_ADDR_LEN);
    CopyMem (Pp2DevicePath->Pp2Mac.MacAddress.Addr, Snp->Mode->PermanentAddress.Addr, NET_ETHER_ADDR_LEN);
  } else {
    if (NewMac == NULL) {
      ReturnUnlock (SavedTpl, EFI_INVALID_PARAMETER);
    }
    CopyMem (Snp->Mode->CurrentAddress.Addr, NewMac->Addr, NET_ETHER_ADDR_LEN);
    CopyMem (Pp2DevicePath->Pp2Mac.MacAddress.Addr, NewMac->Addr, NET_ETHER_ADDR_LEN);
  }

  /* Update parser with new unicast address */
  Ret = Mvpp2PrsMacDaAccept(Mvpp2Shared, Port->Id, Snp->Mode->CurrentAddress.Addr, TRUE);
  if (Ret != 0) {
    DEBUG((DEBUG_ERROR, "Pp2SnpStationAddress - Fail\n"));
    return EFI_DEVICE_ERROR;
  }

  /* Restore TPL and return */
  gBS->RestoreTPL (SavedTpl);

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
Pp2SnpNetStat (
  IN EFI_SIMPLE_NETWORK_PROTOCOL *This,
  IN BOOLEAN                     Reset,
  IN OUT UINTN                   *StatisticsSize   OPTIONAL,
  OUT EFI_NETWORK_STATISTICS     *StatisticsTable  OPTIONAL
  )
{
  return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
Pp2SnpIpToMac (
  IN EFI_SIMPLE_NETWORK_PROTOCOL *This,
  IN BOOLEAN                     IPv6,
  IN EFI_IP_ADDRESS              *IP,
  OUT EFI_MAC_ADDRESS            *MAC
  )
{
  return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
Pp2SnpNvData (
  IN EFI_SIMPLE_NETWORK_PROTOCOL *This,
  IN BOOLEAN                     ReadWrite,
  IN UINTN                       Offset,
  IN UINTN                       BufferSize,
  IN OUT VOID                    *Buffer
  )
{
  return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
Pp2SnpGetStatus (
  IN EFI_SIMPLE_NETWORK_PROTOCOL *Snp,
  OUT UINT32                     *InterruptStatus OPTIONAL,
  OUT VOID                       **TxBuf OPTIONAL
  )
{
  PP2DXE_CONTEXT *Pp2Context = INSTANCE_FROM_SNP(Snp);
  PP2DXE_PORT *Port = &Pp2Context->Port;
  BOOLEAN LinkUp;
  EFI_TPL SavedTpl;

  SavedTpl = gBS->RaiseTPL (TPL_CALLBACK);

  if (!Pp2Context->Initialized)
    ReturnUnlock(SavedTpl, EFI_NOT_READY);

  LinkUp = Port->AlwaysUp ? TRUE : MvGop110PortIsLinkUp(Port);

  if (LinkUp != Snp->Mode->MediaPresent) {
    DEBUG((DEBUG_INFO, "Pp2Dxe%d: Link ", Pp2Context->Instance));
    DEBUG((DEBUG_INFO, LinkUp ? "up\n" : "down\n"));
  }
  Snp->Mode->MediaPresent = LinkUp;

  if (TxBuf != NULL) {
    *TxBuf = QueueRemove (Pp2Context);
  }

  ReturnUnlock(SavedTpl, EFI_SUCCESS);
}

EFI_STATUS
EFIAPI
Pp2SnpTransmit (
  IN EFI_SIMPLE_NETWORK_PROTOCOL *This,
  IN UINTN                       HeaderSize,
  IN UINTN                       BufferSize,
  IN VOID                        *Buffer,
  IN EFI_MAC_ADDRESS             *SrcAddr  OPTIONAL,
  IN EFI_MAC_ADDRESS             *DestAddr OPTIONAL,
  IN UINT16                      *EtherTypePtr OPTIONAL
  )
{
  PP2DXE_CONTEXT *Pp2Context = INSTANCE_FROM_SNP(This);
  PP2DXE_PORT *Port = &Pp2Context->Port;
  MVPP2_TX_QUEUE *AggrTxq = Mvpp2Shared->AggrTxqs;
  MVPP2_TX_DESC *TxDesc;
  EFI_STATUS Status;
  INTN PollingCount;
  INTN TxSent;
  UINT8 *DataPtr = Buffer;
  UINT16 EtherType;
  UINT32 State = This->Mode->State;
  EFI_TPL SavedTpl;

  if (This == NULL || Buffer == NULL) {
    DEBUG((DEBUG_ERROR, "Pp2Dxe: NULL Snp or Buffer\n"));
    return EFI_INVALID_PARAMETER;
  }

  if (HeaderSize != 0) {
    ASSERT (HeaderSize == This->Mode->MediaHeaderSize);
    ASSERT (EtherTypePtr != NULL);
    ASSERT (DestAddr != NULL);
  }

  SavedTpl = gBS->RaiseTPL (TPL_CALLBACK);

  /* Check that driver was started and initialised */
  if (State != EfiSimpleNetworkInitialized) {
    switch (State) {
    case EfiSimpleNetworkStopped:
      DEBUG((DEBUG_WARN, "Pp2Dxe%d: not started\n", Pp2Context->Instance));
      ReturnUnlock (SavedTpl, EFI_NOT_STARTED);
    case EfiSimpleNetworkStarted:
    /* Fall through */
    default:
      DEBUG((DEBUG_ERROR, "Pp2Dxe%d: wrong state\n", Pp2Context->Instance));
      ReturnUnlock (SavedTpl, EFI_DEVICE_ERROR);
    }
  }

  if (!This->Mode->MediaPresent) {
    DEBUG((DEBUG_ERROR, "Pp2Dxe: link not ready\n"));
    ReturnUnlock(SavedTpl, EFI_NOT_READY);
  }

  EtherType = HTONS (*EtherTypePtr);

  /* Fetch next descriptor */
  TxDesc = Mvpp2TxqNextDescGet(AggrTxq);

  if (!TxDesc) {
    DEBUG((DEBUG_ERROR, "No tx descriptor to use\n"));
    ReturnUnlock(SavedTpl, EFI_OUT_OF_RESOURCES);
  }

  if (HeaderSize != 0) {
    CopyMem(DataPtr, DestAddr, NET_ETHER_ADDR_LEN);

    if (SrcAddr != NULL)
      CopyMem(DataPtr + NET_ETHER_ADDR_LEN, SrcAddr, NET_ETHER_ADDR_LEN);
    else
      CopyMem(DataPtr + NET_ETHER_ADDR_LEN, &This->Mode->CurrentAddress, NET_ETHER_ADDR_LEN);

    CopyMem(DataPtr + NET_ETHER_ADDR_LEN * 2, &EtherType, 2);
  }

  /* Set descriptor fields */
  TxDesc->command =  MVPP2_TXD_IP_CSUM_DISABLE | MVPP2_TXD_L4_CSUM_NOT |
                     MVPP2_TXD_F_DESC | MVPP2_TXD_L_DESC;
  TxDesc->DataSize = BufferSize;
  TxDesc->PacketOffset = (PhysAddrT)DataPtr & MVPP2_TX_DESC_ALIGN;
  Mvpp2x2TxdescPhysAddrSet((PhysAddrT)DataPtr & ~MVPP2_TX_DESC_ALIGN, TxDesc);
  TxDesc->PhysTxq = Mvpp2TxqPhys(Port->Id, 0);

  InvalidateDataCacheRange (DataPtr, BufferSize);

  /* Issue send */
  Mvpp2AggrTxqPendDescAdd(Port, 1);

  /*
   * Egress processing:
   * Wait until packet is passed from per-cpu aggregated queue
   * to physical per-port TXQ.
   */
  PollingCount = 0;
  TxSent = Mvpp2AggrTxqPendDescNumGet(Mvpp2Shared, 0);
  do {
    if (PollingCount++ > MVPP2_TX_SEND_MAX_POLLING_COUNT) {
      DEBUG((DEBUG_ERROR, "Pp2Dxe: transmit polling failed\n"));
      ReturnUnlock(SavedTpl, EFI_TIMEOUT);
    }
    TxSent = Mvpp2AggrTxqPendDescNumGet(Mvpp2Shared, 0);
  } while (TxSent);

  /* Wait for packet to be transmitted by hardware. */
  PollingCount = 0;
  TxSent = Mvpp2TxqSentDescProc(Port, &Port->Txqs[0]);
  while (!TxSent) {
    if (PollingCount++ > MVPP2_TX_SEND_MAX_POLLING_COUNT) {
      DEBUG((DEBUG_ERROR, "Pp2Dxe: transmit polling failed\n"));
      ReturnUnlock(SavedTpl, EFI_TIMEOUT);
    }
    TxSent = Mvpp2TxqSentDescProc(Port, &Port->Txqs[0]);
  }

  /*
   * At this point TxSent has increased - HW sent the packet
   * Add buffer to completion queue and return.
   */
  Status = QueueInsert (Pp2Context, Buffer);
  ReturnUnlock (SavedTpl, Status);
}

EFI_STATUS
EFIAPI
Pp2SnpReceive (
  IN EFI_SIMPLE_NETWORK_PROTOCOL *This,
  OUT UINTN                      *HeaderSize OPTIONAL,
  IN OUT UINTN                   *BufferSize,
  OUT VOID                       *Buffer,
  OUT EFI_MAC_ADDRESS            *SrcAddr OPTIONAL,
  OUT EFI_MAC_ADDRESS            *DstAddr OPTIONAL,
  OUT UINT16                     *EtherType OPTIONAL
  )
{
  INTN ReceivedPackets;
  PP2DXE_CONTEXT *Pp2Context = INSTANCE_FROM_SNP(This);
  PP2DXE_PORT *Port = &Pp2Context->Port;
  UINTN PhysAddr, VirtAddr;
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_TPL SavedTpl;
  UINT32 StatusReg;
  INTN PoolId;
  UINTN PktLength;
  UINT8 *DataPtr;
  MVPP2_RX_DESC *RxDesc;
  MVPP2_RX_QUEUE *Rxq = &Port->Rxqs[0];

  ASSERT (Port != NULL);
  ASSERT (Rxq != NULL);

  SavedTpl = gBS->RaiseTPL (TPL_CALLBACK);
  ReceivedPackets = Mvpp2RxqReceived(Port, Rxq->Id);

  if (ReceivedPackets == 0) {
    ReturnUnlock(SavedTpl, EFI_NOT_READY);
  }

  /* Process one packet per call */
  RxDesc = Mvpp2RxqNextDescGet(Rxq);
  StatusReg = RxDesc->status;

  /* extract addresses from descriptor */
  PhysAddr = RxDesc->BufPhysAddrKeyHash & MVPP22_ADDR_MASK;
  VirtAddr = RxDesc->BufCookieBmQsetClsInfo & MVPP22_ADDR_MASK;

  /* Drop packets with error or with buffer header (MC, SG) */
  if ((StatusReg & MVPP2_RXD_BUF_HDR) || (StatusReg & MVPP2_RXD_ERR_SUMMARY)) {
    DEBUG((DEBUG_WARN, "Pp2Dxe: dropping packet\n"));
    Status = EFI_DEVICE_ERROR;
    goto drop;
  }

  PktLength = (UINTN) RxDesc->DataSize - 2;
  if (PktLength > *BufferSize) {
    *BufferSize = PktLength;
    DEBUG((DEBUG_ERROR, "Pp2Dxe: buffer too small\n"));
    ReturnUnlock(SavedTpl, EFI_BUFFER_TOO_SMALL);
  }

  CopyMem (Buffer, (VOID*) (PhysAddr + 2), PktLength);
  *BufferSize = PktLength;

  if (HeaderSize != NULL) {
    *HeaderSize = Pp2Context->Snp.Mode->MediaHeaderSize;
  }

  DataPtr = Buffer;

  /* Extract the destination address */
  if (DstAddr != NULL) {
    ZeroMem (DstAddr, sizeof(EFI_MAC_ADDRESS));
    CopyMem (DstAddr, &DataPtr[0], NET_ETHER_ADDR_LEN);
  }

  /* Get the source address */
  if (SrcAddr != NULL) {
    ZeroMem (SrcAddr, sizeof(EFI_MAC_ADDRESS));
    CopyMem (SrcAddr, &DataPtr[6], NET_ETHER_ADDR_LEN);
  }

  /* Obtain Ether Type */
  if (EtherType != NULL) {
    *EtherType = NTOHS (*(UINT16 *)(&DataPtr[12]));
  }

drop:
  /* Refill: pass packet back to BM */
  PoolId = (StatusReg & MVPP2_RXD_BM_POOL_ID_MASK) >> MVPP2_RXD_BM_POOL_ID_OFFS;
  Mvpp2BmPoolPut(Mvpp2Shared, PoolId, PhysAddr, VirtAddr);

  /* Update counters with 1 packet received and 1 packet refilled */
  Mvpp2RxqStatusUpdate(Port, Rxq->Id, 1, 1);

  ReturnUnlock(SavedTpl, Status);
}

EFI_STATUS
Pp2DxeSnpInstall (
  IN PP2DXE_CONTEXT *Pp2Context
  )
{
  EFI_HANDLE Handle = NULL;
  EFI_STATUS Status;
  PP2_DEVICE_PATH *Pp2DevicePath;
  EFI_SIMPLE_NETWORK_MODE *SnpMode;

  Pp2DevicePath = AllocateCopyPool (sizeof (PP2_DEVICE_PATH), &Pp2DevicePathTemplate);
  if (Pp2DevicePath == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  SnpMode = AllocateZeroPool (sizeof (EFI_SIMPLE_NETWORK_MODE));
  if (SnpMode == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  /* Copy SNP data from templates */
  CopyMem (&Pp2Context->Snp, &Pp2SnpTemplate, sizeof (EFI_SIMPLE_NETWORK_PROTOCOL));
  CopyMem (SnpMode, &Pp2SnpModeTemplate, sizeof (EFI_SIMPLE_NETWORK_MODE));

  /* Handle device path of the controller */
  Pp2DevicePath->Pp2Mac.MacAddress.Addr[5] = Pp2Context->Instance + 1;
  Pp2Context->Signature = PP2DXE_SIGNATURE;
  Pp2Context->DevicePath = Pp2DevicePath;
  Pp2DevicePath->Pp2Mac.IfType = SnpMode->IfType;

  /* Update SNP Mode */
  CopyMem (SnpMode->CurrentAddress.Addr, Pp2DevicePath->Pp2Mac.MacAddress.Addr, NET_ETHER_ADDR_LEN);
  CopyMem (SnpMode->PermanentAddress.Addr, Pp2DevicePath->Pp2Mac.MacAddress.Addr, NET_ETHER_ADDR_LEN);
  ZeroMem (&SnpMode->MCastFilter, MAX_MCAST_FILTER_CNT * sizeof(EFI_MAC_ADDRESS));
  SetMem (&SnpMode->BroadcastAddress, sizeof (EFI_MAC_ADDRESS), 0xFF);

  Pp2Context->Snp.Mode = SnpMode;

  /* Install protocol */
  Status = gBS->InstallMultipleProtocolInterfaces (
      &Handle,
      &gEfiSimpleNetworkProtocolGuid, &Pp2Context->Snp,
      &gEfiDevicePathProtocolGuid, Pp2DevicePath,
      NULL
      );

  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, "Failed to install protocols.\n"));
  }

  return Status;
}

STATIC
VOID
Pp2DxeParsePortPcd (
  IN PP2DXE_CONTEXT *Pp2Context
  )
{
  UINT8 *PortIds, *GopIndexes, *PhyConnectionTypes, *AlwaysUp, *Speed;

  PortIds = PcdGetPtr (PcdPp2PortIds);
  GopIndexes = PcdGetPtr (PcdPp2GopIndexes);
  PhyConnectionTypes = PcdGetPtr (PcdPhyConnectionTypes);
  AlwaysUp = PcdGetPtr (PcdPp2InterfaceAlwaysUp);
  Speed = PcdGetPtr (PcdPp2InterfaceSpeed);

  ASSERT (PcdGetSize (PcdPp2GopIndexes) == PcdGetSize (PcdPp2PortIds));
  ASSERT (PcdGetSize (PcdPhyConnectionTypes) == PcdGetSize (PcdPp2PortIds));
  ASSERT (PcdGetSize (PcdPp2InterfaceAlwaysUp) == PcdGetSize (PcdPp2PortIds));
  ASSERT (PcdGetSize (PcdPp2InterfaceSpeed) == PcdGetSize (PcdPp2PortIds));

  Pp2Context->Port.Id = PortIds[Pp2Context->Instance];
  Pp2Context->Port.GopIndex = GopIndexes[Pp2Context->Instance];
  Pp2Context->Port.PhyInterface = PhyConnectionTypes[Pp2Context->Instance];
  Pp2Context->Port.AlwaysUp = AlwaysUp[Pp2Context->Instance];
  Pp2Context->Port.Speed = Speed[Pp2Context->Instance];
  Pp2Context->Port.GmacBase = PcdGet64 (PcdPp2GmacBaseAddress) +
                              PcdGet32 (PcdPp2GmacDevSize) * Pp2Context->Port.GopIndex;
  Pp2Context->Port.XlgBase = PcdGet64 (PcdPp2XlgBaseAddress) +
                             PcdGet32 (PcdPp2XlgDevSize) * Pp2Context->Port.GopIndex;
}

EFI_STATUS
EFIAPI
Pp2DxeInitialise (
  IN EFI_HANDLE ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  PP2DXE_CONTEXT *Pp2Context = NULL;
  EFI_STATUS Status;
  INTN Index;
  VOID *BufferSpace;
  UINT32 NetCompConfig = 0;
  UINT8 NumPorts = PcdGet32 (PcdPp2NumPorts);

  if (NumPorts == 0) {
    DEBUG((DEBUG_ERROR, "Pp2Dxe: port number set to 0\n"));
    return EFI_INVALID_PARAMETER;
  }

  /* Initialize private data */
  Mvpp2Shared = AllocateZeroPool (sizeof (MVPP2_SHARED));
  if (Mvpp2Shared == NULL) {
    DEBUG((DEBUG_ERROR, "Allocation fail.\n"));
    return EFI_OUT_OF_RESOURCES;
  }

  Mvpp2Shared->Base = PcdGet64 (PcdPp2SharedAddress);
  Mvpp2Shared->Rfu1Base = PcdGet64 (PcdPp2Rfu1BaseAddress);
  Mvpp2Shared->SmiBase = PcdGet64 (PcdPp2SmiBaseAddress);
  Mvpp2Shared->Tclk = PcdGet32 (PcdPp2ClockFrequency);

  /* Prepare buffers */
  BufferSpace = UncachedAllocateAlignedZeroPool (BD_SPACE, MVPP2_BUFFER_ALIGN_SIZE);
  if (BufferSpace == NULL) {
    DEBUG((DEBUG_ERROR, "Failed to allocate buffer space\n"));
    return EFI_OUT_OF_RESOURCES;
  }

  BufferLocation.TxDescs = BufferSpace;
  BufferLocation.AggrTxDescs = (MVPP2_TX_DESC *)((UINTN)BufferSpace + MVPP2_MAX_TXD * sizeof(MVPP2_TX_DESC));
  BufferLocation.RxDescs = (MVPP2_RX_DESC *)((UINTN)BufferSpace +
                                             (MVPP2_MAX_TXD + MVPP2_AGGR_TXQ_SIZE) * sizeof(MVPP2_TX_DESC));
  BufferLocation.RxBuffers = (DmaAddrT)(BufferSpace +
                                        (MVPP2_MAX_TXD + MVPP2_AGGR_TXQ_SIZE) * sizeof(MVPP2_TX_DESC) +
                                        MVPP2_MAX_RXD * sizeof(MVPP2_RX_DESC));

  /* Initialize HW */
  Mvpp2AxiConfig(Mvpp2Shared);
  Pp2DxeBmPoolInit();
  Mvpp2RxFifoInit(Mvpp2Shared);

  Mvpp2Shared->PrsShadow = AllocateZeroPool (sizeof(MVPP2_PRS_SHADOW) * MVPP2_PRS_TCAM_SRAM_SIZE);
  if (Mvpp2Shared->PrsShadow == NULL) {
    DEBUG((DEBUG_ERROR, "Failed to allocate PrsShadow\n"));
    return EFI_OUT_OF_RESOURCES;
  }

  Status = Mvpp2PrsDefaultInit(Mvpp2Shared);
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, "Failed to intialize prs\n"));
    return EFI_DEVICE_ERROR;
  }

  Mvpp2ClsInit(Mvpp2Shared);

  Status = Pp2DxeBmStart();
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, "Pp2Dxe: BM start error\n"));
    return Status;
  }

  /* Initialize aggregated transmit queues */
  Mvpp2Shared->AggrTxqs = AllocateZeroPool (sizeof(MVPP2_TX_QUEUE));
  if (Mvpp2Shared->AggrTxqs == NULL) {
    DEBUG((DEBUG_ERROR, "Failed to allocate aggregated Txqs\n"));
    return EFI_OUT_OF_RESOURCES;
  }

  Mvpp2Shared->AggrTxqs->Descs = BufferLocation.AggrTxDescs;
  Mvpp2Shared->AggrTxqs->Id = 0;
  Mvpp2Shared->AggrTxqs->LogId = 0;
  Mvpp2Shared->AggrTxqs->Size = MVPP2_AGGR_TXQ_SIZE;

  for (Index = 0; Index < NumPorts; Index++) {
    Pp2Context = AllocateZeroPool (sizeof (PP2DXE_CONTEXT));
    if (Pp2Context == NULL) {
      /*
       * If allocation fails, all resources allocated before will get freed
       * at ExitBootServices, as only EfiBootServicesData is used.
       */
      DEBUG((DEBUG_ERROR, "Allocation fail.\n"));
      return EFI_OUT_OF_RESOURCES;
    }

    /* Instances are enumerated from 0 */
    Pp2Context->Instance = Index;

    /* Install SNP protocol */
    Status = Pp2DxeSnpInstall(Pp2Context);
    if (EFI_ERROR(Status)) {
      return Status;
    }

    Pp2DxeParsePortPcd(Pp2Context);
    Pp2Context->Port.TxpNum = 1;
    Pp2Context->Port.Priv = Mvpp2Shared;
    Pp2Context->Port.FirstRxq = 4 * Pp2Context->Instance;

    /* Gather accumulated configuration data of all ports' MAC's */
    NetCompConfig |= MvpPp2xGop110NetcCfgCreate(&Pp2Context->Port);

    MvGop110PortInit(&Pp2Context->Port);
    MvGop110FlCfg(&Pp2Context->Port);

    Status = gBS->CreateEvent (
                 EVT_SIGNAL_EXIT_BOOT_SERVICES,
                 TPL_NOTIFY,
                 Pp2DxeHalt,
                 Pp2Context,
                 &Pp2Context->EfiExitBootServicesEvent
               );

    if (EFI_ERROR(Status)) {
      return Status;
    }
  }

  MvGop110NetcInit(&Pp2Context->Port, NetCompConfig, MV_NETC_FIRST_PHASE);
  MvGop110NetcInit(&Pp2Context->Port, NetCompConfig, MV_NETC_SECOND_PHASE);

  return EFI_SUCCESS;
}
