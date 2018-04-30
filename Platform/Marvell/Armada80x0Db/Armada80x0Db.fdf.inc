#
#  Copyright (C) 2018 Marvell International Ltd. and its affiliates
#
#  This program and the accompanying materials
#  are licensed and made available under the terms and conditions of the BSD License
#  which accompanies this distribution.  The full text of the license may be found at
#  http://opensource.org/licenses/bsd-license.php
#
#  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
#  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#

# Per-board additional content of the DXE phase firmware volume

  INF Silicon/Marvell/Drivers/Gpio/MvPca95xxDxe/MvPca95xxDxe.inf

  # DTB
  INF RuleOverride = DTB Silicon/Marvell/Armada7k8k/DeviceTree/Armada80x0Db.inf

!if $(ARCH) == AARCH64
  # ACPI support
  INF RuleOverride = ACPITABLE Silicon/Marvell/Armada7k8k/AcpiTables/Armada80x0Db.inf
!endif
