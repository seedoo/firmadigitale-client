@echo off
@setlocal enableextensions
@cd /d "%~dp0"
SET ProgFiles86Root=%ProgramFiles(x86)%
IF NOT "%ProgFiles86Root%"=="" GOTO win64
SET ProgFiles86Root=%ProgramFiles%

:win64

echo "Usage: sing_file.bat SLOTID FILEPATH_TO_SIGN"
echo.
echo.

"openssl.exe" cms -nosmimecap -md sha256 -nodetach -binary -cades -stream -outform DER -sign -signer certificate.pem -inkey %1 -keyform engine -in %2 -out %2.p7m -engine pkcs11
pause