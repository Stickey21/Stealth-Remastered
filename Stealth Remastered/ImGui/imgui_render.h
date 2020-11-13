#pragma once

class CRender
{
public:
	CRender()
	{
		pESPFont = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\arialbd.ttf", 20.f);
	};

	ImFont* pESPFont;

	float DrawString(ImFont* pFont, const std::string& Text, CVector vecPos, float fSize, ImColor Color, bool bCenter);
	void DrawLine(CVector vecFrom, CVector vecTo, ImColor Color, float fThickness);
	void D3DLine(CVector vecFrom, CVector vecTo, ImColor Color, float fThickness);
	void DrawCircle(CVector vecPos, float fRadius, ImColor Color, float fThickness);
	void DrawCircleFilled(CVector vecPos, float fRadius, ImColor Color);
	void DrawRect(CVector vecFrom, CVector vecTo, ImColor Color, float fThickness);
	void DrawRectFilled(CVector vecFrom, CVector vecTo, ImColor Color);
	void DrawBoundingBox(CVector vecCorner[8], ImColor Color, float fThickness, bool bFilled = false, ImColor Filled = ImColor(0, 0, 0, 0));
	void DrawImageRotated(ImTextureID ImTex, ImVec2 vecCenter, ImVec2 vecSize, float fAngle);
};

extern CRender* pRender;
