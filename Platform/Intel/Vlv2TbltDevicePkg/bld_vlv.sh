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
export WORKSPACE=$(pwd)
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
cd ./edk2

## Clean up previous build files.
if [ -e $CORE_PATH/EDK2.log ]; then
  rm $CORE_PATH/EDK2.log
fi

if [ -e $CORE_PATH/Unitool.log ]; then
  rm $CORE_PATH/Unitool.log
fi

if [ -e $CORE_PATH/Conf/target.txt ]; then
  rm $CORE_PATH/Conf/target.txt
fi

if [ -e $CORE_PATH/Conf/tools_def.txt ]; then
  rm $CORE_PATH/Conf/tools_def.txt
fi

if [ -e $CORE_PATH/Conf/build_rule.txt ]; then
  rm $CORE_PATH/Conf/build_rule.txt
fi

## Setup EDK environment. Edksetup puts new copies of target.txt, tools_def.txt, build_rule.txt in WorkSpace\Conf
## Also run edksetup as soon as possible to avoid it from changing environment variables we're overriding
. edksetup.sh BaseTools
make -C BaseTools

## Define platform specific environment variables.
PLATFORM_PACKAGE=Vlv2TbltDevicePkg
PLATFORM_PKG_PATH=$PLATFORM_PATH/$PLATFORM_PACKAGE
config_file=$PLATFORM_PKG_PATH/PlatformPkgConfig.dsc
auto_config_inc=$PLATFORM_PKG_PATH/AutoPlatformCFG.txt

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
sed -i '/^ACTIVE_PLATFORM/d' Conf/target.txt
sed -i '/^TARGET /d' Conf/target.txt
sed -i '/^TARGET_ARCH/d' Conf/target.txt
sed -i '/^TOOL_CHAIN_TAG/d' Conf/target.txt
sed -i '/^MAX_CONCURRENT_THREAD_NUMBER/d' Conf/target.txt

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
MAX_CONCURRENT_THREAD_NUMBER=1
echo ACTIVE_PLATFORM = $ACTIVE_PLATFORM                           >> Conf/target.txt
echo TARGET          = $TARGET                                    >> Conf/target.txt
echo TOOL_CHAIN_TAG  = $TOOL_CHAIN_TAG                            >> Conf/target.txt
echo MAX_CONCURRENT_THREAD_NUMBER = $MAX_CONCURRENT_THREAD_NUMBER >> Conf/target.txt
if [ $Arch == "IA32" ]; then
  echo TARGET_ARCH   = IA32                                       >> Conf/target.txt
else
  echo TARGET_ARCH   = IA32 X64                                   >> Conf/target.txt
fi

##**********************************************************************
## Build BIOS
##**********************************************************************
echo "Invoking EDK2 build..."
build

##**********************************************************************
## Post Build processing and cleanup
##**********************************************************************
echo Skip "Running fce..."

##**********************************************************************
## Build Capsules
##**********************************************************************
build -p %PLATFORM_PACKAGE%\PlatformCapsule.dsc

echo
echo -------------------- The EDKII BIOS build has successfully completed. --------------------
echo
