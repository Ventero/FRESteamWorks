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
	import flash.events.Event;
	import flash.events.EventDispatcher;
	import flash.events.IEventDispatcher;
	import flash.events.IOErrorEvent;
	import flash.events.ProgressEvent;
	import flash.events.StatusEvent;
	import flash.filesystem.File;
	import flash.utils.ByteArray;
	import flash.utils.IDataInput;
	import flash.utils.IDataOutput;
	import flash.utils.clearInterval;
	import flash.utils.setInterval;

	public class FRESteamWorks extends EventDispatcher {
		[Event(name="steam_response", type="com.amanitadesign.steam.SteamEvent")]

		private static const PATH:String = "NativeApps/Linux/APIWrapper";

		private var _file:File;
		private var _process:NativeProcess;
		private var _tm:int;
		private var _init:Boolean = false;
		private var _error:Boolean = false;
		private var _crashHandlerArgs:Array = null;

		public var isReady:Boolean = false;

		private static const AIRSteam_Init:int = 0;
		private static const AIRSteam_RunCallbacks:int = 1;
		private static const AIRSteam_RequestStats:int = 2;
		private static const AIRSteam_SetAchievement:int = 3;
		private static const AIRSteam_ClearAchievement:int = 4;
		private static const AIRSteam_IsAchievement:int = 5;
		private static const AIRSteam_GetStatInt:int = 6;
		private static const AIRSteam_GetStatFloat:int = 7;
		private static const AIRSteam_SetStatInt:int = 8;
		private static const AIRSteam_SetStatFloat:int = 9;
		private static const AIRSteam_StoreStats:int = 10;
		private static const AIRSteam_ResetAllStats:int = 11;
		private static const AIRSteam_GetFileCount:int = 12;
		private static const AIRSteam_GetFileSize:int = 13;
		private static const AIRSteam_FileExists:int = 14;
		private static const AIRSteam_FileWrite:int = 15;
		private static const AIRSteam_FileRead:int = 16;
		private static const AIRSteam_FileDelete:int = 17;
		private static const AIRSteam_IsCloudEnabledForApp:int = 18;
		private static const AIRSteam_SetCloudEnabledForApp:int = 19;
		private static const AIRSteam_GetUserID:int = 20;
		private static const AIRSteam_GetPersonaName:int = 21;
		private static const AIRSteam_UseCrashHandler:int = 22;

		public function FRESteamWorks (target:IEventDispatcher = null) {
			_file = File.applicationDirectory.resolvePath(PATH);
			_process = new NativeProcess();
			_process.addEventListener(ProgressEvent.STANDARD_ERROR_DATA, eventDispatched);
			_process.addEventListener(IOErrorEvent.STANDARD_INPUT_IO_ERROR, errorCallback);
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

		private function callWrapper(funcName:int, params:Array = null):Boolean {
			_error = false;
			if(!_process.running) return false;

			var stdin:IDataOutput = _process.standardInput;
			stdin.writeUTFBytes(funcName + "\n");

			if (params) {
				for(var i:int = 0; i < params.length; ++i) {
					if(params[i] is ByteArray) {
						var length:uint = params[i].length;
						// length + 1 for the added newline
						stdin.writeUTFBytes(String(length + 1) + "\n");
						stdin.writeBytes(params[i]);
						stdin.writeUTFBytes("\n");
					} else {
						stdin.writeUTFBytes(String(params[i]) + "\n");
					}
				}
			}

			return !_error;
		}

		private function waitForData(output:IDataInput):uint {
			while(!output.bytesAvailable) {
				// wait
				if(!_process.running) return 0;
			}

			return output.bytesAvailable;
		}

		private function readBoolResponse():Boolean {
			if(!_process.running) return false;
			var stdout:IDataInput = _process.standardOutput;
			var avail:uint = waitForData(stdout);

			var response:String = stdout.readUTFBytes(1);
			return (response == "t");
		}

		private function readIntResponse():int {
			if(!_process.running) return 0;
			var stdout:IDataInput = _process.standardOutput;
			var avail:uint = waitForData(stdout);

			var response:String = stdout.readUTFBytes(avail);
			return parseInt(response, 10);
		}

		private function readFloatResponse():Number {
			if(!_process.running) return 0.0;
			var stdout:IDataInput = _process.standardOutput;
			var avail:uint = waitForData(stdout);

			var response:String = stdout.readUTFBytes(avail)
			return parseFloat(response);
		}

		private function readStringResponse():String {
			if(!_process.running) return "";
			var stdout:IDataInput = _process.standardOutput;
			var avail:uint = waitForData(stdout);

			var response:String = stdout.readUTFBytes(avail)
			return response;
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

		public function requestStats():Boolean {
			if(!callWrapper(AIRSteam_RequestStats)) return false;
			return readBoolResponse();
		}

		public function runCallbacks():Boolean {
			if(!callWrapper(AIRSteam_RunCallbacks)) return false;
			return true;
		}

		public function getUserID():String {
			if(!callWrapper(AIRSteam_GetUserID)) return "";
			return readStringResponse();
		}

		public function getPersonaName():String {
			if(!callWrapper(AIRSteam_GetPersonaName)) return "";
			return readStringResponse();
		}

		public function useCrashHandler(appID:uint, version:String, date:String, time:String):Boolean {
			// only allow calls before SteamAPI_Init was called
			if(_init) return false;
			_crashHandlerArgs = [appID, version, date, time];
			return true;
		}

		public function setAchievement(id:String):Boolean {
			if(!callWrapper(AIRSteam_SetAchievement, [id])) return false;
			return readBoolResponse();
		}

		public function clearAchievement(id:String):Boolean {
			if(!callWrapper(AIRSteam_ClearAchievement, [id])) return false;
			return readBoolResponse();
		}

		public function isAchievement(id:String):Boolean {
			if(!callWrapper(AIRSteam_IsAchievement, [id])) return false;
			return readBoolResponse();
		}

		public function getStatInt(id:String):int {
			if(!callWrapper(AIRSteam_GetStatInt, [id])) return 0;
			return readIntResponse();
		}

		public function getStatFloat(id:String):Number {
			if(!callWrapper(AIRSteam_GetStatFloat, [id])) return 0.0;
			return readFloatResponse();
		}

		public function setStatInt(id:String, value:int):Boolean {
			if(!callWrapper(AIRSteam_SetStatInt, [id, value])) return false;
			return readBoolResponse();
		}

		public function setStatFloat(id:String, value:Number):Boolean {
			if(!callWrapper(AIRSteam_SetStatFloat, [id, value])) return false;
			return readBoolResponse();
		}

		public function storeStats():Boolean {
			if(!callWrapper(AIRSteam_StoreStats)) return false;
			return readBoolResponse();
		}

		public function resetAllStats(bAchievementsToo:Boolean):Boolean {
			if(!callWrapper(AIRSteam_ResetAllStats, [bAchievementsToo])) return false;
			return readBoolResponse();
		}

		public function getFileCount():int {
			if(!callWrapper(AIRSteam_GetFileCount)) return 0;
			return readIntResponse();
		}

		public function getFileSize(fileName:String):int {
			if(!callWrapper(AIRSteam_GetFileSize, [fileName])) return 0;
			return readIntResponse();
		}

		public function fileExists(fileName:String):Boolean {
			if(!callWrapper(AIRSteam_FileExists, [fileName])) return false;
			return readBoolResponse();
		}

		public function fileWrite(fileName:String, data:ByteArray):Boolean {
			if(!callWrapper(AIRSteam_FileWrite, [fileName, data])) return false;
			return readBoolResponse();
		}

		public function fileRead(fileName:String, data:ByteArray):Boolean {
			if(!callWrapper(AIRSteam_FileRead, [fileName])) return false;

			var success:Boolean = readBoolResponse();
			if(success) {
				var content:String = readStringResponse();
				data.writeUTFBytes(content);
				data.position = 0;
			}

			return success;
		}

		public function fileDelete(fileName:String):Boolean {
			if(!callWrapper(AIRSteam_FileDelete, [fileName])) return false;
			return readBoolResponse();
		}

		public function isCloudEnabledForApp():Boolean {
			if(!callWrapper(AIRSteam_IsCloudEnabledForApp)) return false;
			return readBoolResponse();
		}

		public function setCloudEnabledForApp(enabled:Boolean):Boolean {
			if(!callWrapper(AIRSteam_SetCloudEnabledForApp, [enabled])) return false;
			return readBoolResponse();
		}
	}
}
