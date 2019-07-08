#!/usr/bin/env bash
##**********************************************************************
## Function define
##**********************************************************************
function Usage() {
  echo
  echo "***************************************************************************"
  echo "Build BIOS rom for VLV platforms."
  echo
  echo "Usage: bld_vlv.sh  PlatformType [Build Target]"
  echo
  echo
  echo "       Platform Types:  MNW2"
  echo "       Build Targets:   Debug, Release  (default: Debug)"
  echo
  echo "***************************************************************************"
  echo "Press any key......"
  read
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
Arch=X64
SpiLock=0

export CORE_PATH=$WORKSPACE/edk2
export PLATFORM_PATH=$WORKSPACE/edk2-platforms/Platform/Intel/
export SILICON_PATH=$WORKSPACE/edk2-platforms/Silicon/Intel/
export BINARY_PATH=$WORKSPACE/edk2-non-osi/Silicon/Intel/
export PACKAGES_PATH=$PLATFORM_PATH:$SILICON_PATH:$BINARY_PATH:$CORE_PATH

## Clean up previous build files.
if [ -e $WORKSPACE/EDK2.log ]; then
  rm $WORKSPACE/EDK2.log
fi

if [ -e $WORKSPACE/Unitool.log ]; then
  rm $WORKSPACE/Unitool.log
fi

if [ -e $WORKSPACE/Conf/target.txt ]; then
  rm $WORKSPACE/Conf/target.txt
fi

if [ -e $WORKSPACE/Conf/tools_def.txt ]; then
  rm $WORKSPACE/Conf/tools_def.txt
fi

if [ -e $WORKSPACE/Conf/build_rule.txt ]; then
  rm $WORKSPACE/Conf/build_rule.txt
fi

## Setup EDK environment. Edksetup puts new copies of target.txt, tools_def.txt, build_rule.txt in WorkSpace\Conf
## Also run edksetup as soon as possible to avoid it from changing environment variables we're overriding
cd $CORE_PATH
. edksetup.sh BaseTools
make -C BaseTools

## Define platform specific environment variables.
PLATFORM_NAME=Vlv2TbltDevicePkg
PLATFORM_PACKAGE=Vlv2TbltDevicePkg
PLATFORM_PKG_PATH=$PLATFORM_PATH/$PLATFORM_PACKAGE
config_file=$PLATFORM_PKG_PATH/PlatformPkgConfig.dsc
auto_config_inc=$PLATFORM_PKG_PATH/AutoPlatformCFG.txt

cd $PLATFORM_PKG_PATH

## create new AutoPlatformCFG.txt file
if [ -f "$auto_config_inc" ]; then
  rm $auto_config_inc
fi
touch $auto_config_inc

##**********************************************************************
## Parse command line arguments
##**********************************************************************

## Optional arguments
for (( i=1; i<=$#; ))
  do
    if [ "$1" == "/?" ]; then
      Usage
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/Q" ]; then
      Build_Flags="$Build_Flags --quiet"
      shift
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/L" ]; then
      Build_Flags="$Build_Flags -j EDK2.log"
      shift
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/C" ]; then
      echo Removing previous build files ...
      if [ -d "Build" ]; then
        rm -r Build
      fi
      shift
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/X64" ]; then
      Arch=X64
      shift
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/IA32" ]; then
      Arch=IA32
      shift
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/YL" ]; then
      SpiLock=1
      shift      
    else
      break
    fi
  done

## Required argument(s)
if [ "$2" == "" ]; then
  Usage
fi

if [ $Arch == "IA32" ]; then
  echo DEFINE X64_CONFIG = FALSE >> $auto_config_inc
else
  echo DEFINE X64_CONFIG = TRUE  >> $auto_config_inc
fi

## -- Build flags settings for each Platform --
##    AlpineValley (ALPV):  SVP_PF_BUILD = TRUE,   ENBDT_PF_BUILD = FALSE,  TABLET_PF_BUILD = FALSE,  BYTI_PF_BUILD = FALSE, IVI_PF_BUILD = FALSE
##       BayleyBay (BBAY):  SVP_PF_BUILD = FALSE,  ENBDT_PF_BUILD = TRUE,   TABLET_PF_BUILD = FALSE,  BYTI_PF_BUILD = FALSE, IVI_PF_BUILD = FALSE
##         BayLake (BLAK):  SVP_PF_BUILD = FALSE,  ENBDT_PF_BUILD = FALSE,  TABLET_PF_BUILD = TRUE,   BYTI_PF_BUILD = FALSE, IVI_PF_BUILD = FALSE
##      Bakersport (BYTI):  SVP_PF_BUILD = FALSE,  ENBDT_PF_BUILD = FALSE,  TABLET_PF_BUILD = FALSE,  BYTI_PF_BUILD = TRUE, IVI_PF_BUILD = FALSE
## Crestview Hills (CVHS):  SVP_PF_BUILD = FALSE,  ENBDT_PF_BUILD = FALSE,  TABLET_PF_BUILD = FALSE,  BYTI_PF_BUILD = TRUE, IVI_PF_BUILD = TRUE
##            FFD8 (BLAK):  SVP_PF_BUILD = FALSE,  ENBDT_PF_BUILD = FALSE,  TABLET_PF_BUILD = TRUE,   BYTI_PF_BUILD = FALSE, IVI_PF_BUILD = FALSE
echo "Setting  $1  platform configuration and BIOS ID..."
if [ "$(echo $1 | tr 'a-z' 'A-Z')" == "MNW2" ]; then
  echo DEFINE ENBDT_PF_BUILD = TRUE >> $auto_config_inc
else
  echo "Error - Unsupported PlatformType: $1"
  Usage
fi

if [ "$(echo $2 | tr 'a-z' 'A-Z')" == "RELEASE" ]; then
  export TARGET=RELEASE
else
  export TARGET=DEBUG
fi

##**********************************************************************
## Additional EDK Build Setup/Configuration
##**********************************************************************
echo "Ensuring correct build directory is present for GenBiosId..."

echo Modifying Conf files for this build...
## Remove lines with these tags from target.txt
sed -i '/^ACTIVE_PLATFORM/d' $WORKSPACE/Conf/target.txt
sed -i '/^TARGET /d' $WORKSPACE/Conf/target.txt
sed -i '/^TARGET_ARCH/d' $WORKSPACE/Conf/target.txt
sed -i '/^TOOL_CHAIN_TAG/d' $WORKSPACE/Conf/target.txt
sed -i '/^MAX_CONCURRENT_THREAD_NUMBER/d' $WORKSPACE/Conf/target.txt

gcc_version=$(gcc -v 2>&1 | tail -1 | awk '{print $3}')
case $gcc_version in
      [1-3].*|4.[0-7].*)
        echo MNW2 requires GCC4.8 or later
        exit 1
        ;;
      4.8.*)
        TARGET_TOOLS=GCC48
        ;;
      4.9.*|6.[0-2].*)
        TARGET_TOOLS=GCC49
        ;;
      *)
        TARGET_TOOLS=GCC5
        ;;
esac

ACTIVE_PLATFORM=$PLATFORM_PKG_PATH/PlatformPkgGcc"$Arch".dsc
export TOOL_CHAIN_TAG=$TARGET_TOOLS
MAX_CONCURRENT_THREAD_NUMBER=8
echo ACTIVE_PLATFORM = $ACTIVE_PLATFORM                           >> $WORKSPACE/Conf/target.txt
echo TARGET          = $TARGET                                    >> $WORKSPACE/Conf/target.txt
echo TOOL_CHAIN_TAG  = $TOOL_CHAIN_TAG                            >> $WORKSPACE/Conf/target.txt
echo MAX_CONCURRENT_THREAD_NUMBER = $MAX_CONCURRENT_THREAD_NUMBER >> $WORKSPACE/Conf/target.txt
if [ $Arch == "IA32" ]; then
  echo TARGET_ARCH   = IA32                                       >> $WORKSPACE/Conf/target.txt
else
  echo TARGET_ARCH   = IA32 X64                                   >> $WORKSPACE/Conf/target.txt
fi


## Set and Create $BUILD_PATH if necessary
if [ ! -d ${WORKSPACE}/Build ]; then
  mkdir ${WORKSPACE}/Build
fi

if [ $Arch == "IA32" ]; then
  if [ ! -d ${WORKSPACE}/Build/${PLATFORM_NAME}IA32 ]; then
    mkdir ${WORKSPACE}/Build/${PLATFORM_NAME}IA32
  fi
  BUILD_PATH=${WORKSPACE}/Build/${PLATFORM_NAME}IA32/${TARGET}_${TOOL_CHAIN_TAG}
else
  if [ ! -d ${WORKSPACE}/Build/${PLATFORM_NAME} ]; then
    mkdir ${WORKSPACE}/Build/${PLATFORM_NAME}
  fi
  BUILD_PATH=${WORKSPACE}/Build/${PLATFORM_NAME}/${TARGET}_${TOOL_CHAIN_TAG}
fi

if [ ! -d $BUILD_PATH ]; then
  mkdir $BUILD_PATH
fi

##**********************************************************************
## Generate BIOS ID
##**********************************************************************

echo BOARD_ID       = MNW2MAX >  $BUILD_PATH/BiosId.env
echo BOARD_REV      = 1       >> $BUILD_PATH/BiosId.env
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
build

##**********************************************************************
## Post Build processing and cleanup
##**********************************************************************
echo Skip "Running fce..."

cp -f $BUILD_PATH/FV/VLV.fd $BUILD_PATH/FV/Vlv.ROM

##**********************************************************************
## Build Capsules
##**********************************************************************
build -p $PLATFORM_PKG_PATH/PlatformCapsuleGcc.dsc

echo
echo -------------------- The EDKII BIOS build has successfully completed. --------------------
echo
