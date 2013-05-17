/*
 *  FRESteamWorks.as
 *  This file is part of FRESteamWorks.
 *
 *  Created by David ´Oldes´ Oliva on 3/29/12.
 *  Contributors: Ventero <http://github.com/Ventero>
 *  Copyright (c) 2012 Amanita Design. All rights reserved.
 *  Copyright (c) 2012-2013 Level Up Labs, LLC. All rights reserved.
 */

package com.amanitadesign.steam
{
	import flash.events.Event;
	import flash.events.EventDispatcher;
	import flash.events.IEventDispatcher;
	import flash.events.StatusEvent;
	import flash.external.ExtensionContext;
	import flash.utils.ByteArray;
	import flash.utils.clearInterval;
	import flash.utils.setInterval;

	public class FRESteamWorks extends EventDispatcher
	{
		[Event(name="steam_response", type="com.amanitadesign.steam.SteamEvent")]

		private var _ExtensionContext:ExtensionContext;
		private var _tm:int;

		public var isReady:Boolean = false;

		public function FRESteamWorks(target:IEventDispatcher=null)
		{
			_ExtensionContext = ExtensionContext.createExtensionContext("com.amanitadesign.steam.FRESteamWorks", null);
			_ExtensionContext.addEventListener(StatusEvent.STATUS, handleStatusEvent);
			super(target);
		}

		private function handleStatusEvent(event:StatusEvent):void{
			//_ExtensionContext.removeEventListener(StatusEvent.STATUS, handleStatusEvent);
			var req_type:int = new int(event.code);
			var response:int = new int(event.level);
			var sEvent:SteamEvent = new SteamEvent(SteamEvent.STEAM_RESPONSE, req_type, response);
			trace("handleStatusEvent: "+req_type+" "+response);
			dispatchEvent(sEvent);
		}

		public function dispose():void
		{
			clearInterval(_tm);
			_ExtensionContext.removeEventListener(StatusEvent.STATUS, handleStatusEvent);
			_ExtensionContext.dispose();
		}

		public function init():Boolean
		{
			isReady = _ExtensionContext.call("AIRSteam_Init") as Boolean;
			if(isReady)	_tm = setInterval(runCallbacks, 100);
			return isReady;
		}

		// automatically generated from API_to_lib.rb
		public function runCallbacks():Boolean
		{
			return _ExtensionContext.call("AIRSteam_RunCallbacks") as Boolean;
		}

		public function getUserID():String
		{
			return _ExtensionContext.call("AIRSteam_GetUserID") as String;
		}

		public function getAppID():uint
		{
			return _ExtensionContext.call("AIRSteam_GetAppID") as uint;
		}

		public function getPersonaName():String
		{
			return _ExtensionContext.call("AIRSteam_GetPersonaName") as String;
		}

		public function useCrashHandler(appID:uint, version:String, date:String, time:String):Boolean
		{
			return _ExtensionContext.call("AIRSteam_UseCrashHandler", appID, version, date, time) as Boolean;
		}

		/* stats/achievements */
		public function requestStats():Boolean
		{
			return _ExtensionContext.call("AIRSteam_RequestStats") as Boolean;
		}

		public function setAchievement(name:String):Boolean
		{
			return _ExtensionContext.call("AIRSteam_SetAchievement", name) as Boolean;
		}

		public function clearAchievement(name:String):Boolean
		{
			return _ExtensionContext.call("AIRSteam_ClearAchievement", name) as Boolean;
		}

		public function isAchievement(name:String):Boolean
		{
			return _ExtensionContext.call("AIRSteam_IsAchievement", name) as Boolean;
		}

		public function getStatInt(name:String):int
		{
			return _ExtensionContext.call("AIRSteam_GetStatInt", name) as int;
		}

		public function getStatFloat(name:String):Number
		{
			return _ExtensionContext.call("AIRSteam_GetStatFloat", name) as Number;
		}

		public function setStatInt(name:String, value:int):Boolean
		{
			return _ExtensionContext.call("AIRSteam_SetStatInt", name, value) as Boolean;
		}

		public function setStatFloat(name:String, value:Number):Boolean
		{
			return _ExtensionContext.call("AIRSteam_SetStatFloat", name, value) as Boolean;
		}

		public function storeStats():Boolean
		{
			return _ExtensionContext.call("AIRSteam_StoreStats") as Boolean;
		}

		public function resetAllStats(achievementsToo:Boolean):Boolean
		{
			return _ExtensionContext.call("AIRSteam_ResetAllStats", achievementsToo) as Boolean;
		}

		/* cloud */
		public function getFileCount():int
		{
			return _ExtensionContext.call("AIRSteam_GetFileCount") as int;
		}

		public function getFileSize(name:String):int
		{
			return _ExtensionContext.call("AIRSteam_GetFileSize", name) as int;
		}

		public function fileExists(name:String):Boolean
		{
			return _ExtensionContext.call("AIRSteam_FileExists", name) as Boolean;
		}

		public function fileWrite(name:String, data:ByteArray):Boolean
		{
			return _ExtensionContext.call("AIRSteam_FileWrite", name, data) as Boolean;
		}

		public function fileRead(name:String, data:ByteArray):Boolean
		{
			return _ExtensionContext.call("AIRSteam_FileRead", name, data) as Boolean;
		}

		public function fileDelete(name:String):Boolean
		{
			return _ExtensionContext.call("AIRSteam_FileDelete", name) as Boolean;
		}

		public function fileShare(name:String):Boolean
		{
			return _ExtensionContext.call("AIRSteam_FileShare", name) as Boolean;
		}

		public function fileShareResult():String
		{
			return _ExtensionContext.call("AIRSteam_FileShareResult") as String;
		}

		public function isCloudEnabledForApp():Boolean
		{
			return _ExtensionContext.call("AIRSteam_IsCloudEnabledForApp") as Boolean;
		}

		public function setCloudEnabledForApp(enabled:Boolean):Boolean
		{
			return _ExtensionContext.call("AIRSteam_SetCloudEnabledForApp", enabled) as Boolean;
		}

		public function getQuota():Array
		{
			return _ExtensionContext.call("AIRSteam_GetQuota") as Array;
		}

		/* ugc/workshop */
		public function UGCDownload(handle:String, priority:int):Boolean
		{
			return _ExtensionContext.call("AIRSteam_UGCDownload", handle, priority) as Boolean;
		}

		public function UGCRead(handle:String, size:int, offset:uint, data:ByteArray):Boolean
		{
			return _ExtensionContext.call("AIRSteam_UGCRead", handle, size, offset, data) as Boolean;
		}

		public function getUGCDownloadProgress(handle:String):Array
		{
			return _ExtensionContext.call("AIRSteam_GetUGCDownloadProgress", handle) as Array;
		}

		public function getUGCDownloadResult(handle:String):DownloadUGCResult
		{
			return _ExtensionContext.call("AIRSteam_GetUGCDownloadResult", handle) as DownloadUGCResult;
		}

		public function publishWorkshopFile(name:String, preview:String, appId:uint, title:String, description:String, visibility:uint, tags:Array, fileType:uint):Boolean
		{
			return _ExtensionContext.call("AIRSteam_PublishWorkshopFile", name, preview, appId, title, description, visibility, tags, fileType) as Boolean;
		}

		public function publishWorkshopFileResult():String
		{
			return _ExtensionContext.call("AIRSteam_PublishWorkshopFileResult") as String;
		}

		public function deletePublishedFile(file:String):Boolean
		{
			return _ExtensionContext.call("AIRSteam_DeletePublishedFile", file) as Boolean;
		}

		public function getPublishedFileDetails(file:String):Boolean
		{
			return _ExtensionContext.call("AIRSteam_GetPublishedFileDetails", file) as Boolean;
		}

		public function getPublishedFileDetailsResult(file:String):FileDetailsResult
		{
			return _ExtensionContext.call("AIRSteam_GetPublishedFileDetailsResult", file) as FileDetailsResult;
		}

		public function enumerateUserPublishedFiles(startIndex:uint):Boolean
		{
			return _ExtensionContext.call("AIRSteam_EnumerateUserPublishedFiles", startIndex) as Boolean;
		}

		public function enumerateUserPublishedFilesResult():WorkshopFilesResult
		{
			return _ExtensionContext.call("AIRSteam_EnumerateUserPublishedFilesResult") as WorkshopFilesResult;
		}

		public function enumeratePublishedWorkshopFiles(type:uint, start:uint, count:uint, days:uint, tags:Array, userTags:Array):Boolean
		{
			return _ExtensionContext.call("AIRSteam_EnumeratePublishedWorkshopFiles", type, start, count, days, tags, userTags) as Boolean;
		}

		public function enumeratePublishedWorkshopFilesResult():WorkshopFileResult
		{
			return _ExtensionContext.call("AIRSteam_EnumeratePublishedWorkshopFilesResult") as WorkshopFileResult;
		}

		public function enumerateUserSubscribedFiles(startIndex:uint):Boolean
		{
			return _ExtensionContext.call("AIRSteam_EnumerateUserSubscribedFiles", startIndex) as Boolean;
		}

		public function enumerateUserSubscribedFilesResult():SubscribedFilesResult
		{
			return _ExtensionContext.call("AIRSteam_EnumerateUserSubscribedFilesResult") as SubscribedFilesResult;
		}

		public function enumerateUserSharedWorkshopFiles(steamID:String, start:uint, required:Array, excluded:Array):Boolean
		{
			return _ExtensionContext.call("AIRSteam_EnumerateUserSharedWorkshopFiles", steamID, start, required, excluded) as Boolean;
		}

		public function enumerateUserSharedWorkshopFilesResult():WorkshopFilesResult
		{
			return _ExtensionContext.call("AIRSteam_EnumerateUserSharedWorkshopFilesResult") as WorkshopFilesResult;
		}

		public function enumeratePublishedFilesByUserAction(action:uint, startIndex:uint):Boolean
		{
			return _ExtensionContext.call("AIRSteam_EnumeratePublishedFilesByUserAction", action, startIndex) as Boolean;
		}

		public function enumeratePublishedFilesByUserActionResult():FilesByUserActionResult
		{
			return _ExtensionContext.call("AIRSteam_EnumeratePublishedFilesByUserActionResult") as FilesByUserActionResult;
		}

		public function subscribePublishedFile(file:String):Boolean
		{
			return _ExtensionContext.call("AIRSteam_SubscribePublishedFile", file) as Boolean;
		}

		public function unsubscribePublishedFile(file:String):Boolean
		{
			return _ExtensionContext.call("AIRSteam_UnsubscribePublishedFile", file) as Boolean;
		}

		public function createPublishedFileUpdateRequest(file:String):String
		{
			return _ExtensionContext.call("AIRSteam_CreatePublishedFileUpdateRequest", file) as String;
		}

		public function updatePublishedFileFile(handle:String, file:String):Boolean
		{
			return _ExtensionContext.call("AIRSteam_UpdatePublishedFileFile", handle, file) as Boolean;
		}

		public function updatePublishedFilePreviewFile(handle:String, preview:String):Boolean
		{
			return _ExtensionContext.call("AIRSteam_UpdatePublishedFilePreviewFile", handle, preview) as Boolean;
		}

		public function updatePublishedFileTitle(handle:String, title:String):Boolean
		{
			return _ExtensionContext.call("AIRSteam_UpdatePublishedFileTitle", handle, title) as Boolean;
		}

		public function updatePublishedFileDescription(handle:String, description:String):Boolean
		{
			return _ExtensionContext.call("AIRSteam_UpdatePublishedFileDescription", handle, description) as Boolean;
		}

		public function updatePublishedFileSetChangeDescription(handle:String, changeDesc:String):Boolean
		{
			return _ExtensionContext.call("AIRSteam_UpdatePublishedFileSetChangeDescription", handle, changeDesc) as Boolean;
		}

		public function updatePublishedFileVisibility(handle:String, visibility:uint):Boolean
		{
			return _ExtensionContext.call("AIRSteam_UpdatePublishedFileVisibility", handle, visibility) as Boolean;
		}

		public function updatePublishedFileTags(handle:String, tags:Array):Boolean
		{
			return _ExtensionContext.call("AIRSteam_UpdatePublishedFileTags", handle, tags) as Boolean;
		}

		public function commitPublishedFileUpdate(handle:String):Boolean
		{
			return _ExtensionContext.call("AIRSteam_CommitPublishedFileUpdate", handle) as Boolean;
		}

		public function getPublishedItemVoteDetails(file:String):Boolean
		{
			return _ExtensionContext.call("AIRSteam_GetPublishedItemVoteDetails", file) as Boolean;
		}

		public function getPublishedItemVoteDetailsResult():ItemVoteDetailsResult
		{
			return _ExtensionContext.call("AIRSteam_GetPublishedItemVoteDetailsResult") as ItemVoteDetailsResult;
		}

		public function updateUserPublishedItemVote(file:String, upvote:Boolean):Boolean
		{
			return _ExtensionContext.call("AIRSteam_UpdateUserPublishedItemVote", file, upvote) as Boolean;
		}

		public function setUserPublishedFileAction(file:String, action:uint):Boolean
		{
			return _ExtensionContext.call("AIRSteam_SetUserPublishedFileAction", file, action) as Boolean;
		}

		/* overlay */
		public function activateGameOverlay(dialog:String):Boolean
		{
			return _ExtensionContext.call("AIRSteam_ActivateGameOverlay", dialog) as Boolean;
		}

		public function activateGameOverlayToUser(dialog:String, steamId:String):Boolean
		{
			return _ExtensionContext.call("AIRSteam_ActivateGameOverlayToUser", dialog, steamId) as Boolean;
		}

		public function activateGameOverlayToWebPage(url:String):Boolean
		{
			return _ExtensionContext.call("AIRSteam_ActivateGameOverlayToWebPage", url) as Boolean;
		}

		public function activateGameOverlayToStore(appId:String, flag:uint):Boolean
		{
			return _ExtensionContext.call("AIRSteam_ActivateGameOverlayToStore", appId, flag) as Boolean;
		}

		public function activateGameOverlayInviteDialog(steamIdLobby:String):Boolean
		{
			return _ExtensionContext.call("AIRSteam_ActivateGameOverlayInviteDialog", steamIdLobby) as Boolean;
		}

		public function isOverlayEnabled():Boolean
		{
			return _ExtensionContext.call("AIRSteam_IsOverlayEnabled") as Boolean;
		}

		/* DLC / subscriptions */
		public function isSubscribedApp(appId:String):Boolean
		{
			return _ExtensionContext.call("AIRSteam_IsSubscribedApp", appId) as Boolean;
		}

		public function isDLCInstalled(appId:String):Boolean
		{
			return _ExtensionContext.call("AIRSteam_IsDLCInstalled", appId) as Boolean;
		}

		public function getDLCCount():int
		{
			return _ExtensionContext.call("AIRSteam_GetDLCCount") as int;
		}

		public function installDLC(appId:String):Boolean
		{
			return _ExtensionContext.call("AIRSteam_InstallDLC", appId) as Boolean;
		}

		public function uninstallDLC(appId):Boolean
		{
			return _ExtensionContext.call("AIRSteam_UninstallDLC", appId) as Boolean;
		}

		public function DLCInstalledResult():uint
		{
			return _ExtensionContext.call("AIRSteam_DLCInstalledResult") as uint;
		}
	}
}
