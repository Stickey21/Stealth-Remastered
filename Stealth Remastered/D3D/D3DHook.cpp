#include "main.h"

CD3DHook* pD3DHook;

HRESULT __stdcall CD3DHook::Hooked_Present(IDirect3DDevice9 *pDevice, CONST RECT *pSrcRect, CONST RECT *pDestRect, HWND hDestWindow, CONST RGNDATA *pDirtyRegion)
{
	if (!pDevice)
		return pD3DHook->Orginal_Present(pDevice, pSrcRect, pDestRect, hDestWindow, pDirtyRegion);

	if (pSAMP->isInited)
	{
		if (!pD3DHook->bInit)
		{
			pKeyHook = new CKeyHook();
			pTextures = new CTextures(pDevice);
			pMenu = new CMenu(pDevice);
			pRender = new CRender();
			pD3DHook->bInit = true;
		}

		pD3DHook->pD3DDevice = pDevice;
		Cheat::Update();
		Cheat::Render();	
	}

	return pD3DHook->Orginal_Present(pDevice, pSrcRect, pDestRect, hDestWindow, pDirtyRegion);
}

HRESULT __stdcall CD3DHook::Hooked_Reset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentParams)
{
	if (!pDevice)
		return pD3DHook->Orginal_Reset(pDevice, pPresentParams);

	if (pD3DHook->bInit)
		ImGui_ImplDX9_InvalidateDeviceObjects();

	return pD3DHook->Orginal_Reset(pDevice, pPresentParams);
}