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
		private static const PATH:String = "NativeApps/Linux/APIWrapper";

		private var _file:File;
		private var _process:NativeProcess;
		private var _tm:int;
		private var _init:Boolean = false;
		private var _error:Boolean = false;
		private var _crashHandlerArgs:Array = null;
		private var _buf:ByteArray;

		public var isReady:Boolean = false;

		// function list automatically generated with generateAPI.rb
		private static const AIRSteam_Init:int = 0;
		private static const AIRSteam_RunCallbacks:int = 1;
		private static const AIRSteam_GetUserID:int = 2;
		private static const AIRSteam_GetAppID:int = 3;
		private static const AIRSteam_GetPersonaName:int = 4;
		private static const AIRSteam_UseCrashHandler:int = 5;
		/* stats/achievements */
		private static const AIRSteam_RequestStats:int = 6;
		private static const AIRSteam_SetAchievement:int = 7;
		private static const AIRSteam_ClearAchievement:int = 8;
		private static const AIRSteam_IsAchievement:int = 9;
		private static const AIRSteam_GetStatInt:int = 10;
		private static const AIRSteam_GetStatFloat:int = 11;
		private static const AIRSteam_SetStatInt:int = 12;
		private static const AIRSteam_SetStatFloat:int = 13;
		private static const AIRSteam_StoreStats:int = 14;
		private static const AIRSteam_ResetAllStats:int = 15;
		/* cloud */
		private static const AIRSteam_GetFileCount:int = 16;
		private static const AIRSteam_GetFileSize:int = 17;
		private static const AIRSteam_FileExists:int = 18;
		private static const AIRSteam_FileWrite:int = 19;
		private static const AIRSteam_FileRead:int = 20;
		private static const AIRSteam_FileDelete:int = 21;
		private static const AIRSteam_FileShare:int = 22;
		private static const AIRSteam_FileShareResult:int = 23;
		private static const AIRSteam_IsCloudEnabledForApp:int = 24;
		private static const AIRSteam_SetCloudEnabledForApp:int = 25;
		private static const AIRSteam_GetQuota:int = 26;
		/* ugc/workshop */
		private static const AIRSteam_UGCDownload:int = 27;
		private static const AIRSteam_UGCRead:int = 28;
		private static const AIRSteam_GetUGCDownloadProgress:int = 29;
		private static const AIRSteam_GetUGCDownloadResult:int = 30;
		private static const AIRSteam_PublishWorkshopFile:int = 31;
		private static const AIRSteam_PublishWorkshopFileResult:int = 32;
		private static const AIRSteam_DeletePublishedFile:int = 33;
		private static const AIRSteam_GetPublishedFileDetails:int = 34;
		private static const AIRSteam_GetPublishedFileDetailsResult:int = 35;
		private static const AIRSteam_EnumerateUserPublishedFiles:int = 36;
		private static const AIRSteam_EnumerateUserPublishedFilesResult:int = 37;
		private static const AIRSteam_EnumeratePublishedWorkshopFiles:int = 38;
		private static const AIRSteam_EnumeratePublishedWorkshopFilesResult:int = 39;
		private static const AIRSteam_EnumerateUserSubscribedFiles:int = 40;
		private static const AIRSteam_EnumerateUserSubscribedFilesResult:int = 41;
		private static const AIRSteam_EnumerateUserSharedWorkshopFiles:int = 42;
		private static const AIRSteam_EnumerateUserSharedWorkshopFilesResult:int = 43;
		private static const AIRSteam_EnumeratePublishedFilesByUserAction:int = 44;
		private static const AIRSteam_EnumeratePublishedFilesByUserActionResult:int = 45;
		private static const AIRSteam_SubscribePublishedFile:int = 46;
		private static const AIRSteam_UnsubscribePublishedFile:int = 47;
		private static const AIRSteam_CreatePublishedFileUpdateRequest:int = 48;
		private static const AIRSteam_UpdatePublishedFileFile:int = 49;
		private static const AIRSteam_UpdatePublishedFilePreviewFile:int = 50;
		private static const AIRSteam_UpdatePublishedFileTitle:int = 51;
		private static const AIRSteam_UpdatePublishedFileDescription:int = 52;
		private static const AIRSteam_UpdatePublishedFileSetChangeDescription:int = 53;
		private static const AIRSteam_UpdatePublishedFileVisibility:int = 54;
		private static const AIRSteam_UpdatePublishedFileTags:int = 55;
		private static const AIRSteam_CommitPublishedFileUpdate:int = 56;
		private static const AIRSteam_GetPublishedItemVoteDetails:int = 57;
		private static const AIRSteam_GetPublishedItemVoteDetailsResult:int = 58;
		private static const AIRSteam_GetUserPublishedItemVoteDetails:int = 59;
		private static const AIRSteam_GetUserPublishedItemVoteDetailsResult:int = 60;
		private static const AIRSteam_UpdateUserPublishedItemVote:int = 61;
		private static const AIRSteam_SetUserPublishedFileAction:int = 62;
		/* overlay */
		private static const AIRSteam_ActivateGameOverlay:int = 63;
		private static const AIRSteam_ActivateGameOverlayToUser:int = 64;
		private static const AIRSteam_ActivateGameOverlayToWebPage:int = 65;
		private static const AIRSteam_ActivateGameOverlayToStore:int = 66;
		private static const AIRSteam_ActivateGameOverlayInviteDialog:int = 67;
		private static const AIRSteam_IsOverlayEnabled:int = 68;
		/* DLC / subscriptions */
		private static const AIRSteam_IsSubscribedApp:int = 69;
		private static const AIRSteam_IsDLCInstalled:int = 70;
		private static const AIRSteam_GetDLCCount:int = 71;
		private static const AIRSteam_InstallDLC:int = 72;
		private static const AIRSteam_UninstallDLC:int = 73;
		private static const AIRSteam_DLCInstalledResult:int = 74;

		public function FRESteamWorks (target:IEventDispatcher = null) {
			_file = File.applicationDirectory.resolvePath(PATH);
			_process = new NativeProcess();
			_process.addEventListener(ProgressEvent.STANDARD_ERROR_DATA, eventDispatched);
			_process.addEventListener(IOErrorEvent.STANDARD_INPUT_IO_ERROR, errorCallback);
			_buf = new ByteArray();

			registerClassAlias("com.amanitadesign.steam.DownloadUGCResult", DownloadUGCResult);
			registerClassAlias("com.amanitadesign.steam.FileDetailsResult", FileDetailsResult);
			registerClassAlias("com.amanitadesign.steam.FilesByUserActionResult", FilesByUserActionResult);
			registerClassAlias("com.amanitadesign.steam.ItemVoteDetailsResult", ItemVoteDetailsResult);
			registerClassAlias("com.amanitadesign.steam.SubscribedFilesResult", SubscribedFilesResult);
			registerClassAlias("com.amanitadesign.steam.UserFilesResult", UserFilesResult);
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

		private function startProcess():void {
			var startupInfo:NativeProcessStartupInfo = new NativeProcessStartupInfo();
			startupInfo.executable = _file;

			_process.start(startupInfo);
		}

		public function init():Boolean {
			if(!_file.exists) return false;

			try {
				startProcess();
			} catch(e:Error) {
				return false;
			}

			// try to start the process a second time, but this time ignoring any
			// potential errors since we will definitely get one about the process
			// already running. this seems to give the runtime enough time to check
			// for any stderr output of the initial call to startProcess(), so that we
			// now can check if it actually started: in case it couldn't one of the
			// libraries it depends on, it prints something like "error while loading
			// shared libraries" to stderr, which we're trying to detect here.
			// process.running is unreliable, in that it's always set to true,
			// even if the process didn't start at all
			try {
				startProcess();
			} catch(e:Error) {
				// no-op
			}
			if(!_process.running) return false;
			if(_process.standardError.bytesAvailable > 0) return false;

			// initialization seems to be successful
			_init = true;

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
					writeValue(stdin, params[i]);
				}
			}

			return !_error;
		}

		private function writeValue(output:IDataOutput, value:*):void {
			if (value === null || value === undefined) {
				// no data, so length 0
				output.writeUTFBytes(0 + "\n");
			} else if(value is ByteArray) {
				var length:uint = value.length;
				// length + 1 for the added newline
				output.writeUTFBytes(String(length + 1) + "\n");
				output.writeBytes(value);
				output.writeUTFBytes("\n");
			} else if(value is String) {
				output.writeUTFBytes(String(value.length + 1) + "\n" + value + "\n");
			} else if(value is Array) {
				output.writeUTFBytes(value.length + "\n");
				for (var el:int = 0; el < value.length; ++el) {
					writeValue(output, value[el]);
				}
			} else if(value is int || value is uint || value is Number || value is Boolean) {
				output.writeUTFBytes(String(value) + "\n");
			} else {
				throw new ArgumentError("Cannot write value " + value);
			}
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

		private function readResponse():* {
			if (!_process.running) return null;
			var stdout:IDataInput = _process.standardOutput;

			// first, wait for the length prefix
			if (!waitForData(stdout)) return null;
			var length:uint = _buf.readObject() as uint;

			// AIR seems to have a limit of around 32k bytes data on stdin
			// we can synchronously wait for, so for large data we use a temp
			// file instead
			if (length > 16384) {
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
			_crashHandlerArgs = [appID, version, date, time];
			return true;
		}

		public function fileRead(name:String, data:ByteArray):Boolean {
			if(!callWrapper(AIRSteam_FileRead, [name])) return false;

			var success:Boolean = readBoolResponse();
			if(success) {
				var response:ByteArray = readResponse() as ByteArray;
				data.writeBytes(response);
				data.position = 0;
			}

			return true;
		}

		public function UGCRead(handle:String, size:int, offset:uint, data:ByteArray):Boolean {
			if(!callWrapper(AIRSteam_UGCRead, [handle, size, offset])) return false;
			var success:Boolean = readBoolResponse();

			if(success) {
				var response:ByteArray = readResponse() as ByteArray;
				data.writeBytes(response);
				data.position = 0;
			}

			return true;
		}

		/*
		 * The below code was autogenerated with generateAPI.rb
		 */

		public function getUserID():String {
			if(!callWrapper(AIRSteam_GetUserID, [])) return "";
			return readStringResponse();
		}

		public function getAppID():uint {
			if(!callWrapper(AIRSteam_GetAppID, [])) return 0;
			return readIntResponse();
		}

		public function getPersonaName():String {
			if(!callWrapper(AIRSteam_GetPersonaName, [])) return "";
			return readStringResponse();
		}

		/* stats/achievements */
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
			if(!callWrapper(AIRSteam_GetStatFloat, [name])) return 0.0;
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

		/* cloud */
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

		public function fileDelete(name:String):Boolean {
			if(!callWrapper(AIRSteam_FileDelete, [name])) return false;
			return readBoolResponse();
		}

		public function fileShare(name:String):Boolean {
			if(!callWrapper(AIRSteam_FileShare, [name])) return false;
			return readBoolResponse();
		}

		public function fileShareResult():String {
			if(!callWrapper(AIRSteam_FileShareResult, [])) return "";
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
			if(!callWrapper(AIRSteam_GetQuota, [])) return null;
			return readResponse() as Array;
		}

		/* ugc/workshop */
		public function UGCDownload(handle:String, priority:int):Boolean {
			if(!callWrapper(AIRSteam_UGCDownload, [handle, priority])) return false;
			return readBoolResponse();
		}

		public function getUGCDownloadProgress(handle:String):Array {
			if(!callWrapper(AIRSteam_GetUGCDownloadProgress, [handle])) return null;
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
			if(!callWrapper(AIRSteam_PublishWorkshopFileResult, [])) return "";
			return readStringResponse();
		}

		public function deletePublishedFile(file:String):Boolean {
			if(!callWrapper(AIRSteam_DeletePublishedFile, [file])) return false;
			return readBoolResponse();
		}

		public function getPublishedFileDetails(file:String, maxAge:int):Boolean {
			if(!callWrapper(AIRSteam_GetPublishedFileDetails, [file, maxAge]))
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
			if(!callWrapper(AIRSteam_CreatePublishedFileUpdateRequest, [file])) return "";
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

		/* overlay */
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

		/* DLC / subscriptions */
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
	}
}
