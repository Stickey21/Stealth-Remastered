#pragma once

class CVisuals
{
public:
	CVisuals()
	{
		fHitmarkerAlpha = 0.f;
		oCalculateAspectRatio = (tCalculateAspectRatio)0x6FF420;
		DetourRestoreAfterWith();
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		pSecure->SDetourAttach(&(PVOID&)oCalculateAspectRatio, hkCalculateAspectRatio);
		DetourTransactionCommit();
	}

	~CVisuals()
	{
		DetourRestoreAfterWith();
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourDetach(&(PVOID&)oCalculateAspectRatio, hkCalculateAspectRatio);
		DetourTransactionCommit();
	}

	void Render();
	void Update();

	bool isPlayerVisible(int iPlayerID, bool bDistance);
	ImColor getESPColor(int iPlayerID);

	void ESP_NameTags(int iPlayerID, CPed* pPed);
	void ESP_Snipeline(int iPlayerID, CPed* pPed);
	void ESP_Box(int iPlayerID, CPed* pPed);
	void ESP_Bones(int iPlayerID, CPed* pPed);
	void ESP_Chams(int iPlayerID, CPed* pPed);
	void ESP_Distance(int iPlayerID, CPed* pPed);

	void ESP_Vehicle();

	void NoDrugsEffect();
	void WeatherChanger();
	void FPSFuncs();

	struct stDMGInformer
	{
		CVector vecPos = CVector(0, 0, 0);
		int iWeapon = 0;
		ULONGLONG dwTick = 0;
	} g_DMGInformer[10];

	float fHitmarkerAlpha;

private:
	typedef float(__cdecl* tCalculateAspectRatio)();
	static float __cdecl hkCalculateAspectRatio();
	tCalculateAspectRatio oCalculateAspectRatio;
};

extern CVisuals* pVisuals;
