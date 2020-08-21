#pragma once

namespace Utils
{
	void CalcScreenCoors(CVector* vecWorld, CVector* vecScreen);
	void CalcWorldCoors(CVector* vecScreen, CVector* vecWorld);
	int isGTAMenuActive();
	float getDistance(CVector vecTarget);
	float getDistanceFromCamera(CVector vecTarget);
	void DisembarkInstantly();
	CVector* getBonePosition(CPed* pPed, ePedBones bone, CVector* vecPosition);
	void PerformAnimation(const char* szBlockName, const char* szAnimName, int iTime, bool bLoop, bool bUpdatePosition, bool bInterruptable, bool bFreezeLastFrame, bool bRunInSequence, bool bOffsetPed, bool bHoldLastFrame);
	bool isPlayingAnimation(CEntity* pEntity, char* szAnimName);
	float vecLength(CVector vecPos);
}

