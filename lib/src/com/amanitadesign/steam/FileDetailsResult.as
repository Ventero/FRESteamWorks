/*
 *  FileDetailsResult.as
 *  This file is part of FRESteamWorks.
 *
 *  Created by Ventero <http://github.com/Ventero> on 2013-04-24
 *  Copyright (c) 2013 Level Up Labs, LLC. All rights reserved.
 */

package com.amanitadesign.steam {
	public class FileDetailsResult {
		public var result:int;
		public var file:String;
		public var creatorAppID:uint;
		public var consumerAppID:uint;
		public var title:String;
		public var description:String;
		public var fileHandle:String;
		public var previewFileHandle:String;
		public var owner:String;
		public var timeCreated:uint;
		public var timeUpdated:uint;
		public var visibility:int;
		public var banned:Boolean;
		public var tags:String;
		public var tagsTruncated:Boolean;
		public var fileName:String;
		public var fileSize:int;
		public var previewFileSize:int;
		public var url:String;
		public var fileType:int;
	}
}
