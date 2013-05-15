REM absolute paths to all the relevant directories and binaries

REM path to the main AIR SDK directory, so that %AIR_SDK%\bin\adl exists
set "AIR_SDK=%USERPROFILE%\Documents\AdobeAIRSDK"
REM path to the FLEX SDK directory, so that %FLEX_SDK%\bin\compc exists
REM might be identical to AIR_SDK
set "FLEX_SDK=%ProgramFiles(x86)%\FlashDevelop\Tools\flexsdk"
REM path to the Steam SDK directory, so that %STEAM_SDK%\redistributable_bin exist
set "STEAM_SDK=%USERPROFILE%\Documents\Steam\sdk"
REM path to the unzip binary
set "UNZIP=%ProgramFiles(x86)%\GnuWin32\bin\unzip"
