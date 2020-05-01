/** @file
*  Header defining the Base addresses, sizes, flags etc for chassis 1
*
*  Copyright 2017-2020 NXP
*
*  SPDX-License-Identifier: BSD-2-Clause-Patent
*
**/

#ifndef NXP_CHASSIS_H_
#define NXP_CHASSIS_H_

/*
 * Setup SMMU in bypass mode
 * and also set its pagesize
 */
VOID
SmmuInit (
  VOID
  );

#endif /* NXP_CHASSIS_H_ */
