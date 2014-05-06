/*
 *  FRESteamWorksTest.as
 *  This file is part of FRESteamWorks.
 *
 *  Created by David ´Oldes´ Oliva on 3/29/12.
 *  Contributors: Ventero <http://github.com/Ventero>
 *  Copyright (c) 2012 Amanita Design. All rights reserved.
 *  Copyright (c) 2012-2013 Level Up Labs, LLC. All rights reserved.
 */

package {
	import com.amanitadesign.steam.FRESteamWorks;

	import com.amanitadesign.steam.SteamEvent;
	import com.amanitadesign.steam.SteamResults;

	import com.amanitadesign.steam.LeaderboardEntry;
	import com.amanitadesign.steam.UserStatsConstants;
	import com.amanitadesign.steam.UploadLeaderboardScoreResult;

	import com.amanitadesign.steam.FriendConstants;
	import com.amanitadesign.steam.SteamConstants;
	import com.amanitadesign.steam.UserConstants;
	import com.amanitadesign.steam.UserStatsConstants;
	import com.amanitadesign.steam.WorkshopConstants;

	import com.amanitadesign.steam.DownloadUGCResult;
	import com.amanitadesign.steam.FileDetailsResult;
	import com.amanitadesign.steam.FilesByUserActionResult;
	import com.amanitadesign.steam.ItemVoteDetailsResult;
	import com.amanitadesign.steam.SubscribedFilesResult;
	import com.amanitadesign.steam.UserFilesResult;
	import com.amanitadesign.steam.UserVoteDetails;
	import com.amanitadesign.steam.WorkshopFilesResult;

	import flash.desktop.NativeApplication;
	import flash.display.SimpleButton;
	import flash.display.Sprite;
	import flash.display.StageDisplayState;
	import flash.events.Event;
	import flash.events.InvokeEvent;
	import flash.events.MouseEvent;
	import flash.text.TextField;
	import flash.utils.ByteArray;
	import flash.utils.setTimeout;

	public class FRESteamWorksTest extends Sprite {
		public var Steamworks:FRESteamWorks = new FRESteamWorks();
		public var tf:TextField;

		private var _container:Sprite;
		private var _buttonContainer:Sprite;
		private var _fileContainer:Sprite;
		private var _enumerateContainer:Sprite;
		private var _overlayContainer:Sprite;
		private var _leaderboardsContainer:Sprite;
		private var _appId:uint;
		private var _userId:String;

		public function FRESteamWorksTest() {
			// wait for the invoke event before setting up UI etc
			NativeApplication.nativeApplication.addEventListener(InvokeEvent.INVOKE, onInvoke);
		}

		public function onInvoke(invokeEvent:InvokeEvent):void {
			tf = new TextField();
			tf.x = 160;
			tf.width = stage.stageWidth - tf.x;
			tf.height = stage.stageHeight;
			addChild(tf);

			_container = new Sprite();
			addChild(_container);

			_buttonContainer = new Sprite();
			_fileContainer = new Sprite();
			_enumerateContainer = new Sprite();
			_overlayContainer = new Sprite();
			_leaderboardsContainer = new Sprite();
			_container.addChild(_buttonContainer);

			addButton("Check stats/achievements", checkAchievements, _buttonContainer);
			addButton("Toggle achievement", toggleAchievement, _buttonContainer);
			addButton("Check friends", checkFriends, _buttonContainer);
			addButton("Toggle cloud enabled", toggleCloudEnabled, _buttonContainer);
			addButton("Toggle fullscreen", toggleFullscreen, _buttonContainer);
			addButton("Get auth ticket", getAuthTicket, _buttonContainer);
			addButton("Invalid API call", invalidCall, _buttonContainer);
			addButton("File actions", null, _buttonContainer, _fileContainer);
			addButton("Enumerate workshop", null, _buttonContainer, _enumerateContainer);
			addButton("Show overlay", null, _buttonContainer, _overlayContainer);
			addButton("Query leaderboard", null, _buttonContainer, _leaderboardsContainer);

			addButton("Back", null, _enumerateContainer);
			addButton("Sub'd files (unsub/dl/action)", enumerateSubscribedFiles, _enumerateContainer);
			addButton("User published files", enumerateUserPublishedFiles, _enumerateContainer);
			addButton("User shared files", enumerateSharedFiles, _enumerateContainer);
			addButton("All published files (vote)", enumerateWorkshopFiles, _enumerateContainer);
			addButton("By Action (played)", enumeratePlayedFiles, _enumerateContainer);
			addButton("By Action (completed)", enumerateCompletedFiles, _enumerateContainer);

			addButton("Back", null, _fileContainer);
			addButton("Toggle file", toggleFile, _fileContainer);
			addButton("Publish file", publishFile, _fileContainer);
			addButton("Delete published file", deletePublishedFile, _fileContainer);
			addButton("Update file", updateFile, _fileContainer);

			addButton("Back", null, _overlayContainer);
			addButton("Show friends overlay", activateOverlay, _overlayContainer);
			addButton("Show user overlay", activateOverlayToUser, _overlayContainer);
			addButton("Show webpage overlay", activateOverlayToWebpage, _overlayContainer);
			addButton("Show store overlay", activateOverlayToStore, _overlayContainer);
			addButton("Show overlay invite dialog", activateOverlayInvite, _overlayContainer);

			addButton("Back", null, _leaderboardsContainer);
			addButton("Get top global", getTopScores, _leaderboardsContainer);
			addButton("Get user", getUserScore, _leaderboardsContainer);
			addButton("Get user with data", getUserScoreWithData, _leaderboardsContainer);
			addButton("Get around user", getScoresAroundUser, _leaderboardsContainer);
			addButton("Get friends", getFriendScores, _leaderboardsContainer);
			addButton("Upload, keep best", uploadScore, _leaderboardsContainer);
			addButton("Upload, force", uploadForceScore, _leaderboardsContainer);
			addButton("Upload with data", uploadScoreWithData, _leaderboardsContainer);
			addButton("Invalid download", invalidLeaderboardEntries, _leaderboardsContainer);

			Steamworks.addEventListener(SteamEvent.STEAM_RESPONSE, onSteamResponse);
			Steamworks.addOverlayWorkaround(stage, true);
			NativeApplication.nativeApplication.addEventListener(Event.EXITING, onExit);
			try {
				CONFIG::linux {
					if(Steamworks.startProcess("invalid/path")) {
						log("FAILED: Initialization with invalid path succeeded?!");
						return;
					} else {
						log("Steamworks.startProcess() with invalid path passed");
					}
					if (!Steamworks.startProcess()) {
						log("FAILED: Steamworks.startProcess() with default path.");
						return;
					}
				}

				processCommandLine(invokeEvent);

				//Steamworks.useCrashHandler(480, "1.0", "Feb 20 2013", "21:42:20");
				if(!Steamworks.init()){
					log("STEAMWORKS API is NOT available");
					return;
				}

				log("STEAMWORKS API is available\n");
				_userId = Steamworks.getUserID();
				log("User ID: " + _userId);
				_appId = Steamworks.getAppID();
				log("App ID: " + _appId);
				log("Persona name: " + Steamworks.getPersonaName());
				log("Current game language: " + Steamworks.getCurrentGameLanguage());
				log("Available game languages: " + Steamworks.getAvailableGameLanguages());
				log("requestStats() == " + Steamworks.requestStats());
				log("requestGlobalStats(3) == " + Steamworks.requestGlobalStats(3));
				log("isCloudEnabledForApp() == "+Steamworks.isCloudEnabledForApp());
				log("getFileCount() == "+Steamworks.getFileCount());
				log("fileExists('test.txt') == "+Steamworks.fileExists('test.txt'));
				log("getDLCCount() == " + Steamworks.getDLCCount());
				log("isSubscribedApp(" + _appId + ") == " + Steamworks.isSubscribedApp(_appId));
				log("isSubscribedApp(999999) == " + Steamworks.isSubscribedApp(999999));
				// TODO: find better appID to test this for (i.e. one that can
				//       potentially be installed)
				log("isDLCInstalled(999999) == " + Steamworks.isDLCInstalled(999999));
				log("findLeaderboard('Feet Traveled') == " + Steamworks.findLeaderboard('Feet Traveled'));
				log("getEnv('SteamTenfoot') == " + Steamworks.getEnv("SteamTenfoot"));
				log("getEnv('HopefullyNotSet') == " + Steamworks.getEnv('HopefullyNotSet'));

				var quota:Array = Steamworks.getQuota();
				log("getQuota() == " + (quota ? "[" + quota.join(", ") + "]" : "null"));

				Steamworks.resetAllStats(true);
			} catch(e:Error) {
				log("*** ERROR ***");
				log(e.message);
				log(e.getStackTrace());
			}

		}

		private function processCommandLine(invokeEvent:InvokeEvent):void {
			// process command-line arguments
			var argn:uint = 0;
			var argCount:uint = invokeEvent.arguments.length;
			while (argn < argCount) {
				var arg:String = invokeEvent.arguments[argn];
				++argn;
				if (arg == '-appid') {
					if (argn < argCount) {
						testRestartAppIfNecessary(invokeEvent.arguments[argn]);
						++argn;
					} else {
						log("FRESteamWorksTest: 'appid' requires a parameter");
						NativeApplication.nativeApplication.exit(-1);
					}
				} else {
					// invalid argument
					log("FRESteamWorksTest called with invalid argument: " + arg);
					NativeApplication.nativeApplication.exit(-1);
				}
			}
		}

		private function log(value:String):void{
			tf.appendText(value+"\n");
			tf.scrollV = tf.maxScrollV;
		}

		public function testRestartAppIfNecessary(appid:uint):void {
			if (Steamworks.restartAppIfNecessary(appid)) {
				log("App started outside of Steam with no app_id.txt : Steam will restart");
				NativeApplication.nativeApplication.exit(0);
			}
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
			log("getGlobalStatInt('NumGames') == "+Steamworks.getGlobalStatInt('NumGames'));
			log("getGlobalStatFloat('FeetTraveled') == "+Steamworks.getGlobalStatFloat('FeetTraveled'));

			var history:Array = Steamworks.getGlobalStatHistoryInt('NumGames', 3);
			log("getGlobalStatHistoryInt('NumGames', 3) == " +
				history + " " + (history ? history.length : "0"));
			for (var i:int = 0; i < history.length; ++i)
				log("history[" + i + "] == " + history[i]);

			history = Steamworks.getGlobalStatHistoryFloat('FeetTraveled', 10);
			log("getGlobalStatHistoryFloat('FeetTraveled', 10) == " +
				history + " " + (history ? history.length : "0"));
			for (i = 0; i < history.length; ++i)
				log("history[" + i + "] == " + history[i]);
		}

		private function toggleAchievement(e:Event = null):void {
			if(!Steamworks.isReady) return;

			var result:Boolean = Steamworks.isAchievement("ACH_WIN_ONE_GAME");
			log("isAchievement('ACH_WIN_ONE_GAME') == " + result);
			if(!result) {
				log("setAchievement('ACH_WIN_ONE_GAME') == "+Steamworks.setAchievement("ACH_WIN_ONE_GAME"));
			} else {
				log("clearAchievement('ACH_WIN_ONE_GAME') == "+Steamworks.clearAchievement("ACH_WIN_ONE_GAME"));
			}
		}

		private function checkFriends(e:Event = null):void {
			if (!Steamworks.isReady) return;

			var flags:int = FriendConstants.FRIENDFLAG_Immediate;
			var count:int = Steamworks.getFriendCount(flags);
			var id:String;
			log("getFriendCount(Immediate) == " + count);
			if (count > 0) {
				id = Steamworks.getFriendByIndex(0, flags);
				log("getFriendByIndex(0, Immediate) == " + id);
				log("getFriendPersonaName(" + id + ") == " + Steamworks.getFriendPersonaName(id));
			}

			flags = FriendConstants.FRIENDFLAG_Blocked | FriendConstants.FRIENDFLAG_Ignored;
			count = Steamworks.getFriendCount(flags);
			log("getFriendCount(Blocked | Ignored) == " + count);
			if (count > 0) {
				id = Steamworks.getFriendByIndex(0, flags);
				log("getFriendByIndex(0, Blocked | Ignored) == " + id);
				log("getFriendPersonaName(" + id + ") == " + Steamworks.getFriendPersonaName(id));
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

			if(!publishedFile || publishedFile == WorkshopConstants.PUBLISHEDFILEID_Invalid) {
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

		private function getAuthTicket(e:Event = null):void {
			if(!Steamworks.isReady) return;

			authTicket = new ByteArray();
			authHandle = Steamworks.getAuthSessionTicket(authTicket)
			log("getAuthSessionTicket(ticket) == " + authHandle);
			logTicket(authTicket);
		}

		private function logTicket(ticket:ByteArray):void {
			var s:String = "";
			for (var i:int = 0; i < ticket.length; ++i) {
				var n:String = ticket[i].toString(16);
				s += (n.length < 2 ? "0" : "") + n;
			}
			log("Ticket: " + ticket.bytesAvailable + "//" + ticket.length + "\n" + s);
		}

		private function activateOverlay(e:Event = null):void {
			if(!Steamworks.isReady) return;

			log("isOverlayEnabled() == " + Steamworks.isOverlayEnabled());
			log("activateGameOverlay('Friends') == " + Steamworks.activateGameOverlay("Friends"));
			setTimeout(function():void {
				log("isOverlayEnabled() == " + Steamworks.isOverlayEnabled());
			}, 1000);
		}

		private function activateOverlayToUser(e:Event = null):void {
			if(!Steamworks.isReady) return;

			var res:Boolean = Steamworks.activateGameOverlayToUser("steamid", _userId);
			log("activateGameOverlay('steamid', " + _userId + ") == " + res);
		}

		private function activateOverlayToWebpage(e:Event = null):void {
			if(!Steamworks.isReady) return;

			var res:Boolean = Steamworks.activateGameOverlayToWebPage("http://www.steamgames.com/");
			log("activateGameOverlayToWebPage(...) == " + res);
		}

		private function activateOverlayToStore(e:Event = null):void {
			if(!Steamworks.isReady) return;

			var res:Boolean = Steamworks.activateGameOverlayToStore(_appId,
				WorkshopConstants.OVERLAYSTOREFLAG_None);
			log("activateGameOverlayToStore(" + _appId + ", " +
				WorkshopConstants.OVERLAYSTOREFLAG_None + ") == " + res);
		}

		private function activateOverlayInvite(e:Event = null):void {
			if(!Steamworks.isReady) return;

			log("activateGameOverlayInviteDialog('0') == " +
				Steamworks.activateGameOverlayInviteDialog("0"));
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

		private function enumeratePlayedFiles(e:Event = null):void {
			if(!Steamworks.isReady) return;

			var res:Boolean = Steamworks.enumeratePublishedFilesByUserAction(
				WorkshopConstants.FILEACTION_Played, 0);
			log("enumeratePublishedFilesByUserAction(Played, 0) == " + res);
		}

		private function enumerateCompletedFiles(e:Event = null):void {
			if(!Steamworks.isReady) return;

			var res:Boolean = Steamworks.enumeratePublishedFilesByUserAction(
				WorkshopConstants.FILEACTION_Completed, 0);
			log("enumeratePublishedFilesByUserAction(Completed, 0) == " + res);
		}

		private function updateFile(e:Event = null):void {
			if(!publishedFile || publishedFile == WorkshopConstants.PUBLISHEDFILEID_Invalid) {
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

		private function invalidCall(e:Event = null):void {
			if(!Steamworks.isReady) return;

			var res:Boolean = Steamworks.getPublishedFileDetails(undefined);
			log("getPublishedFileDetails(undefined) == " + res);
		}

		private function getTopScores(e:Event = null):void {
			if(!Steamworks.isReady) return;

			if(!leaderboard) {
				log("No Leaderboard handle set")
				return;
			}

			log("downloadLeaderboardEntries(...) == " + Steamworks.downloadLeaderboardEntries(
				leaderboard, UserStatsConstants.DATAREQUEST_Global, 1, 10));
		}

		private function getUserScore(e:Event = null):void {
			getScoresAroundUser(e, 0, 0);
		}

		private function getUserScoreWithData(e:Event = null):void {
			scoreDetails = 3;
			getScoresAroundUser(e, 0, 0);
		}

		private function getScoresAroundUser(e:Event = null, start:int = -4, end:int = 5):void {
			if(!Steamworks.isReady) return;

			if(!leaderboard) {
				log("No Leaderboard handle set")
				return;
			}

			log("downloadLeaderboardEntries(...) == " + Steamworks.downloadLeaderboardEntries(
				leaderboard, UserStatsConstants.DATAREQUEST_GlobalAroundUser, start, end));
		}

		private function getFriendScores(e:Event = null):void {
			if(!Steamworks.isReady) return;

			if(!leaderboard) {
				log("No Leaderboard handle set")
				return;
			}

			log("downloadLeaderboardEntries(...) == " + Steamworks.downloadLeaderboardEntries(
				leaderboard, UserStatsConstants.DATAREQUEST_Friends));
		}

		private function uploadScore(e:Event = null):void {
			if(!Steamworks.isReady) return;

			if(!leaderboard) {
				log("No Leaderboard handle set")
				return;
			}

			log("uploadScore(...) == " + Steamworks.uploadLeaderboardScore(leaderboard,
				UserStatsConstants.UPLOADSCOREMETHOD_KeepBest, 15))
		}

		private function uploadForceScore(e:Event = null):void {
			if(!Steamworks.isReady) return;

			if(!leaderboard) {
				log("No Leaderboard handle set")
				return;
			}

			log("uploadScore(...) == " + Steamworks.uploadLeaderboardScore(leaderboard,
				UserStatsConstants.UPLOADSCOREMETHOD_ForceUpdate, 20))
		}

		private function uploadScoreWithData(e:Event = null):void {
			if(!Steamworks.isReady) return;

			if(!leaderboard) {
				log("No Leaderboard handle set")
				return;
			}

			scoreDetails = 3;
			log("uploadScore(...) == " + Steamworks.uploadLeaderboardScore(leaderboard,
				UserStatsConstants.UPLOADSCOREMETHOD_ForceUpdate, 20, [1, 2, 3]))
		}

		private function invalidLeaderboardEntries(e:Event = null):void {
			if (!Steamworks.isReady) return;

			if (!leaderboard) {
				log("No Leaderboard handle set, continuing anyway");
			}

			log("downloadLeaderboardEntriesResult(3) == " +
				Steamworks.downloadLeaderboardEntriesResult(3));
		}

		private var id:String;
		private var ugcHandle:String;
		private var publishedFile:String;
		private var leaderboard:String;
		private var scoreDetails:int = 0;
		private var authHandle:uint = 0;
		private var authTicket:ByteArray = null;
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
				case SteamConstants.RESPONSE_OnGlobalStatsReceived:
					log("RESPONSE_OnGlobalStatsReceived: " + e.response);
					break;
				case SteamConstants.RESPONSE_OnFindLeaderboard:
					log("RESPONSE_OnFindLeaderboad: " + e.response);
					if(e.response != SteamResults.OK) {
						log("FAILED!");
						break;
					}

					if (leaderboard) {
						// result of findOrCreateLeaderboard
						var newLeaderboard:String = Steamworks.findLeaderboardResult();
						log("findLeaderboardResult() == " + newLeaderboard);
						if (newLeaderboard != leaderboard)
							log("FAILURE: findOrCreateLeaderboard() returned different leaderboard");

						break;
					}

					leaderboard = Steamworks.findLeaderboardResult();
					var name:String = Steamworks.getLeaderboardName(leaderboard)
					var sortMethod:int = Steamworks.getLeaderboardSortMethod(leaderboard);
					var displayType:int = Steamworks.getLeaderboardDisplayType(leaderboard)
					log("findLeaderboardResult() == " + leaderboard);
					log("getLeaderboardName(...) == " + name);
					log("getLeaderboardEntryCount(...) == " + Steamworks.getLeaderboardEntryCount(leaderboard));
					log("getLeaderboardSortMethod(...) == " + sortMethod);
					log("getLeaderboardDisplayType(...) == " + displayType);
					log("findOrCreateLeaderboard(...) == " + Steamworks.findOrCreateLeaderboard(
						name, sortMethod, displayType));
					break;
				case SteamConstants.RESPONSE_OnDownloadLeaderboardEntries:
					log("RESPONSE_OnDownloadLeaderboardEntries: " + e.response);
					if(e.response != SteamResults.OK) {
						log("FAILED!");
						break;
					}

					var entries:Array = Steamworks.downloadLeaderboardEntriesResult(scoreDetails);
					log("downloadLeaderboardEntriesResult(" + scoreDetails + ") == " + (entries ? ("Array, size " + entries.length) : "null"));
					for(i = 0; i < entries.length; ++i) {
						var en:LeaderboardEntry = entries[i];
						log(i + ": " + en.userID + ", " + en.globalRank + ", " + en.score + ", " + en.numDetails + "//" + en.details.length);
						for(var d:int = 0; d < en.details.length; ++d)
							log("\tdetails[" + d + "] == " + en.details[d]);
					}
					scoreDetails = 0;
					break;
				case SteamConstants.RESPONSE_OnUploadLeaderboardScore:
					log("RESPONSE_OnUploadLeaderboardScore: " + e.response);
					if(e.response != SteamResults.OK) {
						log("FAILED!");
						break;
					}

					var sr:UploadLeaderboardScoreResult = Steamworks.uploadLeaderboardScoreResult();
					log("uploadLeaderboardScoreResult() == " + sr);
					log("success: " + sr.success + ", score: " + sr.score + ", changed: " + sr.scoreChanged +
						", rank: " + sr.previousGlobalRank + " -> " + sr.newGlobalRank);
					getUserScore(null);
					break;
				case SteamConstants.RESPONSE_OnFileShared:
					log("RESPONSE_OnFileShared: " + e.response);
					if(e.response != SteamResults.OK) {
						log("FAILED!");
						break;
					}

					log("fileShareResult() == " + Steamworks.fileShareResult());
					apiCall = Steamworks.publishWorkshopFile("test.txt", "", _appId,
						"Test.txt", "Test.txt", WorkshopConstants.VISIBILITY_Private,
						["TestTag"], WorkshopConstants.FILETYPE_Community);
					log("publishWorkshopFile('test.txt' ...) == " + apiCall);
					break;
				case SteamConstants.RESPONSE_OnPublishWorkshopFile:
					log("RESPONSE_OnPublishWorkshopFile: " + e.response);
					if(e.response != SteamResults.OK) {
						log("FAILED!");
						break;
					}

					publishedFile = Steamworks.publishWorkshopFileResult();
					log("File published as " + publishedFile);
					if (publishedFile == WorkshopConstants.PUBLISHEDFILEID_Invalid) {
						log("FAILED!");
						break;
					}

					log("subscribePublishedFile(...) == " + Steamworks.subscribePublishedFile(publishedFile));
					break;
				case SteamConstants.RESPONSE_OnEnumerateUserSubscribedFiles:
					log("RESPONSE_OnEnumerateUserSubscribedFiles: " + e.response);
					if(e.response != SteamResults.OK) {
						log("FAILED!");
						break;
					}

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

					if(subRes.resultsReturned > 0) {
						apiCall = Steamworks.setUserPublishedFileAction(
							subRes.publishedFileId[0], WorkshopConstants.FILEACTION_Played);
						log("setUserPublishedFileAction(..., Played) == " + apiCall);
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
					if(e.response != SteamResults.OK) {
						log("FAILED!");
						break;
					}

					var fileRes:WorkshopFilesResult = Steamworks.enumeratePublishedWorkshopFilesResult();
					log("Workshop files: " + fileRes.resultsReturned + "/" + fileRes.totalResults);
					for(i = 0; i < fileRes.resultsReturned; i++) {
						log(i + ": " + fileRes.publishedFileId[i] + " - " + fileRes.score[i]);
					}
					if(fileRes.resultsReturned > 0) {
						var f:String = fileRes.publishedFileId[0];
						apiCall = Steamworks.updateUserPublishedItemVote(f, true);
						log("updateUserPublishedItemVote(" + f + ", true) == " + apiCall);
						apiCall = Steamworks.getPublishedItemVoteDetails(f);
						log("getPublishedItemVoteDetails(" + f + ") == " + apiCall);
						apiCall = Steamworks.getUserPublishedItemVoteDetails(f);
						log("getUserPublishedItemVoteDetails(" + f + ") == " + apiCall);
					}
					break;
				case SteamConstants.RESPONSE_OnEnumeratePublishedFilesByUserAction:
					log("RESPONSE_OnEnumeratePublishedFilesByUserAction: " + e.response);
					if(e.response != SteamResults.OK) {
						log("FAILED!");
						break;
					}

					var actionRes:FilesByUserActionResult = Steamworks.enumeratePublishedFilesByUserActionResult();
					// TODO: m_eAction seems to be uninitialized?
					log("Files for action " + actionRes.action + ": " +
						actionRes.resultsReturned + "/" + actionRes.totalResults);

					for(i = 0; i < actionRes.resultsReturned; i++) {
						log(i + ": " + actionRes.publishedFileId[i] + " - " + actionRes.timeUpdated[i]);
						if (actionRes.action == WorkshopConstants.FILEACTION_Played) {
							log("setUserPublishedFileAction(..., Completed) == " +
								Steamworks.setUserPublishedFileAction(actionRes.publishedFileId[i],
								WorkshopConstants.FILEACTION_Completed));
						}
					}
					break;
				case SteamConstants.RESPONSE_OnGetPublishedFileDetails:
					log("RESPONSE_OnGetPublishedFileDetails: " + e.response);
					if(e.response != SteamResults.OK) {
						log("FAILED!");
						break;
					}

					var res:FileDetailsResult = Steamworks.getPublishedFileDetailsResult(id);
					log("Result for " + id + ": " + res);
					if(res) {
						log("File: " + res.fileName + ", handle: " + res.fileHandle);
						ugcHandle = res.fileHandle;
						apiCall = Steamworks.UGCDownload(res.fileHandle, 0);
						log("UGCDownload(...) == " + apiCall);
						var progress:Array = Steamworks.getUGCDownloadProgress(res.fileHandle);
						log("getUGCDownloadProgress(...) == " + progress);
						setTimeout(function():void {
							var progress:Array = Steamworks.getUGCDownloadProgress(res.fileHandle);
							log("getUGCDownloadProgress(...) == " + progress);
						}, 1000);
					}
					break;
				case SteamConstants.RESPONSE_OnUGCDownload:
					log("RESPONSE_OnUGCDownload: " + e.response);
					if(e.response != SteamResults.OK) {
						log("FAILED!");
						break;
					}

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
					if(e.response != SteamResults.OK) {
						log("FAILED!");
						break;
					}

					var voteDetails:ItemVoteDetailsResult = Steamworks.getPublishedItemVoteDetailsResult();
					log("getPublishedItemVoteDetails() == " + (voteDetails ? voteDetails.result : "null"));
					if(!voteDetails) return;

					// no native JSON support for Linux ...
					log("votes: " + voteDetails.votesFor + "//" + voteDetails.votesAgainst +
						", score: " + voteDetails.score + ", reports: " + voteDetails.reports);
					break;
				case SteamConstants.RESPONSE_OnGetUserPublishedItemVoteDetails:
					log("RESPONSE_OnGetUserPublishedItemVoteDetails: " + e.response);
					if(e.response != SteamResults.OK) {
						log("FAILED!");
						break;
					}

					var userVoteDetails:UserVoteDetails = Steamworks.getUserPublishedItemVoteDetailsResult();
					log("getUserPublishedItemVoteDetails() == " + (userVoteDetails ? userVoteDetails.result : "null"));
					if(!userVoteDetails) return;

					log("vote: " + userVoteDetails.vote);
					break;

				case SteamConstants.RESPONSE_OnGetAuthSessionTicketResponse:
					log("RESPONSE_OnGetAuthSessionTicketResponse: " + e.response);
					if(e.response != SteamResults.OK) {
						log("FAILED!");
						break;
					}

					if (authHandle == UserConstants.AUTHTICKET_Invalid) {
						log("Invalid authHandle (cancelled?)");
						break;
					}

					var realAuthHandle:uint = Steamworks.getAuthSessionTicketResult();
					log("getAuthSessionTicketResult() == " + realAuthHandle);
					log("equal to original handle? " + (realAuthHandle == authHandle));
					authHandle = realAuthHandle;

					log("beginAuthSession(ticket, " + _userId + ") == " + Steamworks.beginAuthSession(
						authTicket, _userId));

					break;
				case SteamConstants.RESPONSE_OnValidateAuthTicketResponse:
					log("RESPONSE_OnValidateAuthTicketResponse: " + e.response);
					if(e.response != UserConstants.SESSION_OK) {
						log("FAILED!");
						break;
					}

					/* should be equal to UserConstants.LICENSE_HasLicense */
					log("userHasLicenseForApp(...) == " + (Steamworks.userHasLicenseForApp(_userId, _appId)) +
						"( == " + UserConstants.LICENSE_HasLicense + "?)");
					log("userHasLicenseForApp(..., 999999) == " + Steamworks.userHasLicenseForApp(_userId, 999999));

					log("endAuthSession(" + _userId + ") == " + Steamworks.endAuthSession( _userId));
					log("cancelAuthTicket(" + authHandle + ") == " + Steamworks.cancelAuthTicket(authHandle));
					authHandle = UserConstants.AUTHTICKET_Invalid;

					break;
				case SteamConstants.RESPONSE_OnMicroTxnAuthorizationResponse:
					log("RESPONSE_OnMicroTxnAuthorizationResponse: " + e.response);
					log("MicroTxnOrderIDResult() == " + Steamworks.microTxnOrderIDResult());
					if(e.response != SteamResults.OK) {
						log("FAILED!");
					} else {
						log("SUCCESS!");
					}

					break;
				default:
					log("STEAMresponse type:"+e.req_type+" response:"+e.response);
			}
		}

		private function onExit(e:Event):void{
			log("Exiting application, cleaning up");
			Steamworks.dispose();
		}

		private function showButtonContainer(cont:Sprite):void {
			while (_container.numChildren > 0)
				_container.removeChildAt(0);

			_container.addChild(cont);
		}

		private function addButton(label:String, callback:Function, container:Sprite,
			showContainer:Sprite = null):void {
			var button:Sprite = new Sprite();
			button.graphics.beginFill(0xaaaaaa);
			button.graphics.drawRoundRect(0, 0, 150, 25, 5, 5);
			button.graphics.endFill();
			button.buttonMode = true;
			button.useHandCursor = true;
			button.x = 5;
			button.y = container.height + 5;

			button.addEventListener(MouseEvent.CLICK, function(e:MouseEvent):void {
				showButtonContainer(showContainer || _buttonContainer);
				if (callback != null) callback();
			});

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
			container.addChild(button);
		}
	}
}
