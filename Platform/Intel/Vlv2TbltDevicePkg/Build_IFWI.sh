#!/usr/bin/env bash
##**********************************************************************
## Function define
##**********************************************************************
function Usage ( ) {
  echo
  echo "Script to build BIOS firmware and stitch the entire IFWI."
  echo
  echo "Usage: Build_IFWI.sh [options]  PlatformType  BuildTarget"
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
  echo "   Build_IFWI.sh MNW2 debug           : X64 Debug build for MinnowMax"
  echo "   Build_IFWI.sh /IA32 MNW2 release   : IA32 Release build for MinnowMax"
  echo
  exit 0
}

set -e

## Assign initial values
exitCode=0
Build_Flags=

## Check whether WORKSPACE is set or not
if [[ -z "$WORKSPACE" ]]; then
  echo "Please export WORKSPACE before running Build_IFWI.sh"
  echo "See the details in Readme.md"
  exit 1
fi

## Create $WORKSPACE/Conf if necessary
if [ ! -d $WORKSPACE/Conf ]; then
  mkdir $WORKSPACE/Conf
fi

## Parse Optional arguments
if [ "$1" == "/?" ]; then
  Usage
fi

for (( i=1; i<=$#; ))
  do
    if [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/L" ]; then
      Build_Flags="$Build_Flags /l"
      shift
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/Y" ]; then
      Build_Flags="$Build_Flags /y"
      shift
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/M" ]; then
      Build_Flags="$Build_Flags /m"
      shift
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/C" ]; then
      Build_Flags="$Build_Flags /c"
      shift
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/X64" ]; then
      Build_Flags="$Build_Flags /x64"
      shift
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/IA32" ]; then
      Build_Flags="$Build_Flags /IA32"
      shift
    else
      break
    fi
  done

## Require 2 input parameters
if [ "$2" == "" ]; then
  Usage
fi

## Assign required arguments
Platform_Type=$1
Build_Target=$2

## Build BIOS
echo "======================================================================"
echo "Build_IFWI:  Calling BIOS build Script..."
. bld_vlv.sh $Build_Flags $Platform_Type $Build_Target

echo
echo Finished Building BIOS.

## Start Integration process
echo ======================================================================
echo Skip "Build_IFWI:  Calling IFWI Stitching Script..."

echo
echo Build_IFWI is finished.
echo The final IFWI file is located in Stitch
echo ======================================================================
