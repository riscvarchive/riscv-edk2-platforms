# @file
#   Linux script file to generate UEFI capsules for system firmware and
#   firmware for sample devices
#
# Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
#
# SPDX-License-Identifier: BSD-2-Clause-Patent
#

cd $(dirname $0)

EFI_DIR=$WORKSPACE/Build/Vlv2TbltDevicePkg/${TARGET}_${TOOL_CHAIN_TAG}/X64/
CAP_DIR=$WORKSPACE/Build/Vlv2TbltDevicePkg/Capsules
SCRIPT_DIR=$WORKSPACE/edk2-platforms/Platform/Intel/Vlv2TbltDevicePkg/Feature/Capsule/GenerateCapsule

rm -rf $CAP_DIR
mkdir -p $CAP_DIR/SampleDevelopment
mkdir -p $CAP_DIR/NewCert
mkdir -p $CAP_DIR/TestCert
if [ "$TARGET" == "DEBUG" ]; then
    cp $EFI_DIR/CapsuleApp.efi $CAP_DIR/SampleDevelopment/CapsuleApp.efi
    cp $EFI_DIR/CapsuleApp.efi $CAP_DIR/NewCert/CapsuleApp.efi
    cp $EFI_DIR/CapsuleApp.efi $CAP_DIR/TestCert/CapsuleApp.efi
    . $SCRIPT_DIR/GenCapsuleMinnowMax.sh
else
    cp $EFI_DIR/CapsuleApp.efi $CAP_DIR/SampleDevelopment/CapsuleAppRelease.efi
    cp $EFI_DIR/CapsuleApp.efi $CAP_DIR/NewCert/CapsuleAppRelease.efi
    cp $EFI_DIR/CapsuleApp.efi $CAP_DIR/TestCert/CapsuleAppRelease.efi
    . $SCRIPT_DIR/GenCapsuleMinnowMaxRelease.sh
fi
. $SCRIPT_DIR/GenCapsuleSampleColor.sh Blue  149DA854-7D19-4FAA-A91E-862EA1324BE6
. $SCRIPT_DIR/GenCapsuleSampleColor.sh Green 79179BFD-704D-4C90-9E02-0AB8D968C18A
. $SCRIPT_DIR/GenCapsuleSampleColor.sh Red   72E2945A-00DA-448E-9AA7-075AD840F9D4
