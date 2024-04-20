@echo off
pushd %~dp0

:DoStart
where /q cl.exe || goto DoSetVCVars
if not exist build mkdir build
pushd build

set OUTNAME=PhraglibTest
set DEBUG_BUILD=1
set SRC_NAME=lameball.c

set COPTS=-nologo -MT -Gm- -GR- -EHa -Oi -FC -W4 -wd4201 -wd4100 -wd4211
set LINK=-subsystem:windows -opt:ref

:DoBuild
if %DEBUG_BUILD%==1 goto DoDebug
if %DEBUG_BUILD%==0 goto DoRelease

:DoDebug
echo.
echo ===== DEBUG =====
echo.
set DBG_COPTS=-Z7 -Fm %COPTS%
set DBG_DEF=-DBUILD_DEBUG=1 -DWIN32
del /q *.*
cl -I..\src -Fe"%OUTNAME%.exe" %DBG_COPTS% %DBG_DEF% ..\src\%SRC_NAME% ..\src\PL\PL.c -link %LINK%
del /q *.obj
goto DoEnd

:DoRelease
echo.
echo ===== RELEASE =====
echo.
set RLS_COPTS=-O2 %COPTS%
set RLS_DEF=-DWIN32
del /q *.*
cl -I..\src -Fe"%OUTNAME%.exe" %RLS_COPTS% %RLS_DEF% ..\src\%SRC_NAME% ..\src\PL\PL.c -link %LINK%
del /q *.obj
goto DoEnd

:DoSetVCVars
echo == Searching for vcvarsall.bat ==
set vcvarsallfile=
pushd "c:\Program Files"
for /f "delims=" %%a in ('dir vcvarsall* /s /p /b') do set "vcvarsallfile=%%a"
popd
call "%vcvarsallfile%" x64
where /q cl.exe || goto DoErrorNoVCVars
goto DoStart

:DoErrorNoVCVars
echo.
echo ERROR Could not find vcvarsall.bat
echo either it is not on C: drive or VisualStudio is not installed.
echo (or Microsoft have changed the vcvarsall.bat name in the future, which isn't totally unlikely)
echo Ensure VisualStudio is installed in its default location first
echo then run this bat again. Exiting.
echo.
pause
goto DoEnd

:DoEnd
popd
popd