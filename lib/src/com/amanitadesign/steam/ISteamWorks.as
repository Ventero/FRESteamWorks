/*
 *  ISteamWorks.as
 *  This file is part of FRESteamWorks.
 *
 *  Created by Ventero <http://github.com/Ventero> on 2013-08-28
 *  Copyright (c) 2013 Level Up Labs, LLC. All rights reserved.
 */

package com.amanitadesign.steam {
	import flash.display.DisplayObjectContainer;
	import flash.events.IEventDispatcher;
	import flash.utils.ByteArray;

	public interface ISteamWorks extends IEventDispatcher {
		[Event(name="steamResponse", type="com.amanitadesign.steam.SteamEvent")]
		function init():Boolean;
		function dispose():void;

		function addOverlayWorkaround(container:DisplayObjectContainer,
			alwaysVisible:Boolean = false, color:uint = 0x000000):void;

		// the below list was automatically generated with generateAPI.rb

		// START GENERATED CODE
		function runCallbacks():Boolean
		function getUserID():String
		function getAppID():uint
		function getAvailableGameLanguages():String
		function getCurrentGameLanguage():String
		function getPersonaName():String
		function useCrashHandler(appID:uint, version:String, date:String, time:String):Boolean
		/* stats/achievements */
		function requestStats():Boolean
		function setAchievement(name:String):Boolean
		function clearAchievement(name:String):Boolean
		function isAchievement(name:String):Boolean
		function getStatInt(name:String):int
		function getStatFloat(name:String):Number
		function setStatInt(name:String, value:int):Boolean
		function setStatFloat(name:String, value:Number):Boolean
		function storeStats():Boolean
		function resetAllStats(achievementsToo:Boolean):Boolean
		/* cloud */
		function getFileCount():int
		function getFileSize(name:String):int
		function fileExists(name:String):Boolean
		function fileWrite(name:String, data:ByteArray):Boolean
		function fileRead(name:String, data:ByteArray):Boolean
		function fileDelete(name:String):Boolean
		function fileShare(name:String):Boolean
		function fileShareResult():String
		function isCloudEnabledForApp():Boolean
		function setCloudEnabledForApp(enabled:Boolean):Boolean
		function getQuota():Array
		/* ugc/workshop */
		function UGCDownload(handle:String, priority:int):Boolean
		function UGCRead(handle:String, size:int, offset:uint, data:ByteArray):Boolean
		function getUGCDownloadProgress(handle:String):Array
		function getUGCDownloadResult(handle:String):DownloadUGCResult
		function publishWorkshopFile(name:String, preview:String, appId:uint, title:String, description:String, visibility:uint, tags:Array, fileType:uint):Boolean
		function publishWorkshopFileResult():String
		function deletePublishedFile(file:String):Boolean
		function getPublishedFileDetails(file:String, maxAge:int = 0):Boolean
		function getPublishedFileDetailsResult(file:String):FileDetailsResult
		function enumerateUserPublishedFiles(startIndex:uint):Boolean
		function enumerateUserPublishedFilesResult():UserFilesResult
		function enumeratePublishedWorkshopFiles(type:uint, start:uint, count:uint, days:uint, tags:Array, userTags:Array):Boolean
		function enumeratePublishedWorkshopFilesResult():WorkshopFilesResult
		function enumerateUserSubscribedFiles(startIndex:uint):Boolean
		function enumerateUserSubscribedFilesResult():SubscribedFilesResult
		function enumerateUserSharedWorkshopFiles(steamID:String, start:uint, required:Array, excluded:Array):Boolean
		function enumerateUserSharedWorkshopFilesResult():UserFilesResult
		function enumeratePublishedFilesByUserAction(action:uint, startIndex:uint):Boolean
		function enumeratePublishedFilesByUserActionResult():FilesByUserActionResult
		function subscribePublishedFile(file:String):Boolean
		function unsubscribePublishedFile(file:String):Boolean
		function createPublishedFileUpdateRequest(file:String):String
		function updatePublishedFileFile(handle:String, file:String):Boolean
		function updatePublishedFilePreviewFile(handle:String, preview:String):Boolean
		function updatePublishedFileTitle(handle:String, title:String):Boolean
		function updatePublishedFileDescription(handle:String, description:String):Boolean
		function updatePublishedFileSetChangeDescription(handle:String, changeDesc:String):Boolean
		function updatePublishedFileVisibility(handle:String, visibility:uint):Boolean
		function updatePublishedFileTags(handle:String, tags:Array):Boolean
		function commitPublishedFileUpdate(handle:String):Boolean
		function getPublishedItemVoteDetails(file:String):Boolean
		function getPublishedItemVoteDetailsResult():ItemVoteDetailsResult
		function getUserPublishedItemVoteDetails(file:String):Boolean
		function getUserPublishedItemVoteDetailsResult():UserVoteDetails
		function updateUserPublishedItemVote(file:String, upvote:Boolean):Boolean
		function setUserPublishedFileAction(file:String, action:uint):Boolean
		/* overlay */
		function activateGameOverlay(dialog:String):Boolean
		function activateGameOverlayToUser(dialog:String, steamId:String):Boolean
		function activateGameOverlayToWebPage(url:String):Boolean
		function activateGameOverlayToStore(appId:uint, flag:uint):Boolean
		function activateGameOverlayInviteDialog(steamIdLobby:String):Boolean
		function isOverlayEnabled():Boolean
		/* DLC / subscriptions */
		function isSubscribedApp(appId:uint):Boolean
		function isDLCInstalled(appId:uint):Boolean
		function getDLCCount():int
		function installDLC(appId:uint):Boolean
		function uninstallDLC(appId:uint):Boolean
		function DLCInstalledResult():uint
		// END GENERATED CODE
	}
}
