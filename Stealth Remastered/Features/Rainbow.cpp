#include "main.h"

CRainbow* pRainbow = new CRainbow;

void CRainbow::Update()
{
	if (pMenu->iLogoPressed >= 10)
	{
		static float fRainbow;
		fRainbow += 0.005f;
		if (fRainbow > 1.f) fRainbow = 0.f;
		colLogo = Color::FromHSB(fRainbow, 1.f, 1.f);
	}
}