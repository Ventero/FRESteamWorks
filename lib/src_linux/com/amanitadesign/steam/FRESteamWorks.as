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

		public function FRESteamWorks (target:IEventDispatcher = null) {
			_file = File.applicationDirectory.resolvePath(PATH);
			_process = new NativeProcess();
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
			_process.addEventListener(ProgressEvent.STANDARD_ERROR_DATA, eventDispatched);
			_process.addEventListener(IOErrorEvent.STANDARD_OUTPUT_IO_ERROR, errorCallback);
		}

		public function init():Boolean {
			trace("Init called");
			if(!_file.exists) return false;
			startProcess();
			if(!_process.running) return false;

			callWrapper(AIRSteam_Init);
			isReady = readBoolResponse();
			if(isReady) _tm = setInterval(runCallbacks, 100);
			return isReady;
		}

		private function callWrapper(funcName:int, params:Array = null):void{
			if(!_process.running) startProcess();

			trace("Calling: " + funcName + "//" + params);
			var stdin:IDataOutput = _process.standardInput;
			stdin.writeUTFBytes(funcName + "\n");

			if(!params) return;

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

		private function waitForData(output:IDataInput):uint {
			while(!output.bytesAvailable) {
				// wait
			}

			return output.bytesAvailable;
		}

		private function readBoolResponse():Boolean {
			if(!_process.running) return false;
			var stdout:IDataInput = _process.standardOutput;
			var avail:uint = waitForData(stdout);

			var response:String = stdout.readUTFBytes(1);
			trace(response + "//" + (response == "t"));
			return (response == "t");
		}

		private function readIntResponse():int {
			if(!_process.running) return 0;
			var stdout:IDataInput = _process.standardOutput;
			var avail:uint = waitForData(stdout);

			var response:String = stdout.readUTFBytes(avail);
			trace(response);
			return parseInt(response, 10);
		}

		private function readFloatResponse():Number {
			if(!_process.running) return 0.0;
			var stdout:IDataInput = _process.standardOutput;
			var avail:uint = waitForData(stdout);

			var response:String = stdout.readUTFBytes(avail)
			trace(response);
			return parseFloat(response);
		}

		private function readStringResponse():String {
			if(!_process.running) return "";
			var stdout:IDataInput = _process.standardOutput;
			var avail:uint = waitForData(stdout);

			var response:String = stdout.readUTFBytes(avail)
			trace(response);
			return response;
		}

		private function eventDispatched(e:ProgressEvent):void {
			var stderr:IDataInput = _process.standardError;
			var avail:uint = stderr.bytesAvailable;
			var data:String = stderr.readUTFBytes(avail);
			trace("Got progress event, data available: " + avail);
			trace("Content: " + data);

			var pattern:RegExp = /__event__<(\d+),(\d+)>/g;
			var result:Object;
			while((result = pattern.exec(data))) {
				var req_type:int = new int(result[1]);
				var response:int = new int(result[2]);
				trace("Got event: " + req_type + "//" + response);
				var steamEvent:SteamEvent = new SteamEvent(SteamEvent.STEAM_RESPONSE, req_type, response);
				dispatchEvent(steamEvent);
			}
		}

		private function errorCallback(e:IOErrorEvent):void {
			// the process doesn't accept our input anymore, try to restart it
			if(_process.running) {
				_process.closeInput();
				_process.exit();
			}

			startProcess();
		}

		public function requestStats():Boolean {
			trace("requestStats called");
			callWrapper(AIRSteam_RequestStats);
			return readBoolResponse();
		}

		public function runCallbacks():Boolean {
			trace("runCallbacks called");
			callWrapper(AIRSteam_RunCallbacks);
			return true;
		}

		public function setAchievement(id:String):Boolean {
			trace("setAchievement called");
			callWrapper(AIRSteam_SetAchievement, [id]);
			return readBoolResponse();
		}

		public function clearAchievement(id:String):Boolean {
			trace("clearAchievement called");
			callWrapper(AIRSteam_ClearAchievement, [id]);
			return readBoolResponse();
		}

		public function isAchievement(id:String):Boolean {
			trace("isAchievement called");
			callWrapper(AIRSteam_IsAchievement, [id]);
			return readBoolResponse();
		}

		public function getStatInt(id:String):int {
			trace("getStatInt called");
			callWrapper(AIRSteam_GetStatInt, [id]);
			return readIntResponse();
		}

		public function getStatFloat(id:String):Number {
			trace("getStatFloat called");
			callWrapper(AIRSteam_GetStatFloat, [id]);
			return readFloatResponse();
		}

		public function setStatInt(id:String, value:int):Boolean {
			trace("setStatInt called");
			callWrapper(AIRSteam_SetStatInt, [id, value]);
			return readBoolResponse();
		}

		public function setStatFloat(id:String, value:Number):Boolean {
			trace("setStatFloat called");
			callWrapper(AIRSteam_SetStatFloat, [id, value]);
			return readBoolResponse();
		}

		public function storeStats():Boolean {
			trace("storeStats called");
			callWrapper(AIRSteam_StoreStats);
			return readBoolResponse();
		}

		public function resetAllStats(bAchievementsToo:Boolean):Boolean {
			trace("resetAllStats called");
			callWrapper(AIRSteam_ResetAllStats, [bAchievementsToo]);
			return readBoolResponse();
		}

		public function getFileCount():int {
			trace("getFileCount called");
			callWrapper(AIRSteam_GetFileCount);
			return readIntResponse();
		}

		public function getFileSize(fileName:String):int {
			trace("getFileSize called");
			callWrapper(AIRSteam_GetFileSize, [fileName]);
			return readIntResponse();
		}

		public function fileExists(fileName:String):Boolean {
			trace("fileExists called");
			callWrapper(AIRSteam_FileExists, [fileName]);
			return readBoolResponse();
		}

		public function fileWrite(fileName:String, data:ByteArray):Boolean {
			trace("fileWrite called");
			callWrapper(AIRSteam_FileWrite, [fileName, data]);
			return readBoolResponse();
		}

		public function fileRead(fileName:String, data:ByteArray):Boolean {
			trace("fileRead called");
			callWrapper(AIRSteam_FileRead, [fileName]);

			var success:Boolean = readBoolResponse();
			if(success) {
				var content:String = readStringResponse();
				data.writeUTFBytes(content);
				data.position = 0;
			}

			return success;
		}

		public function fileDelete(fileName:String):Boolean {
			trace("fileDelete called");
			callWrapper(AIRSteam_FileDelete, [fileName]);
			return readBoolResponse();
		}

		public function isCloudEnabledForApp():Boolean {
			trace("isCloudEnabledForApp called");
			callWrapper(AIRSteam_IsCloudEnabledForApp);
			return readBoolResponse();
		}

		public function setCloudEnabledForApp(enabled:Boolean):Boolean {
			trace("setCloudEnabledForApp called");
			callWrapper(AIRSteam_SetCloudEnabledForApp, [enabled]);
			return readBoolResponse();
		}
	}
}
