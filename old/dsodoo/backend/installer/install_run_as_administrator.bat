@echo off
@setlocal enableextensions
@cd /d "%~dp0"
SET ROOT=C:\usr\local\ssl
SET ProgFiles86Root=%ProgramFiles(x86)%
IF NOT "%ProgFiles86Root%"=="" GOTO win64
SET ProgFiles86Root=%ProgramFiles%
:win64

echo Now installing files on: %ROOT% (Everything will be replaced!!!)
echo.
pause
mkdir %ROOT%
xcopy /R /C /E /H /Y ..\bin\*.* "%ROOT%"
echo.

echo Now install OpenSC (abort if previous done)
echo.
pause
"opensc-0.15.0-win32.msi"
xcopy /R /C /E /H /Y "%ROOT%\libeay32.dll" "%ProgFiles86Root%\OpenSC Project\OpenSC\tools\"
xcopy /R /C /E /H /Y "%ROOT%\ssleay32.dll" "%ProgFiles86Root%\OpenSC Project\OpenSC\tools\"
xcopy /R /C /E /H /Y "%ROOT%\libp11-2.dll" "%ProgFiles86Root%\OpenSC Project\OpenSC\tools\"

if "%path%"=="%path:C:\dsodoo\=%" (
    echo Env Path Not Found
    setx path "%path%;C:\dsodoo\;"
)
echo.

echo Now install DsOdoo
echo.
pause
mkdir C:\dsodoo
xcopy /R /C /E /H /Y ..\dsodoo\*.* C:\dsodoo\
dsodoo.reg
assoc .tos=
assoc .tos=tosfile
ftype tosfile="C:\dsodoo\dsodoo.exe" "%%1"
pause

