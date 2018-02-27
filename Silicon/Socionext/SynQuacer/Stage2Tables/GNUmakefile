## @file
#
#  Copyright (c) 2018, Linaro, Ltd. All rights reserved.<BR>
#
#  This program and the accompanying materials
#  are licensed and made available under the terms and conditions of the BSD License
#  which accompanies this distribution.  The full text of the license may be found at
#  http://opensource.org/licenses/bsd-license.php
#
#  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
#  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#
##

.PHONY: all
all: $(OUTPUT_DIR)/Stage2Tables.bin

$(OUTPUT_DIR)/Stage2Tables.bin: $(OUTPUT_DIR)/Stage2Tables.elf
	$(OBJCOPY) $(OBJCOPY_FLAGS) $(^) $(@)

$(OUTPUT_DIR)/Stage2Tables.elf: $(MODULE_DIR)/Stage2Tables.S
	$(ASM) $(ASM_FLAGS) -o $(@) $(^)
