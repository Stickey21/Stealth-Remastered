#include "main.h"

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
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(vecFrom.fX, vecFrom.fY), ImVec2(vecTo.fX, vecTo.fY), Color, fThickness);
}

void CRender::D3DLine(CVector vecFrom, CVector vecTo, ImColor Color, float fThickness)
{
	CVector vecFromScreen, vecToScreen;
	Utils::CalcScreenCoors(&vecFrom, &vecFromScreen);
	Utils::CalcScreenCoors(&vecTo, &vecToScreen);

	if (vecFromScreen.fZ < 1.f || vecToScreen.fZ < 1.f)
		return;

	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(vecFromScreen.fX, vecFromScreen.fY), ImVec2(vecToScreen.fX, vecToScreen.fY), Color, fThickness);
}

void CRender::DrawCircle(CVector vecPos, float fRadius, ImColor Color, float fThickness)
{
	ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(vecPos.fX, vecPos.fY), fRadius * 1.5f, Color, 128, fThickness);
}

void CRender::DrawCircleFilled(CVector vecPos, float fRadius, ImColor Color)
{
	ImGui::GetBackgroundDrawList()->AddCircleFilled(ImVec2(vecPos.fX, vecPos.fY), fRadius * 1.5f, Color, 128);
}

void CRender::DrawRect(CVector vecFrom, CVector vecTo, ImColor Color, float fThickness)
{
	ImGui::GetBackgroundDrawList()->AddRect(ImVec2(vecFrom.fX, vecFrom.fY), ImVec2(vecTo.fX, vecTo.fY), Color, 1.f, 0, fThickness);
}

void CRender::DrawRectFilled(CVector vecFrom, CVector vecTo, ImColor Color)
{
	ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(vecFrom.fX, vecFrom.fY), ImVec2(vecTo.fX, vecTo.fY), Color);
}

void CRender::DrawBoundingBox(CVector vecPos, CVector vecMin, CVector vecMax, float fRotation, ImColor Color, float fThickness, bool bFilled, ImColor Filled)
{
	CVector vecCornerScreen[8], vecCorner[8] =
	{
		CVector(vecMin.fX, vecMin.fY, vecMin.fZ),
		CVector(vecMin.fX, vecMax.fY, vecMin.fZ),
		CVector(vecMax.fX, vecMax.fY, vecMin.fZ),
		CVector(vecMax.fX, vecMin.fY, vecMin.fZ),
		CVector(vecMin.fX, vecMin.fY, vecMax.fZ),
		CVector(vecMin.fX, vecMax.fY, vecMax.fZ),
		CVector(vecMax.fX, vecMax.fY, vecMax.fZ),
		CVector(vecMax.fX, vecMin.fY, vecMax.fZ)
	};

	for (int i = 0; i < 8; i++)
	{
		float fSin = sin(fRotation), fCos = cos(fRotation), fX = vecCorner[i].fX, fY = vecCorner[i].fY;
		vecCorner[i].fX = vecPos.fX + (fX * fCos) - (fY * fSin);
		vecCorner[i].fY = vecPos.fY + (fX * fSin) + (fY * fCos);
		vecCorner[i].fZ += vecPos.fZ;
		Utils::CalcScreenCoors(&vecCorner[i], &vecCornerScreen[i]);

		if (vecCornerScreen[i].fZ < 1.0f)
			return;
	}

	if (bFilled)
	{
		ImColor colorESP = ImColor(Filled.Value.x, Filled.Value.y, Filled.Value.z, 0.2f);
		ImGui::GetBackgroundDrawList()->AddQuadFilled(ImVec2(vecCornerScreen[0].fX, vecCornerScreen[0].fY), ImVec2(vecCornerScreen[1].fX, vecCornerScreen[1].fY), ImVec2(vecCornerScreen[2].fX, vecCornerScreen[2].fY), ImVec2(vecCornerScreen[3].fX, vecCornerScreen[3].fY), colorESP);
		ImGui::GetBackgroundDrawList()->AddQuadFilled(ImVec2(vecCornerScreen[4].fX, vecCornerScreen[4].fY), ImVec2(vecCornerScreen[5].fX, vecCornerScreen[5].fY), ImVec2(vecCornerScreen[6].fX, vecCornerScreen[6].fY), ImVec2(vecCornerScreen[7].fX, vecCornerScreen[7].fY), colorESP);
		ImGui::GetBackgroundDrawList()->AddQuadFilled(ImVec2(vecCornerScreen[0].fX, vecCornerScreen[0].fY), ImVec2(vecCornerScreen[1].fX, vecCornerScreen[1].fY), ImVec2(vecCornerScreen[5].fX, vecCornerScreen[5].fY), ImVec2(vecCornerScreen[4].fX, vecCornerScreen[4].fY), colorESP);
		ImGui::GetBackgroundDrawList()->AddQuadFilled(ImVec2(vecCornerScreen[1].fX, vecCornerScreen[1].fY), ImVec2(vecCornerScreen[2].fX, vecCornerScreen[2].fY), ImVec2(vecCornerScreen[6].fX, vecCornerScreen[6].fY), ImVec2(vecCornerScreen[5].fX, vecCornerScreen[5].fY), colorESP);
		ImGui::GetBackgroundDrawList()->AddQuadFilled(ImVec2(vecCornerScreen[2].fX, vecCornerScreen[2].fY), ImVec2(vecCornerScreen[3].fX, vecCornerScreen[3].fY), ImVec2(vecCornerScreen[7].fX, vecCornerScreen[7].fY), ImVec2(vecCornerScreen[6].fX, vecCornerScreen[6].fY), colorESP);
		ImGui::GetBackgroundDrawList()->AddQuadFilled(ImVec2(vecCornerScreen[3].fX, vecCornerScreen[3].fY), ImVec2(vecCornerScreen[0].fX, vecCornerScreen[0].fY), ImVec2(vecCornerScreen[4].fX, vecCornerScreen[4].fY), ImVec2(vecCornerScreen[7].fX, vecCornerScreen[7].fY), colorESP);
	}

	for (int i = 1; i <= 4; i++)
	{
		DrawLine(vecCornerScreen[i - 1], vecCornerScreen[i % 4], Color, fThickness);
		DrawLine(vecCornerScreen[i - 1], vecCornerScreen[i + 3], Color, fThickness);
		DrawLine(vecCornerScreen[i + 3], vecCornerScreen[i % 4 + 4], Color, fThickness);
	}
}