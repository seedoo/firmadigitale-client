@echo off
@setlocal enableextensions
@cd /d "%~dp0"
SET ProgFiles86Root=%ProgramFiles(x86)%
IF NOT "%ProgFiles86Root%"=="" GOTO win64
SET ProgFiles86Root=%ProgramFiles%
:win64

"openssl.exe" x509 -in certificate.pem -text
pause