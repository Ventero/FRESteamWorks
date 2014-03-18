# Steamworks API for AIR applications #

A simple Steamworks API wrapper to be used as native extension for Adobe AIR
applications. This allows you to implement Steam achievements, leaderboards, cloud
storage, workshop support and much more in your AIR application - on Windows,
OS X and Linux.

This project initially started as a fork of [FRESteamWorks by Oldes/Amanita Design](https://github.com/Oldes/FRESteamWorks/).

# Download #

Pre-built cross-platform ANEs (for both Windows and OS X), as well as Linux
SWCs can be found on http://dump.ventero.de/FRESteamWorks/. The Linux wrapper
binary has to be built manually, see [the Linux section](#linux) for more details.

# Usage #

For Windows/OS X builds, you'll only have to include the ANE in your project like
any normal SWC, add the extension to your application descriptor and include
`extendedDesktop` in the list of supported profiles. For an example, see
[the application descriptor of the FRESteamWorks test application](https://github.com/Ventero/FRESteamWorks/blob/master/test/bin-debug/FRESteamWorksTest-app.xml#L14-17).

Testing your application is a bit more complicated and depends on how your project
is built. For detailed information, see below. For general information regarding
the testing of Steam applications, please refer to the Steamworks SDK documentation.

Since the AIR runtime on Linux doesn't support native extensions, an external
binary is used to communicate with the Steamworks API. For more information
on how to build and include this tool, see [the Linux section](#linux).

## Known Issues ##

Due to a bug in old versions of the AIR runtime, the runtime will crash as soon
as a function is called that returns any of the `com.amanitadesign.steam.*` objects.
For this reason, FRESteamWorks should only be used with AIR runtimes >= 3.7.

Flash Builder 4.6 seems to have an issue with detecting classes that implement
interfaces inside an ANE, which results in the `FRESteamWorks` class not being
visible inside the ANE within Flash Builder when using earlier builds (older than
v0.4-19) of the ANE. This issue is also fixed in Flash Builder 4.7.

With older versions of the ANE (older than v0.4-19), running an application
including that ANE is not possible in Flash Builder 4.6 on OS X. To fix this issue,
simply download to a more recent ANE.

## Flash Builder ##

### Building ###

To include the ANE in your Flash Builder project, simply add it to the list of
"Native Extensions" under "ActionScript Build Path" in your project's properties.
Make sure it gets packaged when building your application by checking the "Package" box
in the "Native Extensions" tab under "ActionScript Build Packaging". The extension
will automatically be added to your application descriptor, but you still might have
to add `extendedDesktop` to the list of supported profiles.

### Running ###

Since by default, Flash Builder uses its installation path as working directory
when running applications, this however isn't enough to test your application in
Flash Builder. This can be fixed by either adding the library's directory to the
dynamic linker's search path (see [the AIR/Flex SDK section](#airflex-sdk) on
how to do this), or by creating a new shortcut to Flash Builder where the working
directory is set to your project's directory and adding the dynamic library to
your project (this does not work on OS X).

### Packaging ###

Just include the native Steamworks library at the top level of your project so
that it gets included when packaging your application.

## AIR/Flex SDK ##

### Building ###

For projects built with the AIR/Flex SDKs, the ANE can be included by simply
adding the ANE path to the external library path (either by adding it to your
build config file, or by adding `-external-library-path+=/path/to/FRESteamWorks.ane`
to your `mxmlc`/`compc` compiler flags). The SWF you're has to be at least version 11.

### Running ###

To run your application with the AIR Debug Launcher (`adl`), you will have to first
unpack the ANE. To do this, rename `FRESteamWorks.ane` to `FRESteamWorks.zip`,
then unzip it and rename the generated folder to `FRESteamWorks.Unpacked.ane`.
Now you can add `-extdir /path/to/folder` to the `adl` command line flags. Here,
`folder` refers the parent folder of the unzipped extension (i.e. the folder that
contains `FRESteamWorks.Unpacked.ane`).

Since the native library included in the ANE is dynamically linked against the
Steamworks library, you'll also have to make sure that the dynamic linker can find
the Steamworks library. When testing, this is easiest done by adding the folder containing
the Steamworks library to your `%PATH%` environment variable on Windows
([example](https://github.com/Ventero/FRESteamWorks/blob/master/test/bin-debug/runWin.bat#L9)),
`DYLD_FALLBACK_LIBRARY_PATH` on OS X ([example](https://github.com/Ventero/FRESteamWorks/blob/master/test/bin-debug/runMac.sh#L6))
or the library itself to `LD_PRELOAD` on Linux ([example](https://github.com/Ventero/FRESteamWorks/blob/master/test/bin-debug/runLinux.sh#L24)).

### Packaging ###

For packaged builds, you'll simply have to include the native Steamworks libraries
in the top level of your build and add the path to a directory containing the
`FRESteamWorks.ane` as `-extdir` to `adt`. For an example, see the test application's
[packaging script](https://github.com/Ventero/FRESteamWorks/blob/master/test/bin-debug/packageWin.bat#L13-18).

## Flash Professional ##

### Building ###

First of all, make sure that your project's target is set to "AIR 3.0 for Desktop"
(or any more recent AIR version). Then, in the AIR preferences (wrench icon next
to target project's target), check the "Extended Desktop" profile and select
"Output as: Application with runtime embedded".

To include the ANE, go to the "Advanced ActionScript 3.0 Settings" (wrench icon
next to the project's script settings), and in the "Library path" tab,
click the native extension button and select `FRESteamWorks.ane`.

### Running ###

See the [Flash Builder section](#running)
on running the application.

### Packaging ###

See the [Flash Builder section](#packaging)
on packaging the application.

# FDT #

FDT is very similar to [Flash Builder](#flash-builder). The main differences
are that the ANE is automatically added to the project and packaged when copying
it into the project's `lib` folder, so that the steps described in
[Flash Builder's building section](#building) are not necessary. However, when adding
the native Steamworks libraries, you have to make sure that they're packaged with
the application by opening the project's properties and adding them under
"FDT AIR Properties" -> "Desktop" -> "Package Contents".

Please also make sure to follow the steps described in [the running section](#running)
to debug your application from within FDT and see the [list of known issues](#known-issues).

# Linux #

The AIR runtime on Linux doesn't support native extensions. Instead, you'll have
to compile your AS3 application against `FRESteamWorksLibLinux.swc` and compile
a wrapper binary that handles the communication between AIR and the Steamworks API.
For details on how to do that, please contact me directly (email address see profile).

# API Documentation #

For a full list of all supported functions, see [lib/API.txt](https://github.com/Ventero/FRESteamWorks/blob/master/lib/API.txt).
In general, the FRESteamWorks API functions are as close representation of the
native Steamworks SDK functions. Thus, for detailed documentation, see the
Steamworks SDK docs.

The objects returned by certain API functions are plain data structures.
For a list of available properties, see [the corresponding source file](https://github.com/Ventero/FRESteamWorks/tree/master/lib/src/com/amanitadesign/steam).

# Building FRESteamWorks #

There shouldn't be any reason for you to manually build the FRESteamWorks.ane,
as pre-built ANEs can be downloaded from http://dump.ventero.de/FRESteamWorks/.
If you need a more recent version than the builds that are available on that site,
you can create an issue in the [bug tracker](https://github.com/Ventero/FRESteamWorks/issues).

If you still want to build the ANE yourself (or build the test application), a few
simple steps have to be followed.

1. Create a config file by copying `config_example.sh` to `config.sh` on OS X,
or `config_example.bat` to `config.bat` and correctly setting up the values within.

2. Open the Visual Studio solution or XCode project (both inside `src/`) and fix
up the include paths (for the Adobe AIR SDK as well as the Steamworks SDK).

3. Build the project in either debug or release mode. This compiles the native
library and automatically builds an ANE for the current platform containing that
library.

4. Optionally, build the test application by running `build.{bat,sh}` inside
`test/bin-debug/` and run it with `runMac.sh` or `runWin.bat`. This automatically
uses the ANE built in step 3.

To build a cross platform ANE, you'll have to compile the native library on both
Windows and OS X and then create an ANE that includes both of those. This is easiest
done by first running `mkdir.sh` in `builds/`, building FRESteamWorks.dll on Windows,
copying it over to the directory created on OS X and then running `compile.sh`
and `build.sh` in that order.

---

# License #

See [LICENSE](https://github.com/Ventero/FRESteamWorks/tree/master/LICENSE).
