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

		Orginal_FireInstantHit = (Prototype_FireInstantHit)(pSAMP->g_dwSAMP_Addr + 0xB05A0);
		Orginal_AddBullet = (Prototype_AddBullet)(pSAMP->g_dwSAMP_Addr + 0xA0BB0);
		Orginal_TargetWeaponRangeMultiplier = (Prototype_TargetWeaponRangeMultiplier)0x73B380;
		DetourRestoreAfterWith();
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		pSecure->SDetourAttach(&(PVOID&)Orginal_FireInstantHit, Hooked_FireInstantHit);
		pSecure->SDetourAttach(&(PVOID&)Orginal_AddBullet, Hooked_AddBullet);
		DetourTransactionCommit();
		pSecure->HookInstallCall((DWORD)0x73FF11, (DWORD)Hooked_TargetWeaponRangeMultiplier);
	};

	~CAimbot()
	{
		DetourRestoreAfterWith();
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourDetach(&(PVOID&)Orginal_FireInstantHit, Hooked_FireInstantHit);
		DetourDetach(&(PVOID&)Orginal_AddBullet, Hooked_AddBullet);
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

	typedef bool(__stdcall* Prototype_FireInstantHit)(void*, CEntity*, CVector*, CVector*, CEntity*, CVector*, CVector*, bool, bool);
	typedef bool(__cdecl* Prototype_AddBullet)(CEntity*, eWeaponType, CVector, CVector);
	typedef float(__cdecl* Prototype_TargetWeaponRangeMultiplier)(CEntity*, CEntity*);

	static bool __stdcall Hooked_FireInstantHit(void* this_, CEntity* pFiringEntity, CVector* pOrigin, CVector* pMuzzle, CEntity* pTargetEntity, CVector* pTarget, CVector* pVec, bool bCrossHairGun, bool bCreateGunFx);
	static bool __cdecl Hooked_AddBullet(CEntity* pCreator, eWeaponType weaponType, CVector vecPosition, CVector vecVelocity);
	static float __cdecl Hooked_TargetWeaponRangeMultiplier(CEntity* pVictim, CEntity* pOwner);

	Prototype_FireInstantHit Orginal_FireInstantHit;
	Prototype_AddBullet Orginal_AddBullet;
	Prototype_TargetWeaponRangeMultiplier Orginal_TargetWeaponRangeMultiplier;
};

extern CAimbot* pAimbot;