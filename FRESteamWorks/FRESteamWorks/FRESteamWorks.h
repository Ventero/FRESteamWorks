#include "FlashRuntimeExtensions.h"

class CSteam
{
private:
	int64 m_iAppID; // Our current AppID
	bool m_bInitialized;

public:
	CSteam();
	~CSteam();
	
	bool RequestStats();
	bool SetAchievement( const char* ID );
	bool ClearAchievement( const char* ID );
	bool IsAchievement( const char* ID );
	bool GetStat( const char* ID, int32 *value );
	bool GetStat( const char* ID, float *value );
	bool SetStat( const char* ID, int32 value );
	bool SetStat( const char* ID, float value );
	bool StoreStats();
	bool ResetAllStats( bool bAchievementsToo );

	void DispatchEvent( const int req_type, const int response );

	STEAM_CALLBACK( CSteam, OnUserStatsReceived, UserStatsReceived_t, 
		m_CallbackUserStatsReceived );
	STEAM_CALLBACK( CSteam, OnUserStatsStored, UserStatsStored_t, 
		m_CallbackUserStatsStored );
	STEAM_CALLBACK( CSteam, OnAchievementStored, 
		UserAchievementStored_t, m_CallbackAchievementStored );
};


// General result codes
enum ResponseTypes
{
	RESPONSE_OnUserStatsReceived,
	RESPONSE_OnUserStatsStored,
	RESPONSE_OnAchievementStored
};
enum ResponseCodes
{
	RESPONSE_OK,
	RESPONSE_FAILED,
};
//

#ifdef __cplusplus
extern "C" {
#endif
	void contextInitializer(void* extData, const uint8_t* ctxType, FREContext ctx, uint32_t* numFunctions, const FRENamedFunction** functions);
	void contextFinalizer(FREContext ctx);
	DllExport void initializer(void** extData, FREContextInitializer* ctxInitializer, FREContextFinalizer* ctxFinalizer);
	DllExport void finalizer(void* extData);
#ifdef __cplusplus
} //extern "C"
#endif
