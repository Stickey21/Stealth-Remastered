#include "main.h"

struct stConfig g_Config;
CConfig* pConfig;

using json = nlohmann::basic_json<std::map, std::vector, std::string, bool, std::int64_t, std::uint64_t, float>;
using value_t = json::value_t;

CConfig::CConfig(const char* szName)
{
	memset(g_Config.g_Aimbot.bAimbotEnabled, false, sizeof(g_Config.g_Aimbot.bAimbotEnabled));

	for (int i = 22; i <= 34; i++)
	{
		g_Config.g_Aimbot.iAimbotConfig[i][RANGE] = 1;
		g_Config.g_Aimbot.iAimbotConfig[i][SILENT] = 1;
		g_Config.g_Aimbot.iAimbotConfig[i][SMOOTH] = 1;
	}

	if (PWSTR pathToDocuments; SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Documents, 0, nullptr, &pathToDocuments)))
	{
		Path = pathToDocuments;
		Path /= szName;
		CoTaskMemFree(pathToDocuments);
	}

    listConfigs();
}

#define WRITE(name, valueName) to_json(j[name], o.valueName, dummy.valueName)

template <typename T>
static void to_json(json& j, const T& o, const T& dummy)
{
    if (o != dummy)
        j = o;
}

static void to_json(json& j, const ImVec4& o, const ImVec4& dummy)
{
    j[0] = o.x;
    j[1] = o.y;
    j[2] = o.z;
    j[3] = o.w;
}

static void to_json(json& j, const stConfig::stAimbot& o, const stConfig::stAimbot& dummy = {})
{
    WRITE("bAimbot", bAimbot);
    WRITE("bSilent", bSilent);
    WRITE("bSmooth", bSmooth);
    WRITE("bProAim", bProAim);
    WRITE("bTriggerbot", bTriggerbot);

    for (int i = 22; i <= 34; i++)
    {
        WRITE(mapWeapons[i].szName + " Range", iAimbotConfig[i][RANGE]);
        WRITE(mapWeapons[i].szName + " Silent", iAimbotConfig[i][SILENT]);
        WRITE(mapWeapons[i].szName + " Range", iAimbotConfig[i][SMOOTH]);
        WRITE(mapWeapons[i].szName + " Enabled", bAimbotEnabled[i]);
    }

    WRITE("fSilentAccuracy", fSilentAccuracy);
    WRITE("bSmoothIsFire", bSmoothIsFire);
    WRITE("bStopOnBody", bStopOnBody);
    WRITE("bSmoothLockY", bSmoothLockY);
    WRITE("bTriggerDelay", bTriggerDelay);
    WRITE("iTriggerDelay", iTriggerDelay);
    WRITE("bTriggerSync", bTriggerSync);
    WRITE("bTeamProtect", bTeamProtect);
    WRITE("bLockThroughObjects", bLockThroughObjects);
    WRITE("bIgnoreMaxDistance", bIgnoreMaxDistance);
    WRITE("bIgnoreAFK", bIgnoreAFK);
    WRITE("bIgnoreEverything", bIgnoreEverything);
    WRITE("iRangeStyle", iRangeStyle);
    WRITE("bDrawRange", bDrawRange);
    WRITE("bDrawTracer", bDrawTracer);
    WRITE("fOutlineThickness", fOutlineThickness);
    WRITE("colorRange", colorRange);
}

static void to_json(json& j, const stConfig::stCombat& o, const stConfig::stCombat& dummy = {})
{
    WRITE("bNoReload", bNoReload);
    WRITE("bAllowCBug", bAllowCBug);
    WRITE("bCBugFreeze", bCBugFreeze);
    WRITE("bCBugAnim", bCBugAnim);
    WRITE("bCBugText", bCBugText);
    WRITE("bCBugWeapon", bCBugWeapon);
    WRITE("bRapidFire", bRapidFire);
    WRITE("fRapidFire", fRapidFire);
    WRITE("bFastCrosshair", bFastCrosshair);
    WRITE("bNoRecoil", bNoRecoil);
    WRITE("bWeaponSwitcher", bWeaponSwitcher);
    WRITE("bFastSwitch", bFastSwitch);
}

static void to_json(json& j, const stConfig::stPlayer& o, const stConfig::stPlayer& dummy = {})
{
    WRITE("bAntiStun", bAntiStun);
    WRITE("iAntiStunChance", iAntiStunChance);
    WRITE("bFastRotation", bFastRotation);
    WRITE("iFastRotation", iFastRotation);
    WRITE("bFastSprint", bFastSprint);
    WRITE("fFastSprint", fFastSprint);
    WRITE("bAirBreak", bAirBreak);
    WRITE("iAirBreak", iAirBreak);
    WRITE("bMapRun", bMapRun);
    WRITE("iMapRun", iMapRun);
    WRITE("bInfiniteRun", bInfiniteRun);
    WRITE("bInfiniteOxygen", bInfiniteOxygen);
    WRITE("bNoFall", bNoFall);
    WRITE("bChangeSkin", bChangeSkin);
    WRITE("iSkinID", iSkinID);
    WRITE("bFastRespawn", bFastRespawn);
    WRITE("bUpsideMode", bUpsideMode);
    WRITE("bInvertWalk", bInvertWalk);
    WRITE("bShakeMode", bShakeMode);
    WRITE("bInvalidAimZ", bInvalidAimZ);
    WRITE("bInvisible", bInvisible);
    WRITE("bFakeLag", bFakeLag);
    WRITE("iFakeLag", iFakeLag);
    WRITE("bStopOnExitVehicle", bStopOnExitVehicle);
    WRITE("iStopOnExitVehicle", iStopOnExitVehicle);
    WRITE("bAutoBikeSpam", bAutoBikeSpam);
    WRITE("bMotorBikeSpam", bMotorBikeSpam);
}

static void to_json(json& j, const stConfig::stVisuals& o, const stConfig::stVisuals& dummy = {})
{
    WRITE("bNameTagsESP", bNameTagsESP);
    WRITE("bWeaponESP", bWeaponESP);
    WRITE("bSnipelineESP", bSnipelineESP);
    WRITE("iSnipelineStyle", iSnipelineStyle);
    WRITE("bBoxESP", bBoxESP);
    WRITE("iBoxStyle", iBoxStyle);
    WRITE("bBoneESP", bBoneESP);
    WRITE("bDistanceESP", bDistanceESP);
    WRITE("bChamsESP", bChamsESP);
    WRITE("fESPThickness", fESPThickness);
    WRITE("bESPTeammates", bESPTeammates);
    WRITE("bESPCustomColors", bESPCustomColors);
    WRITE("bFilledBox", bFilledBox);
    WRITE("colorVisible", colorVisible);
    WRITE("colorNotVisible", colorNotVisible);
    WRITE("bVehicleNameTagsESP", bVehicleNameTagsESP);
    WRITE("bVehicleHPESP", bVehicleHPESP);
    WRITE("bVehicleStatusESP", bVehicleStatusESP);
    WRITE("bVehicleEngineESP", bVehicleEngineESP);
    WRITE("bVehicleDistanceESP", bVehicleDistanceESP);
    WRITE("bDMGInformer", bDMGInformer);
    WRITE("iFontSize", iFontSize);
    WRITE("fDMGTick", fDMGTick);
    WRITE("colorDMG", colorDMG);
    WRITE("bFPSFuncs", bFPSFuncs);
    WRITE("bFPSUnlock", bFPSUnlock);
    WRITE("bFPSBoost", bFPSBoost);
    WRITE("bFakeFPS", bFakeFPS);
    WRITE("iMinFPS", iMinFPS);
    WRITE("iMaxFPS", iMaxFPS);
    WRITE("bNoDrugsEffect", bNoDrugsEffect);
    WRITE("bAspectRatio", bAspectRatio);
    WRITE("fAspectRatio", fAspectRatio);
    WRITE("bWeatherChanger", bWeatherChanger);
    WRITE("iWeather", iWeather);
    WRITE("bTimeChanger", bTimeChanger);
    WRITE("iTime", iTime);
}

static void to_json(json& j, const stConfig::stStyle& o, const stConfig::stStyle& dummy = {})
{
    for (int i = 0; i < ImGuiCol_COUNT; i++)
        WRITE(ImGui::GetStyleColorName(i), vecTheme[i]);
}

static void to_json(json& j, const stConfig::stHotkeys& o, const stConfig::stHotkeys& dummy = {})
{
    WRITE("iWeaponSwitch", iWeaponSwitch);
    WRITE("iAutoBikeSpam", iAutoBikeSpam);
    WRITE("iMotorBikeSpam", iMotorBikeSpam);
}

void removeEmptyObjects(json& j)
{
    for (auto it = j.begin(); it != j.end();) {
        auto& val = it.value();
        if (val.is_object() || val.is_array())
            removeEmptyObjects(val);
        if (val.empty() && !j.is_array())
            it = j.erase(it);
        else
            ++it;
    }
}

template <value_t Type, typename T>
static typename std::enable_if_t<!std::is_same_v<T, bool>> read(const json& j, const char* key, T& o)
{
    if (!j.contains(key))
        return;

    if (const auto& val = j[key]; val.type() == Type)
        val.get_to(o);
}

static void read(const json& j, const char* key, bool& o)
{
    if (!j.contains(key))
        return;

    if (const auto& val = j[key]; val.type() == value_t::boolean)
        val.get_to(o);
}

static void read(const json& j, const char* key, float& o)
{
    if (!j.contains(key))
        return;

    if (const auto& val = j[key]; val.type() == value_t::number_float)
        val.get_to(o);
}

static void read(const json& j, const char* key, int& o)
{
    if (!j.contains(key))
        return;

    if (const auto& val = j[key]; val.is_number_integer())
        val.get_to(o);
}

template <typename T, size_t Size>
static void read(const json& j, const char* key, std::array<T, Size>& o)
{
    if (!j.contains(key))
        return;

    if (const auto& val = j[key]; val.type() == value_t::array && val.size() == o.size()) {
        for (std::size_t i = 0; i < val.size(); ++i) {
            if (!val[i].empty())
                val[i].get_to(o[i]);
        }
    }
}

static void read(const json& j, const char* key, ImVec4& o)
{
    std::array<float, 4> temp;
    read(j, key, temp);
    o = ImVec4(temp[0], temp[1], temp[2], temp[3]);
}

static void from_json(const json& j, stConfig::stAimbot& r)
{
    read(j, "bAimbot", r.bAimbot);
    read(j, "bSilent", r.bSilent);
    read(j, "bSmooth", r.bSmooth);
    read(j, "bProAim", r.bProAim);
    read(j, "bTriggerbot", r.bTriggerbot);

    for (int i = 22; i <= 34; i++)
    {
        read(j, (mapWeapons[i].szName + " Range").c_str() , r.iAimbotConfig[i][RANGE]);
        read(j, (mapWeapons[i].szName + " Silent").c_str(), r.iAimbotConfig[i][SILENT]);
        read(j, (mapWeapons[i].szName + " Range").c_str(), r.iAimbotConfig[i][SMOOTH]);
        read(j, (mapWeapons[i].szName + " Enabled").c_str(), r.bAimbotEnabled[i]);
    }

    read(j, "fSilentAccuracy", r.fSilentAccuracy);
    read(j, "bSmoothIsFire", r.bSmoothIsFire);
    read(j, "bStopOnBody", r.bStopOnBody);
    read(j, "bSmoothLockY", r.bSmoothLockY);
    read(j, "bTriggerDelay", r.bTriggerDelay);
    read(j, "iTriggerDelay", r.iTriggerDelay);
    read(j, "bTriggerSync", r.bTriggerSync);
    read(j, "bTeamProtect", r.bTeamProtect);
    read(j, "bLockThroughObjects", r.bLockThroughObjects);
    read(j, "bIgnoreMaxDistance", r.bIgnoreMaxDistance);
    read(j, "bIgnoreAFK", r.bIgnoreAFK);
    read(j, "bIgnoreEverything", r.bIgnoreEverything);
    read(j, "iRangeStyle", r.iRangeStyle);
    read(j, "bDrawRange", r.bDrawRange);
    read(j, "bDrawTracer", r.bDrawTracer);
    read(j, "fOutlineThickness", r.fOutlineThickness);
    read(j, "colorRange", r.colorRange);
}

static void from_json(const json& j, stConfig::stCombat& r)
{
   read(j, "bNoReload", r.bNoReload);
   read(j, "bAllowCBug", r.bAllowCBug);
   read(j, "bCBugFreeze", r.bCBugFreeze);
   read(j, "bCBugAnim", r.bCBugAnim);
   read(j, "bCBugText", r.bCBugText);
   read(j, "bCBugWeapon", r.bCBugWeapon);
   read(j, "bRapidFire", r.bRapidFire);
   read(j, "fRapidFire", r.fRapidFire);
   read(j, "bFastCrosshair", r.bFastCrosshair);
   read(j, "bNoRecoil", r.bNoRecoil);
   read(j, "bWeaponSwitcher", r.bWeaponSwitcher);
   read(j, "bFastSwitch", r.bFastSwitch);
}

static void from_json(const json& j, stConfig::stPlayer& r)
{
    read(j, "bAntiStun", r.bAntiStun);
    read(j, "iAntiStunChance", r.iAntiStunChance);
    read(j, "bFastRotation", r.bFastRotation);
    read(j, "iFastRotation", r.iFastRotation);
    read(j, "bFastSprint", r.bFastSprint);
    read(j, "fFastSprint", r.fFastSprint);
    read(j, "bAirBreak", r.bAirBreak);
    read(j, "iAirBreak", r.iAirBreak);
    read(j, "bMapRun", r.bMapRun);
    read(j, "iMapRun", r.iMapRun);
    read(j, "bInfiniteRun", r.bInfiniteRun);
    read(j, "bInfiniteOxygen", r.bInfiniteOxygen);
    read(j, "bNoFall", r.bNoFall);
    read(j, "bChangeSkin", r.bChangeSkin);
    read(j, "iSkinID", r.iSkinID);
    read(j, "bFastRespawn", r.bFastRespawn);
    read(j, "bUpsideMode", r.bUpsideMode);
    read(j, "bInvertWalk", r.bInvertWalk);
    read(j, "bShakeMode", r.bShakeMode);
    read(j, "bInvalidAimZ", r.bInvalidAimZ);
    read(j, "bInvisible", r.bInvisible);
    read(j, "bFakeLag", r.bFakeLag);
    read(j, "iFakeLag", r.iFakeLag);
    read(j, "bStopOnExitVehicle", r.bStopOnExitVehicle);
    read(j, "iStopOnExitVehicle", r.iStopOnExitVehicle);
    read(j, "bAutoBikeSpam", r.bAutoBikeSpam);
    read(j, "bMotorBikeSpam", r.bMotorBikeSpam);
}

static void from_json(const json& j, stConfig::stVisuals& r)
{
    read(j, "bNameTagsESP", r.bNameTagsESP);
    read(j, "bWeaponESP", r.bWeaponESP);
    read(j, "bSnipelineESP", r.bSnipelineESP);
    read(j, "iSnipelineStyle", r.iSnipelineStyle);
    read(j, "bBoxESP", r.bBoxESP);
    read(j, "iBoxStyle", r.iBoxStyle);
    read(j, "bBoneESP", r.bBoneESP);
    read(j, "bDistanceESP", r.bDistanceESP);
    read(j, "bChamsESP", r.bChamsESP);
    read(j, "fESPThickness", r.fESPThickness);
    read(j, "bESPTeammates", r.bESPTeammates);
    read(j, "bESPCustomColors", r.bESPCustomColors);
    read(j, "bFilledBox", r.bFilledBox);
    read(j, "colorVisible", r.colorVisible);
    read(j, "colorNotVisible", r.colorNotVisible);
    read(j, "bVehicleNameTagsESP", r.bVehicleNameTagsESP);
    read(j, "bVehicleHPESP", r.bVehicleHPESP);
    read(j, "bVehicleStatusESP", r.bVehicleStatusESP);
    read(j, "bVehicleEngineESP", r.bVehicleEngineESP);
    read(j, "bVehicleDistanceESP", r.bVehicleDistanceESP);
    read(j, "bDMGInformer", r.bDMGInformer);
    read(j, "iFontSize", r.iFontSize);
    read(j, "fDMGTick", r.fDMGTick);
    read(j, "colorDMG", r.colorDMG);
    read(j, "bFPSFuncs", r.bFPSFuncs);
    read(j, "bFPSUnlock", r.bFPSUnlock);
    read(j, "bFPSBoost", r.bFPSBoost);
    read(j, "bFakeFPS", r.bFakeFPS);
    read(j, "iMinFPS", r.iMinFPS);
    read(j, "iMaxFPS", r.iMaxFPS);
    read(j, "bNoDrugsEffect", r.bNoDrugsEffect);
    read(j, "bAspectRatio", r.bAspectRatio);
    read(j, "fAspectRatio", r.fAspectRatio);
    read(j, "bWeatherChanger", r.bWeatherChanger);
    read(j, "iWeather", r.iWeather);
    read(j, "bTimeChanger", r.bTimeChanger);
    read(j, "iTime", r.iTime);
}

static void from_json(const json& j, stConfig::stStyle& r)
{
    for (int i = 0; i < ImGuiCol_COUNT; i++)
        read(j, ImGui::GetStyleColorName(i), r.vecTheme[i]);
}

static void from_json(const json& j, stConfig::stHotkeys& r)
{
    read(j, "iWeaponSwitch", r.iWeaponSwitch);
    read(j, "iAutoBikeSpam", r.iAutoBikeSpam);
    read(j, "iMotorBikeSpam", r.iMotorBikeSpam);
}

void CConfig::Load(size_t index)
{
    json j;

    if (std::ifstream in{ Path / (const char8_t*)Configs[index].c_str() }; in.good())
        in >> j;
    else return;

    Reset();

    read<value_t::object>(j, "Aimbot", g_Config.g_Aimbot);
    read<value_t::object>(j, "Combat", g_Config.g_Combat);
    read<value_t::object>(j, "Player", g_Config.g_Player);
    read<value_t::object>(j, "Visuals", g_Config.g_Visuals);
    read<value_t::object>(j, "Style", g_Config.g_Style);
    read<value_t::object>(j, "Hotkeys", g_Config.g_Hotkeys);
}

void CConfig::Save(size_t index)
{
    std::error_code ec;
    std::filesystem::create_directory(Path, ec);

    if (std::ofstream out{ Path / (const char8_t*)Configs[index].c_str() }; out.good()) 
    {
        json j;
        j["Aimbot"] = g_Config.g_Aimbot;
        j["Combat"] = g_Config.g_Combat;
        j["Player"] = g_Config.g_Player;
        j["Visuals"] = g_Config.g_Visuals;
        j["Style"] = g_Config.g_Style;
        j["Hotkeys"] = g_Config.g_Hotkeys;

        removeEmptyObjects(j);
        out << std::setw(2) << j;
    }
}

void CConfig::Add(const char* szName)
{
    if (*szName && std::find(Configs.cbegin(), Configs.cend(), szName) == Configs.cend()) {
        Configs.emplace_back(szName);
        Save(Configs.size() - 1);
    }
}

void CConfig::Remove(size_t index)
{
    std::error_code ec;
    std::filesystem::remove(Path / (const char8_t*)Configs[index].c_str(), ec);
    Configs.erase(Configs.cbegin() + index);
}

void CConfig::Rename(size_t item, const char* szName)
{
    std::error_code ec;
    std::filesystem::rename(Path / (const char8_t*)Configs[item].c_str(), Path / (const char8_t*)szName, ec);
    Configs[item] = szName;
}

void CConfig::Reset()
{
    g_Config.g_Aimbot = {};
    g_Config.g_Combat = {};
    g_Config.g_Player = {};
    g_Config.g_Visuals = {};
    g_Config.g_Style = {};
    g_Config.g_Hotkeys = {};
}

void CConfig::listConfigs()
{
    Configs.clear();

    std::error_code ec;
    std::transform(std::filesystem::directory_iterator{ Path, ec },
        std::filesystem::directory_iterator{ },
        std::back_inserter(Configs),
        [](const auto& entry) { return std::string{ (const char*)entry.path().filename().u8string().c_str() }; });
}