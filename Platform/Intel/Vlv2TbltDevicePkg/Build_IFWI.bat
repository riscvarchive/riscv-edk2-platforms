@REM @file
@REM   Windows batch file to build BIOS ROM
@REM
@REM Copyright (c) 2006 - 2019, Intel Corporation. All rights reserved.<BR>
@REM SPDX-License-Identifier: BSD-2-Clause-Patent
@REM

@echo off

SetLocal EnableDelayedExpansion EnableExtensions

:: Assign initial values
set exitCode=0
set "Build_Flags= "
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
if /i "%~1"=="/x64" (
    set Build_Flags=%Build_Flags% /x64
    shift
    goto OptLoop
)
if /i "%~1"=="/IA32" (
    set Build_Flags=%Build_Flags% /IA32
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

call %PLATFORM_PATH%\%PLATFORM_PACKAGE%\bld_vlv.bat %Build_Flags% %Platform_Type% %Build_Target%

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
echo Usage: Build_IFWI.bat [options]  PlatformType  BuildTarget
echo.
echo    /c    CleanAll
echo    /l    Generate build log file
echo    /y    Generate build report file
echo    /m    Enable multi-processor build
echo    /IA32 Set Arch to IA32 (default: X64)
echo    /X64  Set Arch to X64 (default: X64)
echo.
echo        Platform Types:  MNW2
echo        Build Targets:   Debug, Release  (default: Debug)
echo.
echo Examples:
echo    Build_IFWI.bat MNW2 debug           : X64 Debug build for MinnowMax
echo    Build_IFWI.bat /IA32 MNW2 release   : IA32 Release build for MinnowMax
echo.
set exitCode=1

:Exit
@REM  CD to platform package.
cd %PLATFORM_PATH%
exit /b %exitCode%

EndLocal
