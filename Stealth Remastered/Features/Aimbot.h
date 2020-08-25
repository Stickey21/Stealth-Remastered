#pragma once

class CAimbot
{
public:
	CAimbot()
	{
		iTargetBone = 1;
		iTargetPlayer = -1;
		bCrosshair = false;
		vecTargetBone = { 0.f, 0.f, 0.f };
		vecCrosshair = { 0.f, 0.f, 0.f };

		oFireInstantHit = (tFireInstantHit)(pSAMP->g_dwSAMP_Addr + 0xB05A0);
		oAddBullet = (tAddBullet)(pSAMP->g_dwSAMP_Addr + 0xA0BB0);
		oTargetWeaponRangeMultiplier = (tTargetWeaponRangeMultiplier)0x73B380;
		DetourRestoreAfterWith();
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		pSecure->SDetourAttach(&(PVOID&)oFireInstantHit, hkFireInstantHit);
		pSecure->SDetourAttach(&(PVOID&)oAddBullet, hkAddBullet);
		DetourTransactionCommit();
		pSecure->HookInstallCall((DWORD)0x73FF11, (DWORD)hkTargetWeaponRangeMultiplier);
	};

	~CAimbot()
	{
		DetourRestoreAfterWith();
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourDetach(&(PVOID&)oFireInstantHit, hkFireInstantHit);
		DetourDetach(&(PVOID&)oAddBullet, hkAddBullet);
		DetourTransactionCommit();
		Memory::memcpy_safe((void*)0x73FF11, "\xE8\x6A\xB4\xFF\xFF", 5);
	};

	void Update();
	void Render();

	CVector vecCrosshair;

private:
	bool bCrosshair;
	int iTargetPlayer, iTargetBone;
	CVector vecTargetBone;

	void GetAimingPlayer();
	void SmoothAimbot();
	void ProAimbot();
	void Triggerbot();

	typedef bool(__stdcall* tFireInstantHit)(void*, CEntity*, CVector*, CVector*, CEntity*, CVector*, CVector*, bool, bool);
	typedef bool(__cdecl* tAddBullet)(CEntity*, eWeaponType, CVector, CVector);
	typedef float(__cdecl* tTargetWeaponRangeMultiplier)(CEntity*, CEntity*);

	static bool __stdcall hkFireInstantHit(void* this_, CEntity* pFiringEntity, CVector* pOrigin, CVector* pMuzzle, CEntity* pTargetEntity, CVector* pTarget, CVector* pVec, bool bCrossHairGun, bool bCreateGunFx);
	static bool __cdecl hkAddBullet(CEntity* pCreator, eWeaponType weaponType, CVector vecPosition, CVector vecVelocity);
	static float __cdecl hkTargetWeaponRangeMultiplier(CEntity* pVictim, CEntity* pOwner);

	tFireInstantHit oFireInstantHit;
	tAddBullet oAddBullet;
	tTargetWeaponRangeMultiplier oTargetWeaponRangeMultiplier;
};

extern CAimbot* pAimbot;