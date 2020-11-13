#pragma once

struct Color
{
	int r, g, b, a;

	Color()
	{
		this->r = 0;
		this->g = 0;
		this->b = 0;
		this->a = 255;
	}

	Color(int r, int g, int b)
	{
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = 255;
	}

	Color(int r, int g, int b, int a)
	{
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
	}

	static Color FromHSB(float hue, float saturation, float brightness)
	{
		float h = hue == 1.0f ? 0 : hue * 6.0f;
		float f = h - (int)h;
		float p = brightness * (1.0f - saturation);
		float q = brightness * (1.0f - saturation * f);
		float t = brightness * (1.0f - (saturation * (1.0f - f)));

		if (h < 1) {
			return Color((BYTE)(brightness * 255), (BYTE)(t * 255), (BYTE)(p * 255));
		}
		else if (h < 2) {
			return Color((BYTE)(q * 255), (BYTE)(brightness * 255), (BYTE)(p * 255));
		}
		else if (h < 3) {
			return Color((BYTE)(p * 255), (BYTE)(brightness * 255), (BYTE)(t * 255));
		}
		else if (h < 4) {
			return Color((BYTE)(p * 255), (BYTE)(q * 255), (BYTE)(brightness * 255));
		}
		else if (h < 5) {
			return Color((BYTE)(t * 255), (BYTE)(p * 255), (BYTE)(brightness * 255));
		}
		else return Color((BYTE)(brightness * 255), (BYTE)(p * 255), (BYTE)(q * 255));
	}

	static ImColor getARGB(Color color)
	{
		return ImColor(color.r / 255.f, color.g / 255.f, color.b / 255.f, color.a / 255.f);
	}

	static ImColor getABGR(Color color)
	{
		return ImColor(color.b / 255.f, color.g / 255.f, color.r / 255.f, color.a / 255.f);
	}
};

class CRainbow
{
public:
	CRainbow()
	{
		oFontSetColor = (tFontSetColor)0x719430;
		oFxSystemRender = (tFxSystemRender)0x4AA160;
		oCSprite2dDraw = (tCSprite2dDraw)0x728350;
		oD3D9VertexShaderRender = (tD3D9VertexShaderRender)0x761030;
		DetourRestoreAfterWith();
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		pSecure->SDetourAttach(&(PVOID&)oFontSetColor, hkFontSetColor);
		pSecure->SDetourAttach(&(PVOID&)oFxSystemRender, hkFxSystemRender);
		pSecure->SDetourAttach(&(PVOID&)oCSprite2dDraw, hkCSprite2dDraw);
		pSecure->SDetourAttach(&(PVOID&)oD3D9VertexShaderRender, hkD3D9VertexShaderRender);
		DetourTransactionCommit();
	};

	void Update();

	Color colorRainbow;

private:
	typedef void(__cdecl* tFontSetColor)(CRGBA);
	typedef void(__thiscall* tFxSystemRender)(FxSystemBP_c*, RwCamera*, float, bool);
	typedef void(__thiscall* tCSprite2dDraw)(CSprite2d*, CRect*, CRGBA*);
	typedef HRESULT(__cdecl* tD3D9VertexShaderRender)(RxD3D9ResEntryHeader*, RxD3D9InstanceData*);

	static void __cdecl hkFontSetColor(CRGBA color);
	static void __fastcall hkFxSystemRender(FxSystemBP_c* _this, void* pUnknown, RwCamera* pCamera, float fDayNightBalance, bool bHeatHaze);
	static void __fastcall hkCSprite2dDraw(CSprite2d* pSprite, void* pUnknown, CRect* pRect, CRGBA* pColor);
	static HRESULT __cdecl hkD3D9VertexShaderRender(RxD3D9ResEntryHeader* resEntry, RxD3D9InstanceData* instData);

	tFontSetColor oFontSetColor;
	tFxSystemRender oFxSystemRender;
	tCSprite2dDraw oCSprite2dDraw;
	tD3D9VertexShaderRender oD3D9VertexShaderRender;
};

extern CRainbow* pRainbow;