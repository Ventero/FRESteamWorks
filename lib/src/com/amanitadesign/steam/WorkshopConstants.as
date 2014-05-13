/*
 *  WorkshopConstants.as
 *  This file is part of FRESteamWorks.
 *
 *  Created by Ventero <http://github.com/Ventero> on 2013-04-24
 *  Copyright (c) 2013 Level Up Labs, LLC. All rights reserved.
 */

package com.amanitadesign.steam {
	public class WorkshopConstants {
		public static const FILETYPE_Community:int = 0;
		public static const FILETYPE_Microtransaction:int = 1;
		public static const FILETYPE_Collection:int = 2;
		public static const FILETYPE_Art:int = 3;
		public static const FILETYPE_Video:int = 4;
		public static const FILETYPE_Screenshot:int = 5;
		public static const FILETYPE_Game:int = 6;
		public static const FILETYPE_Software:int = 7;
		public static const FILETYPE_Concept:int = 8;
		public static const FILETYPE_WebGuide:int = 9;
		public static const FILETYPE_IntegratedGuide:int = 10;
		public static const FILETYPE_Merch:int = 11;
		public static const FILETYPE_ControllerBinding:int = 11;

		public static const ENUMTYPE_RankedByVote:int = 0;
		public static const ENUMTYPE_Recent:int = 1;
		public static const ENUMTYPE_Trending:int = 2;
		public static const ENUMTYPE_FavoritesOfFriends:int = 3;
		public static const ENUMTYPE_VotedByFriends:int = 4;
		public static const ENUMTYPE_ContentByFriends:int = 5;
		public static const ENUMTYPE_RecentFromFollowedUsers:int = 6;

		public static const FILEACTION_Played:int = 0;
		public static const FILEACTION_Completed:int = 1;

		public static const VISIBILITY_Public:int = 0;
		public static const VISIBILITY_FriendsOnly:int = 1;
		public static const VISIBILITY_Private:int = 2;

		// Deprecated, use FriendConstants.STOREFLAG_* instead.
		public static const OVERLAYSTOREFLAG_None:int = 0;
		public static const OVERLAYSTOREFLAG_AddToCart:int = 1;
		public static const OVERLAYSTOREFLAG_AddToCartAndShow:int = 2;

		public static const WORKSHOPVOTE_Unvoted:int = 0;
		public static const WORKSHOPVOTE_VoteFor:int = 1;
		public static const WORKSHOPVOTE_VoteAgainst:int = 2;

		public static const FILEUPDATEHANDLE_Invalid:String = "18446744073709551615"; // 0xffffffffffffffffull
		public static const UGCHANDLE_Invalid:String = "18446744073709551615"; // 0xffffffffffffffffull
		public static const PUBLISHEDFILEID_Invalid:String = "0";
	}
}
