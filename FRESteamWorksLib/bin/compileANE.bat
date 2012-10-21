SET AIR_SDK=%USERPROFILE%\Documents\AdobeAIRSDK
set UNZIP_DIR=%ProgramFiles(x86)%\GnuWin32\bin

"%UNZIP_DIR%\unzip" -o FRESteamWorksLib.swc
call "%AIR_SDK%\bin\adt" -package -target ane FRESteamWorks.ane descriptor.xml -swc FRESteamWorksLib.swc -platform Windows-x86 library.swf FRESteamWorks.dll -platform MacOS-x86 library.swf FRESteamWorks.framework
del library.swf
del catalog.xml
