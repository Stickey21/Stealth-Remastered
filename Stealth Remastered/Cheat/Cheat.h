#pragma once

namespace Cheat
{
	void Render();
	void Update();
	void Unload();
};

static struct stWeapons
{
	int iCategory = 0;
	std::string szName;
} g_Weapons[13] = { 0, "Colt 45", 0, "Silenced Pistol", 0, "Deagle", 2, "Shotgun", 2, "Sawed-off", 2, "Combat Shotgun", 1, "Micro-Uzi", 1, "MP5", 3, "AK-47", 3, "M4", 1, "Tec-9", 4, "Rifle" , 4, "Sniper Rifle" };

static const char* szCategory[5] = { "Handguns", "Sub-Machine Guns", "Shotguns", "Assault Rifles", "Rifles" };

#define RANGE 0
#define SILENT 1
#define SMOOTH 2
#define PROAIM 3