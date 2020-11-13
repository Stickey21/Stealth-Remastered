#include "main.h"

CVisuals* pVisuals;

float __cdecl CVisuals::hkCalculateAspectRatio()
{
	if (g_Config.g_Visuals.bAspectRatio && g_Config.g_Visuals.fAspectRatio >= 0.2f && g_Config.g_Visuals.fAspectRatio <= 2.0f && (!FrontEndMenuManager.m_bWidescreenOn || !TheCamera.m_bWideScreenOn))
	{
		pSecure->Write(0xC3EFA4, g_Config.g_Visuals.fAspectRatio);
		return g_Config.g_Visuals.fAspectRatio;
	}

	return pVisuals->oCalculateAspectRatio();
}

void CVisuals::Render()
{
	if (g_Config.g_Visuals.bVehicleNameTagsESP || g_Config.g_Visuals.bVehicleHPESP || g_Config.g_Visuals.bVehicleStatusESP || g_Config.g_Visuals.bVehicleDistanceESP || g_Config.g_Visuals.bVehicleEngineESP || g_Config.g_Visuals.bVehicleBoundingBoxESP)
		ESP_Vehicle();

	if (g_Config.g_Visuals.bNameTagsESP || g_Config.g_Visuals.bWeaponESP || g_Config.g_Visuals.bSnipelineESP || g_Config.g_Visuals.bBoxESP || g_Config.g_Visuals.bBoneESP || g_Config.g_Visuals.bChamsESP || g_Config.g_Visuals.bDistanceESP)
	{
		for (int i = 0; i < SAMP_MAX_PLAYERS; i++)
		{
			if (!pSAMP->isPlayerStreamed(i) || g_Config.g_Visuals.bESPTeammates && pSAMP->getPlayerColor(i) == pSAMP->getPlayerColor(pSAMP->getPlayers()->sLocalPlayerID))
				continue;

			CPed* pPed = CPools::GetPed(pSAMP->getPlayers()->pRemotePlayer[i]->pPlayerData->pSAMP_Actor->ulGTAEntityHandle);
			if (!pPed)
				continue;

			if (g_Config.g_Visuals.bChamsESP)
				ESP_Chams(i, pPed);
			if (g_Config.g_Visuals.bSnipelineESP)
				ESP_Snipeline(i, pPed);
			if (g_Config.g_Visuals.bNameTagsESP || g_Config.g_Visuals.bWeaponESP)
				ESP_NameTags(i, pPed);
			if (g_Config.g_Visuals.bBoxESP && pPed->IsAlive())
				ESP_Box(i, pPed);
			if (g_Config.g_Visuals.bBoneESP)
				ESP_Bones(i, pPed);
			if (g_Config.g_Visuals.bDistanceESP)
				ESP_Distance(i, pPed);
		}
	}

	if (g_Config.g_Visuals.bDMGInformer)
	{
		for (auto i = 0; i < _countof(g_DMGInformer); i++)
		{
			if (g_DMGInformer[i].dwTick >= GetTickCount64())
			{
				CVector vecScreen;
				g_DMGInformer[i].vecPos.fZ += 0.007f;

				Utils::CalcScreenCoors(&g_DMGInformer[i].vecPos, &vecScreen);
				if (vecScreen.fZ < 1.f)
					continue;

				char szDamage[16];
				sprintf(szDamage, "%0.1f", fWeaponDamage[g_DMGInformer[i].iWeapon]);
				pRender->DrawString(pRender->pESPFont, szDamage, vecScreen, (float)g_Config.g_Visuals.iFontSize, g_Config.g_Visuals.colorDMG, true);
			}
			else g_DMGInformer[i] = { };
		}

		//ImColor colorHitmarker = { 255, 255, 255, (int)(pVisuals->fHitmarkerAlpha * 255) };

		//if (pVisuals->fHitmarkerAlpha >= 0.01f)
		//{
		//	pRender->DrawLine(CVector(pAimbot->vecCrosshair.fX - 10, pAimbot->vecCrosshair.fY - 10, 0), CVector(pAimbot->vecCrosshair.fX - 2.5f, pAimbot->vecCrosshair.fY - 2.5f, 0), colorHitmarker, 1.1f);
		//	pRender->DrawLine(CVector(pAimbot->vecCrosshair.fX - 10, pAimbot->vecCrosshair.fY + 10, 0), CVector(pAimbot->vecCrosshair.fX - 2.5f, pAimbot->vecCrosshair.fY + 2.5f, 0), colorHitmarker, 1.1f);
		//	pRender->DrawLine(CVector(pAimbot->vecCrosshair.fX + 10, pAimbot->vecCrosshair.fY - 10, 0), CVector(pAimbot->vecCrosshair.fX + 2.5f, pAimbot->vecCrosshair.fY - 2.5f, 0), colorHitmarker, 1.1f);
		//	pRender->DrawLine(CVector(pAimbot->vecCrosshair.fX + 10, pAimbot->vecCrosshair.fY + 10, 0), CVector(pAimbot->vecCrosshair.fX + 2.5f, pAimbot->vecCrosshair.fY + 2.5f, 0), colorHitmarker, 1.1f);
		//	pVisuals->fHitmarkerAlpha -= 0.01f;
		//}
	}
}

void CVisuals::Update()
{
	NoDrugsEffect();
	FPSFuncs();
	WeatherChanger();
}

bool CVisuals::isPlayerVisible(int iPlayerID, bool bDistance)
{
	CPed* pPed = CPools::GetPed(pSAMP->getPlayers()->pRemotePlayer[iPlayerID]->pPlayerData->pSAMP_Actor->ulGTAEntityHandle);
	CMatrix matEnt = *pPed->GetMatrix();
	CVector vecBone = *Utils::getBonePosition(pPed, BONE_HEAD, &vecBone);
	if (!CWorld::GetIsLineOfSightClear(*TheCamera.GetGameCamPosition(), vecBone, true, false, false, true, true, false, false) || bDistance && Utils::getDistanceFromCamera(matEnt.pos) > pSAMP->getInfo()->pSettings->fNameTagsDistance)
		return false;
	return true;
}

ImColor CVisuals::getESPColor(int iPlayerID)
{
	if (g_Config.g_Visuals.bESPCustomColors)
		return isPlayerVisible(iPlayerID, false) ? ImColor(g_Config.g_Visuals.colorVisible) : ImColor(g_Config.g_Visuals.colorNotVisible);
	else return pSAMP->getPlayerColor(iPlayerID);
}

void CVisuals::ESP_NameTags(int iPlayerID, CPed* pPed)
{
	if (isPlayerVisible(iPlayerID, true))
		return;

	CVector vecHead, vecHeadScreen;
	Utils::getBonePosition(pPed, BONE_HEAD, &vecHead);
	vecHead.fZ += 0.2f + (Utils::getDistanceFromCamera(pPed->GetMatrix()->pos) * 0.0475f);
	Utils::CalcScreenCoors(&vecHead, &vecHeadScreen);
	if (vecHeadScreen.fZ < 1.f)
		return;

	if (g_Config.g_Visuals.bNameTagsESP)
	{
		if (pSAMP->getPlayers()->pRemotePlayer[iPlayerID]->pPlayerData->iAFKState == 2)
			pRender->DrawString(pRender->pESPFont, "AFK", { vecHeadScreen.fX - 37.f, vecHeadScreen.fY + 16.f, 0 }, 12.f, 0xFF808080, true);

		char szName[64];
		sprintf(szName, "%s (%d)", pSAMP->getPlayerName(iPlayerID), iPlayerID);
		pRender->DrawString(pRender->pESPFont, szName, vecHeadScreen, 15.f, pSAMP->getPlayerColor(iPlayerID), true);

		float fHealth = pSAMP->getPlayers()->pRemotePlayer[iPlayerID]->pPlayerData->fActorHealth;
		float fArmor = pSAMP->getPlayers()->pRemotePlayer[iPlayerID]->pPlayerData->fActorArmor;

		if (fHealth > 100.0f)
			fHealth = 100.0f;

		fHealth *= 40.f / 100.0f;
		fHealth -= (40.f / 2.f);

		ImColor colorHealthBar = *(ImU32*)(pSAMP->g_dwSAMP_Addr + 0x68B0C), colorHealthBarBG = *(ImU32*)(pSAMP->g_dwSAMP_Addr + 0x68B33);

		pRender->DrawRectFilled({ vecHeadScreen.fX - 21.f, (fArmor > 0.0f) ? vecHeadScreen.fY + 25.f : vecHeadScreen.fY + 17.f, 0 }, { vecHeadScreen.fX + 21.f, (fArmor > 0.0f) ? vecHeadScreen.fY + 31.f : vecHeadScreen.fY + 23.f, 0 }, 0xFF000000);
		pRender->DrawRectFilled({ vecHeadScreen.fX - 20.f, (fArmor > 0.0f) ? vecHeadScreen.fY + 26.f : vecHeadScreen.fY + 18.f, 0 }, { vecHeadScreen.fX + 20.f, (fArmor > 0.0f) ? vecHeadScreen.fY + 30.f : vecHeadScreen.fY + 22.f, 0 }, ImColor(colorHealthBarBG.Value.z, colorHealthBarBG.Value.y, colorHealthBarBG.Value.x, colorHealthBarBG.Value.w));
		pRender->DrawRectFilled({ vecHeadScreen.fX - 20.f, (fArmor > 0.0f) ? vecHeadScreen.fY + 26.f : vecHeadScreen.fY + 18.f, 0 }, { vecHeadScreen.fX + fHealth, (fArmor > 0.0f) ? vecHeadScreen.fY + 30.f : vecHeadScreen.fY + 22.f, 0 }, ImColor(colorHealthBar.Value.z, colorHealthBar.Value.y, colorHealthBar.Value.x, colorHealthBar.Value.w));

		if (fArmor > 0.0f)
		{
			ImColor colorArmorBar = *(ImU32*)(pSAMP->g_dwSAMP_Addr + 0x68DD5), colorArmorBarBG = *(ImU32*)(pSAMP->g_dwSAMP_Addr + 0x68E00);

			if (fArmor > 100.0f)
				fArmor = 100.0f;

			fArmor *= 40.f / 100.0f;
			fArmor -= (40.f / 2);

			pRender->DrawRectFilled({ vecHeadScreen.fX - 21.f, vecHeadScreen.fY + 17.f, 0 }, { vecHeadScreen.fX + 21.f, vecHeadScreen.fY + 23.f, 0 }, 0xFF000000);
			pRender->DrawRectFilled({ vecHeadScreen.fX - 20.f, vecHeadScreen.fY + 18.f, 0 }, { vecHeadScreen.fX + 20.f, vecHeadScreen.fY + 22.f, 0 }, ImColor(colorArmorBarBG.Value.z, colorArmorBarBG.Value.y, colorArmorBarBG.Value.x, colorArmorBarBG.Value.w));
			pRender->DrawRectFilled({ vecHeadScreen.fX - 20.f, vecHeadScreen.fY + 18.f, 0 }, { vecHeadScreen.fX + fArmor, vecHeadScreen.fY + 22.f, 0 }, ImColor(colorArmorBar.Value.z, colorArmorBar.Value.y, colorArmorBar.Value.x, colorArmorBar.Value.w));
		}
	}

	if (g_Config.g_Visuals.bWeaponESP)
	{
		ID3DXSprite* pSprite = pSAMP->getDeathList()->pSprite;
		ID3DXFont* pWeaponFont = pSAMP->getDeathList()->pWeaponFont1;
		RECT rect = { (LONG)vecHeadScreen.fX + 20, (LONG)vecHeadScreen.fY + 16, (LONG)vecHeadScreen.fX + 1, (LONG)vecHeadScreen.fY + 1 };

		pSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE);
		pWeaponFont->DrawText(pSprite, "G", -1, &rect, DT_NOCLIP | DT_LEFT, 0xFF000000);
		pWeaponFont->DrawText(pSprite, pSAMP->getWeaponSpriteID(pPed->m_aWeapons[pPed->m_nActiveWeaponSlot].m_nType), -1, &rect, DT_NOCLIP | DT_LEFT, 0xFFFFFFFF);
		pSprite->End();
	}
}

void CVisuals::ESP_Snipeline(int iPlayerID, CPed* pPed)
{
	CVector vecTarget = pPed->GetPosition(), vecTargetScreen;
	g_Config.g_Visuals.iSnipelineStyle ? vecTarget.fZ -= 1.1f : vecTarget.fZ += 1.0f;
	Utils::CalcScreenCoors(&vecTarget, &vecTargetScreen);
	if (vecTargetScreen.fZ < 1.f)
		return;

	pRender->DrawLine(vecTargetScreen, { ImGui::GetIO().DisplaySize.x / 2.f, g_Config.g_Visuals.iSnipelineStyle ? ImGui::GetIO().DisplaySize.y : 0, 0 }, getESPColor(iPlayerID), g_Config.g_Visuals.fESPThickness);
}

void CVisuals::ESP_Box(int iPlayerID, CPed* pPed)
{
	CVector vecTarget = pPed->GetPosition(), vecTargetScreen;
	Utils::CalcScreenCoors(&vecTarget, &vecTargetScreen);

	CVector vecHead = vecTarget, vecFoot = vecTarget, vecHeadScreen, vecFootScreen;
	vecHead.fZ += 1.0f, vecFoot.fZ -= 1.1f;

	Utils::CalcScreenCoors(&vecHead, &vecHeadScreen);
	Utils::CalcScreenCoors(&vecFoot, &vecFootScreen);
	if (vecTargetScreen.fZ < 1.f || vecHeadScreen.fZ < 1.f || vecFootScreen.fZ < 1.f)
		return;

	ImColor colorESP = getESPColor(iPlayerID);

	if (!g_Config.g_Visuals.iBoxStyle)
	{
		float fWidth = abs((vecFootScreen.fY - vecHeadScreen.fY) / 4.f);
		ImColor colorVisible = isPlayerVisible(iPlayerID, false) ? ImColor(g_Config.g_Visuals.colorVisible) : ImColor(g_Config.g_Visuals.colorNotVisible);
		if (g_Config.g_Visuals.bFilledBox)
			pRender->DrawRectFilled({ vecHeadScreen.fX - fWidth, vecHeadScreen.fY, 0 }, { vecFootScreen.fX + fWidth, vecFootScreen.fY, 0 }, ImColor(colorVisible.Value.x, colorVisible.Value.y, colorVisible.Value.z, 0.35f));
		pRender->DrawRect({ vecHeadScreen.fX - fWidth, vecHeadScreen.fY, 0 }, { vecFootScreen.fX + fWidth, vecFootScreen.fY, 0 }, colorESP, g_Config.g_Visuals.fESPThickness);
	}
	else
	{
		CVector vecMin = pPed->GetColModel()->m_boundBox.m_vecMin, vecMax = pPed->GetColModel()->m_boundBox.m_vecMax;

		CVector vecCorner[8] =
		{
			{ vecMin.fX, vecMin.fY, vecMin.fZ },
			{ vecMin.fX, vecMin.fY * -1.f, vecMin.fZ },
			{ vecMin.fX * -1.f, vecMin.fY * -1.f, vecMin.fZ },
			{ vecMin.fX * -1.f, vecMin.fY, vecMin.fZ },
			{ vecMax.fX * -1.f, vecMax.fY * -1.f, vecMax.fZ },
			{ vecMax.fX * -1.f, vecMax.fY, vecMax.fZ },
			{ vecMax.fX, vecMax.fY, vecMax.fZ },
			{ vecMax.fX, vecMax.fY * -1.f, vecMax.fZ }
		};

		for (auto i = 0; i < 8; i++)
			plugin::Command<plugin::Commands::GET_OFFSET_FROM_CHAR_IN_WORLD_COORDS>(pPed, vecCorner[i].fX, vecCorner[i].fY, vecCorner[i].fZ, &vecCorner[i].fX, &vecCorner[i].fY, &vecCorner[i].fZ);

		pRender->DrawBoundingBox(vecCorner, colorESP, g_Config.g_Visuals.fESPThickness, g_Config.g_Visuals.bFilledBox, isPlayerVisible(iPlayerID, false) ? ImColor(g_Config.g_Visuals.colorVisible) : ImColor(g_Config.g_Visuals.colorNotVisible));
	}
}

void CVisuals::ESP_Bones(int iPlayerID, CPed* pPed)
{
	CVector vecBone[55];
	for (int iBone = BONE_PELVIS1; iBone <= BONE_RIGHTFOOT; iBone++)
	{
		switch (iBone)
		{
		case 5: case 4: case 22: case 32: case 23: case 33: case 24: case 34: case 25: case 26: case 35: case 36: case 3: case 2: case 52: case 42: case 53: case 43: case 54: case 44:
			Utils::getBonePosition(pPed, (ePedBones)iBone, &vecBone[iBone]);
			break;
		}
	}

	//CVector vecHeadScreen;
	//Utils::CalcScreenCoors(&vecBone[BONE_HEAD2], &vecHeadScreen);
	//if (vecHeadScreen.fZ < 1.0f)
	//	return;

	ImColor colorESP = getESPColor(iPlayerID);
	//float fDistance = 1.f / (Utils::getDistanceFromCamera(vecBone[BONE_HEAD2]) + 5.f) * 110.f;
	//pRender->DrawCircleFilled(vecHeadScreen, fDistance, colorESP);

	pRender->D3DLine(vecBone[BONE_NECK], vecBone[BONE_UPPERTORSO], colorESP, g_Config.g_Visuals.fESPThickness);
	pRender->D3DLine(vecBone[BONE_UPPERTORSO], vecBone[BONE_RIGHTSHOULDER], colorESP, g_Config.g_Visuals.fESPThickness);
	pRender->D3DLine(vecBone[BONE_UPPERTORSO], vecBone[BONE_LEFTSHOULDER], colorESP, g_Config.g_Visuals.fESPThickness);
	pRender->D3DLine(vecBone[BONE_LEFTSHOULDER], vecBone[BONE_LEFTELBOW], colorESP, g_Config.g_Visuals.fESPThickness);
	pRender->D3DLine(vecBone[BONE_RIGHTSHOULDER], vecBone[BONE_RIGHTELBOW], colorESP, g_Config.g_Visuals.fESPThickness);
	pRender->D3DLine(vecBone[BONE_LEFTELBOW], vecBone[BONE_LEFTWRIST], colorESP, g_Config.g_Visuals.fESPThickness);
	pRender->D3DLine(vecBone[BONE_RIGHTELBOW], vecBone[BONE_RIGHTWRIST], colorESP, g_Config.g_Visuals.fESPThickness);
	pRender->D3DLine(vecBone[BONE_LEFTWRIST], vecBone[BONE_LEFTHAND], colorESP, g_Config.g_Visuals.fESPThickness);
	pRender->D3DLine(vecBone[BONE_LEFTHAND], vecBone[BONE_LEFTTHUMB], colorESP, g_Config.g_Visuals.fESPThickness);
	pRender->D3DLine(vecBone[BONE_RIGHTWRIST], vecBone[BONE_RIGHTHAND], colorESP, g_Config.g_Visuals.fESPThickness);
	pRender->D3DLine(vecBone[BONE_RIGHTHAND], vecBone[BONE_RIGHTTHUMB], colorESP, g_Config.g_Visuals.fESPThickness);
	pRender->D3DLine(vecBone[BONE_UPPERTORSO], vecBone[BONE_SPINE1], colorESP, g_Config.g_Visuals.fESPThickness);
	pRender->D3DLine(vecBone[BONE_SPINE1], vecBone[BONE_PELVIS], colorESP, g_Config.g_Visuals.fESPThickness);
	pRender->D3DLine(vecBone[BONE_PELVIS], vecBone[BONE_RIGHTKNEE], colorESP, g_Config.g_Visuals.fESPThickness);
	pRender->D3DLine(vecBone[BONE_PELVIS], vecBone[BONE_LEFTKNEE], colorESP, g_Config.g_Visuals.fESPThickness);
	pRender->D3DLine(vecBone[BONE_RIGHTKNEE], vecBone[BONE_RIGHTANKLE], colorESP, g_Config.g_Visuals.fESPThickness);
	pRender->D3DLine(vecBone[BONE_LEFTKNEE], vecBone[BONE_LEFTANKLE], colorESP, g_Config.g_Visuals.fESPThickness);
	pRender->D3DLine(vecBone[BONE_RIGHTANKLE], vecBone[BONE_RIGHTFOOT], colorESP, g_Config.g_Visuals.fESPThickness);
	pRender->D3DLine(vecBone[BONE_LEFTANKLE], vecBone[BONE_LEFTFOOT], colorESP, g_Config.g_Visuals.fESPThickness);
}

void CVisuals::ESP_Chams(int iPlayerID, CPed* pPed)
{
	if (!pPed->GetIsOnScreen())
		return;

	DWORD dwCONSTANT, dwARG2, dwCULL;

	pD3DHook->pD3DDevice->GetRenderState(D3DRS_CULLMODE, &dwCULL);
	pD3DHook->pD3DDevice->GetTextureStageState(0, D3DTSS_CONSTANT, &dwCONSTANT);
	pD3DHook->pD3DDevice->GetTextureStageState(0, D3DTSS_COLORARG2, &dwARG2);

	pD3DHook->pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	pD3DHook->pD3DDevice->SetTextureStageState(0, D3DTSS_CONSTANT, 0xFFFFFFFF);
	pD3DHook->pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CONSTANT);

	pPed->Render();

	pD3DHook->pD3DDevice->SetRenderState(D3DRS_CULLMODE, dwCULL);
	pD3DHook->pD3DDevice->SetTextureStageState(0, D3DTSS_CONSTANT, dwCONSTANT);
	pD3DHook->pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, dwARG2);
}

void CVisuals::ESP_Distance(int iPlayerID, CPed* pPed)
{
	CVector vecFoot = pPed->GetPosition(), vecFootScreen;
	vecFoot.fZ -= 1.2f;

	Utils::CalcScreenCoors(&vecFoot, &vecFootScreen);
	if (vecFootScreen.fZ < 1.f)
		return;

	char szDistance[32];
	sprintf(szDistance, "%.2f m", Utils::getDistance(pPed->GetPosition()));
	pRender->DrawString(pRender->pESPFont, szDistance, vecFootScreen, 14.f, 0xFFFFFFFF, true);
}


void CVisuals::ESP_Vehicle()
{
	for (int i = 0; i <= SAMP_MAX_VEHICLES; i++)
	{
		if (!pSAMP->isVehicleStreamed(i))
			continue;

		CVehicle* pVehicle = CPools::GetVehicle(pSAMP->getVehicles()->pSAMP_Vehicle[i]->ulGTAEntityHandle);
		if (!pVehicle)
			continue;

		CVector vecVehicle = pVehicle->m_matrix->pos, vecVehicleScreen;
		Utils::CalcScreenCoors(&vecVehicle, &vecVehicleScreen);
		if (vecVehicleScreen.fZ < 1.f)
			continue;

		if (g_Config.g_Visuals.bVehicleNameTagsESP)
		{
			char szName[64];
			sprintf(szName, "%s (Model: %d)", szVehicleNames[pVehicle->m_nModelIndex - 400], pVehicle->m_nModelIndex);
			vecVehicleScreen.fY -= 14.f;
			vecVehicleScreen.fY = pRender->DrawString(pRender->pESPFont, szName, vecVehicleScreen, 14.f, 0xFFFFFFFF, true);
		}

		if (g_Config.g_Visuals.bVehicleHPESP)
		{
			int iVehicleHealth = (int)pSAMP->getVehicles()->pSAMP_Vehicle[i]->pGTA_Vehicle->hitpoints;

			if (iVehicleHealth > 1000)
				iVehicleHealth = 1000;

			char szHealth[64];
			sprintf(szHealth, "Health: %d", iVehicleHealth);
			vecVehicleScreen.fY = pRender->DrawString(pRender->pESPFont, szHealth, vecVehicleScreen, 14.f, 0xFFFFFFFF, true);
		}

		if (g_Config.g_Visuals.bVehicleEngineESP)
			vecVehicleScreen.fY = pRender->DrawString(pRender->pESPFont, pSAMP->getVehicles()->pSAMP_Vehicle[i]->bIsMotorOn ? "Engine: OFF" : "Engine: ON", vecVehicleScreen, 14.f, 0xFFFFFFFF, true);

		if (g_Config.g_Visuals.bVehicleStatusESP)
			vecVehicleScreen.fY = pRender->DrawString(pRender->pESPFont, pSAMP->getVehicles()->pSAMP_Vehicle[i]->iIsLocked ? "Status: Locked" : "Status: Unlocked", vecVehicleScreen, 14.f, 0xFFFFFFFF, true);

		if (g_Config.g_Visuals.bVehicleDistanceESP)
		{
			char szDistance[64];
			sprintf(szDistance, "Distance: %.2f m", Utils::getDistance(vecVehicle));
			vecVehicleScreen.fY = pRender->DrawString(pRender->pESPFont, szDistance, vecVehicleScreen, 14.f, 0xFFFFFFFF, true);
		}

		if (g_Config.g_Visuals.bVehicleBoundingBoxESP)
		{
			CVector vecMin = pVehicle->GetColModel()->m_boundBox.m_vecMin, vecMax = pVehicle->GetColModel()->m_boundBox.m_vecMax;

			CVector vecCorner[8] =
			{
				{ vecMin.fX, vecMin.fY, vecMin.fZ },
				{ vecMin.fX, vecMin.fY * -1.f, vecMin.fZ },
				{ vecMin.fX * -1.f, vecMin.fY * -1.f, vecMin.fZ },
				{ vecMin.fX * -1.f, vecMin.fY, vecMin.fZ },
				{ vecMax.fX * -1.f, vecMax.fY * -1.f, vecMax.fZ },
				{ vecMax.fX * -1.f, vecMax.fY, vecMax.fZ },
				{ vecMax.fX, vecMax.fY, vecMax.fZ },
				{ vecMax.fX, vecMax.fY * -1.f, vecMax.fZ }
			};

			for (auto i = 0; i < 8; i++)
				plugin::Command<plugin::Commands::GET_OFFSET_FROM_CAR_IN_WORLD_COORDS>(pVehicle, vecCorner[i].fX, vecCorner[i].fY, vecCorner[i].fZ, &vecCorner[i].fX, &vecCorner[i].fY, &vecCorner[i].fZ);

			pRender->DrawBoundingBox(vecCorner, 0xFFFFFFFF, g_Config.g_Visuals.fESPThickness, false, 0xFFFFFFFF);
		}
	}
}

void CVisuals::FPSFuncs()
{
	static bool bFPSUnlock = false;
	if (g_Config.g_Visuals.bFPSFuncs && g_Config.g_Visuals.bFPSUnlock)
	{
		pSecure->memcpy_safe((void*)(pSAMP->g_dwSAMP_Addr + 0x9D9D0), "\x90\x90\x90\x90\x90", 5);
		bFPSUnlock = true;
	}
	else if (bFPSUnlock)
	{
		Memory::memcpy_safe((void*)(pSAMP->g_dwSAMP_Addr + 0x9D9D0), "\xE8\x9B\xF7\xFF\xFF", 5);
		bFPSUnlock = false;
	}

	static bool bFPSBoost = false;
	if (g_Config.g_Visuals.bFPSFuncs && g_Config.g_Visuals.bFPSBoost)
	{
		ImGui::GetStyle().AntiAliasedLines = false;
		ImGui::GetStyle().AntiAliasedFill = false;
		pSecure->memcpy_safe((void*)0x53E227, "\xC3", 1);
		bFPSBoost = true;
	}
	else if (bFPSBoost)
	{
		ImGui::GetStyle().AntiAliasedLines = true;
		ImGui::GetStyle().AntiAliasedFill = true;
		Memory::memcpy_safe((void*)0x53E227, "\xE9", 1);
		bFPSBoost = false;
	}
}

void CVisuals::NoDrugsEffect()
{
	static bool bDrugEffect = false;
	if (g_Config.g_Visuals.bNoDrugsEffect)
	{
		pSecure->memcpy_safe((void*)0x71D76C, "\x90\x90\x90\x90\x90", 5);
		bDrugEffect = true;
	}
	else if (bDrugEffect)
	{
		Memory::memcpy_safe((void*)0x71D76C, "\xA3\x58\x3C\xC7\x00", 5);
		bDrugEffect = false;
	}
}

void CVisuals::WeatherChanger()
{
	if (g_Config.g_Visuals.bWeatherChanger)
		CWeather::ForceWeatherNow(g_Config.g_Visuals.iWeather);

	if (g_Config.g_Visuals.bTimeChanger)
		CClock::SetGameClock(g_Config.g_Visuals.iTime, 0, 0);
}