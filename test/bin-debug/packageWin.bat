SET AIR_SDK=%USERPROFILE%\Documents\AdobeAIRSDK
set STEAM_SDK=%USERPROFILE%\Documents\Steam\sdk
SET ANE_PATH=..\..\FRESteamWorksLib\bin

copy "%STEAM_SDK%\redistributable_bin\steam_api.dll" .
copy "%STEAM_SDK%\redistributable_bin\osx32\libsteam_api.dylib" .

call "%AIR_SDK%\bin\adt.bat" -package -XnoAneValidate -tsa none ^
                             -storetype pkcs12 -keystore CertificateTest.p12 ^
                             -storepass test -target bundle FRESteamWorksTest ^
                             FRESteamWorksTest-app.xml FRESteamWorksTest.swf ^
                             steam_api.dll libsteam_api.dylib steam_appid.txt^
                             -extdir "%ANE_PATH%"

del steam_api.dll libsteam_api.dylib
