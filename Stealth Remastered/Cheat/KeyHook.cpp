#include "main.h"

CKeyHook* pKeyHook;

BYTE __stdcall CKeyHook::hkCPad_UpdateGameKey(int iKey)
{
	int16_t* key_state = (int16_t*)0xB73458;
	for (int i = 0; i < 0x20; i++)
	{
		if (pKeyHook->gameKeyState[i])
		{
			key_state[i] = pKeyHook->gameKeyState[i];
			pKeyHook->gameKeyState[i] = 0;
		}
	}
	return pKeyHook->oCPad_UpdateGameKey((CPad*)0xB7358C, iKey);
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND wnd, UINT umsg, WPARAM wparam, LPARAM lparam);
LRESULT APIENTRY CKeyHook::hkWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (Utils::isGTAMenuActive())
		return pKeyHook->oWndProc(hWnd, msg, wParam, lParam);

	switch (msg)
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
		pKeyHook->bKeyTable[VK_LBUTTON] = (msg == WM_LBUTTONDOWN);
		break;

	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
		pKeyHook->bKeyTable[VK_RBUTTON] = (msg == WM_RBUTTONDOWN);
		break;

	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
		pKeyHook->bKeyTable[VK_MBUTTON] = (msg == WM_MBUTTONDOWN);
		break;

	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
	case WM_KEYDOWN:
	case WM_KEYUP:
	{
		bool bDown = (msg == WM_SYSKEYDOWN || msg == WM_KEYDOWN);
		int	iKey = (int)wParam;
		uint32_t ScanCode = LOBYTE(HIWORD(lParam));

		pKeyHook->bKeyTable[iKey] = bDown;

		switch (iKey)
		{
		case VK_SHIFT:
			if (ScanCode == MapVirtualKey(VK_LSHIFT, 0)) pKeyHook->bKeyTable[VK_LSHIFT] = bDown;
			if (ScanCode == MapVirtualKey(VK_RSHIFT, 0)) pKeyHook->bKeyTable[VK_RSHIFT] = bDown;
			break;

		case VK_CONTROL:
			if (ScanCode == MapVirtualKey(VK_LCONTROL, 0)) pKeyHook->bKeyTable[VK_LCONTROL] = bDown;
			if (ScanCode == MapVirtualKey(VK_RCONTROL, 0)) pKeyHook->bKeyTable[VK_RCONTROL] = bDown;
			break;

		case VK_MENU:
			if (ScanCode == MapVirtualKey(VK_LMENU, 0)) pKeyHook->bKeyTable[VK_LMENU] = bDown;
			if (ScanCode == MapVirtualKey(VK_RMENU, 0)) pKeyHook->bKeyTable[VK_RMENU] = bDown;
			break;
		}
		break;
	}
	}

	if (pMenu->bOpen)
		ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);

	if (isKeyPressed(VK_INSERT))
	{
		pMenu->bOpen = !pMenu->bOpen;
		pMenu->iToggle = 0;
	}

	if (!Utils::isGTAMenuActive() && !pSAMP->getInput()->iInputEnabled)
	{
		Combat::WeaponSwitcher();
		Player::BikeSpam();

		if (g_Config.g_Player.bAirBreak || g_Config.g_Player.bMapRun)
			if (wParam == 'W' || wParam == 'A' || wParam == 'S' || wParam == 'D')
				return pKeyHook->oWndProc(hWnd, WM_KEYUP, wParam, lParam);
	}

	return pKeyHook->oWndProc(hWnd, msg, wParam, lParam);
}

bool isKeyDown(uint8_t iKey)
{
	return pKeyHook->bKeyTable[iKey];
}

bool isKeyReleased(uint8_t iKey)
{
	static bool bPressed[0xFF];
	if (!pKeyHook->bKeyTable[iKey]) {
		if (bPressed[iKey])
			return !(bPressed[iKey] = false);
	}
	else bPressed[iKey] = true;

	return false;
}

bool isKeyPressed(uint8_t iKey)
{
	static bool bPressed[0xFF];
	if (pKeyHook->bKeyTable[iKey]) {
		if (!bPressed[iKey])
			return bPressed[iKey] = true;
	}
	else bPressed[iKey] = false;

	return false;
}