#include "main.h"

HMODULE g_hModule = NULL;

void mainThread()
{
	pSAMP = new CSAMP();

	while (!pSAMP->tryInit())
		Sleep(100);

	pD3DHook = new CD3DHook();
	pAimbot = new CAimbot();
	pVisuals = new CVisuals();
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