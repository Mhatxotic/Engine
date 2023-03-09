@echo off

rem This file is part of the Mhatxotic Engine source repository.
rem @ https://github.com/Mhatxotic/Engine
rem Copyright (c) Mhatxotic Design, 2006-present. All Rights Reserved.

set dir=%cd%
cd \assets\engine
if %errorlevel% neq 0 exit /b

if "%1"=="clang" goto clang
if "%1"=="ms32" goto ms32
if "%1"=="ms64" goto ms64
if "%1"=="ms64d" goto ms64d
if "%1"=="res" goto res
echo Usage: %0 [ms32/ms64/ms64d/clang/res]
goto end

:res
rc -nologo -Isrc -fodebug/engine.res win32/engine.rc
goto end

:ms32
cl -nologo -std:c++20 -Z7 -GF -GL -GS- -O2 -Gy -EHsc -DRELEASE -Iinclude -Iinclude/ft -Fodebug/build.obj -Fddebug/build.pdb -Febin/build.exe src/build.cpp lib/glfw32.lib lib/ssl32.lib lib/lzma32.lib lib/zlib32.lib kernel32.lib user32.lib gdi32.lib ole32.lib version.lib imagehlp.lib dbghelp.lib advapi32.lib psapi.lib winmm.lib comctl32.lib shell32.lib ws2_32.lib crypt32.lib debug/engine.res -link -stack:2097152,1048576 -pdb:debug/build.pdb
goto end

:ms64d
cl -nologo -std:c++20 -GF -MTd -Od -GS -Gs0 -RTCsu -bigobj -Gy -EHsc -DALPHA -Iinclude -Iinclude/ft -Fodebug/build.obj -Fddebug/build.pdb -Febin/build.exe src/build.cpp lib/glfw64d.lib lib/ssl64d.lib lib/lzma64d.lib lib/zlib64d.lib kernel32.lib user32.lib gdi32.lib ole32.lib version.lib imagehlp.lib dbghelp.lib advapi32.lib psapi.lib winmm.lib comctl32.lib shell32.lib ws2_32.lib crypt32.lib debug/engine.res -link -stack:2097152,1048576 -pdb:debug/build.pdb
goto end

:ms64
cl -nologo -std:c++20 -Z7 -GF -GL -GS- -O2 -Gy -EHsc -DRELEASE -Iinclude -Iinclude/ft -Fodebug/build.obj -Fddebug/build.pdb -Febin/build.exe src/build.cpp lib/glfw64.lib lib/ssl64.lib lib/lzma64.lib lib/zlib64.lib kernel32.lib user32.lib gdi32.lib ole32.lib version.lib imagehlp.lib dbghelp.lib advapi32.lib psapi.lib winmm.lib comctl32.lib shell32.lib ws2_32.lib crypt32.lib debug/engine.res -link -stack:2097152,1048576 -pdb:debug/build.pdb
goto end

:clang
clang++ -std=c++20 -c -v -O3 -Iinclude -Iinclude/ft -DRELEASE src/build.cpp -o debug/build.o
if %errorlevel% neq 0 goto end
lld-link -fixed -subsystem:console -largeaddressaware -incremental:no -opt:ref,icf -machine:x64 -pdb:debug/build.pdb -out:bin/build.exe libcmt.lib debug/build.o lib/glfw64.lib lib/ssl64.lib lib/lzma64.lib lib/zlib64.lib kernel32.lib user32.lib gdi32.lib ole32.lib version.lib imagehlp.lib dbghelp.lib advapi32.lib psapi.lib winmm.lib comctl32.lib shell32.lib ws2_32.lib crypt32.lib
goto end

:end
cd /d %dir%
set dir=
echo.
