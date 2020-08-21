#pragma once

class CKeyHook
{
public:
	CKeyHook::CKeyHook()
	{
		memset(bKeyTable, false, sizeof(bKeyTable));
		memset(gameKeyState, 0, sizeof(gameKeyState));
		Orginal_WndProc = (Prototype_WndProc)(pSAMP->g_dwSAMP_Addr + 0x5DB40);
		Orginal_CPad_UpdateGameKey = (Prototype_CPad_UpdateGameKey)0x541C40;
		DetourRestoreAfterWith();
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		pSecure->SDetourAttach(&(PVOID&)Orginal_WndProc, Hooked_WndProc);
		DetourTransactionCommit();
		pSecure->HookInstallCall((DWORD)0x541E17, (DWORD)Hoooked_CPad_UpdateGameKey);
	}

	CKeyHook::~CKeyHook()
	{
		memset(bKeyTable, false, sizeof(bKeyTable));
		memset(gameKeyState, 0, sizeof(gameKeyState));
		DetourRestoreAfterWith();
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourDetach(&(PVOID&)Orginal_WndProc, Hooked_WndProc);
		DetourTransactionCommit();
		Memory::memcpy_safe((void*)0x541E17, "\xE8\x24\xFE\xFF\xFF", 5);
	}

	bool bKeyTable[256];
	int16_t	gameKeyState[32];

private:

	typedef LRESULT(__stdcall* Prototype_WndProc)(HWND, UINT, WPARAM, LPARAM);
	typedef BYTE(__thiscall* Prototype_CPad_UpdateGameKey)(CPad*, int);

	static LRESULT __stdcall Hooked_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static BYTE __stdcall Hoooked_CPad_UpdateGameKey(int iKey);

	Prototype_WndProc Orginal_WndProc;
	Prototype_CPad_UpdateGameKey Orginal_CPad_UpdateGameKey;
};

extern CKeyHook* pKeyHook;

bool isKeyDown(uint8_t key);
bool isKeyPressed(uint8_t key);
bool isKeyReleased(uint8_t vkey);