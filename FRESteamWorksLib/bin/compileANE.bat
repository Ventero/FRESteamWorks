SET AIR_SDK=%USERPROFILE%\Documents\AdobeAIRSDK
set FLEX_SDK=%ProgramFiles(x86)%\FlashDevelop\Tools\flexsdk
set UNZIP_DIR=%ProgramFiles(x86)%\GnuWin32\bin

"%FLEX_SDK%\bin\compc" +configname=air -source-path ..\src -optimize -include-classes com.amanitadesign.steam.FRESteamWorks com.amanitadesign.steam.SteamConstants -swf-version=11 -output FRESteamWorksLib.swc
"%UNZIP_DIR%\unzip" -o FRESteamWorksLib.swc
call "%AIR_SDK%\bin\adt" -package -target ane FRESteamWorks.ane descriptor.xml -swc FRESteamWorksLib.swc -platform Windows-x86 library.swf FRESteamWorks.dll -platform MacOS-x86 library.swf FRESteamWorks.framework
del library.swf
del catalog.xml
