#pragma once

class CMenu
{
public:
	CMenu::CMenu(IDirect3DDevice9* pDevice)
	{
		iToggle = 2;
		iCurrentTab = 0;
		iCurrentCategory = 0;
		bOpen = false;
		fFadeAlpha = 0.f;
		ImGui::CreateContext();
		ImGui::GetIO().IniFilename = NULL;
		ImGui_ImplWin32_Init(GetActiveWindow());
		ImGui_ImplDX9_Init(pDevice);
		pFont = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\verdanab.ttf", 13.1f);
		pIcons = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(uiIconData, uiIconSize, 30);
		Theme();
	};

	CMenu::~CMenu()
	{
		ImGui_ImplDX9_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	};

	void Update();
	void Render();
	void Theme();

	int iToggle, iCurrentTab, iCurrentCategory;
	bool bOpen;

	ImFont* pFont, *pIcons;

private:
	void ActiveTab();
	void NormalTab();
	void ActiveButton();
	void NormalButton();
	void Tooltip(const char* szText);

	float fFadeAlpha;
};

extern CMenu* pMenu;
