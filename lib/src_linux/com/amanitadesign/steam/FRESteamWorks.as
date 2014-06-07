/*
 *  FRESteamWorks.as
 *  This file is part of FRESteamWorks.
 *
 *  Created by Ventero <http://github.com/Ventero>
 *  Copyright (c) 2012-2013 Level Up Labs, LLC. All rights reserved.
 */

package com.amanitadesign.steam {
	import flash.desktop.NativeProcess;
	import flash.desktop.NativeProcessStartupInfo;
	import flash.display.DisplayObjectContainer;
	import flash.events.EventDispatcher;
	import flash.events.IEventDispatcher;
	import flash.events.IOErrorEvent;
	import flash.events.ProgressEvent;
	import flash.events.StatusEvent;
	import flash.filesystem.File;
	import flash.filesystem.FileMode;
	import flash.filesystem.FileStream;
	import flash.net.registerClassAlias;
	import flash.utils.ByteArray;
	import flash.utils.IDataInput;
	import flash.utils.IDataOutput;
	import flash.utils.clearInterval;
	import flash.utils.setInterval;

	public class FRESteamWorks extends EventDispatcher implements ISteamWorks {

		private var _process:NativeProcess;
		private var _tm:int;
		private var _init:Boolean = false;
		private var _error:Boolean = false;
		private var _crashHandlerArgs:Array = null;
		private var _buf:ByteArray;

		public var isReady:Boolean = false;

		// function list automatically generated with generateAPI.rb
		// START GENERATED VALUES
		/********************************/
		/* General Steamworks functions */
		/********************************/
		private static const AIRSteam_Init:int = 0;
		private static const AIRSteam_RunCallbacks:int = 1;
		private static const AIRSteam_GetUserID:int = 2;
		private static const AIRSteam_GetAppID:int = 3;
		private static const AIRSteam_GetAvailableGameLanguages:int = 4;
		private static const AIRSteam_GetCurrentGameLanguage:int = 5;
		private static const AIRSteam_GetPersonaName:int = 6;
		private static const AIRSteam_UseCrashHandler:int = 7;
		private static const AIRSteam_RestartAppIfNecessary:int = 8;
		/************************/
		/* Stats / Achievements */
		/************************/
		private static const AIRSteam_RequestStats:int = 9;
		private static const AIRSteam_SetAchievement:int = 10;
		private static const AIRSteam_ClearAchievement:int = 11;
		private static const AIRSteam_IsAchievement:int = 12;
		private static const AIRSteam_GetStatInt:int = 13;
		private static const AIRSteam_GetStatFloat:int = 14;
		private static const AIRSteam_SetStatInt:int = 15;
		private static const AIRSteam_SetStatFloat:int = 16;
		private static const AIRSteam_StoreStats:int = 17;
		private static const AIRSteam_ResetAllStats:int = 18;
		private static const AIRSteam_RequestGlobalStats:int = 19;
		private static const AIRSteam_GetGlobalStatInt:int = 20;
		private static const AIRSteam_GetGlobalStatFloat:int = 21;
		private static const AIRSteam_GetGlobalStatHistoryInt:int = 22;
		private static const AIRSteam_GetGlobalStatHistoryFloat:int = 23;
		/****************/
		/* Leaderboards */
		/****************/
		private static const AIRSteam_FindLeaderboard:int = 24;
		private static const AIRSteam_FindOrCreateLeaderboard:int = 25;
		private static const AIRSteam_FindLeaderboardResult:int = 26;
		private static const AIRSteam_GetLeaderboardName:int = 27;
		private static const AIRSteam_GetLeaderboardEntryCount:int = 28;
		private static const AIRSteam_GetLeaderboardSortMethod:int = 29;
		private static const AIRSteam_GetLeaderboardDisplayType:int = 30;
		private static const AIRSteam_UploadLeaderboardScore:int = 31;
		private static const AIRSteam_UploadLeaderboardScoreResult:int = 32;
		private static const AIRSteam_DownloadLeaderboardEntries:int = 33;
		private static const AIRSteam_DownloadLeaderboardEntriesResult:int = 34;
		/**************************/
		/* Cloud / Remote Storage */
		/**************************/
		private static const AIRSteam_GetFileCount:int = 35;
		private static const AIRSteam_GetFileSize:int = 36;
		private static const AIRSteam_FileExists:int = 37;
		private static const AIRSteam_FileWrite:int = 38;
		private static const AIRSteam_FileRead:int = 39;
		private static const AIRSteam_FileDelete:int = 40;
		private static const AIRSteam_FileShare:int = 41;
		private static const AIRSteam_FileShareResult:int = 42;
		private static const AIRSteam_IsCloudEnabledForApp:int = 43;
		private static const AIRSteam_SetCloudEnabledForApp:int = 44;
		private static const AIRSteam_GetQuota:int = 45;
		/******************/
		/* UGC / Workshop */
		/******************/
		private static const AIRSteam_UGCDownload:int = 46;
		private static const AIRSteam_UGCRead:int = 47;
		private static const AIRSteam_GetUGCDownloadProgress:int = 48;
		private static const AIRSteam_GetUGCDownloadResult:int = 49;
		private static const AIRSteam_PublishWorkshopFile:int = 50;
		private static const AIRSteam_PublishWorkshopFileResult:int = 51;
		private static const AIRSteam_DeletePublishedFile:int = 52;
		private static const AIRSteam_GetPublishedFileDetails:int = 53;
		private static const AIRSteam_GetPublishedFileDetailsResult:int = 54;
		private static const AIRSteam_EnumerateUserPublishedFiles:int = 55;
		private static const AIRSteam_EnumerateUserPublishedFilesResult:int = 56;
		private static const AIRSteam_EnumeratePublishedWorkshopFiles:int = 57;
		private static const AIRSteam_EnumeratePublishedWorkshopFilesResult:int = 58;
		private static const AIRSteam_EnumerateUserSubscribedFiles:int = 59;
		private static const AIRSteam_EnumerateUserSubscribedFilesResult:int = 60;
		private static const AIRSteam_EnumerateUserSharedWorkshopFiles:int = 61;
		private static const AIRSteam_EnumerateUserSharedWorkshopFilesResult:int = 62;
		private static const AIRSteam_EnumeratePublishedFilesByUserAction:int = 63;
		private static const AIRSteam_EnumeratePublishedFilesByUserActionResult:int = 64;
		private static const AIRSteam_SubscribePublishedFile:int = 65;
		private static const AIRSteam_UnsubscribePublishedFile:int = 66;
		private static const AIRSteam_CreatePublishedFileUpdateRequest:int = 67;
		private static const AIRSteam_UpdatePublishedFileFile:int = 68;
		private static const AIRSteam_UpdatePublishedFilePreviewFile:int = 69;
		private static const AIRSteam_UpdatePublishedFileTitle:int = 70;
		private static const AIRSteam_UpdatePublishedFileDescription:int = 71;
		private static const AIRSteam_UpdatePublishedFileSetChangeDescription:int = 72;
		private static const AIRSteam_UpdatePublishedFileVisibility:int = 73;
		private static const AIRSteam_UpdatePublishedFileTags:int = 74;
		private static const AIRSteam_CommitPublishedFileUpdate:int = 75;
		private static const AIRSteam_GetPublishedItemVoteDetails:int = 76;
		private static const AIRSteam_GetPublishedItemVoteDetailsResult:int = 77;
		private static const AIRSteam_GetUserPublishedItemVoteDetails:int = 78;
		private static const AIRSteam_GetUserPublishedItemVoteDetailsResult:int = 79;
		private static const AIRSteam_UpdateUserPublishedItemVote:int = 80;
		private static const AIRSteam_SetUserPublishedFileAction:int = 81;
		/***********/
		/* Friends */
		/***********/
		private static const AIRSteam_GetFriendCount:int = 82;
		private static const AIRSteam_GetFriendByIndex:int = 83;
		private static const AIRSteam_GetFriendPersonaName:int = 84;
		/******************************/
		/* Authentication & Ownership */
		/******************************/
		private static const AIRSteam_GetAuthSessionTicket:int = 85;
		private static const AIRSteam_GetAuthSessionTicketResult:int = 86;
		private static const AIRSteam_BeginAuthSession:int = 87;
		private static const AIRSteam_EndAuthSession:int = 88;
		private static const AIRSteam_CancelAuthTicket:int = 89;
		private static const AIRSteam_UserHasLicenseForApp:int = 90;
		/***********/
		/* Overlay */
		/***********/
		private static const AIRSteam_ActivateGameOverlay:int = 91;
		private static const AIRSteam_ActivateGameOverlayToUser:int = 92;
		private static const AIRSteam_ActivateGameOverlayToWebPage:int = 93;
		private static const AIRSteam_ActivateGameOverlayToStore:int = 94;
		private static const AIRSteam_ActivateGameOverlayInviteDialog:int = 95;
		private static const AIRSteam_IsOverlayEnabled:int = 96;
		private static const AIRSteam_SetOverlayNotificationPosition:int = 97;
		/***********************/
		/* DLC / subscriptions */
		/***********************/
		private static const AIRSteam_IsSubscribedApp:int = 98;
		private static const AIRSteam_IsDLCInstalled:int = 99;
		private static const AIRSteam_GetDLCCount:int = 100;
		private static const AIRSteam_InstallDLC:int = 101;
		private static const AIRSteam_UninstallDLC:int = 102;
		private static const AIRSteam_DLCInstalledResult:int = 103;
		/********************/
		/* Microtransaction */
		/********************/
		private static const AIRSteam_MicroTxnResult:int = 104;
		/**********************************/
		/* Other non-Steamworks functions */
		/**********************************/
		private static const AIRSteam_GetEnv:int = 105;
		// END GENERATED VALUES

		public function FRESteamWorks (target:IEventDispatcher = null) {
			_process = new NativeProcess();
			_process.addEventListener(ProgressEvent.STANDARD_ERROR_DATA, eventDispatched);
			_process.addEventListener(IOErrorEvent.STANDARD_INPUT_IO_ERROR, errorCallback);
			_buf = new ByteArray();

			registerClassAlias("com.amanitadesign.steam.DownloadUGCResult", DownloadUGCResult);
			registerClassAlias("com.amanitadesign.steam.FileDetailsResult", FileDetailsResult);
			registerClassAlias("com.amanitadesign.steam.FilesByUserActionResult", FilesByUserActionResult);
			registerClassAlias("com.amanitadesign.steam.ItemVoteDetailsResult", ItemVoteDetailsResult);
			registerClassAlias("com.amanitadesign.steam.LeaderboardEntry", LeaderboardEntry);
			registerClassAlias("com.amanitadesign.steam.MicroTxnAuthorizationResponse", MicroTxnAuthorizationResponse);
			registerClassAlias("com.amanitadesign.steam.SubscribedFilesResult", SubscribedFilesResult);
			registerClassAlias("com.amanitadesign.steam.UploadLeaderboardScoreResult", UploadLeaderboardScoreResult);
			registerClassAlias("com.amanitadesign.steam.UserFilesResult", UserFilesResult);
			registerClassAlias("com.amanitadesign.steam.UserStatsConstants", UserStatsConstants);
			registerClassAlias("com.amanitadesign.steam.UserVoteDetails", UserVoteDetails);
			registerClassAlias("com.amanitadesign.steam.WorkshopFilesResult", WorkshopFilesResult);

			super(target);
		}

		public function dispose():void {
			if(_process.running) {
				_process.closeInput();
				_process.exit();
			}
			clearInterval(_tm);
			isReady = false;
		}

		// Call this before init or restartAppIfNecessary to start the APIWrapper process
		public function startProcess(path:String = "NativeApps/Linux/APIWrapper"):Boolean {
			var startupInfo:NativeProcessStartupInfo = new NativeProcessStartupInfo();
			var file:File = File.applicationDirectory.resolvePath(path);
			if (!file.exists) return false;

			startupInfo.executable = file;

			try {
				_process.start(startupInfo);
			} catch(e:Error) {
				return false;
			}

			// Try to start the process a second time, but this time ignoring any
			// potential errors since we might get one about the process already
			// running. This seems to give the runtime enough time to check if the
			// initial attempt to start the process managed to start the process.
			// process.running is unreliable, in that it's set to true even if the
			// process exited shortly after starting. In that case, the callWrapper
			// call in init will fail though, propagating the error back to the caller.
			try {
				_process.start(startupInfo);
			} catch(e:Error) {
				// no-op
			}
			if(!_process.running) return false;

			return true;
		}

		// path argument is deprecated - use startProcess before calling init on Linux
		public function init(path:String = "NativeApps/Linux/APIWrapper"):Boolean {
			if (!_process.running) {
				// Try to start the wrapper process now, but only if it wasn't
				// started previously through an explicit call to startProcess();
				if (!startProcess(path)) return false;
			}

			// initialization seems to be successful
			_init = true;

			// TODO: after the path argument is removed, this can be moved into useCrashHandler
			// UseCrashHandler has to be called before Steam_Init
			// but we still have to make sure the process is actually running
			// so FRESteamWorks#useCrashHandler just sets _crashHandlerArgs
			// and the actual call is handled here
			if(_crashHandlerArgs) {
				if(!callWrapper(AIRSteam_UseCrashHandler, _crashHandlerArgs))
					return false;
			}

			if(!callWrapper(AIRSteam_Init)) return false;
			isReady = readBoolResponse();
			if(isReady) _tm = setInterval(runCallbacks, 100);
			return isReady;
		}

		private function errorCallback(e:IOErrorEvent):void {
			_error = true;
			// the process doesn't accept our input anymore, so just stop it
			clearInterval(_tm);

			if(_process.running) {
				try {
					_process.closeInput();
				} catch(e:*) {
					// no-op
				}

				_process.exit();
			}
		}

		public function addOverlayWorkaround(container:DisplayObjectContainer,
			alwaysVisible:Boolean = false, color:uint = 0x000000):void {
			// no-op for now, since the overlay doesn't work on Linux anyway
		}

		private function callWrapper(funcName:int, params:Array = null):Boolean {
			_error = false;
			if(!_process.running) return false;

			var stdin:IDataOutput = _process.standardInput;
			stdin.writeUTFBytes(funcName + "\n");

			if (params) {
				for(var i:int = 0; i < params.length; ++i) {
					var outbuf:ByteArray = new ByteArray();
					var needTemp:Boolean = writeValue(outbuf, params[i]);
					outbuf.position = 0;
					if (needTemp || outbuf.length > 1024) {
						// send data via tempfile, see readResponse for an explanation
						var tempfile:File = File.createTempFile();
						var stream:FileStream = new FileStream();
						stream.open(tempfile, FileMode.WRITE);
						stream.writeBytes(outbuf);
						stream.close();
						stdin.writeUTFBytes(outbuf.length + "\n");
						stdin.writeUTFBytes(tempfile.nativePath + "\n");
					} else {
						stdin.writeBytes(outbuf);
					}
				}
			}

			return !_error;
		}

		private function writeValue(output:IDataOutput, value:*):Boolean {
			if (value === null || value === undefined) {
				// no data, so length 0
				output.writeUTFBytes(0 + "\n");
			} else if(value is ByteArray) {
				// byte arrays are passed through external files, so we don't need a
				// length marker here
				output.writeBytes(value);
				output.writeUTFBytes("\n");
				return true;
			} else if(value is String) {
				output.writeUTFBytes(String(value.length + 1) + "\n" + value + "\n");
			} else if(value is Array) {
				output.writeUTFBytes(value.length + "\n");
				for (var el:int = 0; el < value.length; ++el) {
					if(!(value[el] is String) && !(value[el] is Number))
						throw new ArgumentError("Only arrays of strings/numbers are supported");
					writeValue(output, value[el]);
				}
			} else if(value is int || value is uint || value is Number || value is Boolean) {
				output.writeUTFBytes(String(value) + "\n");
			} else {
				throw new ArgumentError("Cannot write value " + value);
			}
			return false;
		}

		private function waitForData(output:IDataInput, length:uint = 1):uint {
			// copy over what's left from before
			var tmpBuf:ByteArray = new ByteArray();
			_buf.readBytes(tmpBuf);
			_buf.clear();
			tmpBuf.position = 0;
			tmpBuf.readBytes(_buf);
			_buf.position = 0;

			while(_buf.length < length) {
				// basically just wait for data
				if(!_process.running) return 0;
				try{
					output.readBytes(_buf);
				} catch(e:Error) {
					// for some reason, sometimes the .running check passes even though
					// the process isn't running anymore, leading to an exception here
					return 0;
				}
			}

			_buf.position = 0;
			return _buf.length;
		}

		private function readResponse(forceTempFile:Boolean = false):* {
			if (!_process.running) return null;
			var stdout:IDataInput = _process.standardOutput;

			// first, wait for the length prefix
			if (!waitForData(stdout)) return null;
			var length:uint = _buf.readObject() as uint;

			// AIR seems to have a limit of around 32k bytes data on stdin
			// we can synchronously wait for, so for large data we use a temp
			// file instead. Sometimes the limit seems to be much lower than 32k
			// though, so we define "large data" very conservatively.
			if (forceTempFile || length > 1024) {
				var filename:String = readStringResponse();
				if (!filename) return null;

				var tmpfile:File = new File(filename);
				var stream:FileStream = new FileStream();
				stream.open(tmpfile, FileMode.READ);

				// if we don't have enough data, we're unlikely to get it later,
				// so just skip this call
				var res:* = null;
				if (stream.bytesAvailable >= length)
					res = stream.readObject();

				stream.close();
				tmpfile.deleteFileAsync();
				return res;
			}

			// now, wait for the actual data
			if(!waitForData(stdout, length)) return null;

			return _buf.readObject();

		}

		private function readBoolResponse():Boolean {
			if(!_process.running) return false;
			return readResponse() as Boolean;
		}

		private function readIntResponse():int {
			if(!_process.running) return 0;
			return readResponse() as int;
		}

		private function readFloatResponse():Number {
			if(!_process.running) return 0;
			return readResponse() as Number;
		}

		private function readStringResponse():String {
			if(!_process.running) return "";
			return readResponse() as String;
		}

		private function readByteArrayResponse():ByteArray {
			if(!_process.running) return null;
			return readResponse(true) as ByteArray;
		}

		private function eventDispatched(e:ProgressEvent):void {
			var stderr:IDataInput = _process.standardError;
			var avail:uint = stderr.bytesAvailable;
			var data:String = stderr.readUTFBytes(avail);

			var pattern:RegExp = /__event__<(\d+),(\d+)>/g;
			var result:Object;
			while((result = pattern.exec(data))) {
				var req_type:int = new int(result[1]);
				var response:int = new int(result[2]);
				var steamEvent:SteamEvent = new SteamEvent(SteamEvent.STEAM_RESPONSE, req_type, response);
				dispatchEvent(steamEvent);
			}
		}

		/* special implementations of API functions */
		public function runCallbacks():Boolean {
			if(!callWrapper(AIRSteam_RunCallbacks, [])) return false;
			return true;
		}

		public function useCrashHandler(appID:uint, version:String, date:String, time:String):Boolean {
			// only allow calls before SteamAPI_Init was called
			if(_init) return false;
			if (_process.running)
			{
				// call the wrapper immediately, if it was started
				if(!callWrapper(AIRSteam_UseCrashHandler, _crashHandlerArgs))
					return false;
				else
					return true;
			}
			// TODO: after removal of the path argument to init(), this can always happen immediately
			// TODO: instead of waiting until init is called
			// if(!_process.running) return false;
			_crashHandlerArgs = [appID, version, date, time];
			return true;
		}

		public function restartAppIfNecessary(appID:uint):Boolean {
			if(_init) throw new Error("restartAppIfNecessary must be called before init");
			if(!_process.running) {
				// API wrapper isn't running: try to start with the default path
				if (!startProcess()) {
					// still couldn't start the process - return
					throw new Error("Steam API Wrapper process not started. Call startProcess first.");
				}
			}
			if(!callWrapper(AIRSteam_RestartAppIfNecessary, [appID])) return false;
			return readBoolResponse();
		}

		public function fileRead(name:String, data:ByteArray):Boolean {
			if(!callWrapper(AIRSteam_FileRead, [name])) return false;

			var success:Boolean = readBoolResponse();
			if(success) {
				data.clear();
				data.writeBytes(readByteArrayResponse());
				data.position = 0,
				// dummy value
				readBoolResponse();
			}

			return success;
		}

		public function UGCRead(handle:String, size:int, offset:uint, data:ByteArray):Boolean {
			if(!callWrapper(AIRSteam_UGCRead, [handle, size, offset])) return false;

			var success:Boolean = readBoolResponse();
			if(success) {
				data.clear();
				data.writeBytes(readByteArrayResponse());
				data.position = 0,
				// dummy value
				readBoolResponse();
			}

			return success;
		}

		public function getAuthSessionTicket(ticket:ByteArray):uint {
			if(!callWrapper(AIRSteam_GetAuthSessionTicket, [])) return 0;

			ticket.clear();
			ticket.writeBytes(readByteArrayResponse());
			ticket.position = 0;

			return readIntResponse();
		}

		/*
		 * The below code was autogenerated with generateAPI.rb
		 */

		// START GENERATED CODE
		/********************************/
		/* General Steamworks functions */
		/********************************/
		// manual implementation
		// public function init():Boolean

		// manual implementation
		// public function runCallbacks():Boolean

		public function getUserID():String {
			if(!callWrapper(AIRSteam_GetUserID, [])) return "0";
			return readStringResponse();
		}

		public function getAppID():uint {
			if(!callWrapper(AIRSteam_GetAppID, [])) return 0;
			return readIntResponse();
		}

		public function getAvailableGameLanguages():String {
			if(!callWrapper(AIRSteam_GetAvailableGameLanguages, [])) return "";
			return readStringResponse();
		}

		public function getCurrentGameLanguage():String {
			if(!callWrapper(AIRSteam_GetCurrentGameLanguage, [])) return "";
			return readStringResponse();
		}

		public function getPersonaName():String {
			if(!callWrapper(AIRSteam_GetPersonaName, [])) return "";
			return readStringResponse();
		}

		// manual implementation
		// public function useCrashHandler(appID:uint, version:String, date:String, time:String):Boolean

		// manual implementation
		// public function restartAppIfNecessary(appID:uint):Boolean

		/************************/
		/* Stats / Achievements */
		/************************/
		public function requestStats():Boolean {
			if(!callWrapper(AIRSteam_RequestStats, [])) return false;
			return readBoolResponse();
		}

		public function setAchievement(name:String):Boolean {
			if(!callWrapper(AIRSteam_SetAchievement, [name])) return false;
			return readBoolResponse();
		}

		public function clearAchievement(name:String):Boolean {
			if(!callWrapper(AIRSteam_ClearAchievement, [name])) return false;
			return readBoolResponse();
		}

		public function isAchievement(name:String):Boolean {
			if(!callWrapper(AIRSteam_IsAchievement, [name])) return false;
			return readBoolResponse();
		}

		public function getStatInt(name:String):int {
			if(!callWrapper(AIRSteam_GetStatInt, [name])) return 0;
			return readIntResponse();
		}

		public function getStatFloat(name:String):Number {
			if(!callWrapper(AIRSteam_GetStatFloat, [name])) return 0;
			return readFloatResponse();
		}

		public function setStatInt(name:String, value:int):Boolean {
			if(!callWrapper(AIRSteam_SetStatInt, [name, value])) return false;
			return readBoolResponse();
		}

		public function setStatFloat(name:String, value:Number):Boolean {
			if(!callWrapper(AIRSteam_SetStatFloat, [name, value])) return false;
			return readBoolResponse();
		}

		public function storeStats():Boolean {
			if(!callWrapper(AIRSteam_StoreStats, [])) return false;
			return readBoolResponse();
		}

		public function resetAllStats(achievementsToo:Boolean):Boolean {
			if(!callWrapper(AIRSteam_ResetAllStats, [achievementsToo])) return false;
			return readBoolResponse();
		}

		public function requestGlobalStats(historyDays:int):Boolean {
			if(!callWrapper(AIRSteam_RequestGlobalStats, [historyDays])) return false;
			return readBoolResponse();
		}

		public function getGlobalStatInt(name:String):Number {
			if(!callWrapper(AIRSteam_GetGlobalStatInt, [name])) return 0;
			return readFloatResponse();
		}

		public function getGlobalStatFloat(name:String):Number {
			if(!callWrapper(AIRSteam_GetGlobalStatFloat, [name])) return 0;
			return readFloatResponse();
		}

		public function getGlobalStatHistoryInt(name:String, days:int):Array {
			if(!callWrapper(AIRSteam_GetGlobalStatHistoryInt, [name, days])) return [];
			return readResponse() as Array;
		}

		public function getGlobalStatHistoryFloat(name:String, days:int):Array {
			if(!callWrapper(AIRSteam_GetGlobalStatHistoryFloat, [name, days])) return [];
			return readResponse() as Array;
		}

		/****************/
		/* Leaderboards */
		/****************/
		public function findLeaderboard(name:String):Boolean {
			if(!callWrapper(AIRSteam_FindLeaderboard, [name])) return false;
			return readBoolResponse();
		}

		public function findOrCreateLeaderboard(name:String, sortMethod:uint, displayType:uint):Boolean {
			if(!callWrapper(AIRSteam_FindOrCreateLeaderboard, [name, sortMethod, displayType])) return false;
			return readBoolResponse();
		}

		public function findLeaderboardResult():String {
			if(!callWrapper(AIRSteam_FindLeaderboardResult, [])) return "0";
			return readStringResponse();
		}

		public function getLeaderboardName(handle:String):String {
			if(!callWrapper(AIRSteam_GetLeaderboardName, [handle])) return "";
			return readStringResponse();
		}

		public function getLeaderboardEntryCount(handle:String):int {
			if(!callWrapper(AIRSteam_GetLeaderboardEntryCount, [handle])) return 0;
			return readIntResponse();
		}

		public function getLeaderboardSortMethod(handle:String):uint {
			if(!callWrapper(AIRSteam_GetLeaderboardSortMethod, [handle])) return UserStatsConstants.SORTMETHOD_None;
			return readIntResponse();
		}

		public function getLeaderboardDisplayType(handle:String):uint {
			if(!callWrapper(AIRSteam_GetLeaderboardDisplayType, [handle])) return UserStatsConstants.DISPLAYTYPE_None;
			return readIntResponse();
		}

		public function uploadLeaderboardScore(handle:String, method:uint, score:int, details:Array = null):Boolean {
			if(!callWrapper(AIRSteam_UploadLeaderboardScore, [handle, method, score, details])) return false;
			return readBoolResponse();
		}

		public function uploadLeaderboardScoreResult():UploadLeaderboardScoreResult {
			if(!callWrapper(AIRSteam_UploadLeaderboardScoreResult, [])) return null;
			return readResponse() as UploadLeaderboardScoreResult;
		}

		public function downloadLeaderboardEntries(handle:String, request:uint = UserStatsConstants.DATAREQUEST_GlobalAroundUser, rangeStart:int = -4, rangeEnd:int = 5):Boolean {
			if(!callWrapper(AIRSteam_DownloadLeaderboardEntries, [handle, request, rangeStart, rangeEnd])) return false;
			return readBoolResponse();
		}

		public function downloadLeaderboardEntriesResult(numDetails:int = 0):Array {
			if(!callWrapper(AIRSteam_DownloadLeaderboardEntriesResult, [numDetails])) return [];
			return readResponse() as Array;
		}

		/**************************/
		/* Cloud / Remote Storage */
		/**************************/
		public function getFileCount():int {
			if(!callWrapper(AIRSteam_GetFileCount, [])) return 0;
			return readIntResponse();
		}

		public function getFileSize(name:String):int {
			if(!callWrapper(AIRSteam_GetFileSize, [name])) return 0;
			return readIntResponse();
		}

		public function fileExists(name:String):Boolean {
			if(!callWrapper(AIRSteam_FileExists, [name])) return false;
			return readBoolResponse();
		}

		public function fileWrite(name:String, data:ByteArray):Boolean {
			if(!callWrapper(AIRSteam_FileWrite, [name, data])) return false;
			return readBoolResponse();
		}

		// manual implementation
		// public function fileRead(name:String, data:ByteArray):Boolean

		public function fileDelete(name:String):Boolean {
			if(!callWrapper(AIRSteam_FileDelete, [name])) return false;
			return readBoolResponse();
		}

		public function fileShare(name:String):Boolean {
			if(!callWrapper(AIRSteam_FileShare, [name])) return false;
			return readBoolResponse();
		}

		public function fileShareResult():String {
			if(!callWrapper(AIRSteam_FileShareResult, [])) return WorkshopConstants.UGCHANDLE_Invalid;
			return readStringResponse();
		}

		public function isCloudEnabledForApp():Boolean {
			if(!callWrapper(AIRSteam_IsCloudEnabledForApp, [])) return false;
			return readBoolResponse();
		}

		public function setCloudEnabledForApp(enabled:Boolean):Boolean {
			if(!callWrapper(AIRSteam_SetCloudEnabledForApp, [enabled])) return false;
			return readBoolResponse();
		}

		public function getQuota():Array {
			if(!callWrapper(AIRSteam_GetQuota, [])) return [];
			return readResponse() as Array;
		}

		/******************/
		/* UGC / Workshop */
		/******************/
		public function UGCDownload(handle:String, priority:uint):Boolean {
			if(!callWrapper(AIRSteam_UGCDownload, [handle, priority])) return false;
			return readBoolResponse();
		}

		// manual implementation
		// public function UGCRead(handle:String, size:int, offset:uint, data:ByteArray):Boolean

		public function getUGCDownloadProgress(handle:String):Array {
			if(!callWrapper(AIRSteam_GetUGCDownloadProgress, [handle])) return [];
			return readResponse() as Array;
		}

		public function getUGCDownloadResult(handle:String):DownloadUGCResult {
			if(!callWrapper(AIRSteam_GetUGCDownloadResult, [handle])) return null;
			return readResponse() as DownloadUGCResult;
		}

		public function publishWorkshopFile(name:String, preview:String, appId:uint, title:String, description:String, visibility:uint, tags:Array, fileType:uint):Boolean {
			if(!callWrapper(AIRSteam_PublishWorkshopFile, [name, preview, appId, title, description, visibility, tags, fileType])) return false;
			return readBoolResponse();
		}

		public function publishWorkshopFileResult():String {
			if(!callWrapper(AIRSteam_PublishWorkshopFileResult, [])) return WorkshopConstants.PUBLISHEDFILEID_Invalid;
			return readStringResponse();
		}

		public function deletePublishedFile(file:String):Boolean {
			if(!callWrapper(AIRSteam_DeletePublishedFile, [file])) return false;
			return readBoolResponse();
		}

		public function getPublishedFileDetails(file:String, maxAge:uint = 0):Boolean {
			if(!callWrapper(AIRSteam_GetPublishedFileDetails, [file, maxAge])) return false;
			return readBoolResponse();
		}

		public function getPublishedFileDetailsResult(file:String):FileDetailsResult {
			if(!callWrapper(AIRSteam_GetPublishedFileDetailsResult, [file])) return null;
			return readResponse() as FileDetailsResult;
		}

		public function enumerateUserPublishedFiles(startIndex:uint):Boolean {
			if(!callWrapper(AIRSteam_EnumerateUserPublishedFiles, [startIndex])) return false;
			return readBoolResponse();
		}

		public function enumerateUserPublishedFilesResult():UserFilesResult {
			if(!callWrapper(AIRSteam_EnumerateUserPublishedFilesResult, [])) return null;
			return readResponse() as UserFilesResult;
		}

		public function enumeratePublishedWorkshopFiles(type:uint, start:uint, count:uint, days:uint, tags:Array, userTags:Array):Boolean {
			if(!callWrapper(AIRSteam_EnumeratePublishedWorkshopFiles, [type, start, count, days, tags, userTags])) return false;
			return readBoolResponse();
		}

		public function enumeratePublishedWorkshopFilesResult():WorkshopFilesResult {
			if(!callWrapper(AIRSteam_EnumeratePublishedWorkshopFilesResult, [])) return null;
			return readResponse() as WorkshopFilesResult;
		}

		public function enumerateUserSubscribedFiles(startIndex:uint):Boolean {
			if(!callWrapper(AIRSteam_EnumerateUserSubscribedFiles, [startIndex])) return false;
			return readBoolResponse();
		}

		public function enumerateUserSubscribedFilesResult():SubscribedFilesResult {
			if(!callWrapper(AIRSteam_EnumerateUserSubscribedFilesResult, [])) return null;
			return readResponse() as SubscribedFilesResult;
		}

		public function enumerateUserSharedWorkshopFiles(steamID:String, start:uint, required:Array, excluded:Array):Boolean {
			if(!callWrapper(AIRSteam_EnumerateUserSharedWorkshopFiles, [steamID, start, required, excluded])) return false;
			return readBoolResponse();
		}

		public function enumerateUserSharedWorkshopFilesResult():UserFilesResult {
			if(!callWrapper(AIRSteam_EnumerateUserSharedWorkshopFilesResult, [])) return null;
			return readResponse() as UserFilesResult;
		}

		public function enumeratePublishedFilesByUserAction(action:uint, startIndex:uint):Boolean {
			if(!callWrapper(AIRSteam_EnumeratePublishedFilesByUserAction, [action, startIndex])) return false;
			return readBoolResponse();
		}

		public function enumeratePublishedFilesByUserActionResult():FilesByUserActionResult {
			if(!callWrapper(AIRSteam_EnumeratePublishedFilesByUserActionResult, [])) return null;
			return readResponse() as FilesByUserActionResult;
		}

		public function subscribePublishedFile(file:String):Boolean {
			if(!callWrapper(AIRSteam_SubscribePublishedFile, [file])) return false;
			return readBoolResponse();
		}

		public function unsubscribePublishedFile(file:String):Boolean {
			if(!callWrapper(AIRSteam_UnsubscribePublishedFile, [file])) return false;
			return readBoolResponse();
		}

		public function createPublishedFileUpdateRequest(file:String):String {
			if(!callWrapper(AIRSteam_CreatePublishedFileUpdateRequest, [file])) return WorkshopConstants.FILEUPDATEHANDLE_Invalid;
			return readStringResponse();
		}

		public function updatePublishedFileFile(handle:String, file:String):Boolean {
			if(!callWrapper(AIRSteam_UpdatePublishedFileFile, [handle, file])) return false;
			return readBoolResponse();
		}

		public function updatePublishedFilePreviewFile(handle:String, preview:String):Boolean {
			if(!callWrapper(AIRSteam_UpdatePublishedFilePreviewFile, [handle, preview])) return false;
			return readBoolResponse();
		}

		public function updatePublishedFileTitle(handle:String, title:String):Boolean {
			if(!callWrapper(AIRSteam_UpdatePublishedFileTitle, [handle, title])) return false;
			return readBoolResponse();
		}

		public function updatePublishedFileDescription(handle:String, description:String):Boolean {
			if(!callWrapper(AIRSteam_UpdatePublishedFileDescription, [handle, description])) return false;
			return readBoolResponse();
		}

		public function updatePublishedFileSetChangeDescription(handle:String, changeDesc:String):Boolean {
			if(!callWrapper(AIRSteam_UpdatePublishedFileSetChangeDescription, [handle, changeDesc])) return false;
			return readBoolResponse();
		}

		public function updatePublishedFileVisibility(handle:String, visibility:uint):Boolean {
			if(!callWrapper(AIRSteam_UpdatePublishedFileVisibility, [handle, visibility])) return false;
			return readBoolResponse();
		}

		public function updatePublishedFileTags(handle:String, tags:Array):Boolean {
			if(!callWrapper(AIRSteam_UpdatePublishedFileTags, [handle, tags])) return false;
			return readBoolResponse();
		}

		public function commitPublishedFileUpdate(handle:String):Boolean {
			if(!callWrapper(AIRSteam_CommitPublishedFileUpdate, [handle])) return false;
			return readBoolResponse();
		}

		public function getPublishedItemVoteDetails(file:String):Boolean {
			if(!callWrapper(AIRSteam_GetPublishedItemVoteDetails, [file])) return false;
			return readBoolResponse();
		}

		public function getPublishedItemVoteDetailsResult():ItemVoteDetailsResult {
			if(!callWrapper(AIRSteam_GetPublishedItemVoteDetailsResult, [])) return null;
			return readResponse() as ItemVoteDetailsResult;
		}

		public function getUserPublishedItemVoteDetails(file:String):Boolean {
			if(!callWrapper(AIRSteam_GetUserPublishedItemVoteDetails, [file])) return false;
			return readBoolResponse();
		}

		public function getUserPublishedItemVoteDetailsResult():UserVoteDetails {
			if(!callWrapper(AIRSteam_GetUserPublishedItemVoteDetailsResult, [])) return null;
			return readResponse() as UserVoteDetails;
		}

		public function updateUserPublishedItemVote(file:String, upvote:Boolean):Boolean {
			if(!callWrapper(AIRSteam_UpdateUserPublishedItemVote, [file, upvote])) return false;
			return readBoolResponse();
		}

		public function setUserPublishedFileAction(file:String, action:uint):Boolean {
			if(!callWrapper(AIRSteam_SetUserPublishedFileAction, [file, action])) return false;
			return readBoolResponse();
		}

		/***********/
		/* Friends */
		/***********/
		public function getFriendCount(flags:uint):int {
			if(!callWrapper(AIRSteam_GetFriendCount, [flags])) return 0;
			return readIntResponse();
		}

		public function getFriendByIndex(index:int, flags:uint):String {
			if(!callWrapper(AIRSteam_GetFriendByIndex, [index, flags])) return "0";
			return readStringResponse();
		}

		public function getFriendPersonaName(id:String):String {
			if(!callWrapper(AIRSteam_GetFriendPersonaName, [id])) return "";
			return readStringResponse();
		}

		/******************************/
		/* Authentication & Ownership */
		/******************************/
		// manual implementation
		// public function getAuthSessionTicket(ticket:ByteArray):uint

		public function getAuthSessionTicketResult():uint {
			if(!callWrapper(AIRSteam_GetAuthSessionTicketResult, [])) return UserConstants.AUTHTICKET_Invalid;
			return readIntResponse();
		}

		public function beginAuthSession(ticket:ByteArray, steamID:String):int {
			if(!callWrapper(AIRSteam_BeginAuthSession, [ticket, steamID])) return UserConstants.BEGINAUTH_InvalidTicket;
			return readIntResponse();
		}

		public function endAuthSession(steamID:String):Boolean {
			if(!callWrapper(AIRSteam_EndAuthSession, [steamID])) return false;
			return readBoolResponse();
		}

		public function cancelAuthTicket(ticketHandle:uint):Boolean {
			if(!callWrapper(AIRSteam_CancelAuthTicket, [ticketHandle])) return false;
			return readBoolResponse();
		}

		public function userHasLicenseForApp(steamID:String, appID:uint):int {
			if(!callWrapper(AIRSteam_UserHasLicenseForApp, [steamID, appID])) return UserConstants.LICENSE_NoAuth;
			return readIntResponse();
		}

		/***********/
		/* Overlay */
		/***********/
		public function activateGameOverlay(dialog:String):Boolean {
			if(!callWrapper(AIRSteam_ActivateGameOverlay, [dialog])) return false;
			return readBoolResponse();
		}

		public function activateGameOverlayToUser(dialog:String, steamId:String):Boolean {
			if(!callWrapper(AIRSteam_ActivateGameOverlayToUser, [dialog, steamId])) return false;
			return readBoolResponse();
		}

		public function activateGameOverlayToWebPage(url:String):Boolean {
			if(!callWrapper(AIRSteam_ActivateGameOverlayToWebPage, [url])) return false;
			return readBoolResponse();
		}

		public function activateGameOverlayToStore(appId:uint, flag:uint):Boolean {
			if(!callWrapper(AIRSteam_ActivateGameOverlayToStore, [appId, flag])) return false;
			return readBoolResponse();
		}

		public function activateGameOverlayInviteDialog(steamIdLobby:String):Boolean {
			if(!callWrapper(AIRSteam_ActivateGameOverlayInviteDialog, [steamIdLobby])) return false;
			return readBoolResponse();
		}

		public function isOverlayEnabled():Boolean {
			if(!callWrapper(AIRSteam_IsOverlayEnabled, [])) return false;
			return readBoolResponse();
		}

		public function setOverlayNotificationPosition(position:uint):Boolean {
			if(!callWrapper(AIRSteam_SetOverlayNotificationPosition, [position])) return false;
			return readBoolResponse();
		}

		/***********************/
		/* DLC / subscriptions */
		/***********************/
		public function isSubscribedApp(appId:uint):Boolean {
			if(!callWrapper(AIRSteam_IsSubscribedApp, [appId])) return false;
			return readBoolResponse();
		}

		public function isDLCInstalled(appId:uint):Boolean {
			if(!callWrapper(AIRSteam_IsDLCInstalled, [appId])) return false;
			return readBoolResponse();
		}

		public function getDLCCount():int {
			if(!callWrapper(AIRSteam_GetDLCCount, [])) return 0;
			return readIntResponse();
		}

		public function installDLC(appId:uint):Boolean {
			if(!callWrapper(AIRSteam_InstallDLC, [appId])) return false;
			return readBoolResponse();
		}

		public function uninstallDLC(appId:uint):Boolean {
			if(!callWrapper(AIRSteam_UninstallDLC, [appId])) return false;
			return readBoolResponse();
		}

		public function DLCInstalledResult():uint {
			if(!callWrapper(AIRSteam_DLCInstalledResult, [])) return 0;
			return readIntResponse();
		}

		/********************/
		/* Microtransaction */
		/********************/
		public function microTxnResult():MicroTxnAuthorizationResponse {
			if(!callWrapper(AIRSteam_MicroTxnResult, [])) return null;
			return readResponse() as MicroTxnAuthorizationResponse;
		}

		/**********************************/
		/* Other non-Steamworks functions */
		/**********************************/
		public function getEnv(name:String):String {
			if(!callWrapper(AIRSteam_GetEnv, [name])) return "";
			return readStringResponse();
		}

		// END GENERATED CODE
	}
}
