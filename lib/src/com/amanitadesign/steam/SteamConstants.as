/*
 *  SteamConstants.as
 *  This file is part of FRESteamWorks.
 *
 *  Created by David ´Oldes´ Oliva on 3/29/12.
 *  Contributors: Ventero <http://github.com/Ventero>
 *  Copyright (c) 2012 Amanita Design. All rights reserved.
 *  Copyright (c) 2012-2013 Level Up Labs, LLC. All rights reserved.
 */

package com.amanitadesign.steam {
	public class SteamConstants {
		/* response types */
		public static const RESPONSE_OnUserStatsReceived:int = 0;
		public static const RESPONSE_OnUserStatsStored:int = 1;
		public static const RESPONSE_OnAchievementStored:int = 2;
		public static const RESPONSE_OnFindLeaderboard:int = 3;
		public static const RESPONSE_OnUploadLeaderboardScore:int = 4;
		public static const RESPONSE_OnDownloadLeaderboardEntries:int = 5;
		public static const RESPONSE_OnGameOverlayActivated:int = 6;
		public static const RESPONSE_OnFileShared:int = 7;
		public static const RESPONSE_OnUGCDownload:int = 8;
		public static const RESPONSE_OnPublishWorkshopFile:int = 9;
		public static const RESPONSE_OnDeletePublishedFile:int = 10;
		public static const RESPONSE_OnGetPublishedFileDetails:int = 11;
		public static const RESPONSE_OnEnumerateUserPublishedFiles:int = 12;
		public static const RESPONSE_OnEnumeratePublishedWorkshopFiles:int = 13;
		public static const RESPONSE_OnEnumerateUserSubscribedFiles:int = 14;
		public static const RESPONSE_OnEnumerateUserSharedWorkshopFiles:int = 15;
		public static const RESPONSE_OnEnumeratePublishedFilesByUserAction:int = 16;
		public static const RESPONSE_OnCommitPublishedFileUpdate:int = 17;
		public static const RESPONSE_OnSubscribePublishedFile:int = 18;
		public static const RESPONSE_OnUnsubscribePublishedFile:int = 19;
		public static const RESPONSE_OnGetPublishedItemVoteDetails:int = 20;
		public static const RESPONSE_OnGetUserPublishedItemVoteDetails:int = 21;
		public static const RESPONSE_OnUpdateUserPublishedItemVote:int = 22;
		public static const RESPONSE_OnSetUserPublishedFileAction:int = 23;
		public static const RESPONSE_OnGetAuthSessionTicketResponse:int = 24;
		public static const RESPONSE_OnValidateAuthTicketResponse:int = 25;
		public static const RESPONSE_OnDLCInstalled:int = 26;
	}

}
