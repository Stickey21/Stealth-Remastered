#include "main.h"

CRakClient* pRakClient;

bool CRakClient::RPC(int uniqueID, BitStream* bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel, bool shiftTimestamp)
{
	Memory::memcpy_safe((void*)(pSAMP->g_dwSAMP_Addr + 0x3A560), "\x55\x8B\xEC\x6A\xFF", 5);
	return (tRPC(vTable[25]))(pSAMP->getInfo()->pRakClientInterface, &uniqueID, bitStream, priority, reliability, orderingChannel, shiftTimestamp);
}

bool CRakClient::Send(BitStream* bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel)
{
	Memory::memcpy_safe((void*)(pSAMP->g_dwSAMP_Addr + 0x33DC0), "\x6A\xFF\x68\x6B\x0C\xD1\x03", 7);
	Memory::memcpy_safe((void*)(pSAMP->g_dwSAMP_Addr + 0x37490), "\x53\x55\x56\x8D\xA9\xE9\x07\x00\x00", 9);
	return (tSend(vTable[6]))(pSAMP->getInfo()->pRakClientInterface, bitStream, priority, reliability, orderingChannel);
}

bool __fastcall CRakClient::hkRPC(void* _this, void* pUnknown, int* uniqueID, BitStream* bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel, bool shiftTimestamp)
{
	if (*uniqueID == 181)
	{
		BYTE byteCounter[4] = { 0x0 };
		for (auto i = 0; i < 4; i++)
			byteCounter[i] = *reinterpret_cast<BYTE*>(pSAMP->g_dwSAMPCAC_Addr + 0x94194 + i);

		DWORD dwCounter = ((0x1 << 24) | (byteCounter[2] << 16) | ((byteCounter[1] + (byteCounter[0] >= 0x14 ? byteCounter[3] : 0x0)) << 8) | (BYTE)(byteCounter[0] - 0x14));
		*reinterpret_cast<int*>(dwCounter) = -1;

		return false;
	}

	if (*uniqueID == 185)
	{
		DWORD dwAddress = 0x0;
		BYTE byteSize = 0, byteContent[4] = { 0x0 };

		bitStream->Read(dwAddress);
		bitStream->Read(byteSize);
		for (int i = 0; i < byteSize; i++)
			bitStream->Read(byteContent[i]);

		bitStream->SetWriteOffset(0x28);

		pSAMP->addMessageToChat(0xFFFFFFFF, "{EE5555}CAC Read: {FFFFFF}Address: 0x%X | Size: %d | Content: %.2X %.2X %.2X %.2X | SA:MP Base: 0x%X", dwAddress, byteSize, byteContent[0], byteContent[1], byteContent[2], byteContent[3], pSAMP->g_dwSAMP_Addr);

		for (auto i = 0; i < byteSize; i++)
		{
			int iAddress = pSecure->isAddressSecured(dwAddress + i);
			if (iAddress != -1)
				bitStream->Write<BYTE>(pSecure->vecMemory[iAddress].origByte);
			else bitStream->Write<BYTE>(byteContent[i]);
		}
	}

	if (*uniqueID == RPC_ExitVehicle)
	{
		if (g_Config.g_Player.bStopOnExitVehicle && FindPlayerVehicle(-1, false) && FindPlayerVehicle(-1, false)->m_vecMoveSpeed.Magnitude() > 0.40f)
		{
			Player::dwExitTick = GetTickCount64();
			Player::bExitVehicle = true;
		}
	}

	return pRakClient->oRPC(_this, uniqueID, bitStream, priority, reliability, orderingChannel, shiftTimestamp);
}

bool __fastcall CRakClient::hkSend(void* _this, void* Unknown, BitStream* bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel)
{
	BYTE packetId;
	bitStream->Read(packetId);

	bool bEditBulletSync = false;
	bool bEditAimSync = false;
	bool bEditFootSync = false;
	bool bEditVehicleSync = false;
	bool bEditStatusSync = false;

	if (g_Config.g_Player.bFakeLag)
	{
		static ULONGLONG dwSyncTick = GetTickCount64();
		if (GetTickCount64() - dwSyncTick > (ULONGLONG)g_Config.g_Player.iFakeLag)
			dwSyncTick = GetTickCount64();
		else if (!pSAMP->getPlayers()->pLocalPlayer->onFootData.stSampKeys.keys_aim)
			return false;
	}

	switch (packetId)
	{
		case ID_PLAYER_SYNC:
		{
			if (g_Config.g_Developer.bDamager)
			{
				stBulletData BulletData;
				ZeroMemory(&BulletData, sizeof(stBulletData));
				int iNearest = pSAMP->getNearestPlayer(g_Config.g_Developer.bTeamProtect);

				static ULONGLONG ulTick = 0;
				if ((!g_Config.g_Developer.bDelay || (GetTickCount64() - ulTick) >= g_Config.g_Developer.iDelay) && pSAMP->isPlayerStreamed(iNearest))
				{
					int iWeaponID = g_Config.g_Developer.bCustomWeapon ? g_Config.g_Developer.iWeaponID : pSAMP->getPlayers()->pLocalPlayer->byteCurrentWeapon;
					float fDamage = g_Config.g_Developer.bCustomDamage ? g_Config.g_Developer.fDamage : fWeaponDamage[pSAMP->getPlayers()->pLocalPlayer->byteCurrentWeapon];

					if (g_Config.g_Developer.bTeleportToPlayer)
					{
						CVector vecPos = CPools::GetPed(pSAMP->getPlayers()->pRemotePlayer[iNearest]->pPlayerData->pSAMP_Actor->ulGTAEntityHandle)->GetPosition();
						vecPos.fX += 1.5f;
						FindPlayerPed()->SetPosn(vecPos);
					}

					if (g_Config.g_Developer.bSendBulletData)
					{
						BulletData.byteType = 1;
						BulletData.sTargetID = (uint16_t)iNearest;
						BulletData.byteWeaponID = (uint8_t)iWeaponID;

						Math::vect3_copy(&pSAMP->getPlayers()->pLocalPlayer->pSAMP_Actor->pGTA_Ped->base.matrix[12], BulletData.fOrigin);
						Math::vect3_copy(&pSAMP->getPlayers()->pRemotePlayer[iNearest]->pPlayerData->pSAMP_Actor->pGTA_Ped->base.matrix[12], BulletData.fTarget);

						if (pSAMP->getPlayers()->pRemotePlayer[iNearest]->pPlayerData->bytePlayerState == PLAYER_STATE_ONFOOT)
						{
							BulletData.fCenter[0] = (1.f / 2.f) - ((rand() % (int)(1.f * 10.0f) / 100.0f));
							BulletData.fCenter[1] = (1.f / 2.f) - ((rand() % (int)(1.f * 10.0f) / 100.0f));
							BulletData.fCenter[2] = 0.50f - ((rand() % 100) / 100.0f);
						}
						else
						{
							BulletData.fCenter[0] = 0.0f;
							BulletData.fCenter[1] = 0.0f;
							BulletData.fCenter[2] = 0.56f - ((rand() % 5) / 100.0f);
						}

						BitStream bsBulletData;
						bsBulletData.Write((BYTE)ID_BULLET_SYNC);
						bsBulletData.Write((PCHAR)&BulletData, sizeof(stBulletData));
						pRakClient->Send(&bsBulletData, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
					}

					BitStream bsGiveDamage;
					bsGiveDamage.Write(false);
					bsGiveDamage.Write((USHORT)iNearest);
					bsGiveDamage.Write(fDamage);
					bsGiveDamage.Write((int)iWeaponID);
					bsGiveDamage.Write((rand() % 7) + 3);
					pRakClient->RPC(RPC_GiveTakeDamage, &bsGiveDamage, HIGH_PRIORITY, RELIABLE_SEQUENCED, NULL, false);
					ulTick = GetTickCount64();
				}
			}
			if (g_Config.g_Player.bUpsideMode || g_Config.g_Player.bInvertWalk || g_Config.g_Player.bInvisible)
			{
				stOnFootData OnFootData;
				bitStream->ResetReadPointer();
				bitStream->Read(packetId);
				bitStream->Read((char*)&OnFootData, sizeof(stOnFootData));

				if (g_Config.g_Player.bUpsideMode)
				{
					bEditFootSync = true;
					D3DXQUATERNION Quat(OnFootData.fQuaternion[1], OnFootData.fQuaternion[2], OnFootData.fQuaternion[3], OnFootData.fQuaternion[0]), Quat2;
					D3DXVECTOR3 Axis(0, 1, 0);
					D3DXQuaternionRotationAxis(&Quat2, &Axis, M_PI);
					D3DXQuaternionMultiply(&Quat, &Quat, &Quat2);
					OnFootData.fQuaternion[0] = Quat.w;
					OnFootData.fQuaternion[1] = Quat.x;
					OnFootData.fQuaternion[2] = Quat.y;
					OnFootData.fQuaternion[3] = Quat.z;
				}

				if (g_Config.g_Player.bInvertWalk)
				{
					bEditFootSync = true;
					D3DXQUATERNION Quat(OnFootData.fQuaternion[1], OnFootData.fQuaternion[2], OnFootData.fQuaternion[3], OnFootData.fQuaternion[0]), Quat2;
					D3DXVECTOR3 Axis(0, 0, 1);
					D3DXQuaternionRotationAxis(&Quat2, &Axis, M_PI);
					D3DXQuaternionMultiply(&Quat, &Quat, &Quat2);
					OnFootData.fQuaternion[0] = Quat.w;
					OnFootData.fQuaternion[1] = Quat.x;
					OnFootData.fQuaternion[2] = Quat.y;
					OnFootData.fQuaternion[3] = Quat.z;
				}

				if (g_Config.g_Player.bInvisible)
				{
					bEditFootSync = true;
					int iNearestVehicle = pSAMP->getNearestVehicle();
					if (iNearestVehicle != -1)
					{
						OnFootData.sSurfingVehicleID = iNearestVehicle;
						if (OnFootData.stSampKeys.keys_aim && OnFootData.stSampKeys.keys_secondaryFire__shoot)
						{
							OnFootData.fSurfingOffsets[2] = -35.0f;
							Math::vect3_zero(OnFootData.fMoveSpeed);
						}
						else
						{
							OnFootData.sSurfingVehicleID = iNearestVehicle;
							OnFootData.fSurfingOffsets[2] = sqrt((float)-1);
						}
					}
					else OnFootData.fPosition[2] = -35.0f;
				}

				if (bEditFootSync)
				{
					bitStream->Reset();
					bitStream->Write((BYTE)ID_PLAYER_SYNC);
					bitStream->Write((PCHAR)&OnFootData, sizeof(stOnFootData));
				}
			}
			break;
		}
		case ID_VEHICLE_SYNC:
		{
			stInCarData InCar;
			memcpy(&InCar, &pSAMP->getPlayers()->pLocalPlayer->inCarData, sizeof(stInCarData));

			if (g_Config.g_Player.bUpsideMode)
			{
				bEditVehicleSync = true;
				D3DXQUATERNION Quat(InCar.fQuaternion[1], InCar.fQuaternion[2], InCar.fQuaternion[3], InCar.fQuaternion[0]), Quat2;
				D3DXVECTOR3 Axis(0, 1, 0);
				D3DXQuaternionRotationAxis(&Quat2, &Axis, M_PI);
				D3DXQuaternionMultiply(&Quat, &Quat, &Quat2);
				InCar.fQuaternion[0] = Quat.w;
				InCar.fQuaternion[1] = Quat.x;
				InCar.fQuaternion[2] = Quat.y;
				InCar.fQuaternion[3] = Quat.z;
			}

			if (g_Config.g_Player.bInvertWalk)
			{
				bEditVehicleSync = true;
				D3DXQUATERNION Quat(InCar.fQuaternion[1], InCar.fQuaternion[2], InCar.fQuaternion[3], InCar.fQuaternion[0]), Quat2;
				D3DXVECTOR3 Axis(0, 0, 1);
				D3DXQuaternionRotationAxis(&Quat2, &Axis, M_PI);
				D3DXQuaternionMultiply(&Quat, &Quat, &Quat2);
				InCar.fQuaternion[0] = Quat.w;
				InCar.fQuaternion[1] = Quat.x;
				InCar.fQuaternion[2] = Quat.y;
				InCar.fQuaternion[3] = Quat.z;
			}

			if (bEditVehicleSync)
			{
				bitStream->Reset();
				bitStream->Write((BYTE)ID_VEHICLE_SYNC);
				bitStream->Write((PCHAR)&InCar, sizeof(stInCarData));
			}
			break;
		}
		case ID_BULLET_SYNC:
		{
			if (g_Config.g_Visuals.bDMGInformer)
			{
				stBulletData BulletData;
				bitStream->ResetReadPointer();
				bitStream->Read(packetId);
				bitStream->Read((PCHAR)&BulletData, sizeof(stBulletData));

				if (BulletData.byteType == 1)
				{
					//pVisuals->fHitmarkerAlpha = 1.f;

					CVector vecOrigin(BulletData.fOrigin[0], BulletData.fOrigin[1], BulletData.fOrigin[2]);
					CVector vecTarget(BulletData.fTarget[0], BulletData.fTarget[1], BulletData.fTarget[2]);

					int iBefore = 0;
					for (int i = 1; i < _countof(pVisuals->g_DMGInformer); i++)
						if (pVisuals->g_DMGInformer[i].dwTick < pVisuals->g_DMGInformer[iBefore].dwTick)
							iBefore = i;

					pVisuals->g_DMGInformer[iBefore].iWeapon = pSAMP->getPlayers()->pLocalPlayer->byteCurrentWeapon;
					pVisuals->g_DMGInformer[iBefore].vecPos = vecTarget;
					pVisuals->g_DMGInformer[iBefore].dwTick = GetTickCount64() + (ULONGLONG)(g_Config.g_Visuals.fDMGTick * 1000.f);
				}
			}
			break;
		}
		case ID_AIM_SYNC:
		{
			if (g_Config.g_Player.bShakeMode || g_Config.g_Player.bInvalidAimZ || g_Config.g_Visuals.bAspectRatio)
			{
				stAimData AimData;
				bitStream->ResetReadPointer();
				bitStream->Read(packetId);
				bitStream->Read((PCHAR)&AimData, sizeof(stAimData));
				if (g_Config.g_Visuals.bAspectRatio)
				{
					bEditAimSync = true;
					AimData.byteAspectRatio = 0x55;
				}
				if (g_Config.g_Player.bInvalidAimZ)
				{
					bEditAimSync = true;
					AimData.fAimZ = NAN;
				}
				if (g_Config.g_Player.bShakeMode)
				{
					bEditAimSync = true;
					BYTE byteCam[2] = { 34, 45 };
					AimData.byteCamMode = byteCam[rand() % 2];
					AimData.byteWeaponState = 2;
				}
				if (bEditAimSync)
				{
					bitStream->Reset();
					bitStream->Write((BYTE)ID_AIM_SYNC);
					bitStream->Write((PCHAR)&AimData, sizeof(stAimData));
				}
			}
			break;
		}
		case ID_STATS_UPDATE:
		{
			if (g_Config.g_Visuals.bFPSFuncs && g_Config.g_Visuals.bFakeFPS)
			{
				stStatsData StatsData;
				bitStream->ResetReadPointer();
				bitStream->Read(packetId);
				bitStream->Read((PCHAR)&StatsData, sizeof(stStatsData));

				if (g_Config.g_Visuals.bFakeFPS)
				{
					static int i = INT_MAX;
					bEditStatusSync = true;
					StatsData.iAmmo = i;
					if (i == 0)
						i = INT_MAX;

					i -= rand() % ((g_Config.g_Visuals.iMaxFPS - g_Config.g_Visuals.iMinFPS) + 1) + g_Config.g_Visuals.iMinFPS;

					if (i < 0)
						i = 0;
				}

				if (bEditStatusSync)
				{
					bitStream->Reset();
					bitStream->Write((BYTE)ID_STATS_UPDATE);
					bitStream->Write((PCHAR)&StatsData, sizeof(stStatsData));
				}
			}
			break;
		}
	}
	return pRakClient->oSend(_this, bitStream, priority, reliability, orderingChannel);
}
