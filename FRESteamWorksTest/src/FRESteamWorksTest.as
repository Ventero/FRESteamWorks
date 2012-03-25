package
{
	import com.amanitadesign.steam.FRESteamWorks;
	import com.amanitadesign.steam.SteamConstants;
	import com.amanitadesign.steam.SteamEvent;
	
	import flash.display.SimpleButton;
	import flash.display.Sprite;
	import flash.events.MouseEvent;
	import flash.text.TextField;
	
	public class FRESteamWorksTest extends Sprite
	{
		public var Steamworks:FRESteamWorks = new FRESteamWorks();
		public var tf:TextField;
		public function FRESteamWorksTest()
		{
			tf = new TextField();
			tf.width = stage.stageWidth;
			tf.height = stage.stageHeight;
			addChild(tf);
			
			tf.addEventListener(MouseEvent.MOUSE_DOWN, onClick);
				
			Steamworks.addEventListener(SteamEvent.STEAM_RESPONSE, onSteamResponse);

			if(Steamworks.init()){
				tf.appendText(" joooooooooo\n");
				tf.appendText("isAchievement('ACH_WIN_ONE_GAME') == "+Steamworks.isAchievement("ACH_WIN_ONE_GAME")+"\n");
				tf.appendText("isAchievement('ACH_TRAVEL_FAR_SINGLE') == "+Steamworks.isAchievement("ACH_TRAVEL_FAR_SINGLE")+"\n");
				tf.appendText("setStatFloat('FeetTraveled') == "+Steamworks.setStatFloat('FeetTraveled', 21.3)+"\n");
				tf.appendText("setStatInt('NumGames', 2) == "+Steamworks.setStatInt('NumGames', 2)+"\n");
				Steamworks.storeStats();
				tf.appendText("getStatInt('NumGames') == "+Steamworks.getStatInt('NumGames')+"\n");
				tf.appendText("getStatFloat('FeetTraveled') == "+Steamworks.getStatFloat('FeetTraveled')+"\n");
				
				//Steamworks.clearAchievement("ACH_WIN_ONE_GAME");
				//Steamworks.requestStats();
				Steamworks.resetAllStats(true);
			}else {
				tf.appendText(" neeeeee\n");
			}
			
		}
		public function onClick(e:MouseEvent):void{
			if(Steamworks.isReady){
				if(!Steamworks.isAchievement("ACH_WIN_ONE_GAME")) {
					tf.appendText("setting.. ACH_WIN_ONE_GAME\n");
					Steamworks.setAchievement("ACH_WIN_ONE_GAME");
				}
				//Steamworks.storeStats();
			} else {
				tf.appendText("not able to set achievement\n");
			}
		}
		public function onSteamResponse(e:SteamEvent):void{
			switch(e.req_type){
				case SteamConstants.RESPONSE_OnUserStatsStored:
					tf.appendText("RESPONSE_OnUserStatsStored: "+e.response+"\n");
					break;
				case SteamConstants.RESPONSE_OnUserStatsReceived:
					tf.appendText("RESPONSE_OnUserStatsReceived: "+e.response+"\n");
					break;
				case SteamConstants.RESPONSE_OnAchievementStored:
					tf.appendText("RESPONSE_OnAchievementStored: "+e.response+"\n");
					break;
				default:
					tf.appendText("STEAMresponse type:"+e.req_type+" response:"+e.response+"\n");
			}
		}
	}
}