/** LsSerDes.h
 The Header file of SerDes Module for Chassis 2

 Copyright 2017-2019 NXP

 SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef LS_SERDES_H_
#define LS_SERDES_H_

#include <Uefi/UefiBaseType.h>

#define SRDS_MAX_LANES     4

typedef enum {
  None = 0,
  Pcie1,
  Pcie2,
  Pcie3,
  Sata,
  SgmiiFm1Dtsec1,
  SgmiiFm1Dtsec2,
  SgmiiFm1Dtsec5,
  SgmiiFm1Dtsec6,
  SgmiiFm1Dtsec9,
  SgmiiFm1Dtsec10,
  QsgmiiFm1A,
  XfiFm1Mac9,
  XfiFm1Mac10,
  Sgmii2500Fm1Dtsec2,
  Sgmii2500Fm1Dtsec5,
  Sgmii2500Fm1Dtsec9,
  Sgmii2500Fm1Dtsec10,
  SerdesPrtclCount
} SERDES_PROTOCOL;

typedef enum {
  Srds1  = 0,
  Srds2,
  SrdsMaxNum
} SERDES_NUMBER;

typedef struct {
  UINT16 Protocol;
  UINT8  SrdsLane[SRDS_MAX_LANES];
} SERDES_CONFIG;

typedef VOID
(*SERDES_PROBE_LANES_CALLBACK) (
  IN SERDES_PROTOCOL LaneProtocol,
  IN VOID *Arg
  );

VOID
SerDesProbeLanes(
  IN SERDES_PROBE_LANES_CALLBACK SerDesLaneProbeCallback,
  IN VOID *Arg
  );

#endif /* LS_SERDES_H_ */
