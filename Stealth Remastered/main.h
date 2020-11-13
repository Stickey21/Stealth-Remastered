#pragma once

#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <string>
#include <stdlib.h>
#include <vector>
#include <memory>
#include <Psapi.h>
#include <ShlObj.h>
#include <filesystem>
#include <map>
#include <random>
#include <algorithm>

#include "detours.h"
#pragma comment(lib, "detours.lib")
#pragma comment(lib, "d3d9.lib")

#include "plugin.h"
#include "CWorld.h"
#include "CCamera.h"
#include "CAnimManager.h"
#include "CStreaming.h"
#include "CRunningScript.h"
#include "CPlayerPed.h"
#include "CModelInfo.h"
#include "CDraw.h"
#include "CMenuManager.h"
#include "CClock.h"
#include "CWeather.h"
#include "CRadar.h"
#include "CTxdStore.h"
#include "CTaskSimpleRunNamedAnim.h"
#include "ePedBones.h"
#include "extensions/ScriptCommands.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx9.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_render.h"
#include "ImGui/imgui_custom.h"

#include "Cheat/Cheat.h"
#include "Cheat/json/json.hpp"
#include "Cheat/Config/Config.h"
#include "Cheat/Secure.h"

#include "Cheat/Structs.h"
#include "Cheat/Memory.h"
#include "Cheat/Textures.h"
#include "Cheat/Icons.h"

#include "SAMP/SAMP.h"

#include "Cheat/Hooks.h"

#include "Cheat/KeyHook.h"
#include "Cheat/MathStuff.h"
#include "Cheat/Utils.h"

#include "D3D/D3DHook.h"

#include "SAMP/RakNet/BitStream.h"
#include "SAMP/RakNet/RakClient.h"

#include "Features/Rainbow.h"
#include "Features/Aimbot.h"
#include "Features/Combat.h"
#include "Features/Player.h"
#include "Features/Visuals.h"

#include "Menu/Menu.h"

extern HMODULE g_hModule;