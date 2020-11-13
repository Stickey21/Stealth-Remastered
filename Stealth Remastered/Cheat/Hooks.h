#pragma once

class CHooks
{
public:
	CHooks()
	{
		pCurrentPed = NULL;
		oPed_Render = (tCPed_Render)0x534310;
		DetourRestoreAfterWith();
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		pSecure->SDetourAttach(&(PVOID&)oPed_Render, hkCPed_Render);
		DetourTransactionCommit();
	}

	~CHooks()
	{
		DetourRestoreAfterWith();
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourDetach(&(PVOID&)oPed_Render, hkCPed_Render);
		DetourTransactionCommit();
	}

	CPed* pCurrentPed;

private:
	typedef void(__thiscall* tCPed_Render)(CPed* pPed);
	static void __fastcall hkCPed_Render(CPed* pPed, void* pUnknown);
	tCPed_Render oPed_Render;
};

extern CHooks* pHooks;