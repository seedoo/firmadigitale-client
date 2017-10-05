@echo off
@setlocal enableextensions
@cd /d "%~dp0"
SET ProgFiles86Root=%ProgramFiles(x86)%
IF NOT "%ProgFiles86Root%"=="" GOTO win64
SET ProgFiles86Root=%ProgramFiles%
:win64

echo.
echo Now create a new app on backend\dsodoo folder
echo.
pause

python setup.py py2exe
xcopy /R /C /E /H /Y dist\dsodoo.exe ..\backend\dsodoo\
xcopy /R /C /E /H /Y dist\library.zip ..\backend\dsodoo\
xcopy /R /C /E /H /Y dist\icon_dsodoo.ico ..\backend\dsodoo\
xcopy /R /C /E /H /Y dist\config.ini ..\backend\dsodoo\
xcopy /R /C /E /H /Y dist\cardko.png ..\backend\dsodoo\
xcopy /R /C /E /H /Y dist\cardok.png ..\backend\dsodoo\
xcopy /R /C /E /H /Y dist\header_logo.png ..\backend\dsodoo\
xcopy /R /C /E /H /Y dist\main.glade ..\backend\dsodoo\
xcopy /R /C /E /H /Y dist\pin.glade ..\backend\dsodoo\
xcopy /R /C /E /H /Y dist\config.glade ..\backend\dsodoo\
attrib +h ..\backend\dsodoo\*.png
attrib +h ..\backend\dsodoo\*.zip
attrib +h ..\backend\dsodoo\*.glade
rmdir /S dist
rmdir /S build
del /Q /S *.pyc
del /Q /S *.log