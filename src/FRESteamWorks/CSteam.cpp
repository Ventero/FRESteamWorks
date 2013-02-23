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

std::string CSteam::GetUserID() {
	if (!m_bInitialized) return "";

	uint64 id = SteamUser()->GetSteamID().ConvertToUint64();
	std::stringstream stream;
	stream << id;
	return stream.str();
}

std::string CSteam::GetPersonaName() {
	if (!m_bInitialized) return "";

	return std::string(SteamFriends()->GetPersonaName());
}

// stats/achievements

bool CSteam::SetAchievement(std::string name) {
	if (!m_bInitialized) return false;

	SteamUserStats()->SetAchievement(name.c_str());
	return SteamUserStats()->StoreStats();
}

bool CSteam::ClearAchievement(std::string name) {
	if (!m_bInitialized) return false;

	SteamUserStats()->ClearAchievement(name.c_str());
	return SteamUserStats()->StoreStats();
}

bool CSteam::IsAchievement(std::string name) {
	bool result = false;
	if (m_bInitialized) SteamUserStats()->GetAchievement(name.c_str(), &result);
	return result;
}

bool CSteam::GetStat(std::string name, int32 *value) {
	if (!m_bInitialized) return false;

	return SteamUserStats()->GetStat(name.c_str(), value);
}

bool CSteam::GetStat(std::string name, float *value) {
	if (!m_bInitialized) return false;

	return SteamUserStats()->GetStat(name.c_str(), value);
}

bool CSteam::SetStat(std::string name, int32 value) {
	if (!m_bInitialized) return false;

	return SteamUserStats()->SetStat(name.c_str(), value);
}

bool CSteam::SetStat(std::string name, float value) {
	if (!m_bInitialized) return false;

	return SteamUserStats()->SetStat(name.c_str(), value);
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

// remote storage

int32 CSteam::GetFileCount() {
	if (!m_bInitialized) return 0;

	return SteamRemoteStorage()->GetFileCount();
}

int32 CSteam::GetFileSize(std::string name) {
	if (!m_bInitialized) return 0;

	return SteamRemoteStorage()->GetFileSize(name.c_str());
}

bool CSteam::FileExists(std::string name) {
	if (!m_bInitialized) return false;

	return SteamRemoteStorage()->FileExists(name.c_str());
}

bool CSteam::FileWrite(std::string name, const void* data, int32 length) {
	if (!m_bInitialized) return false;

	return SteamRemoteStorage()->FileWrite(name.c_str(), data, length);
}

// caller is responsible of deleting data when return value is != 0
int32 CSteam::FileRead(std::string name, char** content) {
	if (!m_bInitialized) return 0;

	int32 size = SteamRemoteStorage()->GetFileSize(name.c_str());
	if (size == 0) return 0;

	char* data = new char[size];
	int32 read = SteamRemoteStorage()->FileRead(name.c_str(), data, size);
	if(read == 0) {
		delete data;
		return 0;
	}

	*content = data;
	return read;
}

bool CSteam::FileDelete(std::string name) {
	if (!m_bInitialized) return false;

	return SteamRemoteStorage()->FileDelete(name.c_str());
}

bool CSteam::IsCloudEnabledForApp() {
	if (!m_bInitialized) return false;

	return SteamRemoteStorage()->IsCloudEnabledForApp();
}

bool CSteam::SetCloudEnabledForApp(bool enabled) {
	if (!m_bInitialized) return false;

	SteamRemoteStorage()->SetCloudEnabledForApp(enabled);
	return enabled == SteamRemoteStorage()->IsCloudEnabledForApp();
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
