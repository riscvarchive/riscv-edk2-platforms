/********************************************************************************
Copyright (C) 2017 Marvell International Ltd.

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

#ifndef __MVHWDESCLIB_H__
#define __MVHWDESCLIB_H__

#include <Library/MvComPhyLib.h>
#include <Library/NonDiscoverableDeviceRegistrationLib.h>

//
// Helper macros
//

// Check if device is enabled - it expects PCD to be read to '<type>DeviceTable' array
#define MVHW_DEV_ENABLED(type, index) (type ## DeviceTable[index])

//
// CommonPhy devices description template definition
//
#define MVHW_MAX_COMPHY_DEVS       4

typedef struct {
  UINT8 ComPhyDevCount;
  UINTN ComPhyBaseAddresses[MVHW_MAX_COMPHY_DEVS];
  UINTN ComPhyHpipe3BaseAddresses[MVHW_MAX_COMPHY_DEVS];
  UINTN ComPhyLaneCount[MVHW_MAX_COMPHY_DEVS];
  UINTN ComPhyMuxBitCount[MVHW_MAX_COMPHY_DEVS];
  MV_COMPHY_CHIP_TYPE ComPhyChipType[MVHW_MAX_COMPHY_DEVS];
} MVHW_COMPHY_DESC;

//
// NonDiscoverable devices description template definition
//
#define MVHW_MAX_XHCI_DEVS         4
#define MVHW_MAX_AHCI_DEVS         4
#define MVHW_MAX_SDHCI_DEVS        4

typedef struct {
  // XHCI
  UINT8 XhciDevCount;
  UINTN XhciBaseAddresses[MVHW_MAX_XHCI_DEVS];
  UINTN XhciMemSize[MVHW_MAX_XHCI_DEVS];
  NON_DISCOVERABLE_DEVICE_DMA_TYPE XhciDmaType[MVHW_MAX_XHCI_DEVS];
  // AHCI
  UINT8 AhciDevCount;
  UINTN AhciBaseAddresses[MVHW_MAX_AHCI_DEVS];
  UINTN AhciMemSize[MVHW_MAX_AHCI_DEVS];
  NON_DISCOVERABLE_DEVICE_DMA_TYPE AhciDmaType[MVHW_MAX_AHCI_DEVS];
  // SDHCI
  UINT8 SdhciDevCount;
  UINTN SdhciBaseAddresses[MVHW_MAX_SDHCI_DEVS];
  UINTN SdhciMemSize[MVHW_MAX_SDHCI_DEVS];
  NON_DISCOVERABLE_DEVICE_DMA_TYPE SdhciDmaType[MVHW_MAX_SDHCI_DEVS];
} MVHW_NONDISCOVERABLE_DESC;

//
// RealTimeClock devices description template definition
//
#define MVHW_MAX_RTC_DEVS         2

typedef struct {
  UINT8 RtcDevCount;
  UINTN RtcBaseAddresses[MVHW_MAX_RTC_DEVS];
  UINTN RtcMemSize[MVHW_MAX_RTC_DEVS];
} MVHW_RTC_DESC;

//
// Platform description of CommonPhy devices
//
#define MVHW_CP0_COMPHY_BASE       0xF2441000
#define MVHW_CP0_HPIPE3_BASE       0xF2120000
#define MVHW_CP0_COMPHY_LANES      6
#define MVHW_CP0_COMPHY_MUX_BITS   4
#define MVHW_CP1_COMPHY_BASE       0xF4441000
#define MVHW_CP1_HPIPE3_BASE       0xF4120000
#define MVHW_CP1_COMPHY_LANES      6
#define MVHW_CP1_COMPHY_MUX_BITS   4

#define DECLARE_A7K8K_COMPHY_TEMPLATE \
STATIC \
MVHW_COMPHY_DESC mA7k8kComPhyDescTemplate = {\
  2,\
  { MVHW_CP0_COMPHY_BASE, MVHW_CP1_COMPHY_BASE },\
  { MVHW_CP0_HPIPE3_BASE, MVHW_CP1_HPIPE3_BASE },\
  { MVHW_CP0_COMPHY_LANES, MVHW_CP1_COMPHY_LANES },\
  { MVHW_CP0_COMPHY_MUX_BITS, MVHW_CP1_COMPHY_MUX_BITS },\
  { MvComPhyTypeCp110, MvComPhyTypeCp110 }\
}

//
// Platform description of NonDiscoverable devices
//
#define MVHW_CP0_XHCI0_BASE        0xF2500000
#define MVHW_CP0_XHCI1_BASE        0xF2510000
#define MVHW_CP1_XHCI0_BASE        0xF4500000
#define MVHW_CP1_XHCI1_BASE        0xF4510000

#define MVHW_CP0_AHCI0_BASE        0xF2540000
#define MVHW_CP0_AHCI0_ID          0
#define MVHW_CP1_AHCI0_BASE        0xF4540000
#define MVHW_CP1_AHCI0_ID          1

#define MVHW_AP0_SDHCI0_BASE       0xF06E0000
#define MVHW_CP0_SDHCI0_BASE       0xF2780000

#define DECLARE_A7K8K_NONDISCOVERABLE_TEMPLATE   \
STATIC \
MVHW_NONDISCOVERABLE_DESC mA7k8kNonDiscoverableDescTemplate = {\
  4, /* XHCI */\
  { MVHW_CP0_XHCI0_BASE, MVHW_CP0_XHCI1_BASE, MVHW_CP1_XHCI0_BASE, MVHW_CP1_XHCI1_BASE },\
  { SIZE_16KB, SIZE_16KB, SIZE_16KB, SIZE_16KB },\
  { NonDiscoverableDeviceDmaTypeCoherent, NonDiscoverableDeviceDmaTypeCoherent,\
    NonDiscoverableDeviceDmaTypeCoherent, NonDiscoverableDeviceDmaTypeCoherent },\
  2, /* AHCI */\
  { MVHW_CP0_AHCI0_BASE, MVHW_CP1_AHCI0_BASE },\
  { SIZE_8KB, SIZE_8KB },\
  { NonDiscoverableDeviceDmaTypeCoherent, NonDiscoverableDeviceDmaTypeCoherent },\
  2, /* SDHCI */\
  { MVHW_AP0_SDHCI0_BASE, MVHW_CP0_SDHCI0_BASE },\
  { SIZE_1KB, SIZE_1KB },\
  { NonDiscoverableDeviceDmaTypeCoherent, NonDiscoverableDeviceDmaTypeCoherent }\
}

//
// Platform description of RealTimeClock devices
//
#define MVHW_CP0_RTC0_BASE       0xF2284000
#define MVHW_CP1_RTC0_BASE       0xF4284000

#define DECLARE_A7K8K_RTC_TEMPLATE \
STATIC \
MVHW_RTC_DESC mA7k8kRtcDescTemplate = {\
  2,\
  { MVHW_CP0_RTC0_BASE, MVHW_CP1_RTC0_BASE },\
  { SIZE_4KB, SIZE_4KB }\
}

#endif /* __MVHWDESCLIB_H__ */
