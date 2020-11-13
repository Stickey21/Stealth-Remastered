#include "main.h"

HMODULE g_hModule = NULL;

void mainThread()
{
	pConfig = new CConfig("Stealth");
	pSAMP = new CSAMP();

	while (!pSAMP->tryInit())
		Sleep(100);

	pHooks = new CHooks();
	pD3DHook = new CD3DHook();
	pAimbot = new CAimbot();
	pVisuals = new CVisuals();
	//pRainbow = new CRainbow();
	pRakClient = new CRakClient();

	pSAMP->isInited = true;

	ExitThread(0);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReasonForCall, LPVOID lpReserved)
{
	switch (dwReasonForCall)
	{
		case DLL_PROCESS_ATTACH:
		{
			g_hModule = hModule;
			DisableThreadLibraryCalls(hModule);
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)mainThread, NULL, NULL, NULL);
			break;
		}
	}
	return TRUE;
}