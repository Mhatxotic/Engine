@echo off

rem *** This file is part of the Mhatxotic Engine source repository.
rem *** @ https://github.com/Mhatxotic/Engine
rem *** Copyright (c) Mhatxotic Design, 2006-present. All Rights Reserved.

rem *** Drive and path to assets...
set loc=d:\assets\engine

rem *** Which compiler (specify 'cl' or 'clang-cl')...
set cc=cl

rem *** Release parameters...
set clr=-nologo -Zc:__cplusplus -std:c++20 -Z7 -GF -GL -GS- -O2 -Gy -EHsc -DRELEASE

rem *** Debug parameters...
set cld=-nologo -Zc:__cplusplus -std:c++20 -GF -MTd -Od -GS -Gs0 -RTCsu -bigobj -Gy -EHsc -DALPHA

rem *** Common parameters...
set clc=-Iinclude -Iinclude/ft -Fodebug/build.obj -Fddebug/build.pdb -Febin/build.exe src/build.cpp

rem *** Libraries needed...
set l32=lib/lua32.lib lib/glfw32.lib lib/ssl32.lib lib/lzma32.lib lib/zlib32.lib
set l64=lib/lua64.lib lib/glfw64.lib lib/ssl64.lib lib/lzma64.lib lib/zlib64.lib
set lapi=kernel32.lib user32.lib gdi32.lib ole32.lib version.lib imagehlp.lib dbghelp.lib advapi32.lib psapi.lib winmm.lib comctl32.lib shell32.lib ws2_32.lib crypt32.lib debug/engine.res

rem *** Linker parameters...
set link=-incremental:no -opt:ref,icf -stack:2097152,1048576 -pdb:debug/build.pdb

rem *** Main script...
set dir=%cd%
cd /d %loc%
if %errorlevel% neq 0 exit /b

if "%1"=="ms32" goto ms32
if "%1"=="ms32d" goto ms32d
if "%1"=="ms64" goto ms64
if "%1"=="ms64d" goto ms64d
if "%1"=="res" goto res
echo Usage: %0 [ms32/ms32d/ms64/ms64d/res]
goto end

:res
rc -nologo -Isrc -fodebug/engine.res win32/engine.rc
goto end

:ms32d
%cc% %cld% %clc% %l32% %lapi% -link -largeaddressaware %link%
goto end

:ms32
%cc% %clr% %clc% %l32% %lapi% -link -largeaddressaware %link%
goto end

:ms64d
%cc% %cld% %clc% %l64% %lapi% -link %link%
goto end

:ms64
%cc% %clr% %clc% %l64% %lapi% -link %link%
goto end

:end
cd /d %dir%
set dir=
set link=
set locp=
set locd=
set l64=
set l32=
set clr=
set clc=
set cc=
echo.
