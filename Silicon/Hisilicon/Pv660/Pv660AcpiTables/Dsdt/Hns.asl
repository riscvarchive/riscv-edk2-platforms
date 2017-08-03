/** @file
  Differentiated System Description Table Fields (DSDT)

  Copyright (c) 2014, ARM Ltd. All rights reserved.<BR>
    This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

Scope(_SB)
{
  Device (MDIO)
  {
      OperationRegion(CLKR, SystemMemory, 0x80000338, 8)
    Field(CLKR, DWordAcc, NoLock, Preserve) {
      CLKE, 1,  // clock enable
      , 31,
      CLKD, 1,  // clode disable
      , 31,
    }
      OperationRegion(RSTR, SystemMemory, 0x80000A38, 8)
    Field(RSTR, DWordAcc, NoLock, Preserve) {
      RSTE, 1,  // reset
      , 31,
      RSTD, 1,  // de-reset
      , 31,
    }

    Name(_HID, "HISI0141")
    Name (_CRS, ResourceTemplate (){
      Memory32Fixed (ReadWrite, 0x803c0000 , 0x10000)
            })

    Method(_RST, 0, Serialized) {
      Store (0x1, RSTE)
      Sleep (10)
      Store (0x1, CLKD)
      Sleep (10)
      Store (0x1, RSTD)
      Sleep (10)
      Store (0x1, CLKE)
      Sleep (10)
    }
  }

  Device (DSF0)
  {
    OperationRegion(H3SR, SystemMemory, 0xC0000180, 4)
    Field(H3SR, DWordAcc, NoLock, Preserve) {
          H3ST, 1,
          , 31,  //RESERVED
        }
    OperationRegion(H4SR, SystemMemory, 0xC0000190, 4)
    Field(H4SR, DWordAcc, NoLock, Preserve) {
          H4ST, 1,
          , 31,  //RESERVED
        }
    // DSAF RESET
    OperationRegion(DRER, SystemMemory, 0xC0000A00, 8)
    Field(DRER, DWordAcc, NoLock, Preserve) {
          DRTE, 1,
          , 31,  //RESERVED
          DRTD, 1,
          , 31,  //RESERVED
        }
    // NT RESET
    OperationRegion(NRER, SystemMemory, 0xC0000A08, 8)
    Field(NRER, DWordAcc, NoLock, Preserve) {
          NRTE, 1,
          , 31,  //RESERVED
          NRTD, 1,
          , 31,  //RESERVED
        }
    // XGE RESET
    OperationRegion(XRER, SystemMemory, 0xC0000A10, 8)
    Field(XRER, DWordAcc, NoLock, Preserve) {
          XRTE, 31,
          , 1,    //RESERVED
          XRTD, 31,
          , 1,    //RESERVED
        }

    // GE RESET
    OperationRegion(GRTR, SystemMemory, 0xC0000A18, 16)
    Field(GRTR, DWordAcc, NoLock, Preserve) {
          GR0E, 30,
          , 2,    //RESERVED
          GR0D, 30,
          , 2,    //RESERVED
          GR1E, 18,
          , 14,  //RESERVED
          GR1D, 18,
          , 14,  //RESERVED
        }
    // PPE RESET
    OperationRegion(PRTR, SystemMemory, 0xC0000A48, 8)
    Field(PRTR, DWordAcc, NoLock, Preserve) {
          PRTE, 10,
          , 22,  //RESERVED
          PRTD, 10,
          , 22,  //RESERVED
        }

    // RCB PPE COM RESET
    OperationRegion(RRTR, SystemMemory, 0xC0000A88, 8)
    Field(RRTR, DWordAcc, NoLock, Preserve) {
          RRTE, 1,
          , 31,  //RESERVED
          RRTD, 1,
          , 31,  //RESERVED
        }

    // ROCE

    // CPLD LED

    // Serdes
    OperationRegion(H4LR, SystemMemory, 0xC2288100, 0x1000)
    Field(H4LR, DWordAcc, NoLock, Preserve) {
          H4L0, 16,    // port0
          H4R0, 16,    //RESERVED
          Offset (0x400),
          H4L1, 16,    // port1
          H4R1, 16,    //RESERVED
          Offset (0x800),
          H4L2, 16,    // port2
          H4R2, 16,    //RESERVED
          Offset (0xc00),
          H4L3, 16,    // port3
          H4R3, 16,    //RESERVED
        }
    OperationRegion(H3LR, SystemMemory, 0xC2208900, 0x800)
    Field(H3LR, DWordAcc, NoLock, Preserve) {
          H3L2, 16,    // port4
          , 16,    //RESERVED
          Offset (0x400),
          H3L3, 16,    // port5
          , 16,    //RESERVED
        }
    Name (_HID, "HISI00B1")
    Name (_CCA, 1) // Cache-coherent controller
    Name (_CRS, ResourceTemplate (){
      Memory32Fixed (ReadWrite, 0xc5000000 , 0x890000)
                        Memory32Fixed (ReadWrite, 0xc7000000 , 0x60000)
      Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive,,,)
        {
          149,150,151,152,153,154,26,27,155,156,157,158,159,160,  //[14] ge fifo err 8 / xge 6
          6,7,8,9,16,17,18,19,22,23,24,25,      //[12] rcb com 4*3
          0,1,2,3,4,5,12,13,          //[8] ppe tnl 0-7
          128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,144,145,146,147,148,  //[21] dsaf event int 3+18
          161,162,163,164,
        }
      Interrupt (ResourceConsumer, Edge, ActiveHigh, Exclusive,,,)
        {
          384, 385, 386, 387, 388, 389, 390, 391, 392, 393, 394, 395, 396, 397, 398, 399,  //[256] sevice rcb 2*128
          400, 401, 402, 403, 404, 405, 406, 407, 408, 409, 410, 411, 412, 413, 414, 415,
          416, 417, 418, 419, 420, 421, 422, 423, 424, 425, 426, 427, 428, 429, 430, 431,
          432, 433, 434, 435, 436, 437, 438, 439, 440, 441, 442, 443, 444, 445, 446, 447,
          448, 449, 450, 451, 452, 453, 454, 455, 456, 457, 458, 459, 460, 461, 462, 463,
          464, 465, 466, 467, 468, 469, 470, 471, 472, 473, 474, 475, 476, 477, 478, 479,
          480, 481, 482, 483, 484, 485, 486, 487, 488, 489, 490, 491, 492, 493, 494, 495,
          496, 497, 498, 499, 500, 501, 502, 503, 504, 505, 506, 507, 508, 509, 510, 511,
          512, 513, 514, 515, 516, 517, 518, 519, 520, 521, 522, 523, 524, 525, 526, 527,
          528, 529, 530, 531, 532, 533, 534, 535, 536, 537, 538, 539, 540, 541, 542, 543,
          544, 545, 546, 547, 548, 549, 550, 551, 552, 553, 554, 555, 556, 557, 558, 559,
          560, 561, 562, 563, 564, 565, 566, 567, 568, 569, 570, 571, 572, 573, 574, 575,
        }
                })
                Name (_DSD, Package () {
                        ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
                                Package () {"mode", "6port-16rss"},
                                Package () {"buf-size", 4096},
                                Package () {"desc-num", 1024},
        Package () {"interrupt-parent", Package() {\_SB.MBI1}},
                        }
                })

    //reset XGE port
    //Arg0 : XGE port index in dsaf
    //Arg1 : 0 reset, 1 cancle reset
    Method(XRST, 2, Serialized) {
      ShiftLeft (0x2082082, Arg0, Local0)
      Or (Local0, 0x1, Local0)

      If (LEqual (Arg1, 0)) {
        Store(Local0, XRTE)
      } Else  {
        Store(Local0, XRTD)
      }
    }

    //reset XGE core
    //Arg0 : XGE port index in dsaf
    //Arg1 : 0 reset, 1 cancle reset
    Method(XCRT, 2, Serialized) {
      ShiftLeft (0x2080, Arg0, Local0)

      If (LEqual (Arg1, 0)) {
        Store(Local0, XRTE)
      } Else  {
        Store(Local0, XRTD)
      }
    }

    //reset GE port
    //Arg0 : GE port index in dsaf
    //Arg1 : 0 reset, 1 cancle reset
    Method(GRST, 2, Serialized) {
      If (LLessEqual (Arg0, 5)) {
        //Service port
        ShiftLeft (0x1041041, Arg0, Local0)
        ShiftLeft (0x1, Arg0, Local1)

        If (LEqual (Arg1, 0)) {
          Store(Local1, GR1E)
          Store(Local0, GR0E)
        } Else  {
          Store(Local0, GR0D)
          Store(Local1, GR1D)
        }
      }
    }

    //reset PPE port
    //Arg0 : PPE port index in dsaf
    //Arg1 : 0 reset, 1 cancle reset
    Method(PRST, 2, Serialized) {
      ShiftLeft (0x1, Arg0, Local0)
      If (LEqual (Arg1, 0)) {
        Store(Local0, PRTE)
      } Else  {
        Store(Local0, PRTD)
      }
    }

    // Set Serdes Loopback
    //Arg0 : port
    //Arg1 : 0 disable, 1 enable
    Method(SRLP, 2, Serialized) {
      ShiftLeft (Arg1, 10, Local0)
      Switch (ToInteger(Arg0))
      {
        case (0x0){
          Store (H4L0, Local1)
          And (Local1, 0xfffffbff, Local1)
          Or (Local0, Local1, Local0)
          Store (Local0, H4L0)
        }
        case (0x1){
          Store (H4L1, Local1)
          And (Local1, 0xfffffbff, Local1)
          Or (Local0, Local1, Local0)
          Store (Local0, H4L1)
        }
        case (0x2){
          Store (H4L2, Local1)
          And (Local1, 0xfffffbff, Local1)
          Or (Local0, Local1, Local0)
          Store (Local0, H4L2)
        }
        case (0x3){
          Store (H4L3, Local1)
          And (Local1, 0xfffffbff, Local1)
          Or (Local0, Local1, Local0)
          Store (Local0, H4L3)
        }
        case (0x4){
          Store (H3L2, Local1)
          And (Local1, 0xfffffbff, Local1)
          Or (Local0, Local1, Local0)
          Store (Local0, H3L2)
        }
        case (0x5){
          Store (H3L3, Local1)
          And (Local1, 0xfffffbff, Local1)
          Or (Local0, Local1, Local0)
          Store (Local0, H3L3)
        }
      }
    }

    //Reset
    //Arg0 : reset type (1: dsaf; 2: ppe; 3:XGE core; 4:XGE; 5:G3)
    //Arg1 : port
    //Arg2 : 0 disable, 1 enable
    Method(DRST, 3, Serialized)
    {
      Switch (ToInteger(Arg0))
      {
        //DSAF reset
        case (0x1)
        {
          Store (Arg2, Local0)
          If (LEqual (Local0, 0))
          {
            Store (0x1, DRTE)
            Store (0x1, NRTE)
            Sleep (10)
            Store (0x1, RRTE)
          }
          Else
          {
            Store (0x1, DRTD)
            Store (0x1, NRTD)
            Sleep (10)
            Store (0x1, RRTD)
          }
        }
        //Reset PPE port
        case (0x2)
        {
          Store (Arg1, Local0)
          Store (Arg2, Local1)
          PRST (Local0, Local1)
        }

        //Reset XGE core
        case (0x3)
        {
          Store (Arg1, Local0)
          Store (Arg2, Local1)
          XCRT (Local0, Local1)
        }
        //Reset XGE port
        case (0x4)
        {
          Store (Arg1, Local0)
          Store (Arg2, Local1)
          XRST (Local0, Local1)
        }

        //Reset GE port
        case (0x5)
        {
          Store (Arg1, Local0)
          Store (Arg2, Local1)
          GRST (Local0, Local1)
        }
      }
    }

    // _DSM Device Specific Method
    //
    // Arg0: UUID Unique function identifier
    // Arg1: Integer Revision Level
    // Arg2: Integer Function Index
    //   0 : Return Supported Functions bit mask
    //   1 : Reset Sequence
    //    Arg3[0] : reset type (1:dsaf; 2:ppe; 3:xge core; 4:xge; 5: ge)
    //    Arg3[1] : port index in dsaf
    //    Arg3[2] : 0 reset, 1 cancle reset
    //   2 : Set Serdes Loopback
    //    Arg3[0] : port
    //    Arg3[1] : 0 disable, 1 enable
    //   3 : LED op set
    //    Arg3[0] : op type
    //    Arg3[1] : port
    //    Arg3[2] : para
    //   4 : Get port type (GE or XGE)
    //    Arg3[0] : port index in dsaf
    //    Return : 0 GE, 1 XGE
    //   5 : Get sfp status
    //    Arg3[0] : port index in dsaf
    //    Return : 0 no sfp, 1 have sfp
    // Arg3: Package Parameters
    Method (_DSM, 4, Serialized)
    {
      If (LEqual(Arg0,ToUUID("1A85AA1A-E293-415E-8E28-8D690A0F820A")))
      {
        If (LEqual (Arg1, 0x00))
        {
          Switch (ToInteger(Arg2))
          {
            case (0x0)
            {
              Return (Buffer () {0x3F})
            }

            //Reset Sequence
            case (0x1)
            {
              Store (DeRefOf (Index (Arg3, 0)), Local0)
              Store (DeRefOf (Index (Arg3, 1)), Local1)
              Store (DeRefOf (Index (Arg3, 2)), Local2)
              DRST (Local0, Local1, Local2)
            }

            //Set Serdes Loopback
            case (0x2)
            {
              Store (DeRefOf (Index (Arg3, 0)), Local0)
              Store (DeRefOf (Index (Arg3, 1)), Local1)
              SRLP (Local0, Local1)
            }

            //LED op set
            case (0x3)
            {

            }

            // Get port type (GE or XGE)
            case (0x4)
            {
              Store (0, Local1)
              Store (DeRefOf (Index (Arg3, 0)), Local0)
              If (LLessEqual (Local0, 3))
              {
                Store (H4ST, Local1)
              }
              ElseIf (LLessEqual (Local0, 5))
              {
                Store (H3ST, Local1)
              }

              Return (Local1)
            }

            //Get sfp status
            case (0x5)
            {

            }
          }
        }
      }
      Return (Buffer() {0x00})
    }
    Device (PRT0)
    {
      Name (_ADR, 0x0)
      Name (_DSD, Package () {
                        ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
        Package () {
          Package () {"reg", 0},
          Package () {"media-type", "fiber"},
                        }
                })
    }
    Device (PRT1)
    {
      Name (_ADR, 0x1)
      Name (_DSD, Package () {
                        ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
        Package () {
          Package () {"reg", 1},
          Package () {"media-type", "fiber"},
                        }
                })
    }
    Device (PRT4)
    {
      Name (_ADR, 0x4)
      Name (_DSD, Package () {
                        ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
        Package () {
          Package () {"reg", 4},
          Package () {"phy-mode", "sgmii"},
          Package () {"phy-addr", 0},
          Package () {"mdio-node", Package (){\_SB.MDIO}},
          Package () {"media-type", "copper"},
                        }
                })
    }
    Device (PRT5)
    {
      Name (_ADR, 0x5)
      Name (_DSD, Package () {
                        ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
        Package () {
          Package () {"reg", 5},
          Package () {"phy-mode", "sgmii"},
          Package () {"phy-addr", 1},
          Package () {"mdio-node", Package (){\_SB.MDIO}},
          Package () {"media-type", "copper"},
                        }
                })
    }
  }
/*
  Device (DSF1)
  {
    OperationRegion(H3SR, SystemMemory, 0xC0000180, 4)
    Field(H3SR, DWordAcc, NoLock, Preserve) {
          H3ST, 1,
          , 31,  //RESERVED
        }

    // XGE RESET
    OperationRegion(XRER, SystemMemory, 0xC0000A10, 8)
    Field(XRER, DWordAcc, NoLock, Preserve) {
          XRTE, 31,
          , 1,    //RESERVED
          XRTD, 31,
          , 1,    //RESERVED
        }

    // GE RESET
    OperationRegion(GRTR, SystemMemory, 0xC0000A18, 16)
    Field(GRTR, DWordAcc, NoLock, Preserve) {
          GR0E, 30,
          , 2,    //RESERVED
          GR0D, 30,
          , 2,    //RESERVED
          GR1E, 18,
          , 14,  //RESERVED
          GR1D, 18,
          , 14,  //RESERVED
        }

    // PPE RESET
    OperationRegion(PRTR, SystemMemory, 0xC0000A48, 8)
    Field(PRTR, DWordAcc, NoLock, Preserve) {
          PRTE, 10,
          , 22,  //RESERVED
          PRTD, 10,
          , 22,  //RESERVED
        }

    // ROCE

    // CPLD LED

    // Serdes
    OperationRegion(H3LR, SystemMemory, 0xC2208100, 0x4)
    Field(H3LR, DWordAcc, NoLock, Preserve) {
          H3L0, 16,    // debug port0
          , 16,    //RESERVED
        }
                Name(_HID, "HISI00B1")
                Name (_CCA, 1) // Cache-coherent controller
                Name (_CRS, ResourceTemplate (){
                        Memory32Fixed (ReadWrite, 0xc2000000 , 0x890000)

                Interrupt (ResourceConsumer, Edge, ActiveHigh, Exclusive,,,)
        {
          14, 15,
        }
                })
                Name (_DSD, Package () {
                        ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
        Package () {
                                Package () {"mode", "single-port"},
                                Package () {"buf-size", 4096},
                                Package () {"desc-num", 1024},
        Package () {"interrupt-parent", Package() {\_SB.MBI1}},
                        }
                })

      // Set Serdes Loopback
    //Arg0 : port
    //Arg1 : 0 disable, 1 enable
    Method(SRLP, 1, Serialized) {
      ShiftLeft (Arg0, 10, Local0)
      Store (H3L0, Local1)
      And (Local1, 0xfffffbff, Local1)
      Or (Local0, Local1, Local0)
      Store (Local1, H3L0)
    }

    //Reset
    //Arg0 : reset type (1: dsaf; 2: ppe; 3:reserved; 4:reserved; 5:G3)
    //Arg1 : 0 disable, 1 enable
    Method(DRST, 2, Serialized)
    {
      Switch (ToInteger(Arg0))
      {
        //DSAF reset
        case (0x1)
        {
          If (LEqual (Arg1, 0)) {
            Store (0x100, PRTE)
          } Else {
            Store (0x100, PRTD)
          }
        }
        //Reset PPE port
        case (0x2)
        {
          If (LEqual (Arg1, 0)) {
            Store(0x40, PRTE)
          } Else  {
            Store(0x40, PRTD)
          }
        }

        //Reset GE port
        case (0x5)
        {
          If (LEqual (Arg1, 0)) {
            Store(0x15540, GR1E)
            Store(0x100, PRTE)
          } Else {
            Store(0x15540, GR1D)
            Store(0x100, PRTD)
          }
        }
      }
    }
    // _DSM Device Specific Method
    //
    // Arg0: UUID Unique function identifier
    // Arg1: Integer Revision Level
    // Arg2: Integer Function Index
    //   0 : Return Supported Functions bit mask
    //   1 : Reset Sequence
    //    Arg3[0] : reset type (1:dsaf; 2:ppe; 3:reserved; 4:reserved; 5: ge)
    //    Arg3[1] : reserved
    //    Arg3[2] : 0 reset, 1 cancle reset
    //   2 : Set Serdes Loopback
    //    Arg3[0] : reserved
    //    Arg3[1] : 0 disable, 1 enable
    //   3 : LED op set
    //    Arg3[0] : op type
    //    Arg3[1] : reserved
    //    Arg3[2] : para
    //   4 : Get port type (GE or XGE)
    //    Arg3[0] : reserved
    //    Return : 0 GE, 1 XGE
    //   5 : Get sfp status
    //    Arg3[0] : reserved
    //    Return : 0 no sfp, 1 have sfp
    // Arg3: Package Parameters
    Method (_DSM, 4, Serialized)
    {
      If (LEqual(Arg0,ToUUID("1A85AA1A-E293-415E-8E28-8D690A0F820A")))
      {
        If (LEqual (Arg1, 0x00))
        {
          Switch (ToInteger(Arg2))
          {
            case (0x0)
            {
              Return (Buffer () {0x3F})
            }

            //Reset Sequence
            case (0x1)
            {
              Store (DeRefOf (Index (Arg3, 0)), Local0)
              Store (DeRefOf (Index (Arg3, 2)), Local2)
              DRST (Local0, Local2)
            }

            //Set Serdes Loopback
            case (0x2)
            {
              Store (DeRefOf (Index (Arg3, 2)), Local0)
              SRLP (Local0)
            }

            //LED op set
            case (0x3)
            {

            }

            // Get port type (GE or XGE)
            case (0x4)
            {
              Store (H3ST, Local0)
              Return (Local0)
            }

            //Get sfp status
            case (0x5)
            {

            }
          }
        }
      }
      Return (Buffer() {0x00})
    }

    Device (PRT0)
    {
      Name (_ADR, 0x0)
      Name (_DSD, Package () {
                        ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
        Package () {
          Package () {"reg", 0},
          Package () {"phy-mode", "sgmii"},
          Package () {"phy-addr", 0},
          Package () {"mdio-node", Package (){\_SB.MDIO}},
                        }
                })
    }
  }
  Device (DSF2)
  {
    OperationRegion(H3SR, SystemMemory, 0xC0000180, 4)
    Field(H3SR, DWordAcc, NoLock, Preserve) {
          H3ST, 1,
          , 31,  //RESERVED
        }

    // XGE RESET
    OperationRegion(XRER, SystemMemory, 0xC0000A10, 8)
    Field(XRER, DWordAcc, NoLock, Preserve) {
          XRTE, 31,
          , 1,    //RESERVED
          XRTD, 31,
          , 1,    //RESERVED
        }

    // GE RESET
    OperationRegion(GRTR, SystemMemory, 0xC0000A18, 16)
    Field(GRTR, DWordAcc, NoLock, Preserve) {
          GR0E, 30,
          , 2,    //RESERVED
          GR0D, 30,
          , 2,    //RESERVED
          GR1E, 18,
          , 14,  //RESERVED
          GR1D, 18,
          , 14,  //RESERVED
        }

    // PPE RESET
    OperationRegion(PRTR, SystemMemory, 0xC0000A48, 8)
    Field(PRTR, DWordAcc, NoLock, Preserve) {
          PRTE, 10,
          , 22,  //RESERVED
          PRTD, 10,
          , 22,  //RESERVED
        }

    // ROCE

    // CPLD LED

    // Serdes
    OperationRegion(H3LR, SystemMemory, 0xC2208500, 0x4)
    Field(H3LR, DWordAcc, NoLock, Preserve) {
          H3L1, 16,    // debug port1
          , 16,    //RESERVED
        }
                Name(_HID, "HISI00B1")
                Name (_CCA, 1) // Cache-coherent controller
                Name (_CRS, ResourceTemplate (){
                        Memory32Fixed (ReadWrite, 0xc2100000 , 0x890000)

                Interrupt (ResourceConsumer, Edge, ActiveHigh, Exclusive,,,)
        {
          20, 21,
        }
                })
                Name (_DSD, Package () {
                        ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
        Package () {
                                Package () {"mode", "single-port"},
                                Package () {"buf-size", 4096},
                                Package () {"desc-num", 1024},
        Package () {"interrupt-parent", Package() {\_SB.MBI1}},
                        }
                })

      // Set Serdes Loopback
    //Arg0 : port
    //Arg1 : 0 disable, 1 enable
    Method(SRLP, 1, Serialized) {
      ShiftLeft (Arg0, 10, Local0)
      Store (H3L1, Local1)
      And (Local1, 0xfffffbff, Local1)
      Or (Local0, Local1, Local0)
      Store (Local1, H3L1)
    }

    //Reset
    //Arg0 : reset type (1: dsaf; 2: ppe; 3:reserved; 4:reserved; 5:G3)
    //Arg1 : 0 disable, 1 enable
    Method(DRST, 2, Serialized)
    {
      Switch (ToInteger(Arg0))
      {
        //DSAF reset
        case (0x1)
        {
          If (LEqual (Arg1, 0)) {
            Store (0x200, PRTE)
          } Else {
            Store (0x2200, PRTD)
          }
        }

        //Reset PPE port
        case (0x2)
        {
          If (LEqual (Arg1, 0)) {
            Store(0x80, PRTE)
          } Else  {
            Store(0x80, PRTD)
          }
        }

        //Reset GE port
        case (0x5)
        {
          If (LEqual (Arg1, 0)) {
            Store(0x2aa80, GR1E)
            Store(0x200, PRTE)
          } Else {
            Store(0x2aa80, GR1D)
            Store(0x200, PRTD)
          }
        }
      }
    }
    // _DSM Device Specific Method
    //
    // Arg0: UUID Unique function identifier
    // Arg1: Integer Revision Level
    // Arg2: Integer Function Index
    //   0 : Return Supported Functions bit mask
    //   1 : Reset sequence
    //    Arg3[0] : reset type (1:dsaf; 2:ppe; 3:reserved; 4:reserved; 5: ge)
    //    Arg3[1] : reserved
    //    Arg3[2] : 0 reset, 1 cancle reset
    //   2 : Set Serdes Loopback
    //    Arg3[0] : reserved
    //    Arg3[1] : 0 disable, 1 enable
    //   3 : LED op set
    //    Arg3[0] : op type
    //    Arg3[1] : reserved
    //    Arg3[2] : para
    //   4 : Get port type (GE or XGE)
    //    Arg3[0] : reserved
    //    Return : 0 GE, 1 XGE
    //   5 : Get sfp status
    //    Arg3[0] : reserved
    //    Return : 0 no sfp, 1 have sfp
    // Arg3: Package Parameters
    Method (_DSM, 4, Serialized)
    {
      If (LEqual(Arg0,ToUUID("1A85AA1A-E293-415E-8E28-8D690A0F820A")))
      {
        If (LEqual (Arg1, 0x00))
        {
          Switch (ToInteger(Arg2))
          {
            case (0x0)
            {
              Return (Buffer () {0x3F})
            }

            //Reset Sequence
            case (0x1)
            {
              Store (DeRefOf (Index (Arg3, 0)), Local0)
              Store (DeRefOf (Index (Arg3, 2)), Local2)
              DRST (Local0, Local2)
            }

            //Set Serdes Loopback
            case (0x2)
            {
              Store (DeRefOf (Index (Arg3, 2)), Local0)
              SRLP (Local0)
            }

            //LED op set
            case (0x3)
            {

            }

            // Get port type (GE or XGE)
            case (0x4)
            {
              Store (H3ST, Local0)
              Return (Local0)
            }

            //Get sfp status
            case (0x5)
            {

            }
          }
        }
      }
      Return (Buffer() {0x00})
    }

    Device (PRT0)
    {
      Name (_ADR, 0x0)
      Name (_DSD, Package () {
                        ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
        Package () {
          Package () {"reg", 0},
          Package () {"phy-mode", "sgmii"},
          Package () {"phy-addr", 1},
          Package () {"mdio-node", Package (){\_SB.MDIO}},
                        }
                })
    }
  }
*/
  Device (ETH5) {
    Name(_HID, "HISI00C1")
    Name (_CCA, 1) // Cache-coherent controller
    Name (_DSD, Package () {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
        Package () {"local-mac-address", Package () { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }}, // 6-bytes
        Package () {"ae-handle", Package (){\_SB.DSF0}},
        Package () {"port-idx-in-ae", 5},
      }
    })
  }
  Device (ETH4) {
    Name(_HID, "HISI00C1")
    Name (_CCA, 1) // Cache-coherent controller
    Name (_DSD, Package () {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
        Package () {"local-mac-address", Package () { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }}, // 6-bytes
        Package () {"ae-handle", Package (){\_SB.DSF0}},
        Package () {"port-idx-in-ae", 4},
      }
    })
  }
  Device (ETH0) {
    Name(_HID, "HISI00C1")
    Name (_CCA, 1) // Cache-coherent controller
    Name (_DSD, Package () {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
        Package () {"local-mac-address", Package () { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }}, // 6-bytes
        Package () {"ae-handle", Package (){\_SB.DSF0}},
        Package () {"port-idx-in-ae", 0},
      }
    })
  }
  Device (ETH1) {
    Name(_HID, "HISI00C1")
    Name (_CCA, 1) // Cache-coherent controller
    Name (_DSD, Package () {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
        Package () {"local-mac-address", Package () { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }}, // 6-bytes
        Package () {"ae-handle", Package (){\_SB.DSF0}},
        Package () {"port-idx-in-ae", 1},
      }
    })
  }

}
