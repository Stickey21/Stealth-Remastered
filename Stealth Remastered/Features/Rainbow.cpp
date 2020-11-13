#include "main.h"

CRainbow* pRainbow;

void CRainbow::Update()
{
	static float fRainbow;
	fRainbow += 0.005f;
	if (fRainbow > 1.f) fRainbow = 0.f;
	colorRainbow = Color::FromHSB(fRainbow, 1.f, 1.f);
}

void __cdecl CRainbow::hkFontSetColor(CRGBA color)
{
	color.r = pRainbow->colorRainbow.b;
	color.g = pRainbow->colorRainbow.g;
	color.b = pRainbow->colorRainbow.r;

	for (auto i = 0; i < 16; i++)
		*reinterpret_cast<DWORD*>(0xBAB22C + (sizeof(DWORD) * i)) = Color::getABGR(pRainbow->colorRainbow);

	return pRainbow->oFontSetColor(color);
}

void __fastcall CRainbow::hkFxSystemRender(FxSystemBP_c* _this, void* pUnknown, RwCamera* pCamera, float fDayNightBalance, bool bHeatHaze)
{
	if (!bHeatHaze)
	{
		DWORD dwConstant, dwARG1, dwOP;
		IDirect3DDevice9* pDevice = reinterpret_cast<IDirect3DDevice9*>(RwD3D9GetCurrentD3DDevice());

		pDevice->GetTextureStageState(0, D3DTSS_CONSTANT, &dwConstant);
		pDevice->GetTextureStageState(0, D3DTSS_COLORARG1, &dwARG1);
		pDevice->GetTextureStageState(0, D3DTSS_COLOROP, &dwOP);

		pDevice->SetTextureStageState(0, D3DTSS_CONSTANT, Color::getARGB(pRainbow->colorRainbow));
		pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_CONSTANT);
		pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);

		pRainbow->oFxSystemRender(_this, pCamera, fDayNightBalance, bHeatHaze);

		pDevice->SetTextureStageState(0, D3DTSS_CONSTANT, dwConstant);
		pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, dwARG1);
		pDevice->SetTextureStageState(0, D3DTSS_COLOROP, dwOP);

		return;
	}

	return pRainbow->oFxSystemRender(_this, pCamera, fDayNightBalance, bHeatHaze);
}

void __fastcall CRainbow::hkCSprite2dDraw(CSprite2d* pSprite, void* pUnknown, CRect* pRect, CRGBA* pColor)
{
	if (!strcmp(pSprite->m_pTexture->name, "radardisc"))
	{
		DWORD dwConstant, dwARG1, dwOP;
		IDirect3DDevice9* pDevice = reinterpret_cast<IDirect3DDevice9*>(RwD3D9GetCurrentD3DDevice());

		pDevice->GetTextureStageState(1, D3DTSS_CONSTANT, &dwConstant);
		pDevice->GetTextureStageState(1, D3DTSS_COLORARG1, &dwARG1);
		pDevice->GetTextureStageState(1, D3DTSS_COLOROP, &dwOP);

		pDevice->SetTextureStageState(1, D3DTSS_CONSTANT, Color::getARGB(pRainbow->colorRainbow));
		pDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CONSTANT);
		pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_SELECTARG1);

		pRainbow->oCSprite2dDraw(pSprite, pRect, pColor);

		pDevice->SetTextureStageState(1, D3DTSS_CONSTANT, dwConstant);
		pDevice->SetTextureStageState(1, D3DTSS_COLORARG1, dwARG1);
		pDevice->SetTextureStageState(1, D3DTSS_COLOROP, dwOP);

		return;
	}

	return pRainbow->oCSprite2dDraw(pSprite, pRect, pColor);
}

HRESULT __cdecl CRainbow::hkD3D9VertexShaderRender(RxD3D9ResEntryHeader* resEntry, RxD3D9InstanceData* instData)
{
	
	return pRainbow->oD3D9VertexShaderRender(resEntry, instData);
}