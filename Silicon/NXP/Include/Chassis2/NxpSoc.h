/** Soc.h
*  Header defining the Base addresses, sizes, flags etc for chassis 1
*
*  Copyright 2017-2019 NXP
*
*  SPDX-License-Identifier: BSD-2-Clause-Patent
*
**/

#ifndef NXP_SOC_H_
#define NXP_SOC_H_

#define HWA_CGA_M1_CLK_SEL         0xe0000000
#define HWA_CGA_M1_CLK_SHIFT       29

#define TP_CLUSTER_EOC_MASK        0xc0000000  /* end of clusters mask */
#define NUM_CC_PLLS                2
#define CLK_FREQ                   100000000
#define MAX_CPUS                   4
#define NUM_FMAN                   1
#define CHECK_CLUSTER(Cluster)    ((Cluster & TP_CLUSTER_EOC_MASK) == 0x0)

/* RCW SERDES MACRO */
#define RCWSR_INDEX                4
#define RCWSR_SRDS1_PRTCL_MASK     0xffff0000
#define RCWSR_SRDS1_PRTCL_SHIFT    16
#define RCWSR_SRDS2_PRTCL_MASK     0x0000ffff
#define RCWSR_SRDS2_PRTCL_SHIFT    0

/* SMMU Defintions */
#define SMMU_BASE_ADDR             0x09000000
#define SMMU_REG_SCR0              (SMMU_BASE_ADDR + 0x0)
#define SMMU_REG_SACR              (SMMU_BASE_ADDR + 0x10)
#define SMMU_REG_IDR1              (SMMU_BASE_ADDR + 0x24)
#define SMMU_REG_NSCR0             (SMMU_BASE_ADDR + 0x400)
#define SMMU_REG_NSACR             (SMMU_BASE_ADDR + 0x410)

#define SCR0_USFCFG_MASK           0x00000400
#define SCR0_CLIENTPD_MASK         0x00000001
#define SACR_PAGESIZE_MASK         0x00010000
#define IDR1_PAGESIZE_MASK         0x80000000

typedef struct {
  UINTN FreqProcessor[MAX_CPUS];
  UINTN FreqSystemBus;
  UINTN FreqDdrBus;
  UINTN FreqLocalBus;
  UINTN FreqSdhc;
  UINTN FreqFman[NUM_FMAN];
  UINTN FreqQman;
} SYS_INFO;

/* Device Configuration and Pin Control */
typedef struct {
  UINT32   PorSr1;         /* POR status 1 */
#define CHASSIS2_CCSR_PORSR1_RCW_MASK  0xFF800000
  UINT32   PorSr2;         /* POR status 2 */
  UINT8    Res008[0x20-0x8];
  UINT32   GppOrCr1;       /* General-purpose POR configuration */
  UINT32   GppOrCr2;
  UINT32   DcfgFuseSr;    /* Fuse status register */
  UINT8    Res02c[0x70-0x2c];
  UINT32   DevDisr;        /* Device disable control */
  UINT32   DevDisr2;       /* Device disable control 2 */
  UINT32   DevDisr3;       /* Device disable control 3 */
  UINT32   DevDisr4;       /* Device disable control 4 */
  UINT32   DevDisr5;       /* Device disable control 5 */
  UINT32   DevDisr6;       /* Device disable control 6 */
  UINT32   DevDisr7;       /* Device disable control 7 */
  UINT8    Res08c[0x94-0x8c];
  UINT32   CoreDisrU;      /* uppper portion for support of 64 cores */
  UINT32   CoreDisrL;      /* lower portion for support of 64 cores */
  UINT8    Res09c[0xa0-0x9c];
  UINT32   Pvr;            /* Processor version */
  UINT32   Svr;            /* System version */
  UINT32   Mvr;            /* Manufacturing version */
  UINT8    Res0ac[0xb0-0xac];
  UINT32   RstCr;          /* Reset control */
  UINT32   RstRqPblSr;     /* Reset request preboot loader status */
  UINT8    Res0b8[0xc0-0xb8];
  UINT32   RstRqMr1;       /* Reset request mask */
  UINT8    Res0c4[0xc8-0xc4];
  UINT32   RstRqSr1;       /* Reset request status */
  UINT8    Res0cc[0xd4-0xcc];
  UINT32   RstRqWdTmrL;     /* Reset request WDT mask */
  UINT8    Res0d8[0xdc-0xd8];
  UINT32   RstRqWdtSrL;     /* Reset request WDT status */
  UINT8    Res0e0[0xe4-0xe0];
  UINT32   BrrL;            /* Boot release */
  UINT8    Res0e8[0x100-0xe8];
  UINT32   RcwSr[16];      /* Reset control word status */
#define CHASSIS2_RCWSR0_SYS_PLL_RAT_SHIFT  25
#define CHASSIS2_RCWSR0_SYS_PLL_RAT_MASK  0x1f
#define CHASSIS2_RCWSR0_MEM_PLL_RAT_SHIFT  16
#define CHASSIS2_RCWSR0_MEM_PLL_RAT_MASK  0x3f
  UINT8    Res140[0x200-0x140];
  UINT32   ScratchRw[4];   /* Scratch Read/Write */
  UINT8    Res210[0x300-0x210];
  UINT32   ScratcHw1R[4];  /* Scratch Read (Write once) */
  UINT8    Res310[0x400-0x310];
  UINT32   CrstSr[12];
  UINT8    Res430[0x500-0x430];
  /* PCI Express n Logical I/O Device Number register */
  UINT32   DcfgCcsrPex1LiodNr;
  UINT32   DcfgCcsrPex2LiodNr;
  UINT32   DcfgCcsrPex3LiodNr;
  UINT32   DcfgCcsrPex4LiodNr;
  /* RIO n Logical I/O Device Number register */
  UINT32   DcfgCcsrRio1LiodNr;
  UINT32   DcfgCcsrRio2LiodNr;
  UINT32   DcfgCcsrRio3LiodNr;
  UINT32   DcfgCcsrRio4LiodNr;
  /* USB Logical I/O Device Number register */
  UINT32   DcfgCcsrUsb1LiodNr;
  UINT32   DcfgCcsrUsb2LiodNr;
  UINT32   DcfgCcsrUsb3LiodNr;
  UINT32   DcfgCcsrUsb4LiodNr;
  /* SD/MMC Logical I/O Device Number register */
  UINT32   DcfgCcsrSdMmc1LiodNr;
  UINT32   DcfgCcsrSdMmc2LiodNr;
  UINT32   DcfgCcsrSdMmc3LiodNr;
  UINT32   DcfgCcsrSdMmc4LiodNr;
  /* RIO Message Unit Logical I/O Device Number register */
  UINT32   DcfgCcsrRiomaintLiodNr;
  UINT8    Res544[0x550-0x544];
  UINT32   SataLiodNr[4];
  UINT8    Res560[0x570-0x560];
  UINT32   DcfgCcsrMisc1LiodNr;
  UINT32   DcfgCcsrMisc2LiodNr;
  UINT32   DcfgCcsrMisc3LiodNr;
  UINT32   DcfgCcsrMisc4LiodNr;
  UINT32   DcfgCcsrDma1LiodNr;
  UINT32   DcfgCcsrDma2LiodNr;
  UINT32   DcfgCcsrDma3LiodNr;
  UINT32   DcfgCcsrDma4LiodNr;
  UINT32   DcfgCcsrSpare1LiodNr;
  UINT32   DcfgCcsrSpare2LiodNr;
  UINT32   DcfgCcsrSpare3LiodNr;
  UINT32   DcfgCcsrSpare4LiodNr;
  UINT8    Res5a0[0x600-0x5a0];
  UINT32   DcfgCcsrPblSr;
  UINT32   PamuBypENr;
  UINT32   DmaCr1;
  UINT8    Res60c[0x610-0x60c];
  UINT32   DcfgCcsrGenSr1;
  UINT32   DcfgCcsrGenSr2;
  UINT32   DcfgCcsrGenSr3;
  UINT32   DcfgCcsrGenSr4;
  UINT32   DcfgCcsrGenCr1;
  UINT32   DcfgCcsrGenCr2;
  UINT32   DcfgCcsrGenCr3;
  UINT32   DcfgCcsrGenCr4;
  UINT32   DcfgCcsrGenCr5;
  UINT32   DcfgCcsrGenCr6;
  UINT32   DcfgCcsrGenCr7;
  UINT8    Res63c[0x658-0x63c];
  UINT32   DcfgCcsrcGenSr1;
  UINT32   DcfgCcsrcGenSr0;
  UINT8    Res660[0x678-0x660];
  UINT32   DcfgCcsrcGenCr1;
  UINT32   DcfgCcsrcGenCr0;
  UINT8    Res680[0x700-0x680];
  UINT32   DcfgCcsrSrIoPstecr;
  UINT32   DcfgCcsrDcsrCr;
  UINT8    Res708[0x740-0x708]; /* add more registers when needed */
  UINT32   TpItyp[64];          /* Topology Initiator Type Register */
  struct {
    UINT32 Upper;
    UINT32 Lower;
  } TpCluster[16];
  UINT8    Res8c0[0xa00-0x8c0]; /* add more registers when needed */
  UINT32   DcfgCcsrQmBmWarmRst;
  UINT8    Resa04[0xa20-0xa04]; /* add more registers when needed */
  UINT32   DcfgCcsrReserved0;
  UINT32   DcfgCcsrReserved1;
} CCSR_GUR;

/* Supplemental Configuration Unit */
typedef struct {
  UINT8  Res000[0x070-0x000];
  UINT32 Usb1Prm1Cr;
  UINT32 Usb1Prm2Cr;
  UINT32 Usb1Prm3Cr;
  UINT32 Usb2Prm1Cr;
  UINT32 Usb2Prm2Cr;
  UINT32 Usb2Prm3Cr;
  UINT32 Usb3Prm1Cr;
  UINT32 Usb3Prm2Cr;
  UINT32 Usb3Prm3Cr;
  UINT8  Res094[0x100-0x094];
  UINT32 Usb2Icid;
  UINT32 Usb3Icid;
  UINT8  Res108[0x114-0x108];
  UINT32 DmaIcid;
  UINT32 SataIcid;
  UINT32 Usb1Icid;
  UINT32 QeIcid;
  UINT32 SdhcIcid;
  UINT32 EdmaIcid;
  UINT32 EtrIcid;
  UINT32 Core0SftRst;
  UINT32 Core1SftRst;
  UINT32 Core2SftRst;
  UINT32 Core3SftRst;
  UINT8  Res140[0x158-0x140];
  UINT32 AltCBar;
  UINT32 QspiCfg;
  UINT8  Res160[0x180-0x160];
  UINT32 DmaMcr;
  UINT8  Res184[0x188-0x184];
  UINT32 GicAlign;
  UINT32 DebugIcid;
  UINT8  Res190[0x1a4-0x190];
  UINT32 SnpCnfGcr;
#define CCSR_SCFG_SNPCNFGCR_SECRDSNP         BIT31
#define CCSR_SCFG_SNPCNFGCR_SECWRSNP         BIT30
#define CCSR_SCFG_SNPCNFGCR_SATARDSNP        BIT23
#define CCSR_SCFG_SNPCNFGCR_SATAWRSNP        BIT22
#define CCSR_SCFG_SNPCNFGCR_USB1RDSNP        BIT21
#define CCSR_SCFG_SNPCNFGCR_USB1WRSNP        BIT20
#define CCSR_SCFG_SNPCNFGCR_USB2RDSNP        BIT15
#define CCSR_SCFG_SNPCNFGCR_USB2WRSNP        BIT16
#define CCSR_SCFG_SNPCNFGCR_USB3RDSNP        BIT13
#define CCSR_SCFG_SNPCNFGCR_USB3WRSNP        BIT14
  UINT8  Res1a8[0x1ac-0x1a8];
  UINT32 IntpCr;
  UINT8  Res1b0[0x204-0x1b0];
  UINT32 CoreSrEnCr;
  UINT8  Res208[0x220-0x208];
  UINT32 RvBar00;
  UINT32 RvBar01;
  UINT32 RvBar10;
  UINT32 RvBar11;
  UINT32 RvBar20;
  UINT32 RvBar21;
  UINT32 RvBar30;
  UINT32 RvBar31;
  UINT32 LpmCsr;
  UINT8  Res244[0x400-0x244];
  UINT32 QspIdQScr;
  UINT32 EcgTxcMcr;
  UINT32 SdhcIoVSelCr;
  UINT32 RcwPMuxCr0;
  /**Setting RCW PinMux Register bits 17-19 to select USB2_DRVVBUS
  *Setting RCW PinMux Register bits 21-23 to select USB2_PWRFAULT
  *Setting RCW PinMux Register bits 25-27 to select USB3_DRVVBUS
  Setting RCW PinMux Register bits 29-31 to select USB3_DRVVBUS*/
#define CCSR_SCFG_RCWPMUXCRO_SELCR_USB      0x3333
  /**Setting RCW PinMux Register bits 17-19 to select USB2_DRVVBUS
  *Setting RCW PinMux Register bits 21-23 to select USB2_PWRFAULT
  *Setting RCW PinMux Register bits 25-27 to select IIC4_SCL
  Setting RCW PinMux Register bits 29-31 to select IIC4_SDA*/
#define CCSR_SCFG_RCWPMUXCRO_NOT_SELCR_USB  0x3300
  UINT32 UsbDrvVBusSelCr;
#define CCSR_SCFG_USBDRVVBUS_SELCR_USB1      0x00000000
#define CCSR_SCFG_USBDRVVBUS_SELCR_USB2      0x00000001
#define CCSR_SCFG_USBDRVVBUS_SELCR_USB3      0x00000003
  UINT32 UsbPwrFaultSelCr;
#define CCSR_SCFG_USBPWRFAULT_INACTIVE       0x00000000
#define CCSR_SCFG_USBPWRFAULT_SHARED         0x00000001
#define CCSR_SCFG_USBPWRFAULT_DEDICATED      0x00000002
#define CCSR_SCFG_USBPWRFAULT_USB3_SHIFT     4
#define CCSR_SCFG_USBPWRFAULT_USB2_SHIFT     2
#define CCSR_SCFG_USBPWRFAULT_USB1_SHIFT     0
  UINT32 UsbRefclkSelcr1;
  UINT32 UsbRefclkSelcr2;
  UINT32 UsbRefclkSelcr3;
  UINT8  Res424[0x600-0x424];
  UINT32 ScratchRw[4];
  UINT8  Res610[0x680-0x610];
  UINT32 CoreBCr;
  UINT8  Res684[0x1000-0x684];
  UINT32 Pex1MsiIr;
  UINT32 Pex1MsiR;
  UINT8  Res1008[0x2000-0x1008];
  UINT32 Pex2;
  UINT32 Pex2MsiR;
  UINT8  Res2008[0x3000-0x2008];
  UINT32 Pex3MsiIr;
  UINT32 Pex3MsiR;
} CCSR_SCFG;

#define USB_TXVREFTUNE        0x9
#define USB_SQRXTUNE          0xFC7FFFFF
#define USB_PCSTXSWINGFULL    0x47
#define USB_PHY_RX_EQ_VAL_1   0x0000
#define USB_PHY_RX_EQ_VAL_2   0x8000
#define USB_PHY_RX_EQ_VAL_3   0x8003
#define USB_PHY_RX_EQ_VAL_4   0x800b

/*USB_PHY_SS memory map*/
typedef struct {
  UINT16 IpIdcodeLo;
  UINT16 SupIdcodeHi;
  UINT8  Res4[0x0006-0x0004];
  UINT16 RtuneDebug;
  UINT16 RtuneStat;
  UINT16 SupSsPhase;
  UINT16 SsFreq;
  UINT8  ResE[0x0020-0x000e];
  UINT16 Ateovrd;
  UINT16 MpllOvrdInLo;
  UINT8  Res24[0x0026-0x0024];
  UINT16 SscOvrdIn;
  UINT8  Res28[0x002A-0x0028];
  UINT16 LevelOvrdIn;
  UINT8  Res2C[0x0044-0x002C];
  UINT16 ScopeCount;
  UINT8  Res46[0x0060-0x0046];
  UINT16 MpllLoopCtl;
  UINT8  Res62[0x006C-0x0062];
  UINT16 SscClkCntrl;
  UINT8  Res6E[0x2002-0x006E];
  UINT16 Lane0TxOvrdInHi;
  UINT16 Lane0TxOvrdDrvLo;
  UINT8  Res2006[0x200C-0x2006];
  UINT16 Lane0RxOvrdInHi;
  UINT8  Res200E[0x2022-0x200E];
  UINT16 Lane0TxCmWaitTimeOvrd;
  UINT8  Res2024[0x202A-0x2024];
  UINT16 Lane0TxLbertCtl;
  UINT16 Lane0RxLbertCtl;
  UINT16 Lane0RxLbertErr;
  UINT8  Res2030[0x205A-0x2030];
  UINT16 Lane0TxAltBlock;
} CCSR_USB_PHY;

/* Clocking */
typedef struct {
  struct {
    UINT32 ClkCnCSr;    /* core cluster n clock control status */
    UINT8  Res004[0x0c];
    UINT32 ClkcGHwAcSr; /* Clock generator n hardware accelerator */
    UINT8 Res014[0x0c];
  } ClkcSr[4];
  UINT8  Res040[0x780]; /* 0x100 */
  struct {
    UINT32 PllCnGSr;
    UINT8  Res804[0x1c];
  } PllCgSr[NUM_CC_PLLS];
  UINT8  Res840[0x1c0];
  UINT32 ClkPCSr;  /* 0xa00 Platform clock domain control/status */
  UINT8  Resa04[0x1fc];
  UINT32 PllPGSr;  /* 0xc00 Platform PLL General Status */
  UINT8  Resc04[0x1c];
  UINT32 PllDGSr;  /* 0xc20 DDR PLL General Status */
  UINT8  Resc24[0x3dc];
} CCSR_CLOCK;

VOID
GetSysInfo (
  OUT SYS_INFO *
  );

UINT32
EFIAPI
GurRead (
  IN  UINTN     Address
  );

#endif /* NXP_SOC_H_ */
