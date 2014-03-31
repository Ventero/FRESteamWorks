/*
 *  UserStatsConstants.as
 *  This file is part of FRESteamWorks.
 *
 *  Created by Ventero <http://github.com/Ventero> on 2014-03-31
 *  Copyright (c) 2014 Level Up Labs, LLC. All rights reserved.
 */

package com.amanitadesign.steam {
	public class UserConstants {
		public static const BEGINAUTH_OK:int = 0;
		public static const BEGINAUTH_InvalidTicket:int = 1;
		public static const BEGINAUTH_DuplicateRequest:int = 2;
		public static const BEGINAUTH_InvalidVersion:int = 3;
		public static const BEGINAUTH_GameMismatch:int = 4;
		public static const BEGINAUTH_ExpiredTicket:int = 5;

		public static const LICENSE_HasLicense:int = 0;
		public static const LICENSE_DoesNotHaveLicense:int = 1;
		public static const LICENSE_NoAuth:int = 2;

		public static const SESSION_OK:int = 0;
		public static const SESSION_UserNotConnectedToSteam:int = 1;
		public static const SESSION_NoLicenseOrExpired:int = 2;
		public static const SESSION_VACBanned:int = 3;
		public static const SESSION_LoggedInElseWhere:int = 4;
		public static const SESSION_VACCheckTimedOut:int = 5;
		public static const SESSION_AuthTicketCanceled:int = 6;
		public static const SESSION_AuthTicketInvalidAlreadyUsed:int = 7;
		public static const SESSION_AuthTicketInvalid:int = 8;

		public static const AUTHTICKET_Invalid:uint = 0;
	}
}
