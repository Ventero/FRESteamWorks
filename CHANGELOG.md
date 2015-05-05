Changelog
=========

The builds described in this file can be [downloaded here](http://dump.ventero.de/FRESteamWorks/).
A full list of implemented functions can be found by following the link in the
version number.

### [HEAD](https://github.com/Ventero/FRESteamWorks/blob/HEAD/lib/API.txt)
 - Added `setEnv` to allow setting environment variables.
 - Fixed `enumerateUserPublishedFilesResult` to correctly return all results (issue #13).
 - Implemented `indicateAchievementProgress`.
 - Removed the deprecated (and broken) `useCrashHandler`.

### [v0.5-2-gf59d5ba](https://github.com/Ventero/FRESteamWorks/blob/v0.5-2-gf59d5ba/lib/API.txt)
 - Fixed `microTxnResult` to properly return null if no results available, and correctly initialize the `authorized` property for returned objects.

### [v0.5](https://github.com/Ventero/FRESteamWorks/blob/v0.5/lib/API.txt)
 - Implemented `setOverlayNotificationPosition`
 - Added a callback for microtransaction authorizations (thanks to @Unsanctioned)
 - Added `restartAppIfNecessary` (again, thanks to @Unsanctioned)
 - Exposed `startProcess(path:String)` on Linux, which has to be called before any other API function if the API wrapper binary is in a non-standard location
 - Deprecated the (undocumented) `path` argument to `init` on Linux and `useCrashHandler`

### [v0.4-54-gdd1b399](https://github.com/Ventero/FRESteamWorks/blob/v0.4-54-gdd1b399/lib/API.txt)
 - Implemented global stats

### [v0.4-46-g8526828](https://github.com/Ventero/FRESteamWorks/blob/v0.4-46-g8526828/lib/API.txt)
 - Fixed `getStat{Int,Float}` on Linux (which would previously always return 0)
 - Added support for basic authentication & ownership checks

### [v0.4-36-g5ff1e97](https://github.com/Ventero/FRESteamWorks/blob/v0.4-36-g5ff1e97/lib/API.txt)
 - Added `findOrCreateLeaderboard`
 - Implemented basic friends API

### [v0.4-30-g207935d](https://github.com/Ventero/FRESteamWorks/blob/v0.4-30-g207935d/lib/API.txt)
 - Handle certain cases of API failures more gracefully
 - Implemented `getEnv`

### [v0.4-19-g0cc78a0](https://github.com/Ventero/FRESteamWorks/blob/v0.4-19-g0cc78a0/lib/API.txt)
 - Work around a bug in Flash Builder where certain classes from the ANE would be ignored.

### [v0.4](https://github.com/Ventero/FRESteamWorks/blob/v0.4/lib/API.txt)
 - Added Steam Workshop support
 - Various leaderboards related functions added
 - Added support for some DLC related functions
 - Implemented several `activateGameOverlayTo___` functions
