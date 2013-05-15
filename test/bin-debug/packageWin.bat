@echo off
if not exist ..\..\config.bat (
    echo "FRESteamWorks/config.bat is not set up!"
    goto :eof
)
call ..\..\config.bat

set ANE_PATH=..\..\lib\bin

copy "%STEAM_SDK%\redistributable_bin\steam_api.dll" .
copy "%STEAM_SDK%\redistributable_bin\osx32\libsteam_api.dylib" .

call "%AIR_SDK%\bin\adt.bat" -package -XnoAneValidate -tsa none ^
                             -storetype pkcs12 -keystore CertificateTest.p12 ^
                             -storepass test -target bundle FRESteamWorksTest ^
                             FRESteamWorksTest-app.xml FRESteamWorksTest.swf ^
                             steam_api.dll libsteam_api.dylib steam_appid.txt ^
                             -extdir "%ANE_PATH%"

del steam_api.dll libsteam_api.dylib
