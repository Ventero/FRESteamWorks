/*
 *  SteamControllerState.as
 *  This file is part of FRESteamWorks.
 *
 *  Created by Ventero <http://github.com/Ventero> on 2014-01-22
 *  Copyright (c) 2014 Level Up Labs, LLC. All rights reserved.
 */

package com.amanitadesign.steam
{
	public class SteamControllerState
	{
		public var packetNum:uint;

		// low and high 32 bit of the button bit flags
		public var buttonsLo:uint;
		public var buttonsHi:uint;

		public var leftPadX:int;
		public var leftPadY:int;
		public var rightPadX:int;
		public var rightPadY:int;

		public function getButtons():SteamControllerButtonsState {
			return new SteamControllerButtonsState(buttonsLo, buttonsHi);
		}
	}
}
