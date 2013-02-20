/*
 *  CSteam.cpp
 *  This file is part of FRESteamWorks.
 *
 *  Created by David ´Oldes´ Oliva on 3/29/12.
 *  Contributors: Ventero <http://github.com/Ventero>
 *  Copyright (c) 2012 Amanita Design. All rights reserved.
 *  Copyright (c) 2012-2013 Level Up Labs, LLC. All rights reserved.
 */

#include "CSteam.h"

CSteam::CSteam():
    m_CallbackUserStatsReceived(this, &CSteam::OnUserStatsReceived),
    m_CallbackUserStatsStored(this, &CSteam::OnUserStatsStored),
    m_CallbackAchievementStored(this, &CSteam::OnAchievementStored),
    m_CallbackGameOverlayActivated(this, &CSteam::OnGameOverlayActivated)
{
	m_iAppID = SteamUtils()->GetAppID();
	m_bInitialized = (SteamUserStats() != NULL && SteamUser() != NULL);
	RequestStats();
}

CSteam::~CSteam() {}

bool CSteam::RequestStats() {
	// Is Steam loaded? If not we can't get stats.
	if (!m_bInitialized) return false;

	// Is the user logged on? If not we can't get stats.
	if (!SteamUser()->BLoggedOn()) return false;

	// Request user stats.
	return SteamUserStats()->RequestCurrentStats();
}

bool CSteam::SetAchievement(const char* ID) {
	if (!m_bInitialized) return false;

	SteamUserStats()->SetAchievement(ID);
	return SteamUserStats()->StoreStats();
}

bool CSteam::ClearAchievement(const char* ID) {
	if (!m_bInitialized) return false;

	SteamUserStats()->ClearAchievement(ID);
	return SteamUserStats()->StoreStats();
}

bool CSteam::IsAchievement(const char* ID) {
	bool result = false;
	if (m_bInitialized) SteamUserStats()->GetAchievement(ID, &result);
	return result;
}

bool CSteam::GetStat(const char* ID, int32 *value) {
	if (!m_bInitialized) return false;

	return SteamUserStats()->GetStat(ID, value);
}

bool CSteam::GetStat(const char* ID, float *value) {
	if (!m_bInitialized) return false;

	return SteamUserStats()->GetStat(ID, value);
}

bool CSteam::SetStat(const char* ID, int32 value) {
	if (!m_bInitialized) return false;

	return SteamUserStats()->SetStat(ID, value);
}

bool CSteam::SetStat(const char* ID, float value) {
	if (!m_bInitialized) return false;

	return SteamUserStats()->SetStat(ID, value);
}

bool CSteam::StoreStats() {
	if (!m_bInitialized) return false;

	return SteamUserStats()->StoreStats();
}

bool CSteam::ResetAllStats(bool bAchievementsToo) {
	if (!m_bInitialized) return false;

	SteamUserStats()->ResetAllStats(bAchievementsToo);
	return SteamUserStats()->StoreStats();
}

void CSteam::DispatchEvent(const int req_type, const int response) {
	char code[5];
	char level[5];

	snprintf(code, sizeof(code), "%d", req_type);
	snprintf(level, sizeof(level), "%d", response);
	DispatchEvent(code, level);
}

void CSteam::OnUserStatsReceived(UserStatsReceived_t *pCallback) {
	// we may get callbacks for other games' stats arriving, ignore them
	if (m_iAppID != pCallback->m_nGameID) return;

	DispatchEvent(RESPONSE_OnUserStatsReceived, pCallback->m_eResult);
}


void CSteam::OnUserStatsStored(UserStatsStored_t *pCallback) {
	// we may get callbacks for other games' stats arriving, ignore them
	if (m_iAppID != pCallback->m_nGameID) return;

	DispatchEvent(RESPONSE_OnUserStatsStored, pCallback->m_eResult);
}


void CSteam::OnAchievementStored(UserAchievementStored_t *pCallback) {
	// we may get callbacks for other games' stats arriving, ignore them
	if (m_iAppID != pCallback->m_nGameID) return;

	DispatchEvent(RESPONSE_OnAchievementStored, RESPONSE_OK);
}

void CSteam::OnGameOverlayActivated(GameOverlayActivated_t *pCallback) {
	DispatchEvent(RESPONSE_OnGameOverlayActivated, pCallback->m_bActive);
}
