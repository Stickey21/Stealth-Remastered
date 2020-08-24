#include "main.h"

CAimbot* pAimbot;

void CAimbot::Update()
{
	int iMode = TheCamera.m_aCams[0].m_nMode;
	bCrosshair = iMode == 53 || iMode == 7;

	if (pSAMP->getPlayers()->pLocalPlayer->byteCurrentWeapon != 34)
	{
		vecCrosshair.fX = ImGui::GetIO().DisplaySize.x * 0.5299999714f;
		vecCrosshair.fY = ImGui::GetIO().DisplaySize.y * 0.4f;
	}
	else
	{
		vecCrosshair.fX = ImGui::GetIO().DisplaySize.x / 2.f;
		vecCrosshair.fY = ImGui::GetIO().DisplaySize.y / 2.f;
	}

	GetAimingPlayer();
	SmoothAimbot();
	ProAimbot();
	Triggerbot();
}

void CAimbot::Render()
{
	if (bCrosshair && g_Config.g_Aimbot.bAimbot && g_Config.g_Aimbot.bAimbotEnabled[pSAMP->getPlayers()->pLocalPlayer->byteCurrentWeapon])
	{
		if (g_Config.g_Aimbot.bDrawRange)
			if (!g_Config.g_Aimbot.iRangeStyle)
				pRender->DrawCircleFilled(vecCrosshair, (float)g_Config.g_Aimbot.iAimbotConfig[pSAMP->getPlayers()->pLocalPlayer->byteCurrentWeapon][RANGE], g_Config.g_Aimbot.colorRange);
			else pRender->DrawCircle(vecCrosshair, (float)g_Config.g_Aimbot.iAimbotConfig[pSAMP->getPlayers()->pLocalPlayer->byteCurrentWeapon][RANGE], g_Config.g_Aimbot.colorRange, g_Config.g_Aimbot.fOutlineThickness);

		if (g_Config.g_Aimbot.bDrawTracer && iTargetPlayer != -1)
			pRender->DrawLine(vecCrosshair, vecTargetBone, ImColor(0.f, 1.f, 0.f), g_Config.g_Aimbot.fOutlineThickness);
	}
}

void CAimbot::GetAimingPlayer()
{
	iTargetPlayer = -1;
	float fNearestDistance = 9999.f;

	if (g_Config.g_Aimbot.bAimbot || g_Config.g_Aimbot.bSmooth || g_Config.g_Aimbot.bSilent || g_Config.g_Aimbot.bProAim)
	{
		CVector vecCamera, vecOrigin, vecTarget;
		Utils::getBonePosition(FindPlayerPed(), BONE_RIGHTWRIST, &vecOrigin);
		TheCamera.Find3rdPersonCamTargetVector(100.f, vecOrigin, &vecCamera, &vecTarget);

		for (int i = 0; i < SAMP_MAX_PLAYERS; i++)
		{
			if (!pSAMP->isPlayerStreamed(i))
				continue;

			CPed* pPed = CPools::GetPed(pSAMP->getPlayers()->pRemotePlayer[i]->pPlayerData->pSAMP_Actor->ulGTAEntityHandle);
			if (!pPed || !pPed->IsAlive())
				continue;

			if (g_Config.g_Aimbot.bTeamProtect && pSAMP->getPlayerColor(i) == pSAMP->getPlayerColor(pSAMP->getPlayers()->sLocalPlayerID))
				continue;

			if (!g_Config.g_Aimbot.bIgnoreEverything)
			{
				if (g_Config.g_Aimbot.bIgnoreAFK && pSAMP->getPlayers()->pRemotePlayer[i]->pPlayerData->iAFKState == 2)
					continue;

				float fTargetDistance = Math::vect3_dist(&pSAMP->getPlayers()->pRemotePlayer[i]->pPlayerData->pSAMP_Actor->pGTA_Ped->base.matrix[12], &pSAMP->getPlayers()->pLocalPlayer->pSAMP_Actor->pGTA_Ped->base.matrix[12]);
				if (!g_Config.g_Aimbot.bIgnoreMaxDistance && fTargetDistance > fWeaponRange[pSAMP->getPlayers()->pLocalPlayer->byteCurrentWeapon])
					continue;
			}

			for (auto iBone : iBoneList)
			{
				CVector vecBone, vecBoneScreen;
				Utils::getBonePosition(pPed, (ePedBones)iBoneList[iBone], &vecBone);
				Utils::CalcScreenCoors(&vecBone, &vecBoneScreen);
				if (vecBoneScreen.fZ < 1.0f)
					continue;

				float fCentreDistance = Math::vect2_dist(&vecCrosshair, &vecBoneScreen);
				if (g_Config.g_Aimbot.bAimbot && fCentreDistance >= (float)g_Config.g_Aimbot.iAimbotConfig[pSAMP->getPlayers()->pLocalPlayer->byteCurrentWeapon][RANGE] * 1.5f)
					continue;

				if (!g_Config.g_Aimbot.bIgnoreEverything && !g_Config.g_Aimbot.bLockThroughObjects && !CWorld::GetIsLineOfSightClear(vecCamera, vecBone, true, true, false, true, true, true, false))
					continue;

				if (fCentreDistance <= fNearestDistance)
				{
					fNearestDistance = fCentreDistance;
					iTargetPlayer = i;
					iTargetBone = iBoneList[iBone];
					vecTargetBone = vecBoneScreen;
					break;
				}
			}
		}
	}
}

bool __stdcall CAimbot::Hooked_FireInstantHit(void* this_, CEntity* pFiringEntity, CVector* pOrigin, CVector* pMuzzle, CEntity* pTargetEntity, CVector* pTarget, CVector* pVec, bool bCrossHairGun, bool bCreateGunFx)
{
	if (pFiringEntity == (CEntity*)FindPlayerPed() && g_Config.g_Aimbot.bSilent && pAimbot->iTargetPlayer != -1 && g_Config.g_Aimbot.bAimbotEnabled[pSAMP->getPlayers()->pLocalPlayer->byteCurrentWeapon] && rand() % 100 <= g_Config.g_Aimbot.iAimbotConfig[pSAMP->getPlayers()->pLocalPlayer->byteCurrentWeapon][SILENT])
	{
		CPed* pPed = CPools::GetPed(pSAMP->getPlayers()->pRemotePlayer[pAimbot->iTargetPlayer]->pPlayerData->pSAMP_Actor->ulGTAEntityHandle);
		if (pPed)
		{
			CVector vecCamera, vecOrigin, vecTarget;
			Utils::getBonePosition(FindPlayerPed(), BONE_RIGHTWRIST, &vecOrigin);
			TheCamera.Find3rdPersonCamTargetVector(100.f, vecOrigin, &vecCamera, &vecTarget);
			if (pAimbot->bCrosshair)
				pOrigin = &vecCamera;

			CVector vecBone;
			Utils::getBonePosition(pPed, (ePedBones)pAimbot->iTargetBone, &vecBone);
			pTarget = &vecBone;

			pSecure->memcpy_safe((void*)0x740B4E, g_Config.g_Aimbot.bLockThroughObjects || g_Config.g_Aimbot.bIgnoreEverything ? "\x6A\x00\x6A\x00" : "\x6A\x01\x6A\x01", 4);
			pSecure->Write(0x8D6114, (float)(g_Config.g_Aimbot.fSilentAccuracy / 20.f));
		}
	}
	else
	{
		Memory::memcpy_safe((void*)0x740B4E, "\x6A\x01\x6A\x01", 4);
		*(float*)0x8D6114 = 5.f;
	}
	return pAimbot->Orginal_FireInstantHit(this_, pFiringEntity, pOrigin, pMuzzle, pTargetEntity, pTarget, pVec, bCrossHairGun, bCreateGunFx);
}

bool __cdecl CAimbot::Hooked_AddBullet(CEntity* pCreator, eWeaponType weaponType, CVector vecPosition, CVector vecVelocity)
{
	if (pCreator == (CEntity*)FindPlayerPed() && g_Config.g_Aimbot.bSilent && pAimbot->iTargetPlayer != -1 && g_Config.g_Aimbot.bAimbotEnabled[34] && rand() % 100 <= g_Config.g_Aimbot.iAimbotConfig[34][SILENT])
	{
		CPed* pPed = CPools::GetPed(pSAMP->getPlayers()->pRemotePlayer[pAimbot->iTargetPlayer]->pPlayerData->pSAMP_Actor->ulGTAEntityHandle);
		if (pPed)
		{
			CVector vecBone;
			Utils::getBonePosition(pPed, (ePedBones)pAimbot->iTargetBone, &vecBone);
			vecVelocity = vecBone - vecPosition;

			pSecure->memcpy_safe((void*)0x736212, g_Config.g_Aimbot.bLockThroughObjects || g_Config.g_Aimbot.bIgnoreEverything ? "\x6A\x00\x6A\x00" : "\x6A\x01\x6A\x01", 4);
		}
	}
	else Memory::memcpy_safe((void*)0x736212, "\x6A\x01\x6A\x01", 4);

	return pAimbot->Orginal_AddBullet(pCreator, weaponType, vecPosition, vecVelocity);
}

float __cdecl CAimbot::Hooked_TargetWeaponRangeMultiplier(CEntity* pVictim, CEntity* pOwner)
{
	if (pOwner == (CEntity*)FindPlayerPed() && (g_Config.g_Aimbot.bIgnoreMaxDistance || g_Config.g_Aimbot.bIgnoreEverything))
		return 100.f;

	return pAimbot->Orginal_TargetWeaponRangeMultiplier(pVictim, pOwner);
}

void CAimbot::SmoothAimbot()
{
	if (g_Config.g_Aimbot.bSmooth && g_Config.g_Aimbot.bAimbotEnabled[pSAMP->getPlayers()->pLocalPlayer->byteCurrentWeapon] && iTargetPlayer != -1 && bCrosshair)
	{
		if (g_Config.g_Aimbot.bSmoothIsFire && !GetAsyncKeyState(VK_LBUTTON))
			return;

		CPed* pPed = CPools::GetPed(pSAMP->getPlayers()->pRemotePlayer[pAimbot->iTargetPlayer]->pPlayerData->pSAMP_Actor->ulGTAEntityHandle);
		if (!pPed) 
			return;

		if (g_Config.g_Aimbot.bStopOnBody)
		{
			CVector vecCamera, vecOrigin, vecTarget;
			if (pSAMP->getPlayers()->pLocalPlayer->byteCurrentWeapon != 34)
			{
				Utils::getBonePosition(FindPlayerPed(), BONE_RIGHTWRIST, &vecOrigin);
				TheCamera.Find3rdPersonCamTargetVector(100.f, vecOrigin, &vecCamera, &vecTarget);
			}
			else
			{
				CVector vecFront = TheCamera.m_aCams[0].m_vecFront;
				vecFront.Normalise();
				vecOrigin = *TheCamera.GetGameCamPosition();
				vecOrigin += (vecFront * 2.0f);
				vecTarget = vecOrigin + (vecFront * 100.f);
			}
			CColPoint pCollision; CEntity* pCollisionEntity = NULL;
			bool bCollision = CWorld::ProcessLineOfSight(vecCamera, vecTarget, pCollision, pCollisionEntity, !g_Config.g_Aimbot.bLockThroughObjects, !g_Config.g_Aimbot.bLockThroughObjects, true, true, true, true, false, true);
			if (bCollision && pCollisionEntity && pCollisionEntity->m_nType == ENTITY_TYPE_PED && pCollisionEntity == (CEntity*)pPed)
				return;
		}

		CVector vecOrigin = *TheCamera.GetGameCamPosition(), vecTarget = *Utils::getBonePosition(pPed, (ePedBones)BONE_PELVIS1, &vecTarget), vecVector = vecOrigin - vecTarget;

		float* fAspectRatio = (float*)0xC3EFA4;
		float* fCrosshairOffset = (float*)0xB6EC10;
		float fFix = 0.f, fVecX = 0.f, fZ = 0.f, fX = 0.f;

		if (TheCamera.m_aCams[0].m_nMode == 53 || TheCamera.m_aCams[0].m_nMode == 55)
		{
			float fMult = tan(TheCamera.FindCamFOV() / 2.0f * 0.017453292f);
			fZ = M_PI - atan2(1.0f, fMult * ((1.0f - fCrosshairOffset[0] * 2.0f) * (1.0f / *fAspectRatio)));
			fX = M_PI - atan2(1.0f, fMult * (fCrosshairOffset[1] * 2.0f - 1.0f));
		}
		else fX = fZ = M_PI / 2;

		BYTE byteWeapon = pSAMP->getPlayers()->pLocalPlayer->byteCurrentWeapon;
		if (byteWeapon >= 22 && byteWeapon <= 29 || byteWeapon == 32) fFix = 0.04253f;
		else if (byteWeapon == 30 || byteWeapon == 31) fFix = 0.028f;
		else if (byteWeapon == 33) fFix = 0.01897f;

		float fDistX = sqrt(vecVector.fX * vecVector.fX + vecVector.fY * vecVector.fY);

		if (vecVector.fX <= 0.0 && vecVector.fY >= 0.0 || vecVector.fX >= 0.0 && vecVector.fY >= 0.0)
			fVecX = (acosf(vecVector.fX / fDistX) + fFix) - TheCamera.m_aCams[0].m_fHorizontalAngle;
		if (vecVector.fX >= 0.0 && vecVector.fY <= 0.0 || vecVector.fX <= 0.0 && vecVector.fY <= 0.0)
			fVecX = (-acosf(vecVector.fX / fDistX) + fFix) - TheCamera.m_aCams[0].m_fHorizontalAngle;

		float fSmoothX = fVecX / (g_Config.g_Aimbot.iAimbotConfig[byteWeapon][SMOOTH] * 2);

		if (fSmoothX > -1.0 && fSmoothX < 0.5 && fVecX > -2.0 && fVecX < 2.0)
			TheCamera.m_aCams[0].m_fHorizontalAngle += fSmoothX;

		if (g_Config.g_Aimbot.bSmoothLockY)
		{
			float fDistZ = sqrt(vecVector.fX * vecVector.fX + vecVector.fY * vecVector.fY);
			float fSmoothZ = (atan2f(fDistZ, vecVector.fZ) - fZ - TheCamera.m_aCams[0].m_fVerticalAngle) / (g_Config.g_Aimbot.iAimbotConfig[byteWeapon][SMOOTH] * 2);
			TheCamera.m_aCams[0].m_fVerticalAngle += fSmoothZ;
		}
	}
}

void CAimbot::ProAimbot()
{
	if (g_Config.g_Aimbot.bProAim && g_Config.g_Aimbot.bAimbotEnabled[pSAMP->getPlayers()->pLocalPlayer->byteCurrentWeapon] && iTargetPlayer != -1 && bCrosshair && GetAsyncKeyState(VK_LBUTTON))
	{
		CPed* pPed = CPools::GetPed(pSAMP->getPlayers()->pRemotePlayer[pAimbot->iTargetPlayer]->pPlayerData->pSAMP_Actor->ulGTAEntityHandle);
		if (!pPed)
			return;

		CVector vecCamera, vecOrigin, vecTarget;
		if (pSAMP->getPlayers()->pLocalPlayer->byteCurrentWeapon != 34)
		{
			Utils::getBonePosition(FindPlayerPed(), BONE_RIGHTWRIST, &vecOrigin);
			TheCamera.Find3rdPersonCamTargetVector(2.f, vecOrigin, &vecCamera, &vecTarget);
		}
		else
		{
			CVector vecFront = TheCamera.m_aCams[0].m_vecFront;
			vecFront.Normalise();
			vecOrigin =* TheCamera.GetGameCamPosition();
			vecOrigin += (vecFront * 2.0f);
			vecTarget = vecOrigin + (vecFront * 1.5f);
		}

		pPed->SetPosn(vecTarget);
	}
}

void CAimbot::Triggerbot()
{
	if (g_Config.g_Aimbot.bTriggerbot && g_Config.g_Aimbot.bAimbotEnabled[pSAMP->getPlayers()->pLocalPlayer->byteCurrentWeapon] && bCrosshair)
	{
		static ULONGLONG iTick = GetTickCount64();
		if (g_Config.g_Aimbot.bTriggerDelay && GetTickCount64() - iTick < (ULONGLONG)g_Config.g_Aimbot.iTriggerDelay)
			return;

		if (g_Config.g_Aimbot.bTriggerSync)
		{
			if (g_Config.g_Aimbot.bSilent && g_Config.g_Aimbot.bAimbot && iTargetPlayer != -1)
			{
				pKeyHook->gameKeyState[BUTTON_CIRCLE] = 0xFF;
				iTick = GetTickCount64();
			}
		}
		else
		{
			CVector vecCamera, vecOrigin, vecTarget;
			if (pSAMP->getPlayers()->pLocalPlayer->byteCurrentWeapon != 34)
			{
				Utils::getBonePosition(FindPlayerPed(), BONE_RIGHTWRIST, &vecOrigin);
				TheCamera.Find3rdPersonCamTargetVector(100.f, vecOrigin, &vecCamera, &vecTarget);
			}
			else
			{
				CVector vecFront = TheCamera.m_aCams[0].m_vecFront;
				vecFront.Normalise();
				vecOrigin = *TheCamera.GetGameCamPosition();
				vecOrigin += (vecFront * 2.0f);
				vecTarget = vecOrigin + (vecFront * 100.f);
			}

			CColPoint pCollision;
			CEntity* pCollisionEntity = NULL;
			bool bCollision = CWorld::ProcessLineOfSight(vecCamera, vecTarget, pCollision, pCollisionEntity, !g_Config.g_Aimbot.bLockThroughObjects, !g_Config.g_Aimbot.bLockThroughObjects, true, true, true, true, false, true);

			if (bCollision && pCollisionEntity && pCollisionEntity->m_nType == ENTITY_TYPE_PED)
			{
				for (int i = 0; i < SAMP_MAX_PLAYERS; i++)
				{
					if (!pSAMP->isPlayerStreamed(i))
						continue;

					CPed* pPed = CPools::GetPed(pSAMP->getPlayers()->pRemotePlayer[i]->pPlayerData->pSAMP_Actor->ulGTAEntityHandle);

					if (!pPed || pPed == FindPlayerPed())
						continue;

					if (pCollisionEntity != (CEntity*)pPed)
						continue;

					if (pSAMP->getPlayers()->pRemotePlayer[i]->pPlayerData->pSAMP_Actor->pGTA_Ped->hitpoints <= 0.f)
						continue;

					if (g_Config.g_Aimbot.bTeamProtect && pSAMP->getPlayerColor(i) == pSAMP->getPlayerColor(pSAMP->getPlayers()->sLocalPlayerID))
						continue;

					if (!g_Config.g_Aimbot.bIgnoreEverything)
					{
						if (g_Config.g_Aimbot.bIgnoreAFK && pSAMP->getPlayers()->pRemotePlayer[i]->pPlayerData->iAFKState == 2)
							continue;

						float fDistanceFromEnemy = Math::vect3_dist(&pSAMP->getPlayers()->pRemotePlayer[i]->pPlayerData->pSAMP_Actor->pGTA_Ped->base.matrix[12], &pSAMP->getPlayers()->pLocalPlayer->pSAMP_Actor->pGTA_Ped->base.matrix[12]);
						if (!g_Config.g_Aimbot.bIgnoreMaxDistance && fDistanceFromEnemy > fWeaponRange[pSAMP->getPlayers()->pLocalPlayer->byteCurrentWeapon])
							continue;
					}

					pKeyHook->gameKeyState[BUTTON_CIRCLE] = 0xFF;
					iTick = GetTickCount64();
					break;
				}
			}
		}
	}
}