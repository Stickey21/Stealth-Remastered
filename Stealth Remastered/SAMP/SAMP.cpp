#pragma once

#include "main.h"

CSAMP* pSAMP;

bool CSAMP::tryInit()
{
	g_SAMP = *(stSAMP**)(g_dwSAMP_Addr + SAMP_INFO_OFFSET);
	if (g_SAMP == nullptr)
		return false;

	g_Chat = *(stChatInfo**)(g_dwSAMP_Addr + SAMP_CHAT_INFO_OFFSET);
	if (g_Chat == nullptr)
		return false;

	g_Input = *(stInputInfo**)(g_dwSAMP_Addr + SAMP_CHAT_INPUT_INFO_OFFSET);
	if (g_Input == nullptr)
		return false;

	g_DeathList = *(stKillInfo**)(g_dwSAMP_Addr + SAMP_KILL_INFO_OFFSET);
	if (g_DeathList == nullptr)
		return false;

	if (g_SAMP->pRakClientInterface == nullptr)
		return false;

	g_Vehicles = g_SAMP->pPools->pVehicle;
	g_Players = g_SAMP->pPools->pPlayer;

	pSecure->memcpy_safe((void*)(g_dwSAMP_Addr + 0x99230), "\xC3", 1);

	g_dwSAMPCAC_Addr = (DWORD)LoadLibraryA("!sampcac_client.asi");

	return true;
};

void CSAMP::addMessageToChat(D3DCOLOR dwColor, char* szMsg, ...)
{
	if (g_Chat == nullptr) return;
	if (szMsg == NULL) return;

	va_list ap;
	char tmp[512];
	memset(tmp, 0, 512);
	va_start(ap, szMsg);
	vsnprintf(tmp, sizeof(tmp) - 1, szMsg, ap);
	va_end(ap);

	return ((void(__thiscall*) (const void*, int, char*, char*, DWORD, DWORD)) (g_dwSAMP_Addr + SAMP_FUNC_ADDTOCHATWND))((void*)g_Chat, 8, tmp, NULL, dwColor, 0x00);;
}

void CSAMP::toggleSAMPCursor(int iToggle)
{
	if (g_SAMP == NULL) return;
	if (g_Input->iInputEnabled) return;

	void* obj = *(void**)(g_dwSAMP_Addr + SAMP_MISC_INFO);
	((void(__thiscall*) (void*, int, bool)) (g_dwSAMP_Addr + SAMP_FUNC_TOGGLECURSOR))(obj, iToggle ? 3 : 0, !iToggle);
	if (!iToggle)
		((void(__thiscall*) (void*)) (g_dwSAMP_Addr + SAMP_FUNC_CURSORUNLOCKACTORCAM))(obj);
}

bool CSAMP::isPlayerStreamed(const uint16_t playerID)
{
	if (g_Players == NULL)
		return false;
	if (g_Players->iIsListed[playerID] != 1)
		return false;
	if (g_Players->pRemotePlayer[playerID] == NULL)
		return false;
	if (g_Players->pRemotePlayer[playerID]->pPlayerData == NULL)
		return false;
	if (g_Players->pRemotePlayer[playerID]->pPlayerData->pSAMP_Actor == NULL)
		return false;

	return true;
}

const char* CSAMP::getPlayerName(int iPlayerID)
{
	if (g_Players == NULL || iPlayerID < 0 || iPlayerID > SAMP_MAX_PLAYERS)
		return NULL;

	if (iPlayerID < 0 || iPlayerID > SAMP_MAX_PLAYERS)
		return NULL;

	if (iPlayerID == g_Players->sLocalPlayerID)
	{
		if (g_Players->iLocalPlayerNameAllocated <= 0xF)
			return g_Players->szLocalPlayerName;
		return g_Players->pszLocalPlayerName;
	}

	if (g_Players->pRemotePlayer[iPlayerID] == NULL)
		return NULL;

	if (g_Players->pRemotePlayer[iPlayerID]->iNameAllocated <= 0xF)
		return g_Players->pRemotePlayer[iPlayerID]->szPlayerName;

	return g_Players->pRemotePlayer[iPlayerID]->pszPlayerName;
}

D3DCOLOR CSAMP::getPlayerColor(int iPlayerID)
{
	D3DCOLOR* dwColor;
	if (iPlayerID < 0 || iPlayerID >= (SAMP_MAX_PLAYERS + 3))
		return D3DCOLOR_ARGB(0xFF, 0x99, 0x99, 0x99);

	switch (iPlayerID)
	{
	case (SAMP_MAX_PLAYERS):
		return 0xFF888888;

	case (SAMP_MAX_PLAYERS + 1):
		return 0xFF0000AA;

	case (SAMP_MAX_PLAYERS + 2):
		return 0xFF63C0E2;
	}

	dwColor = (D3DCOLOR*)((uint8_t*)g_dwSAMP_Addr + SAMP_COLOR_OFFSET);
	return D3DCOLOR_RGBA(dwColor[iPlayerID] >> 8, dwColor[iPlayerID] >> 16, dwColor[iPlayerID] >> 24, 255);
}

bool CSAMP::isVehicleStreamed(uint16_t vehicleID)
{
	if (g_Vehicles->iIsListed[vehicleID] != 1)
		return false;
	if (g_Vehicles->pSAMP_Vehicle[vehicleID] == NULL)
		return false;
	if (g_Vehicles->pSAMP_Vehicle[vehicleID]->pGTA_Vehicle == NULL)
		return false;
	return true;
}

int CSAMP::getNearestPlayer(bool bTeamProtect)
{
	float fNearestDistance = -1.0f;
	int iPlayerID = -1;

	for (int i = 0; i < SAMP_MAX_PLAYERS; i++)
	{
		if (g_Players->iIsListed[i] != 1)
			continue;
		if (g_Players->pRemotePlayer[i] == NULL)
			continue;
		if (g_Players->pRemotePlayer[i]->pPlayerData == NULL)
			continue;
		if (g_Players->pRemotePlayer[i]->pPlayerData->pSAMP_Actor == NULL)
			continue;
		if (g_Players->pRemotePlayer[i]->pPlayerData->iAFKState == 2)
			continue;
		if (!CPools::GetPed(pSAMP->getPlayers()->pRemotePlayer[i]->pPlayerData->pSAMP_Actor->ulGTAEntityHandle)->IsAlive())
			continue;
		if (bTeamProtect && getPlayerColor(i) == getPlayerColor(g_Players->sLocalPlayerID))
			continue;

		float fDistance = Math::vect3_dist(&g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->base.matrix[12], &g_Players->pRemotePlayer[i]->pPlayerData->pSAMP_Actor->pGTA_Ped->base.matrix[12]);
		if (fNearestDistance == -1.0f || fDistance < fNearestDistance)
		{
			iPlayerID = i;
			fNearestDistance = fDistance;
		}
	}
	return iPlayerID;
}

int CSAMP::getNearestVehicle()
{
	float fNearestDistance = -1.0f;
	int iVehicleID = -1;

	for (int i = 0; i < SAMP_MAX_VEHICLES; i++)
	{
		if (g_Vehicles->iIsListed[i] != 1)
			continue;
		if (g_Vehicles->pSAMP_Vehicle[i] == NULL)
			continue;
		if (g_Vehicles->pSAMP_Vehicle[i]->pGTA_Vehicle == NULL)
			continue;

		float fDistance = Math::vect3_dist(&g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->base.matrix[12], &g_Vehicles->pSAMP_Vehicle[i]->pGTA_Vehicle->base.matrix[12]);
		if (fNearestDistance == -1.0f || fDistance < fNearestDistance)
		{
			fNearestDistance = fDistance;
			iVehicleID = i;
		}
	}

	return iVehicleID;
}

const char* CSAMP::getWeaponSpriteID(char szWeapon)
{
	return ((const char* (__thiscall*)(stKillInfo*, char))(g_dwSAMP_Addr + 0x661B0))(pSAMP->getDeathList(), szWeapon);
}

float fWeaponDamage[55] =
{
	1.0, // 0 - Fist
	1.0, // 1 - Brass knuckles
	1.0, // 2 - Golf club
	1.0, // 3 - Nitestick
	1.0, // 4 - Knife
	1.0, // 5 - Bat
	1.0, // 6 - Shovel
	1.0, // 7 - Pool cue
	1.0, // 8 - Katana
	1.0, // 9 - Chainsaw
	1.0, // 10 - Dildo
	1.0, // 11 - Dildo 2
	1.0, // 12 - Vibrator
	1.0, // 13 - Vibrator 2
	1.0, // 14 - Flowers
	1.0, // 15 - Cane
	82.5, // 16 - Grenade
	0.0, // 17 - Teargas
	1.0, // 18 - Molotov
	9.9, // 19 - Vehicle M4 (custom)
	46.2, // 20 - Vehicle minigun (custom)
	0.0, // 21
	8.25f, // 22 - Colt 45
	13.200001f, // 23 - Silenced
	46.200001f, // 24 - Deagle
	49.500004f,//3.3, // 25 - Shotgun
	49.500004f,//3.3, // 26 - Sawed-off
	39.600002f,//4.95, // 27 - Spas
	6.6f, // 28 - UZI
	8.25f, // 29 - MP5
	9.900001f, // 30 - AK47
	9.900001f, // 31 - M4
	6.6f, // 32 - Tec9
	24.750002f, // 33 - Cuntgun
	41.25f, // 34 - Sniper
	82.5, // 35 - Rocket launcher
	82.5, // 36 - Heatseeker
	1.0, // 37 - Flamethrower
	46.200001f, // 38 - Minigun
	82.5, // 39 - Satchel
	0.0, // 40 - Detonator
	0.33, // 41 - Spraycan
	0.33, // 42 - Fire extinguisher
	0.0, // 43 - Camera
	0.0, // 44 - Night vision
	0.0, // 45 - Infrared
	0.0, // 46 - Parachute
	0.0, // 47 - Fake pistol
	2.64, // 48 - Pistol whip (custom)
	9.9, // 49 - Vehicle
	330.0, // 50 - Helicopter blades
	82.5, // 51 - Explosion
	1.0, // 52 - Car park (custom)
	1.0, // 53 - Drowning
	165.0 // 54 - Splat
};

float fWeaponRange[39] =
{
	0.0, // 0 - Fist
	0.0, // 1 - Brass knuckles
	0.0, // 2 - Golf club
	0.0, // 3 - Nitestick
	0.0, // 4 - Knife
	0.0, // 5 - Bat
	0.0, // 6 - Shovel
	0.0, // 7 - Pool cue
	0.0, // 8 - Katana
	0.0, // 9 - Chainsaw
	0.0, // 10 - Dildo
	0.0, // 11 - Dildo 2
	0.0, // 12 - Vibrator
	0.0, // 13 - Vibrator 2
	0.0, // 14 - Flowers
	0.0, // 15 - Cane
	0.0, // 16 - Grenade
	0.0, // 17 - Teargas
	0.0, // 18 - Molotov
	90.0, // 19 - Vehicle M4 (custom)
	75.0, // 20 - Vehicle minigun (custom)
	0.0, // 21
	35.0, // 22 - Colt 45
	35.0, // 23 - Silenced
	35.0, // 24 - Deagle
	40.0, // 25 - Shotgun
	35.0, // 26 - Sawed-off
	40.0, // 27 - Spas
	35.0, // 28 - UZI
	45.0, // 29 - MP5
	70.0, // 30 - AK47
	90.0, // 31 - M4
	35.0, // 32 - Tec9
	100.0, // 33 - Cuntgun
	320.0, // 34 - Sniper
	0.0, // 35 - Rocket launcher
	0.0, // 36 - Heatseeker
	0.0, // 37 - Flamethrower
	75.0  // 38 - Minigun
};