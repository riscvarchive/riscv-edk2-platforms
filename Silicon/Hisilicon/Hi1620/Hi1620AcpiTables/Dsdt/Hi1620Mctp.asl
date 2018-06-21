/** @file
  Differentiated System Description Table Fields (DSDT)

  Copyright (c) 2014, ARM Ltd. All rights reserved.<BR>
  Copyright (c) 2018, Hisilicon Limited. All rights reserved.<BR>
  Copyright (c) 2015, Linaro Limited. All rights reserved.<BR>
    This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

  Based on the files under ArmPlatformPkg/ArmJunoPkg/AcpiTables/

**/
Scope(_SB)
{
  Device(LOC0) {
    Name(_HID, "HISI02F1")
    Name(_UID, 0)
    Name (_CRS, ResourceTemplate ()  {
      Interrupt(ResourceConsumer, Level, ActiveHigh, Exclusive)
      {
        488,489
      }
    })
  }

  Device(MCT0) {
    Name(_HID, "HISI0301")
    Name(_UID, 0)
    Name (_CRS, ResourceTemplate ()  {
      Interrupt (ResourceConsumer, Edge, ActiveHigh, Exclusive, 0, "\\_SB.MBI4")
      {
        656
      }
    })
  }
}
