# @file
#   Linux script file to generate UEFI capsules for system firmware
#
# Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
#
# SPDX-License-Identifier: BSD-2-Clause-Patent
#

FMP_CAPSULE_VENDOR=Intel
FMP_CAPSULE_GUID=4096267B-DA0A-42EB-B5EB-FEF31D207CB4
FMP_CAPSULE_FILE=MinnowMax.cap
FMP_CAPSULE_VERSION=0x0000000C
FMP_CAPSULE_STRING=0.0.0.12
FMP_CAPSULE_NAME="Intel MinnowMax DEBUG UEFI $FMP_CAPSULE_STRING"
FMP_CAPSULE_LSV=0x00000000
FMP_CAPSULE_PAYLOAD=$WORKSPACE/Build/Vlv2TbltDevicePkg/DEBUG_${TOOL_CHAIN_TAG}/FV/Vlv.ROM
FMP_CAPSULE_DIR=$WORKSPACE/Build/Vlv2TbltDevicePkg/Capsules/

PKCS7SIGN_DIR=$WORKSPACE/edk2/BaseTools/Source/Python/Pkcs7Sign/

if [ ! -e "$FMP_CAPSULE_PAYLOAD" ] ; then
  return
fi

if [ -e NewCert.pem ]; then
  #
  # Sign capsule using OpenSSL with a new certificate
  #
  GenerateCapsule \
    --encode \
    -v \
    --guid $FMP_CAPSULE_GUID \
    --fw-version $FMP_CAPSULE_VERSION \
    --lsv $FMP_CAPSULE_LSV \
    --capflag PersistAcrossReset \
    --capflag InitiateReset \
    --signer-private-cert=NewCert.pem \
    --other-public-cert=NewSub.pub.pem \
    --trusted-public-cert=NewRoot.pub.pem \
    -o $FMP_CAPSULE_DIR/NewCert/$FMP_CAPSULE_FILE \
    $FMP_CAPSULE_PAYLOAD
fi

#
# Sign capsule using OpenSSL with EDK II Test Certificate
#
GenerateCapsule \
  --encode \
  -v \
  --guid $FMP_CAPSULE_GUID \
  --fw-version $FMP_CAPSULE_VERSION \
  --lsv $FMP_CAPSULE_LSV \
  --capflag PersistAcrossReset \
  --capflag InitiateReset \
  --signer-private-cert=$PKCS7SIGN_DIR/TestCert.pem \
  --other-public-cert=$PKCS7SIGN_DIR/TestSub.pub.pem \
  --trusted-public-cert=$PKCS7SIGN_DIR/TestRoot.pub.pem \
  -o $FMP_CAPSULE_DIR/TestCert/$FMP_CAPSULE_FILE \
  $FMP_CAPSULE_PAYLOAD
