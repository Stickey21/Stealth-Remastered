#pragma once

class CD3DHook
{
public:
	CD3DHook()
	{
		bInit = false;
		Orginal_Present = (Prototype_Present)GetDeviceAddress(17);
		Orginal_Reset = (Prototype_Reset)GetDeviceAddress(16);
		DetourRestoreAfterWith();
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		pSecure->SDetourAttach(&(PVOID&)Orginal_Present, Hooked_Present);
		pSecure->SDetourAttach(&(PVOID&)Orginal_Reset, Hooked_Reset);
		DetourTransactionCommit();
	};

	~CD3DHook()
	{
		DetourRestoreAfterWith();
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourDetach(&(PVOID&)Orginal_Present, Hooked_Present);
		DetourDetach(&(PVOID&)Orginal_Reset, Hooked_Reset);
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

	typedef HRESULT(__stdcall* Prototype_Present)(IDirect3DDevice9*, CONST RECT*, CONST RECT*, HWND, CONST RGNDATA*);
	typedef HRESULT(__stdcall* Prototype_Reset)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);

	static HRESULT __stdcall Hooked_Present(IDirect3DDevice9* pDevice, CONST RECT* pSrcRect, CONST RECT* pDestRect, HWND hDestWindow, CONST RGNDATA* pDirtyRegion);
	static HRESULT __stdcall Hooked_Reset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentParams);

	Prototype_Present Orginal_Present;
	Prototype_Reset Orginal_Reset;
};

extern CD3DHook* pD3DHook;