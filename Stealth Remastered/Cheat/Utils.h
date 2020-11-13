#pragma once

namespace Utils
{
	void CalcScreenCoors(CVector* vecWorld, CVector* vecScreen);
	void CalcWorldCoors(CVector* vecScreen, CVector* vecWorld);
	void CalcMapToScreen(ImVec2 vecWindow, CVector* vecMap, CVector* vecScreen, float fSize);
	void CalcScreenToMap(ImVec2 vecWindow, CVector* vecScreen, CVector* vecMap, float fSize);
	float getDistance(CVector vecTarget);
	float getDistanceFromCamera(CVector vecTarget);
	void DisembarkInstantly();
	CVector* getBonePosition(CPed* pPed, ePedBones bone, CVector* vecPosition);
	void PerformAnimation(const char* szBlockName, const char* szAnimName, int iTime, bool bLoop, bool bUpdatePosition, bool bInterruptable, bool bFreezeLastFrame, bool bRunInSequence, bool bOffsetPed, bool bHoldLastFrame);
	bool isPlayingAnimation(CEntity* pEntity, char const* szAnimName);
	float vecLength(CVector vecPos);
}

