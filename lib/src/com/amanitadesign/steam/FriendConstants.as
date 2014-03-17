/*
 *  FriendConstants.as
 *  This file is part of FRESteamWorks.
 *
 *  Created by Ventero <http://github.com/Ventero> on 2013-05-17
 *  Copyright (c) 2013 Level Up Labs, LLC. All rights reserved.
 */

package com.amanitadesign.steam {
	public class FriendConstants {
		public static const FRIENDFLAG_None:int = 0x00;
		public static const FRIENDFLAG_Blocked:int = 0x01;
		public static const FRIENDFLAG_FriendshipRequested:int = 0x02;
		public static const FRIENDFLAG_Immediate:int = 0x04;
		public static const FRIENDFLAG_ClanMember:int = 0x08;
		public static const FRIENDFLAG_OnGameServer:int = 0x10;
		public static const FRIENDFLAG_RequestingFriendship:int = 0x80;
		public static const FRIENDFLAG_RequestingInfo:int = 0x100;
		public static const FRIENDFLAG_Ignored:int = 0x200;
		public static const FRIENDFLAG_IgnoredFriend:int = 0x400;
		public static const FRIENDFLAG_Suggested:int = 0x800;
		public static const FRIENDFLAG_All:int = 0xffff;

		public static const STOREFLAG_None:int = 0;
		public static const STOREFLAG_AddToCart:int = 1;
		public static const STOREFLAG_AddToCartAndShow:int = 2;
	}
}
