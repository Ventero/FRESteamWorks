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
	import com.amanitadesign.steam.SteamResults;
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

		private var _buttonPos:int = 0;
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
			addButton("Delete published file", deletePublishedFile);
			addButton("Toggle fullscreen", toggleFullscreen);
			addButton("Show Friends overlay", activateOverlay);
			addButton("List subscribed files", enumerateSubscribedFiles);
			addButton("List published files", enumerateUserPublishedFiles)
			addButton("List shared files", enumerateSharedFiles);
			addButton("List workshop files", enumerateWorkshopFiles);
			addButton("Update file", updateFile);

			Steamworks.addEventListener(SteamEvent.STEAM_RESPONSE, onSteamResponse);
			Steamworks.addOverlayWorkaround(stage, true);
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
				log("isCloudEnabledForApp() == "+Steamworks.isCloudEnabledForApp());
				log("getFileCount() == "+Steamworks.getFileCount());
				log("fileExists('test.txt') == "+Steamworks.fileExists('test.txt'));
				log("getDLCCount() == " + Steamworks.getDLCCount());
				log("isSubscribedApp(" + _appId + ") == " + Steamworks.isSubscribedApp(_appId));
				log("isSubscribedApp(999999) == " + Steamworks.isSubscribedApp(999999));

				var quota:Array = Steamworks.getQuota();
				log("getQuota() == " + (quota ? "[" + quota.join(", ") + "]" : "null"));

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
				log("fileDelete('updated_test.txt') == "+Steamworks.fileDelete('updated_test.txt'));
			} else {
				log("writeFileToCloud('test.txt','click') == "+writeFileToCloud('test.txt','click'));
				log("writeFileToCloud('updated_test.txt',...) == "+writeFileToCloud('updated_test.txt','updated content'));
			}
		}

		private function publishFile(e:Event = null):void {
			if(!Steamworks.isReady) return;

			// we need to make sure the file has actually been uploaded before we can
			// call publishWorkshopFile
			log("fileShare('test.txt') == " + Steamworks.fileShare("test.txt"));
		}

		private function deletePublishedFile(e:Event = null):void {
			if(!Steamworks.isReady) return;

			if(!publishedFile) {
				log("No file handle set, publish or enumerate first");
				return;
			}

			var res:Boolean = Steamworks.unsubscribePublishedFile(publishedFile);
			log("unsubscribePublishedFile(" + publishedFile + ") == " + res);
			res = Steamworks.deletePublishedFile(publishedFile);
			log("deletePublishedFile(" + publishedFile + ") == " + res);
		}

		private function toggleFullscreen(e:Event = null):void {
			if(stage.displayState == StageDisplayState.NORMAL)
				stage.displayState = StageDisplayState.FULL_SCREEN_INTERACTIVE;
			else
				stage.displayState = StageDisplayState.NORMAL;
		}

		private function activateOverlay(e:Event = null):void {
			if(!Steamworks.isReady) return;

			log("isOverlayEnabled() == " + Steamworks.isOverlayEnabled());
			log("activateGameOverlay('Friends') == " + Steamworks.activateGameOverlay("Friends"));
			log("isOverlayEnabled() == " + Steamworks.isOverlayEnabled());
		}

		private function enumerateSubscribedFiles(e:Event = null):void {
			if(!Steamworks.isReady) return;

			log("enumerateUserSubscribedFiles(0) == " + Steamworks.enumerateUserSubscribedFiles(0));
		}

		private function enumerateUserPublishedFiles(e:Event = null):void {
			if(!Steamworks.isReady) return;

			log("enumerateUserPublishedFiles(0) == " + Steamworks.enumerateUserPublishedFiles(0));
		}

		private function enumerateSharedFiles(e:Event = null):void {
			if(!Steamworks.isReady) return;

			var userID:String = Steamworks.getUserID();
			var res:Boolean = Steamworks.enumerateUserSharedWorkshopFiles(userID, 0, [], []);
			log("enumerateSharedFiles(" + userID + ", 0, [], []) == " + res);
		}

		private function enumerateWorkshopFiles(e:Event = null):void {
			if(!Steamworks.isReady) return;

			var res:Boolean = Steamworks.enumeratePublishedWorkshopFiles(
				WorkshopConstants.ENUMTYPE_RankedByVote, 0, 10, 0, [], []);
			log("enumerateSharedFiles(...) == " + res);
		}

		private function updateFile(e:Event = null):void {
			if(!publishedFile) {
				log("No file handle set, publish or enumerate first");
				return;
			}

			var handle:String = Steamworks.createPublishedFileUpdateRequest(publishedFile);
			log("createPublishedFileUpdateRequest() == " + handle);
			if(handle == WorkshopConstants.FILEUPDATEHANDLE_Invalid) return;

			var res:Boolean = Steamworks.updatePublishedFileDescription(handle,
				"Test updated description");
			log("updatePublishedFileDescription(...) == " + res);
			if(!res) return;

			if(Steamworks.fileExists("updated_test.txt")) {
				res = Steamworks.updatePublishedFileFile(handle, "updated_test.txt");
				log("updatePublishedFileFile(...) == " + res);
				if(!res) return;
			}

			res = Steamworks.updatePublishedFileTitle(handle, "Updated test title");
			log("updatePublishedFileTitle(...) == " + res);
			if(!res) return;

			res = Steamworks.updatePublishedFileTags(handle, ["TestTag", "Updated TestTag"]);
			log("updatePublishedFileTags(...) = " + res);
			if(!res) return;

			res = Steamworks.updatePublishedFileSetChangeDescription(handle, "Test update!");
			log("updatePublishedFileSetChangeDescription(...) == " + res);
			if(!res) return;

			res = Steamworks.updatePublishedFileVisibility(handle, WorkshopConstants.VISIBILITY_Public);
			log("updatePublishedFileVisibility(Public) == " + res);
			if(!res) return;

			res = Steamworks.commitPublishedFileUpdate(handle);
			log("commitPublishedFileUpdate(...) == " + res);
		}

		private var id:String;
		private var ugcHandle:String;
		private var publishedFile:String;
		private function onSteamResponse(e:SteamEvent):void{
			var apiCall:Boolean;
			var i:int;
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
				case SteamConstants.RESPONSE_OnFileShared:
					log("RESPONSE_OnFileShared: " + e.response);
					if(e.response != SteamResults.OK) return;
					log("fileShareResult() == " + Steamworks.fileShareResult());
					apiCall = Steamworks.publishWorkshopFile("test.txt", "", _appId,
						"Test.txt", "Test.txt", WorkshopConstants.VISIBILITY_Private,
						["TestTag"], WorkshopConstants.FILETYPE_Community);
					log("publishWorkshopFile('test.txt' ...) == " + apiCall);
					break;
				case SteamConstants.RESPONSE_OnPublishWorkshopFile:
					log("RESPONSE_OnPublishWorkshopFile: " + e.response);
					if(e.response != SteamResults.OK) return;
					publishedFile = Steamworks.publishWorkshopFileResult();
					log("File published as " + publishedFile);
					log("subscribePublishedFile(...) == " + Steamworks.subscribePublishedFile(publishedFile));
					break;
				case SteamConstants.RESPONSE_OnEnumerateUserSubscribedFiles:
					log("RESPONSE_OnEnumerateUserSubscribedFiles: " + e.response);
					if(e.response != SteamResults.OK) return;
					var subRes:SubscribedFilesResult = Steamworks.enumerateUserSubscribedFilesResult();
					log("User subscribed files: " + subRes.resultsReturned + "/" + subRes.totalResults);
					for(i = 0; i < subRes.resultsReturned; i++) {
						id = subRes.publishedFileId[i];
						apiCall = Steamworks.getPublishedFileDetails(subRes.publishedFileId[i]);
						log(i + ": " + subRes.publishedFileId[i] + " (" + subRes.timeSubscribed[i] + ") - " + apiCall);
					}

					// only unsubscribe if we're subscribed to more than one file, so we
					// can continue testing UGCDownload etc.
					if(subRes.resultsReturned > 1) {
						var subbedFile:String = subRes.publishedFileId[1]
						apiCall = Steamworks.unsubscribePublishedFile(subbedFile);
						log("unsubscribePublishedFile(" + subbedFile + ") == " + apiCall);
					}
					break;
				case SteamConstants.RESPONSE_OnEnumerateUserSharedWorkshopFiles:
				case SteamConstants.RESPONSE_OnEnumerateUserPublishedFiles:
					var shared:Boolean = (e.req_type == SteamConstants.RESPONSE_OnEnumerateUserSharedWorkshopFiles);
					log((shared ?
						"RESPONSE_OnEnumerateUserSharedWorkshopFile" :
						"RESPONSE_OnEnumerateUserPublishedFiles: ") + e.response);
					if(e.response != SteamResults.OK) return;
					var userRes:UserFilesResult = shared ?
						Steamworks.enumerateUserSharedWorkshopFilesResult() :
						Steamworks.enumerateUserPublishedFilesResult();

					log("User " + (shared ? "shared" : "published") +" files: " +
						userRes.resultsReturned + "/" + userRes.totalResults);

					for(i = 0; i < userRes.resultsReturned; i++) {
						log(i + ": " + userRes.publishedFileId[i]);
					}

					if(userRes.resultsReturned > 0)
						publishedFile = userRes.publishedFileId[0];

					break;
				case SteamConstants.RESPONSE_OnEnumeratePublishedWorkshopFiles:
					log("RESPONSE_OnEnumeratePublishedWorkshopFiles: " + e.response);
					if(e.response != SteamResults.OK) return;
					var fileRes:WorkshopFilesResult = Steamworks.enumeratePublishedWorkshopFilesResult();
					log("Workshop files: " + fileRes.resultsReturned + "/" + fileRes.totalResults);
					for(i = 0; i < fileRes.resultsReturned; i++) {
						log(i + ": " + fileRes.publishedFileId[i] + " - " + fileRes.score[i]);
					}
					if(fileRes.resultsReturned > 0) {
						var f:String = fileRes.publishedFileId[0];
						log("updateUserPublishedItemVote(" + f + ", true)");
						apiCall = Steamworks.updateUserPublishedItemVote(f, true);
						log("updateUserPublishedItemVote(" + f + ", true) == " + apiCall);
						apiCall = Steamworks.getPublishedItemVoteDetails(f);
						log("getPublishedItemVoteDetails(" + f + ") == " + apiCall);
					}
					break;
				case SteamConstants.RESPONSE_OnGetPublishedFileDetails:
					log("RESPONSE_OnGetPublishedFileDetails: " + e.response);
					if(e.response != SteamResults.OK) return;
					var res:FileDetailsResult = Steamworks.getPublishedFileDetailsResult(id);
					log("Result for " + id + ": " + res);
					if(res) {
						log("File: " + res.fileName + ", handle: " + res.fileHandle);
						ugcHandle = res.fileHandle;
						apiCall = Steamworks.UGCDownload(res.fileHandle, 0);
						log("UGCDownload(...) == " + apiCall);
					}
					break;
				case SteamConstants.RESPONSE_OnUGCDownload:
					log("RESPONSE_OnUGCDownload: " + e.response);
					if(e.response != SteamResults.OK) return;
					var ugcResult:DownloadUGCResult = Steamworks.getUGCDownloadResult(ugcHandle);
					log("Result for " + ugcHandle + ": " + ugcResult);
					if(ugcResult) {
						log("File: " + ugcResult.fileName + ", handle: " + ugcResult.fileHandle + ", size: " + ugcResult.size);
						var ba:ByteArray = new ByteArray();
						ba.length = ugcResult.size;
						apiCall = Steamworks.UGCRead(ugcResult.fileHandle, ugcResult.size, 0, ba);
						log("UGCRead(...) == " + apiCall);
						if(apiCall) {
							log("Result length: " + ba.position + "//" + ba.length);
							log("Result: " + ba.readUTFBytes(ugcResult.size));
						}
					}
					break;
				case SteamConstants.RESPONSE_OnGetPublishedItemVoteDetails:
					log("RESPONSE_OnGetPublishedItemVoteDetails: " + e.response);
					if(e.response != SteamResults.OK) return;

					var voteDetails:ItemVoteDetailsResult = Steamworks.getPublishedItemVoteDetailsResult();
					log("getPublishedItemVoteDetails() == " + (voteDetails ? voteDetails.result : "null"));
					if(!voteDetails) return;

					// no native JSON support for Linux ...
					log("votes: " + voteDetails.votesFor + "//" + voteDetails.votesAgainst +
						", score: " + voteDetails.score + ", reports: " + voteDetails.reports);
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
			button.graphics.drawRoundRect(0, 0, 150, 25, 5, 5);
			button.graphics.endFill();
			button.buttonMode = true;
			button.useHandCursor = true;
			button.addEventListener(MouseEvent.CLICK, callback);
			button.x = 5;
			button.y = _buttonPos;
			_buttonPos += button.height + 5;

			button.addEventListener(MouseEvent.MOUSE_OVER, function(e:MouseEvent):void {
				button.graphics.clear();
				button.graphics.beginFill(0xccccccc);
				button.graphics.drawRoundRect(0, 0, 150, 25, 5, 5);
				button.graphics.endFill();
			});

			button.addEventListener(MouseEvent.MOUSE_OUT, function(e:MouseEvent):void {
				button.graphics.clear();
				button.graphics.beginFill(0xaaaaaa);
				button.graphics.drawRoundRect(0, 0, 150, 25, 5, 5);
				button.graphics.endFill();
			});

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
