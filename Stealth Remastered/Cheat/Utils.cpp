#include "main.h"

void Utils::CalcScreenCoors(CVector* vecWorld, CVector* vecScreen)
{
	D3DXMATRIX m((float*)(0xB6FA2C));

	DWORD* dwLenX = (DWORD*)(0xC17044);
	DWORD* dwLenY = (DWORD*)(0xC17048);

	vecScreen->fX = (vecWorld->fZ * m._31) + (vecWorld->fY * m._21) + (vecWorld->fX * m._11) + m._41;
	vecScreen->fY = (vecWorld->fZ * m._32) + (vecWorld->fY * m._22) + (vecWorld->fX * m._12) + m._42;
	vecScreen->fZ = (vecWorld->fZ * m._33) + (vecWorld->fY * m._23) + (vecWorld->fX * m._13) + m._43;

	double	fRecip = (double)1.0 / vecScreen->fZ;
	vecScreen->fX *= (float)(fRecip * (*dwLenX));
	vecScreen->fY *= (float)(fRecip * (*dwLenY));
}

void Utils::CalcWorldCoors(CVector* vecScreen, CVector* vecWorld)
{
	D3DXMATRIXA16 m((float*)(0xB6FA2C));

	D3DXMATRIXA16 minv;
	memset(&minv, 0, sizeof(D3DXMATRIXA16));
	m._44 = 1.0f;

	D3DXMatrixInverse(&minv, NULL, &m);

	DWORD* dwLenX = (DWORD*)(0xC17044);
	DWORD* dwLenY = (DWORD*)(0xC17048);

	float fRecip = 1.0f / vecScreen->fZ;
	vecScreen->fX /= fRecip * (*dwLenX);
	vecScreen->fY /= fRecip * (*dwLenY);

	vecWorld->fX = vecScreen->fZ * minv._31 + vecScreen->fY * minv._21 + vecScreen->fX * minv._11 + minv._41;
	vecWorld->fY = vecScreen->fZ * minv._32 + vecScreen->fY * minv._22 + vecScreen->fX * minv._12 + minv._42;
	vecWorld->fZ = vecScreen->fZ * minv._33 + vecScreen->fY * minv._23 + vecScreen->fX * minv._13 + minv._43;
}

int Utils::isGTAMenuActive()
{
	return (int)(*(uint8_t*)0x0BA67A4);
}

float Utils::getDistance(CVector vecTarget)
{
	CVector vecSelf = FindPlayerPed()->GetPosition();
	return sqrt((vecSelf.fX - vecTarget.fX) * (vecSelf.fX - vecTarget.fX) + (vecSelf.fY - vecTarget.fY) * (vecSelf.fY - vecTarget.fY) + (vecSelf.fZ - vecTarget.fZ) * (vecSelf.fZ - vecTarget.fZ));
}

float Utils::getDistanceFromCamera(CVector vecTarget)
{
	CVector vecSrc = TheCamera.m_vecGameCamPos;
	return sqrt((vecSrc.fX - vecTarget.fX) * (vecSrc.fX - vecTarget.fX) + (vecSrc.fY - vecTarget.fY) * (vecSrc.fY - vecTarget.fY) + (vecSrc.fZ - vecTarget.fZ) * (vecSrc.fZ - vecTarget.fZ));
}

void Utils::DisembarkInstantly()
{
	CTaskManager* taskManager = &FindPlayerPed()->m_pIntelligence->m_TaskMgr;

	for (int i = 0; i < TASK_PRIMARY_MAX; i++)
	{
		CTask* pTask = taskManager->m_aPrimaryTasks[i];
		if (pTask)
		{
			pTask->MakeAbortable(FindPlayerPed(), ABORT_PRIORITY_IMMEDIATE, NULL);
			if (i != TASK_PRIMARY_DEFAULT)
				taskManager->SetTask(NULL, i, 0);
		}
	}

	for (int i = 0; i < TASK_SECONDARY_MAX; i++)
	{
		CTask* pTask = taskManager->m_aSecondaryTasks[i];
		if (pTask)
		{
			pTask->MakeAbortable(FindPlayerPed(), ABORT_PRIORITY_IMMEDIATE, NULL);
			if (i != TASK_SECONDARY_FACIAL_COMPLEX)
				taskManager->SetTaskSecondary(NULL, i);
		}
	}
}

void Utils::PerformAnimation(const char* szBlockName, const char* szAnimName, int iTime, bool bLoop, bool bUpdatePosition, bool bInterruptable, bool bFreezeLastFrame, bool bRunInSequence, bool bOffsetPed, bool bHoldLastFrame)
{
	CAnimBlock* pBlock = CAnimManager::GetAnimationBlock(szBlockName);
	if (pBlock)
	{
		bool bLoaded = true;

		if (!pBlock->bLoaded)
		{
			int iTimeToWait = 50;

			CStreaming::RequestModel((((DWORD)pBlock - 0xb5d4a0) / 32) + 25575, 4);
			CStreaming::LoadAllRequestedModels(false);

			while (!pBlock->bLoaded && iTimeToWait != 0)
			{
				iTimeToWait--;
				Sleep(10);
			}

			if (iTimeToWait == 0)
				bLoaded = false;
		}

		if (bLoaded)
		{
			CTaskManager* taskManager = &FindPlayerPed()->m_pIntelligence->m_TaskMgr;
			for (int i = 0; i < TASK_PRIMARY_MAX; i++)
			{
				CTask* pTask = taskManager->m_aPrimaryTasks[i];
				if (pTask)
					pTask->MakeAbortable(FindPlayerPed(), ABORT_PRIORITY_IMMEDIATE, NULL);
			}
			for (int i = 0; i < TASK_SECONDARY_MAX; i++)
			{
				CTask* pTask = taskManager->m_aSecondaryTasks[i];
				if (pTask)
					pTask->MakeAbortable(FindPlayerPed(), ABORT_PRIORITY_IMMEDIATE, NULL);
			}

			FindPlayerPed()->m_pIntelligence->m_TaskMgr.SetTask(NULL, TASK_PRIMARY_EVENT_RESPONSE_TEMP, 0);

			CTask* jumpTask = FindPlayerPed()->m_pIntelligence->m_TaskMgr.FindActiveTaskByType(211);
			if (jumpTask)
				FindPlayerPed()->m_pIntelligence->m_TaskMgr.SetTask(NULL, TASK_PRIMARY_PRIMARY, 0);

			int flags = 0x10;  
			if (bLoop) flags |= 0x2;
			if (bUpdatePosition)
			{
				flags |= 0x40;
				flags |= 0x80;
			}
			if (!bFreezeLastFrame) flags |= 0x08;

			CTask* pTask = new CTaskSimpleRunNamedAnim(szAnimName, pBlock->szName, flags, 4.0f, iTime, !bInterruptable, bRunInSequence, bOffsetPed, bHoldLastFrame);
			if (pTask)
				FindPlayerPed()->m_pIntelligence->m_TaskMgr.SetTask(pTask, TASK_PRIMARY_PRIMARY, false);
		}
		else
		{
			CStreaming::RequestModel((((DWORD)pBlock - 0xb5d4a0) / 32) + 25575, 8);
			CStreaming::LoadAllRequestedModels(false);
		}
	}
}

bool Utils::isPlayingAnimation(CEntity* pEntity, char* szAnimName)
{
	DWORD dwReturn = 0;
	DWORD dwFunc = 0x4D6870;
	DWORD dwThis = (DWORD)pEntity->m_pRwObject;

	_asm
	{
		push    szAnimName
		push    dwThis
		call    dwFunc
		add     esp, 8
		mov     dwReturn, eax
	}

	if (dwReturn) return true;
	else return false;
}

CVector* Utils::getBonePosition(CPed* pPed, ePedBones bone, CVector* vecPosition)
{
	DWORD dwFunc = 0x5E4280;
	DWORD dwThis = (DWORD)pPed;

	_asm
	{
		push    1
		push    bone
		push    vecPosition
		mov     ecx, dwThis
		call    dwFunc
	}

	return vecPosition;
}

float Utils::vecLength(CVector vecPos)
{
	return sqrt((vecPos.fX * vecPos.fX) + (vecPos.fY * vecPos.fY) + (vecPos.fZ * vecPos.fZ));
}