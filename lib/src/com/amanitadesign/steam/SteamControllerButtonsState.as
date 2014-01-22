/*
 *  SteamControllerButtonsState.as
 *  This file is part of FRESteamWorks.
 *
 *  Created by Ventero <http://github.com/Ventero> on 2014-01-22
 *  Copyright (c) 2014 Level Up Labs, LLC. All rights reserved.
 */

package com.amanitadesign.steam
{
	public class SteamControllerButtonsState {
		public var leftTrigger:Boolean = false;
		public var rightTrigger:Boolean = false;

		public var leftBumper:Boolean = false;
		public var rightBumper:Boolean = false;

		public var button0:Boolean = false;
		public var button1:Boolean = false;
		public var button2:Boolean = false;
		public var button3:Boolean = false;

		public var touch0:Boolean = false;
		public var touch1:Boolean = false;
		public var touch2:Boolean = false;
		public var touch3:Boolean = false;

		public var menu:Boolean = false;
		public var steam:Boolean = false;
		public var escape:Boolean = false;

		public var backsideLeft:Boolean = false;
		public var backsideRight:Boolean = false;

		public var leftPadClicked:Boolean = false;
		public var rightPadClicked:Boolean = false;

		public var leftPadFingerdown:Boolean = false;
		public var rightPadFingerdown:Boolean = false;

		public function SteamControllerButtonsState(lowBits:uint, highBits:uint) {
			leftTrigger        = ((lowBits      ) & 1) != 0;
			rightTrigger       = ((lowBits >>  1) & 1) != 0;

			leftBumper         = ((lowBits >>  2) & 1) != 0;
			rightBumper        = ((lowBits >>  3) & 1) != 0;

			button0            = ((lowBits >>  4) & 1) != 0;
			button1            = ((lowBits >>  5) & 1) != 0;
			button2            = ((lowBits >>  6) & 1) != 0;
			button3            = ((lowBits >>  7) & 1) != 0;

			touch0             = ((lowBits >>  8) & 1) != 0;
			touch1             = ((lowBits >>  9) & 1) != 0;
			touch2             = ((lowBits >> 10) & 1) != 0;
			touch3             = ((lowBits >> 11) & 1) != 0;

			menu               = ((lowBits >> 12) & 1) != 0;
			steam              = ((lowBits >> 13) & 1) != 0;
			escape             = ((lowBits >> 14) & 1) != 0;

			backsideLeft       = ((lowBits >> 15) & 1) != 0;
			backsideRight      = ((lowBits >> 16) & 1) != 0;

			leftPadClicked     = ((lowBits >> 17) & 1) != 0;
			rightPadClicked    = ((lowBits >> 18) & 1) != 0;

			leftPadFingerdown  = ((lowBits >> 19) & 1) != 0;
			rightPadFingerdown = ((lowBits >> 20) & 1) != 0;
		}
	}
}
