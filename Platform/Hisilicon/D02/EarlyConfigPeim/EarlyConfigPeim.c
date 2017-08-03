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

#include <Uefi.h>
#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/CacheMaintenanceLib.h>
#include <Library/ArmLib.h>

#include <PlatformArch.h>
#include <Library/PlatformSysCtrlLib.h>

#include <Library/OemMiscLib.h>

#define TIMER_SUBCTRL_BASE                              PcdGet64(PcdPeriSubctrlAddress)
#define ALG_BASE                                        (0xA0000000)
#define PERI_SUB_CTRL_BASE                              (0x80000000)
#define SC_TM_CLKEN0_REG                                (0x2050)
#define SYS_APB_IF_BASE                                 (0x10000)
#define TSENSOR_REG                                     (0x5000)
#define SC_ITS_M3_INT_MUX_SEL_REG                       (0x21F0)
#define SC_HLLC_RESET_DREQ_REG                          (0xA8C)
#define SC_ITS_M3_INT_MUX_SEL_VALUE                     (0xF)
#define SC_HLLC_RESET_DREQ_VALUE                        (0x1f)
#define TSENSOR_CONFIG_VALUE                            (0x1)

VOID PlatformTimerStart (VOID)
{
    // Timer0 clock enable
    MmioWrite32 (TIMER_SUBCTRL_BASE + SC_TM_CLKEN0_REG, 0x3);
}

EFI_STATUS
EFIAPI
EarlyConfigEntry (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
    DEBUG((EFI_D_INFO,"SMMU CONFIG........."));
    SmmuConfigForOS();
    DEBUG((EFI_D_INFO,"Done\n"));

    DEBUG((EFI_D_INFO,"ITS CONFIG........."));
    ITSCONFIG();
    DEBUG((EFI_D_INFO,"Done\n"));

    DEBUG((EFI_D_INFO,"AP CONFIG........."));
    MmioWrite64(FixedPcdGet64(PcdMailBoxAddress), 0x0);
    (void)WriteBackInvalidateDataCacheRange((VOID *) FixedPcdGet64(PcdMailBoxAddress), 8);
    ArmDataSynchronizationBarrier ();
    ArmInstructionSynchronizationBarrier ();

    CoreSelectBoot();

    DEBUG((EFI_D_INFO,"Done\n"));

    DEBUG((EFI_D_INFO,"MN CONFIG........."));
    MN_CONFIG ();
    DEBUG((EFI_D_INFO,"Done\n"));

    DEBUG((EFI_D_INFO,"RTC CONFIG........."));

    MmioWrite32(ALG_BASE + SC_ITS_M3_INT_MUX_SEL_REG, SC_ITS_M3_INT_MUX_SEL_VALUE);

    DEBUG((EFI_D_INFO,"Done\n"));

    DEBUG((EFI_D_INFO,"Tsensor CONFIG........."));

    MmioWrite32(PERI_SUB_CTRL_BASE + SYS_APB_IF_BASE + TSENSOR_REG, TSENSOR_CONFIG_VALUE);
    MmioWrite32(ALG_BASE + SC_HLLC_RESET_DREQ_REG, SC_HLLC_RESET_DREQ_VALUE);

    DEBUG((EFI_D_INFO,"Done\n"));

    DEBUG((EFI_D_INFO,"Timer CONFIG........."));
    PlatformTimerStart ();
    DEBUG((EFI_D_INFO,"Done\n"));

    return EFI_SUCCESS;
}
