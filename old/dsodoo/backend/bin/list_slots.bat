@echo off
@setlocal enableextensions
@cd /d "%~dp0"
SET ProgFiles86Root=%ProgramFiles(x86)%
IF NOT "%ProgFiles86Root%"=="" GOTO win64
SET ProgFiles86Root=%ProgramFiles%
:win64

"%ProgFiles86Root%\OpenSC Project\OpenSC\tools\pkcs11-tool.exe" --module /usr/local/ssl/lib/bit4ipki.dll -O
pause