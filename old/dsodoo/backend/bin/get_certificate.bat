@echo off
@setlocal enableextensions
@cd /d "%~dp0"
SET ProgFiles86Root=%ProgramFiles(x86)%
IF NOT "%ProgFiles86Root%"=="" GOTO win64
SET ProgFiles86Root=%ProgramFiles%

:win64

echo Usage: get_certificate.bat SLOTID
echo.
echo.
"%ProgFiles86Root%\OpenSC Project\OpenSC\tools\pkcs11-tool.exe" --module /usr/local/ssl/lib/bit4ipki.dll -r --type cert --id %1 > "%cd%\certificate.der"
"openssl.exe" x509 -in certificate.der -inform der -text -out certificate.pem
pause