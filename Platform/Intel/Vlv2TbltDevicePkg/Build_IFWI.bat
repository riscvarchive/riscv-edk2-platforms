@REM @file
@REM   Windows batch file to build BIOS ROM
@REM
@REM Copyright (c) 2006 - 2019, Intel Corporation. All rights reserved.<BR>
@REM SPDX-License-Identifier: BSD-2-Clause-Patent
@REM

@echo off

SetLocal EnableDelayedExpansion EnableExtensions

@REM Go to work space directory.
cd ..
cd ..

:: Assign initial values
set exitCode=0
set "Build_Flags= "
set "Stitch_Flags= "
set Arch=X64
set PLATFORM_PACKAGE=Vlv2TbltDevicePkg

set PLATFORM_PATH=%WORKSPACE%
if not exist %PLATFORM_PATH%\%PLATFORM_PACKAGE% (
  if defined PACKAGES_PATH (
    for %%i IN (%PACKAGES_PATH%) DO (
      if exist %%~fi\%PLATFORM_PACKAGE% (
        set PLATFORM_PATH=%%~fi
        goto PlatformPackageFound
      )
    )
  ) else (
    echo.
    echo !!! ERROR !!! Cannot find %PLATFORM_PACKAGE% !!!
    echo.
    goto Exit
  )
)
:PlatformPackageFound

:: Parse Optional arguments
:OptLoop
if /i "%~1"=="/?" goto Usage

if /i "%~1"=="/q" (
    set Build_Flags=%Build_Flags% /q
    shift
    goto OptLoop
)
if /i "%~1"=="/l" (
    set Build_Flags=%Build_Flags% /l
    shift
    goto OptLoop
)
if /i "%~1"=="/y" (
    set Build_Flags=%Build_Flags% /y
    shift
    goto OptLoop
)
if /i "%~1"=="/m" (
    set Build_Flags=%Build_Flags% /m
    shift
    goto OptLoop
)
if /i "%~1" == "/c" (
    set Build_Flags=%Build_Flags% /c
    shift
    goto OptLoop
)
if /i "%~1" == "/ECP" (
    set Build_Flags=%Build_Flags% /ecp
    shift
    goto OptLoop
)

if /i "%~1"=="/s" (
    set Build_Flags=%Build_Flags% /s
    shift
    goto OptLoop
)

if /i "%~1"=="/x64" (
    set Arch=X64
    set Build_Flags=%Build_Flags% /x64
    shift
    goto OptLoop
)

if /i "%~1"=="/IA32" (
    set Arch=IA32
    set Build_Flags=%Build_Flags% /IA32
    shift
    goto OptLoop
)

if /i "%~1"=="/nG" (
    set Stitch_Flags=%Stitch_Flags% /nG
    shift
    goto OptLoop
)
if /i "%~1"=="/nM" (
    set Stitch_Flags=%Stitch_Flags% /nM
    shift
    goto OptLoop
)
if /i "%~1"=="/nB" (
    set Stitch_Flags=%Stitch_Flags% /nB
    shift
    goto OptLoop
)
if /i "%~1"=="/yL" (
    set Stitch_Flags=%Stitch_Flags% /yL
    shift
    goto OptLoop
)

:: Require 2 input parameters
if "%~2"=="" goto Usage

:: Assign required arguments
set Platform_Type=%~1
set Build_Target=%~2

:: Build BIOS
echo ======================================================================
echo Build_IFWI:  Calling BIOS build Script...

call %PLATFORM_PATH%\%PLATFORM_PACKAGE%\bld_vlv.bat %Build_Flags%  %Platform_Type% %Build_Target%

if %ERRORLEVEL% NEQ 0 (
    echo echo  -- Error Building BIOS  & echo.
    set exitCode=1
    goto exit
)
echo.
echo Finished Building BIOS.
goto Exit

:Usage
echo Script to build BIOS firmware and stitch the entire IFWI.
echo.
echo Usage: Build_IFWI.bat [options]  PlatformType  BuildTarget  [IFWI Suffix]
echo.
echo        /c     CleanAll before building
echo        /x64   Set Arch to X64  (default: X64)
echo        /IA32  Set Arch to IA32 (default: X64)
echo        /yL    Enable SPI lock
echo. 
echo        Platform Types:   MNW2
echo        Build Targets:    Release, Debug
echo        IFWI Suffix:      Suffix to append to end of IFWI filename (default: MM_DD_YYYY)
echo.
echo        See  Stitch/Stitch_Config.txt  for additional stitching settings.
echo.
echo        If capsule update is needed, please update CAPSULE_ENABLE = TRUE in Config.dsc.
echo        If recovery is needed, please update RECOVERY_ENABLE = TRUE in Config.dsc.
echo        If either of above is TRUE, please set OPENSSL_PATH in windows evironment
echo        and put openssl.exe there, to generate final capsule image.
echo.
set exitCode=1

:Exit
@REM  CD to platform package.
cd %PLATFORM_PATH%
exit /b %exitCode%

EndLocal
