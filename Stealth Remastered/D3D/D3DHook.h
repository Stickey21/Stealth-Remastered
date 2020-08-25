#pragma once

class CD3DHook
{
public:
	CD3DHook()
	{
		bInit = false;
		oPresent = (tPresent)GetDeviceAddress(17);
		oReset = (tReset)GetDeviceAddress(16);
		DetourRestoreAfterWith();
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		pSecure->SDetourAttach(&(PVOID&)oPresent, hkPresent);
		pSecure->SDetourAttach(&(PVOID&)oReset, hkReset);
		DetourTransactionCommit();
	};

	~CD3DHook()
	{
		DetourRestoreAfterWith();
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourDetach(&(PVOID&)oPresent, hkPresent);
		DetourDetach(&(PVOID&)oReset, hkReset);
		DetourTransactionCommit();
	};

	bool bInit;
	IDirect3DDevice9* pD3DDevice;

private:
	DWORD FindDevice(DWORD dwLen)
	{
		DWORD dwObjBase = 0;
		char infoBuf[MAX_PATH];
		GetSystemDirectoryA(infoBuf, MAX_PATH);
		strcat_s(infoBuf, MAX_PATH, "\\d3d9.dll");
		dwObjBase = (DWORD)LoadLibraryA(infoBuf);
		while (dwObjBase++ < dwObjBase + dwLen)
		{
			if ((*(WORD*) (dwObjBase + 0x00)) == 0x06C7 &&
				(*(WORD*) (dwObjBase + 0x06)) == 0x8689 &&
				(*(WORD*) (dwObjBase + 0x0C)) == 0x8689)
			{
				dwObjBase += 2;
				break;
			}
		}
		return(dwObjBase);
	};

	DWORD GetDeviceAddress(int VTableIndex)
	{
		PDWORD VTable;
		*(DWORD*) &VTable = *(DWORD*)FindDevice(0x128000);
		return VTable[VTableIndex];
	};

	typedef HRESULT(__stdcall* tPresent)(IDirect3DDevice9*, CONST RECT*, CONST RECT*, HWND, CONST RGNDATA*);
	typedef HRESULT(__stdcall* tReset)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);

	static HRESULT __stdcall hkPresent(IDirect3DDevice9* pDevice, CONST RECT* pSrcRect, CONST RECT* pDestRect, HWND hDestWindow, CONST RGNDATA* pDirtyRegion);
	static HRESULT __stdcall hkReset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentParams);

	tPresent oPresent;
	tReset oReset;
};

extern CD3DHook* pD3DHook;