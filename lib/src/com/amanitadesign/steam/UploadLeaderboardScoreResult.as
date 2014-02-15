/*
 *  UploadLeaderboardScoreResult.as
 *  This file is part of FRESteamWorks.
 *
 *  Created by Ventero <http://github.com/Ventero> on 2014-02-14
 *  Copyright (c) 2014 Level Up Labs, LLC. All rights reserved.
 */

package com.amanitadesign.steam {
	public class UploadLeaderboardScoreResult {
		public var success:Boolean;
		public var leaderboardHandle:String;
		public var score:int;
		public var scoreChanged:Boolean;
		public var newGlobalRank:int;
		public var previousGlobalRank:int;
	}
}
