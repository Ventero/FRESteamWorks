/*
 *  FRESteamWorks.h
 *  This file is part of FRESteamWorks.
 *
 *  Created by David ´Oldes´ Oliva on 3/29/12.
 *  Contributors: Ventero <http://github.com/Ventero>
 *  Copyright (c) 2012 Amanita Design. All rights reserved.
 *  Copyright (c) 2012-2013 Level Up Labs, LLC. All rights reserved.
 */

package
{
	import com.amanitadesign.steam.FRESteamWorks;
	import com.amanitadesign.steam.SteamConstants;
	import com.amanitadesign.steam.SteamEvent;
	import com.amanitadesign.steam.WorkshopConstants;

	import flash.desktop.NativeApplication;
	import flash.display.SimpleButton;
	import flash.display.Sprite;
	import flash.display.StageDisplayState;
	import flash.events.Event;
	import flash.events.MouseEvent;
	import flash.text.TextField;
	import flash.utils.ByteArray;

	public class FRESteamWorksTest extends Sprite
	{
		public var Steamworks:FRESteamWorks = new FRESteamWorks();
		public var tf:TextField;

		private var _buttonPos:int = 5;
		private var _appId:uint;

		public function FRESteamWorksTest()
		{
			tf = new TextField();
			tf.x = 160;
			tf.width = stage.stageWidth - tf.x;
			tf.height = stage.stageHeight;
			addChild(tf);

			addButton("Check stats/achievements", checkAchievements);
			addButton("Toggle achievement", toggleAchievement);
			addButton("Toggle cloud enabled", toggleCloudEnabled);
			addButton("Toggle file", toggleFile);
			addButton("Publish file", publishFile);
			addButton("Toggle fullscreen", toggleFullscreen);
			addButton("Show Friends overlay", activateOverlay);
			addButton("List subscribed files", enumerateSubscribedFiles);

			Steamworks.addEventListener(SteamEvent.STEAM_RESPONSE, onSteamResponse);
			NativeApplication.nativeApplication.addEventListener(Event.EXITING, onExit);
			try {
				//Steamworks.useCrashHandler(480, "1.0", "Feb 20 2013", "21:42:20");
				if(!Steamworks.init()){
					log("STEAMWORKS API is NOT available");
					return;
				}

				log("STEAMWORKS API is available\n");
				log("User ID: " + Steamworks.getUserID());
				_appId = Steamworks.getAppID();
				log("App ID: " + _appId);
				log("Persona name: " + Steamworks.getPersonaName());
				log("isCloudEnabledForApp() == "+Steamworks.isCloudEnabledForApp() );
				log("getFileCount() == "+Steamworks.getFileCount() );
				log("fileExists('test.txt') == "+Steamworks.fileExists('test.txt') );

				Steamworks.resetAllStats(true);
			} catch(e:Error) {
				log("*** ERROR ***");
				log(e.message);
				log(e.getStackTrace());
			}

		}
		private function log(value:String):void{
			tf.appendText(value+"\n");
			tf.scrollV = tf.maxScrollV;
		}

		public function writeFileToCloud(fileName:String, data:String):Boolean {
			var dataOut:ByteArray = new ByteArray();
			dataOut.writeUTFBytes(data);
			return Steamworks.fileWrite(fileName, dataOut);
		}

		public function readFileFromCloud(fileName:String):String {
			var dataIn:ByteArray = new ByteArray();
			var result:String;
			dataIn.position = 0;
			dataIn.length = Steamworks.getFileSize(fileName);

			if(dataIn.length>0 && Steamworks.fileRead(fileName,dataIn)){
				result = dataIn.readUTFBytes(dataIn.length);
			}
			return result;
		}

		private function checkAchievements(e:Event = null):void {
			if(!Steamworks.isReady) return;

			// current stats and achievement ids are from steam example app
			log("isAchievement('ACH_WIN_ONE_GAME') == "+Steamworks.isAchievement("ACH_WIN_ONE_GAME"));
			log("isAchievement('ACH_TRAVEL_FAR_SINGLE') == "+Steamworks.isAchievement("ACH_TRAVEL_FAR_SINGLE"));
			log("setStatFloat('FeetTraveled') == "+Steamworks.setStatFloat('FeetTraveled', 21.3));
			log("setStatInt('NumGames', 2) == "+Steamworks.setStatInt('NumGames', 2));
			Steamworks.storeStats();
			log("getStatInt('NumGames') == "+Steamworks.getStatInt('NumGames'));
			log("getStatFloat('FeetTraveled') == "+Steamworks.getStatFloat('FeetTraveled'));
		}

		private function toggleAchievement(e:Event = null):void{
			if(!Steamworks.isReady) return;

			var result:Boolean = Steamworks.isAchievement("ACH_WIN_ONE_GAME");
			log("isAchievement('ACH_WIN_ONE_GAME') == " + result);
			if(!result) {
				log("setAchievement('ACH_WIN_ONE_GAME') == "+Steamworks.setAchievement("ACH_WIN_ONE_GAME"));
			} else {
				log("clearAchievement('ACH_WIN_ONE_GAME') == "+Steamworks.clearAchievement("ACH_WIN_ONE_GAME"));
			}
		}

		private function toggleCloudEnabled(e:Event = null):void {
			if(!Steamworks.isReady) return;

			var enabled:Boolean = Steamworks.isCloudEnabledForApp();
			log("isCloudEnabledForApp() == " + enabled);
			log("setCloudEnabledForApp(" + !enabled + ") == " + Steamworks.setCloudEnabledForApp(!enabled));
			log("isCloudEnabledForApp() == " + Steamworks.isCloudEnabledForApp());
		}

		private function toggleFile(e:Event = null):void {
			if(!Steamworks.isReady) return;

			var result:Boolean = Steamworks.fileExists('test.txt');
			log("fileExists('test.txt') == " + result);
			if(result){
				log("readFileFromCloud('test.txt') == "+readFileFromCloud('test.txt') );
				log("fileDelete('test.txt') == "+Steamworks.fileDelete('test.txt'));
			} else {
				log("writeFileToCloud('test.txt','click') == "+writeFileToCloud('test.txt','click'));
			}
		}

		private function publishFile(e:Event = null):void {
			if(!Steamworks.isReady) return;

			var res:Boolean = Steamworks.publishWorkshopFile("test.txt", "", _appId,
				"Test.txt", "Test.txt", WorkshopConstants.VISIBILITY_Private,
				["TestTag"], WorkshopConstants.FILETYPE_Community);
			log("publishWorkshopFile('test.txt' ...) == " + res);
		}

		private function toggleFullscreen(e:Event = null):void {
			if(stage.displayState == StageDisplayState.NORMAL)
				stage.displayState = StageDisplayState.FULL_SCREEN_INTERACTIVE;
			else
				stage.displayState = StageDisplayState.NORMAL;
		}

		private function activateOverlay(e:Event = null):void {
			if(!Steamworks.isReady) return;

			log("activateGameOverlay('Friends') == " + Steamworks.activateGameOverlay("Friends"));
		}

		private function enumerateSubscribedFiles(e:Event = null):void {
			if(!Steamworks.isReady) return;

			log("enumerateUserSubscribedFiles(0) == " + Steamworks.enumerateUserSubscribedFiles(0));
		}

		private function onSteamResponse(e:SteamEvent):void{
			switch(e.req_type){
				case SteamConstants.RESPONSE_OnUserStatsStored:
					log("RESPONSE_OnUserStatsStored: "+e.response);
					break;
				case SteamConstants.RESPONSE_OnUserStatsReceived:
					log("RESPONSE_OnUserStatsReceived: "+e.response);
					break;
				case SteamConstants.RESPONSE_OnAchievementStored:
					log("RESPONSE_OnAchievementStored: "+e.response);
					break;
				case SteamConstants.RESPONSE_OnPublishWorkshopFile:
					log("RESPONSE_OnPublishWorkshopFile: " + e.response);
					var file:String = Steamworks.publishWorkshopFileResult();
					log("File published as " + file);
					log("subscribePublishedFile(...) == " + Steamworks.subscribePublishedFile(file));
					break;
				case SteamConstants.RESPONSE_OnEnumerateUserSubscribedFiles:
					log("RESPONSE_OnEnumerateUserSubscribedFiles: " + e.response);
					var result:SubscribedFilesResult = Steamworks.enumerateUserSubscribedFilesResult();
					log("User subscribed files: " + result.resultsReturned + "/" + result.totalResults);
					for(var i:int = 0; i < result.resultsReturned; i++)
						log(i + ": " + result.publishedFileId[i] + " (" + result.timeSubscribed[i] + ")");
					break;
				default:
					log("STEAMresponse type:"+e.req_type+" response:"+e.response);
			}
		}

		private function onExit(e:Event):void{
			log("Exiting application, cleaning up");
			Steamworks.dispose();
		}

		private function addButton(label:String, callback:Function):void {
			var button:Sprite = new Sprite();
			button.graphics.beginFill(0xaaaaaa);
			button.graphics.drawRoundRect(0, 0, 150, 30, 5, 5);
			button.graphics.endFill();
			button.buttonMode = true;
			button.useHandCursor = true;
			button.addEventListener(MouseEvent.CLICK, callback);
			button.x = 5;
			button.y = _buttonPos;
			_buttonPos += button.height + 5;

			var text:TextField = new TextField();
			text.text = label;
			text.width = 140;
			text.height = 25;
			text.x = 5;
			text.y = 5;
			text.mouseEnabled = false;

			button.addChild(text);
			addChild(button);
		}
	}
}
