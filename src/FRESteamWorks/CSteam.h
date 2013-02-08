#include <functional>

#include <steam_api.h>

// General result codes
enum ResponseTypes {
	RESPONSE_OnUserStatsReceived,
	RESPONSE_OnUserStatsStored,
	RESPONSE_OnAchievementStored,
	RESPONSE_OnGameOverlayActivated
};

enum ResponseCodes {
	RESPONSE_OK,
	RESPONSE_FAILED,
};

class CSteam {
private:
	uint64 m_iAppID; // Our current AppID
	bool m_bInitialized;
    
	void DispatchEvent(const int req_type, const int response);

protected:
	virtual void DispatchEvent(char* code, char* level) = 0;

public:
	CSteam();
	~CSteam();

	bool RequestStats();
	bool SetAchievement(const char* ID);
	bool ClearAchievement(const char* ID);
	bool IsAchievement(const char* ID);
	bool GetStat(const char* ID, int32 *value);
	bool GetStat(const char* ID, float *value);
	bool SetStat(const char* ID, int32 value);
	bool SetStat(const char* ID, float value);
	bool StoreStats();
	bool ResetAllStats(bool bAchievementsToo);

	STEAM_CALLBACK(CSteam, OnUserStatsReceived, UserStatsReceived_t,
	               m_CallbackUserStatsReceived);
	STEAM_CALLBACK(CSteam, OnUserStatsStored, UserStatsStored_t,
	               m_CallbackUserStatsStored);
	STEAM_CALLBACK(CSteam, OnAchievementStored,
	               UserAchievementStored_t, m_CallbackAchievementStored);
	STEAM_CALLBACK(CSteam, OnGameOverlayActivated, GameOverlayActivated_t,
	               m_CallbackGameOverlayActivated);

};
