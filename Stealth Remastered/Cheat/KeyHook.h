#pragma once

class CKeyHook
{
public:
	CKeyHook::CKeyHook()
	{
		memset(bKeyTable, false, sizeof(bKeyTable));
		memset(gameKeyState, 0, sizeof(gameKeyState));
		oWndProc = (tWndProc)(pSAMP->g_dwSAMP_Addr + 0x5DB40);
		oCPad_UpdateGameKey = (tCPad_UpdateGameKey)0x541C40;
		DetourRestoreAfterWith();
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		pSecure->SDetourAttach(&(PVOID&)oWndProc, hkWndProc);
		DetourTransactionCommit();
		pSecure->HookInstallCall((DWORD)0x541E17, (DWORD)hkCPad_UpdateGameKey);
	}

	CKeyHook::~CKeyHook()
	{
		memset(bKeyTable, false, sizeof(bKeyTable));
		memset(gameKeyState, 0, sizeof(gameKeyState));
		DetourRestoreAfterWith();
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourDetach(&(PVOID&)oWndProc, hkWndProc);
		DetourTransactionCommit();
		Memory::memcpy_safe((void*)0x541E17, "\xE8\x24\xFE\xFF\xFF", 5);
	}

	bool bKeyTable[256];
	int16_t	gameKeyState[32];

private:

	typedef LRESULT(__stdcall* tWndProc)(HWND, UINT, WPARAM, LPARAM);
	typedef BYTE(__thiscall* tCPad_UpdateGameKey)(CPad*, int);

	static LRESULT __stdcall hkWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static BYTE __stdcall hkCPad_UpdateGameKey(int iKey);

	tWndProc oWndProc;
	tCPad_UpdateGameKey oCPad_UpdateGameKey;
};

extern CKeyHook* pKeyHook;

bool isKeyDown(uint8_t key);
bool isKeyPressed(uint8_t key);
bool isKeyReleased(uint8_t vkey);