#!/usr/bin/env bash
##**********************************************************************
## Function define
##**********************************************************************
function Usage() {
  echo
  echo "***************************************************************************"
  echo "Build BIOS rom for VLV platforms."
  echo
  echo "Usage: bld_vlv.sh  [options] PlatformType [Build Target]"
  echo
  echo "   /c    CleanAll"
  echo "   /l    Generate build log file"
  echo "   /y    Generate build report file"
  echo "   /m    Enable multi-processor build"
  echo "   /IA32 Set Arch to IA32 (default: X64)"
  echo "   /X64  Set Arch to X64 (default: X64)"
  echo
  echo "       Platform Types:  MNW2"
  echo "       Build Targets:   Debug, Release  (default: Debug)"
  echo
  echo "Examples:"
  echo "   bld_vlv.sh MNW2                 : X64 Debug build for MinnowMax"
  echo "   bld_vlv.sh /IA32 MNW2 release   : IA32 Release build for MinnowMax"
  echo "***************************************************************************"
  exit 0
}

set -e

cd ..
echo -e $(date)
##**********************************************************************
## Initial Setup
##**********************************************************************
#build_threads=($NUMBER_OF_PROCESSORS)+1
Build_Flags=
exitCode=0
build_threads=1
Arch=X64
GenLog=FALSE
GenReport=FALSE
Clean=FALSE


export CORE_PATH=$WORKSPACE/edk2
export PLATFORM_PATH=$WORKSPACE/edk2-platforms/Platform/Intel/
export SILICON_PATH=$WORKSPACE/edk2-platforms/Silicon/Intel/
export BINARY_PATH=$WORKSPACE/edk2-non-osi/Silicon/Intel/
export PACKAGES_PATH=$PLATFORM_PATH:$SILICON_PATH:$BINARY_PATH:$CORE_PATH

## Setup EDK environment. Edksetup puts new copies of target.txt, tools_def.txt, build_rule.txt in WorkSpace\Conf
## Also run edksetup as soon as possible to avoid it from changing environment variables we're overriding
cd $CORE_PATH
. edksetup.sh BaseTools
make -C BaseTools

## Define platform specific environment variables.
PLATFORM_NAME=Vlv2TbltDevicePkg
PLATFORM_PACKAGE=Vlv2TbltDevicePkg
PLATFORM_PKG_PATH=$PLATFORM_PATH/$PLATFORM_PACKAGE

cd $PLATFORM_PKG_PATH

##**********************************************************************
## Parse command line arguments
##**********************************************************************

## Optional arguments
for (( i=1; i<=$#; ))
  do
    if [ "$1" == "/?" ]; then
      Usage
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/L" ]; then
      GenLog=TRUE
      shift
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/Y" ]; then
      GenReport=TRUE
      shift
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/M" ]; then
      build_threads=8
      shift
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/C" ]; then
      Clean=TRUE
      shift
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/X64" ]; then
      Arch=X64
      shift
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/IA32" ]; then
      Arch=IA32
      shift
    else
      break
    fi
  done

## Required argument(s)
if [ "$2" == "" ]; then
  Usage
fi

echo "Setting  $1  platform configuration and BIOS ID..."

if [ "$(echo $2 | tr 'a-z' 'A-Z')" == "RELEASE" ]; then
  export TARGET=RELEASE
else
  export TARGET=DEBUG
fi

##**********************************************************************
## Detect TOOL_CHAIN_TAG
##**********************************************************************
gcc_version=$(gcc -v 2>&1 | tail -1 | awk '{print $3}')
case $gcc_version in
      [1-3].*|4.[0-7].*)
        echo MNW2 requires GCC4.8 or later
        exit 1
        ;;
      4.8.*)
        export TOOL_CHAIN_TAG=GCC48
        ;;
      4.9.*|6.[0-2].*)
        export TOOL_CHAIN_TAG=GCC49
        ;;
      *)
        export TOOL_CHAIN_TAG=GCC5
        ;;
esac

##**********************************************************************
## Generate $BUILD_PATH and make sure the directory exists
##**********************************************************************
if [ ! -d ${WORKSPACE}/Build ]; then
  mkdir ${WORKSPACE}/Build
fi

if [ ! -d ${WORKSPACE}/Build/${PLATFORM_NAME}${Arch} ]; then
  mkdir ${WORKSPACE}/Build/${PLATFORM_NAME}${Arch}
fi
BUILD_PATH=${WORKSPACE}/Build/${PLATFORM_NAME}${Arch}/${TARGET}_${TOOL_CHAIN_TAG}

if [ ! -d $BUILD_PATH ]; then
  mkdir $BUILD_PATH
fi

##**********************************************************************
## Check for clean operation
##**********************************************************************
if [ $Clean == "TRUE" ]; then
  echo Removing previous build files ...
  if [ -d ${BUILD_PATH} ]; then
    rm -r ${BUILD_PATH}
  fi
  if [ -d ${WORKSPACE}/Conf/.cache ]; then
    rm -r ${WORKSPACE}/Conf/.cache
  fi
  exit 0
fi

##**********************************************************************
## Generate Build_Flags
##**********************************************************************

Build_Flags="$Build_Flags -b $TARGET"
if [ $Arch == "IA32" ]; then
  Build_Flags="$Build_Flags -a IA32"
else
  Build_Flags="$Build_Flags -a IA32 -a X64"
fi
Build_Flags="$Build_Flags -t $TOOL_CHAIN_TAG"
Build_Flags="$Build_Flags -n $build_threads"
Capsule_Build_Flags="$Build_Flags"
Build_Flags="$Build_Flags -p ${PLATFORM_PKG_PATH}/PlatformPkg${Arch}.dsc"
if [ $GenLog == "TRUE" ]; then
  Build_Flags="$Build_Flags -j ${BUILD_PATH}/${PLATFORM_NAME}.log"
fi
if [ $GenReport == "TRUE" ]; then
  Build_Flags="$Build_Flags -y ${BUILD_PATH}/${PLATFORM_NAME}.report"
fi

##**********************************************************************
## Generate BIOS ID
##**********************************************************************

echo BOARD_ID       = MNW2MAX >  $BUILD_PATH/BiosId.env
echo BOARD_REV      = L       >> $BUILD_PATH/BiosId.env
if [ $Arch == "IA32" ]; then
  echo BOARD_EXT      = I32   >> $BUILD_PATH/BiosId.env
fi
if [ $Arch == "X64" ]; then
  echo BOARD_EXT      = X64   >> $BUILD_PATH/BiosId.env
fi
echo VERSION_MAJOR  = 0090    >> $BUILD_PATH/BiosId.env
if [ $TARGET == "DEBUG" ]; then
  echo BUILD_TYPE     = D     >> $BUILD_PATH/BiosId.env
fi
if [ $TARGET == "RELEASE" ]; then
  echo BUILD_TYPE     = R     >> $BUILD_PATH/BiosId.env
fi
echo VERSION_MINOR  = 01      >> $BUILD_PATH/BiosId.env

python $WORKSPACE/edk2-platforms/Platform/Intel/Tools/GenBiosId/GenBiosId.py -i $BUILD_PATH/BiosId.env -o $BUILD_PATH/BiosId.bin -ot $BUILD_PATH/BiosId.txt

##**********************************************************************
## Build BIOS
##**********************************************************************
echo "Invoking EDK2 build..."
echo build $Build_Flags
build $Build_Flags

cp -f $BUILD_PATH/FV/VLV.fd $BUILD_PATH/FV/Vlv.ROM

##**********************************************************************
## Build Capsules
##**********************************************************************
echo "Invoking EDK2 build for capsules..."
echo build $Capsule_Build_Flags -p $PLATFORM_PKG_PATH/PlatformCapsule${Arch}.dsc
build      $Capsule_Build_Flags -p $PLATFORM_PKG_PATH/PlatformCapsule${Arch}.dsc

echo
echo -------------------- The EDKII BIOS build has successfully completed. --------------------
echo
