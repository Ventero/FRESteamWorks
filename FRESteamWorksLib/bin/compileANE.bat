SET AIR_SDK=C:\AirSDK\air_3.2

unzip -o FRESteamWorksLib.swc
call "%AIR_SDK%\bin\adt" -package -target ane FRESteamWorks.ane descriptor.xml -swc FRESteamWorksLib.swc -platform Windows-x86 library.swf FRESteamWorks.dll
del library.swf
del catalog.xml
