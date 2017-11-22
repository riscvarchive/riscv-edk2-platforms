/** @file
 The Header file of SerDes Module for LS1043A

 Copyright 2017-2019 NXP

 SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef SOC_SERDES_H_
#define SOC_SERDES_H_

#ifdef CHASSIS2
#include <Chassis2/LsSerDes.h>
#endif

SERDES_CONFIG SerDes1ConfigTbl[] = {
        /* SerDes 1 */
  {0x1555, {XfiFm1Mac9, Pcie1, Pcie2, Pcie3 } },
  {0x2555, {Sgmii2500Fm1Dtsec9, Pcie1, Pcie2, Pcie3 } },
  {0x4555, {QsgmiiFm1A, Pcie1, Pcie2, Pcie3 } },
  {0x4558, {QsgmiiFm1A,  Pcie1, Pcie2, Sata } },
  {0x1355, {XfiFm1Mac9, SgmiiFm1Dtsec2, Pcie2, Pcie3 } },
  {0x2355, {Sgmii2500Fm1Dtsec9, SgmiiFm1Dtsec2, Pcie2, Pcie3 } },
  {0x3335, {SgmiiFm1Dtsec9, SgmiiFm1Dtsec2, SgmiiFm1Dtsec5, Pcie3 } },
  {0x3355, {SgmiiFm1Dtsec9, SgmiiFm1Dtsec2, Pcie2, Pcie3 } },
  {0x3358, {SgmiiFm1Dtsec9, SgmiiFm1Dtsec2, Pcie2, Sata } },
  {0x3555, {SgmiiFm1Dtsec9, Pcie1, Pcie2, Pcie3 } },
  {0x3558, {SgmiiFm1Dtsec9, Pcie1, Pcie2, Sata } },
  {0x7000, {Pcie1, Pcie1, Pcie1, Pcie1 } },
  {0x9998, {Pcie1, Pcie2, Pcie3, Sata } },
  {0x6058, {Pcie1, Pcie1, Pcie2, Sata } },
  {0x1455, {XfiFm1Mac9, QsgmiiFm1A, Pcie2, Pcie3 } },
  {0x2455, {Sgmii2500Fm1Dtsec9, QsgmiiFm1A, Pcie2, Pcie3 } },
  {0x2255, {Sgmii2500Fm1Dtsec9, Sgmii2500Fm1Dtsec2, Pcie2, Pcie3 } },
  {0x3333, {SgmiiFm1Dtsec9, SgmiiFm1Dtsec2, SgmiiFm1Dtsec5, SgmiiFm1Dtsec6 } },
  {0x1460, {XfiFm1Mac9, QsgmiiFm1A, Pcie3, Pcie3 } },
  {0x2460, {Sgmii2500Fm1Dtsec9, QsgmiiFm1A, Pcie3, Pcie3 } },
  {0x3460, {SgmiiFm1Dtsec9, QsgmiiFm1A, Pcie3, Pcie3 } },
  {0x3455, {SgmiiFm1Dtsec9, QsgmiiFm1A, Pcie2, Pcie3 } },
  {0x9960, {Pcie1, Pcie2, Pcie3, Pcie3 } },
  {0x2233, {Sgmii2500Fm1Dtsec9, SgmiiFm1Dtsec2, SgmiiFm1Dtsec5, SgmiiFm1Dtsec6 }},
  {0x2533, {Sgmii2500Fm1Dtsec9, Pcie1, SgmiiFm1Dtsec5, SgmiiFm1Dtsec6 } },
  {}
};

SERDES_CONFIG *SerDesConfigTbl[] = {
  SerDes1ConfigTbl
};

#endif /* SOC_SERDES_H_ */
