#include "main.h"

CHooks* pHooks;

void __fastcall CHooks::hkCPed_Render(CPed* pPed, void* pUnknown)
{
	pHooks->pCurrentPed = pPed;
	return pHooks->oPed_Render(pPed);
}