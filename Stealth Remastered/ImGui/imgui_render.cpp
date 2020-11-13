#include "main.h"
#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "imgui_internal.h"

CRender* pRender;

float CRender::DrawString(ImFont* pFont, const std::string& szText, CVector vecPos, float fSize, ImColor Color, bool bCenter)
{
	float fY = 0.0f;
	ImVec2 TextSize = pFont->CalcTextSizeA(fSize, FLT_MAX, 0.0f, szText.c_str());

	if (bCenter)
		vecPos.fX -= TextSize.x / 2.0f;

	ImGui::GetBackgroundDrawList()->AddText(pFont, fSize, ImVec2((vecPos.fX) + 1, vecPos.fY + 1), ImGui::GetColorU32(ImVec4(0, 0, 0, Color.Value.w)), szText.c_str());
	ImGui::GetBackgroundDrawList()->AddText(pFont, fSize, ImVec2((vecPos.fX) - 1, vecPos.fY - 1), ImGui::GetColorU32(ImVec4(0, 0, 0, Color.Value.w)), szText.c_str());
	ImGui::GetBackgroundDrawList()->AddText(pFont, fSize, ImVec2((vecPos.fX) + 1, vecPos.fY - 1), ImGui::GetColorU32(ImVec4(0, 0, 0, Color.Value.w)), szText.c_str());
	ImGui::GetBackgroundDrawList()->AddText(pFont, fSize, ImVec2((vecPos.fX) - 1, vecPos.fY + 1), ImGui::GetColorU32(ImVec4(0, 0, 0, Color.Value.w)), szText.c_str());
	ImGui::GetBackgroundDrawList()->AddText(pFont, fSize, ImVec2(vecPos.fX, vecPos.fY), Color, szText.c_str());

	fY = vecPos.fY + TextSize.y;

	return fY;
}

void CRender::DrawLine(CVector vecFrom, CVector vecTo, ImColor Color, float fThickness)
{
	ImGui::GetBackgroundDrawList()->AddLine({ vecFrom.fX, vecFrom.fY }, { vecTo.fX, vecTo.fY }, Color, fThickness);
}

void CRender::D3DLine(CVector vecFrom, CVector vecTo, ImColor Color, float fThickness)
{
	CVector vecFromScreen, vecToScreen;
	Utils::CalcScreenCoors(&vecFrom, &vecFromScreen);
	Utils::CalcScreenCoors(&vecTo, &vecToScreen);

	if (vecFromScreen.fZ < 1.f || vecToScreen.fZ < 1.f)
		return;

	ImGui::GetBackgroundDrawList()->AddLine({ vecFromScreen.fX, vecFromScreen.fY }, { vecToScreen.fX, vecToScreen.fY }, Color, fThickness);
}

void CRender::DrawCircle(CVector vecPos, float fRadius, ImColor Color, float fThickness)
{
	ImGui::GetBackgroundDrawList()->AddCircle({ vecPos.fX, vecPos.fY }, fRadius * 1.5f, Color, 128, fThickness);
}

void CRender::DrawCircleFilled(CVector vecPos, float fRadius, ImColor Color)
{
	ImGui::GetBackgroundDrawList()->AddCircleFilled({ vecPos.fX, vecPos.fY }, fRadius * 1.5f, Color, 128);
}

void CRender::DrawRect(CVector vecFrom, CVector vecTo, ImColor Color, float fThickness)
{
	ImGui::GetBackgroundDrawList()->AddRect({ vecFrom.fX, vecFrom.fY }, { vecTo.fX, vecTo.fY }, Color, 1.f, 15, fThickness);
}

void CRender::DrawRectFilled(CVector vecFrom, CVector vecTo, ImColor Color)
{
	ImGui::GetBackgroundDrawList()->AddRectFilled({ vecFrom.fX, vecFrom.fY }, { vecTo.fX, vecTo.fY }, Color);
}

void CRender::DrawBoundingBox(CVector vecCorner[8], ImColor Color, float fThickness, bool bFilled, ImColor Filled)
{
	CVector vecCornerScreen[8];

	for (auto i = 0; i < 8; i++)
	{
		Utils::CalcScreenCoors(&vecCorner[i], &vecCornerScreen[i]);
		if (vecCornerScreen[i].fZ < 1.0f)
			return;
	}

	if (bFilled)
	{
		ImColor colorESP = ImColor(Filled.Value.x, Filled.Value.y, Filled.Value.z, 0.2f);
		ImGui::GetBackgroundDrawList()->AddQuadFilled({ vecCornerScreen[0].fX, vecCornerScreen[0].fY }, { vecCornerScreen[1].fX, vecCornerScreen[1].fY }, { vecCornerScreen[2].fX, vecCornerScreen[2].fY }, { vecCornerScreen[3].fX, vecCornerScreen[3].fY }, colorESP);
		ImGui::GetBackgroundDrawList()->AddQuadFilled({ vecCornerScreen[4].fX, vecCornerScreen[4].fY }, { vecCornerScreen[5].fX, vecCornerScreen[5].fY }, { vecCornerScreen[6].fX, vecCornerScreen[6].fY }, { vecCornerScreen[7].fX, vecCornerScreen[7].fY }, colorESP);
		ImGui::GetBackgroundDrawList()->AddQuadFilled({ vecCornerScreen[0].fX, vecCornerScreen[0].fY }, { vecCornerScreen[1].fX, vecCornerScreen[1].fY }, { vecCornerScreen[5].fX, vecCornerScreen[5].fY }, { vecCornerScreen[4].fX, vecCornerScreen[4].fY }, colorESP);
		ImGui::GetBackgroundDrawList()->AddQuadFilled({ vecCornerScreen[1].fX, vecCornerScreen[1].fY }, { vecCornerScreen[2].fX, vecCornerScreen[2].fY }, { vecCornerScreen[6].fX, vecCornerScreen[6].fY }, { vecCornerScreen[5].fX, vecCornerScreen[5].fY }, colorESP);
		ImGui::GetBackgroundDrawList()->AddQuadFilled({ vecCornerScreen[2].fX, vecCornerScreen[2].fY }, { vecCornerScreen[3].fX, vecCornerScreen[3].fY }, { vecCornerScreen[7].fX, vecCornerScreen[7].fY }, { vecCornerScreen[6].fX, vecCornerScreen[6].fY }, colorESP);
		ImGui::GetBackgroundDrawList()->AddQuadFilled({ vecCornerScreen[3].fX, vecCornerScreen[3].fY }, { vecCornerScreen[0].fX, vecCornerScreen[0].fY }, { vecCornerScreen[4].fX, vecCornerScreen[4].fY }, { vecCornerScreen[7].fX, vecCornerScreen[7].fY }, colorESP);
	}

	for (auto i = 1; i <= 4; i++)
	{
		DrawLine(vecCornerScreen[i - 1], vecCornerScreen[i % 4], Color, fThickness);
		DrawLine(vecCornerScreen[i - 1], vecCornerScreen[i + 3], Color, fThickness);
		DrawLine(vecCornerScreen[i + 3], vecCornerScreen[i % 4 + 4], Color, fThickness);
	}
}

void CRender::DrawImageRotated(ImTextureID ImTex, ImVec2 vecCenter, ImVec2 vecSize, float fAngle)
{
	float fCos = cosf(fAngle), fSin = sinf(fAngle);

	ImVec2 vecPos[4] =
	{
		vecCenter + ImRotate({ -vecSize.x, -vecSize.y }, fCos, fSin),
		vecCenter + ImRotate({ +vecSize.x, -vecSize.y }, fCos, fSin),
		vecCenter + ImRotate({ +vecSize.x, +vecSize.y }, fCos, fSin),
		vecCenter + ImRotate({ -vecSize.x, +vecSize.y }, fCos, fSin)
	};

	ImVec2 vecUv[4] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

	ImGui::GetBackgroundDrawList()->AddImageQuad(ImTex, vecPos[0], vecPos[1], vecPos[2], vecPos[3], vecUv[0], vecUv[1], vecUv[2], vecUv[3], IM_COL32_WHITE);

}