#include "main.h"

struct stConfig g_Config;
CConfig* pConfig = new CConfig();

CConfig::CConfig()
{
	memset(g_Config.g_Aimbot.bAimbotEnabled, false, sizeof(g_Config.g_Aimbot.bAimbotEnabled));

	for (int i = 22; i <= 34; i++)
	{
		g_Config.g_Aimbot.iAimbotConfig[i][RANGE] = 1;
		g_Config.g_Aimbot.iAimbotConfig[i][SILENT] = 1;
		g_Config.g_Aimbot.iAimbotConfig[i][SMOOTH] = 1;
	}
}