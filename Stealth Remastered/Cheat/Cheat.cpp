#include "main.h"

void Cheat::Render()
{
	if (FrontEndMenuManager.m_bMenuActive)
		return;

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	pAimbot->Render();
	pVisuals->Render();
	pMenu->Render();
	pMenu->RenderMap();

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

void Cheat::Update()
{
	pAimbot->Update();
	Combat::Update();
	Player::Update();
	pVisuals->Update();
	//pRainbow->Update();
	pMenu->Update();
}

DWORD WINAPI UnloadThread(HMODULE hModule)
{
	Sleep(100);
	delete pMenu;
	delete pTextures;
	delete pSAMP;

	FreeLibraryAndExitThread(hModule, 0);
}

void Cheat::Unload()
{
	if (pMenu->bOpen)
		pSAMP->toggleSAMPCursor(0);

	delete pHooks;
	delete pD3DHook;
	delete pKeyHook;
	delete pRakClient;
	delete pAimbot;
	delete pVisuals;

	CreateThread(NULL, NULL, LPTHREAD_START_ROUTINE(UnloadThread), g_hModule, NULL, NULL);
}