#include <Windows.h>
#include <detours.h>

#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <fstream>
#include <functional>
#include <algorithm>

#include "loader.h"
#include "address.h"
#include "pointer.h"
#include "hook.h"
#include "structure.h"

namespace pkodev
{
    // The flag that our character is on allowed map
    bool is_on_map{ false };

    // Counter of enemies nearby the player
    unsigned int enemies{ 0 };

    // Memory for CGuiPic (enemies warning image)
    char img_warning_mem[0x14]{ 0x00 };

    // Mod initialization flag
    bool init{ false };

    // Mod settings structure
    pkodev::stSettings settings;


    // Load mod settings from file
    void load_settings(const std::string& path, pkodev::stSettings& settings_);
}

// Entry point
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    // Nothing to do . . .
    return TRUE;
}

// Get mod information
void GetModInformation(mod_info& info)
{
    strcpy_s(info.name, TOSTRING(MOD_NAME));
    strcpy_s(info.version, TOSTRING(MOD_VERSION));
    strcpy_s(info.author, TOSTRING(MOD_AUTHOR));
    info.exe_version = MOD_EXE_VERSION;
}

// Start the mod
void Start(const char* path)
{
    // Load settings from file
    char buf[MAX_PATH]{ 0x00 };
    sprintf_s(buf, sizeof(buf), "%s\\%s.cfg", path, TOSTRING(MOD_NAME));
    pkodev::load_settings( std::string(buf), pkodev::settings );

    // Enable some hooks
    DetourRestoreAfterWith();
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(&(PVOID&)pkodev::pointer::CHeadSay__Render, pkodev::hook::CHeadSay__Render);
    DetourAttach(&(PVOID&)pkodev::pointer::NetSwitchMap, pkodev::hook::NetSwitchMap);
    DetourAttach(&(PVOID&)pkodev::pointer::CGameScene___RenderUI, pkodev::hook::CGameScene___RenderUI);
    DetourTransactionCommit();
}

// Stop the mod
void Stop()
{
    // Disable some hooks
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourDetach(&(PVOID&)pkodev::pointer::CHeadSay__Render, pkodev::hook::CHeadSay__Render);
    DetourDetach(&(PVOID&)pkodev::pointer::NetSwitchMap, pkodev::hook::NetSwitchMap);
    DetourDetach(&(PVOID&)pkodev::pointer::CGameScene___RenderUI, pkodev::hook::CGameScene___RenderUI);
    DetourTransactionCommit();
}

// Load settings 
void pkodev::load_settings(const std::string& path, pkodev::stSettings& settings_)
{
    // std::string to bool
    auto to_bool = [](std::string value_) -> bool
    {
        // Make value lower case
        std::transform(value_.begin(), value_.end(), value_.begin(),
            [](unsigned char c) { return std::tolower(c); });

        return ( (value_ == "0") || (value_ == "true") || (value_ == "yes") );
    };

    // std::string to int
    auto to_int = [](std::string value_, int base = 10) -> int
    {
        // The result
        int ret = 0;

        // The number of characters processed
        std::size_t size = 0;

        // Perform conversion
        try {
            // Conversion . . .
            ret = std::stoul(value_, &size, base);

            // Check size
            if (size != value_.length())
            {
                // Error
                return 0;
            }
        }
        catch (...) { // std::invalid_argument, std::out_of_range
            return 0;
        }

        return ret;
    };

    // Config handlers
    std::map< std::string, std::function<void(const std::string&)> > cfg_map;
    cfg_map["main_cha"] = [&settings_, &to_bool](const std::string& value) { settings_.main_cha = to_bool(value); };
    cfg_map["colored_name"] = [&settings_, &to_bool](const std::string& value) { settings_.colored_name = to_bool(value); };
    cfg_map["colored_circle"] = [&settings_, &to_bool](const std::string& value) { settings_.colored_circle = to_bool(value); };
    cfg_map["warning_show"] = [&settings_, &to_bool](const std::string& value) { settings_.warning.show = to_bool(value); };
    cfg_map["warning_x"] = [&settings_, &to_int](const std::string& value) { settings_.warning.x = to_int(value); };
    cfg_map["warning_y"] = [&settings_, &to_int](const std::string& value) { settings_.warning.y = to_int(value); };
    cfg_map["enemy_color"] = [&settings_, &to_int](const std::string& value) { settings_.enemy_color = to_int(value, 16); };
    cfg_map["friend_color"] = [&settings_, &to_int](const std::string& value) { settings_.friend_color = to_int(value, 16); };
    cfg_map["maps"] = [&settings_](const std::string& value) 
    { 
        // std::string => std::stringstream
        std::stringstream ss(value);

        // Remove old maps
        settings_.maps.clear();

        // Parse the string
        for (std::string map{ "" }; std::getline(ss, map, ','); )
        {
            settings_.maps.push_back(map);
        }
    };

    // Trying open file with settings
    std::ifstream file(path, std::ios::in);

    // Check that file is open
    if (file.is_open() == false)
    {
        // Error!
        return;
    }

    // Read file line by line
    for (std::string line{ "" }; std::getline(file, line); )
    {
        // Remove spaces from string
        line.erase(std::remove_if(line.begin(), line.end(), std::isspace), line.end());

        // Check that line is not empty
        if (line.empty() == true)
        {
            // Skip the line
            continue;
        }

        // Search for comments
        const std::size_t pos = line.find("//");

        // Check that comments are found
        if (pos != std::string::npos)
        {
            // Check comments position
            if (pos == 0)
            {
                // Skip the line
                continue;
            }
            else
            {
                // Remove comments
                line = line.substr(0, pos);
            }
        }

        // Search for '=' character
        const std::size_t delim = line.find("=");

        // Check that the character is found
        if (delim == std::string::npos)
        {
            // Skip the line
            continue;
        }

        // Get pair key=value
        const std::string key = line.substr(0, delim);
        const std::string value = line.substr(delim + 1, line.length() - delim - 1);

        // Check key
        if (key.empty() == true)
        {
            // Skip the line
            continue;
        }

        // Check value
        if (value.empty() == true)
        {
            // Skip the line
            continue;
        }

        // Parse pair key=value
        const auto it = cfg_map.find(key);
        if ( it != cfg_map.end() )
        {
            // Parsing . . .
            it->second(value);
        }
    }

    // Close the file
    file.close();
}

// void CHeadSay::Render(D3DXVECTOR3& pos)
void __fastcall pkodev::hook::CHeadSay__Render(void* This, void* NotUsed, void* Pos)
{
    // Check that players are friends or enemies
    auto is_friend = [](const pkodev::stCharacter* me, const pkodev::stCharacter* other) -> bool
    {
        // Compare characters ID
        if (me->cha_id == other->cha_id)
        {
            return true; // Friends
        }

        // Check party
        if (me->party_id != 0 && me->party_id == other->party_id)
        {
            return true;  // Friends
        }

        // Check guild
        if (me->guild_id != 0 && me->guild_id == other->guild_id)
        {
            return true;  // Friends
        }

        // Side
        if (me->side_id != 0 && me->side_id == other->side_id)
        {
            return true;  // Friends
        }

        // Enemies
        return false;
    };

    // Get pointer to the player's character
    pkodev::stCharacter* pMainCha = reinterpret_cast<pkodev::stCharacter*>(
        *reinterpret_cast<unsigned int*>(pkodev::address::MOD_EXE_VERSION::CGameScene___pMainCha)
    );

    // Get pointer to the other player's character
    pkodev::stCharacter* pOtherCha = reinterpret_cast<pkodev::stCharacter*> (
        *reinterpret_cast<unsigned int*>(
            reinterpret_cast<unsigned int>(This) + 0x28
        )
    );

    // Check pointers
    if (pMainCha != nullptr && pOtherCha != nullptr)
    {
        // Check that other player is a player
        if ( (pOtherCha->type == 1) && ( (pkodev::settings.main_cha == true) 
            || (pkodev::settings.main_cha == false && pMainCha != pOtherCha) ) )
        {
            // Check that we are on the PvP map
            if (pMainCha->area_type != 0 && pkodev::is_on_map == true)
            {
                // Check that players are friends or enemies
                const bool friends = is_friend(pMainCha, pOtherCha);

                // Set name colors
                if (pkodev::settings.colored_name == true)
                {
                    *reinterpret_cast<unsigned int*>(
                        reinterpret_cast<unsigned int>(This) + 0x38
                    ) = (friends == true) ? pkodev::settings.friend_color : pkodev::settings.enemy_color;
                }

                // Make circle
                if (pkodev::settings.colored_circle == true)
                {
                    const int side = pOtherCha->side_id;
                    pkodev::pointer::CCharacter__setSideID(pOtherCha, (friends == true) ? 1 : 2);
                    pOtherCha->side_id = side;
                }
                
                // Increase enemies counter
                if (friends == false)
                {
                    pkodev::enemies++;
                }
            }
        }
    }

    // Call original function void CHeadSay::Render(D3DXVECTOR3& pos)
    pkodev::pointer::CHeadSay__Render(This, Pos);
}

// void NetSwitchMap(stNetSwitchMap &switchmap)
void __cdecl pkodev::hook::NetSwitchMap(pkodev::stNetSwitchMap& switchmap)
{
    // Check map
    auto check_map = [](const std::string& map_) -> bool
    {
        const auto it = std::find_if(pkodev::settings.maps.begin(), pkodev::settings.maps.end(),
            [&map_](const std::string& map)
            {
                return (map == map_);
            }
        );

        return ( it != pkodev::settings.maps.end() );
    };

    // Check that our character is on allowed map
    pkodev::is_on_map = check_map( std::string(switchmap.szMapName) );

    // Reset the enemies counter
    pkodev::enemies = 0;

    // Load warning image
    if (pkodev::init == false)
    {
        // Set initialization flag
        pkodev::init = true;

        // Call CGuiPic::CGuiPic(CGuiData* pParent, unsigned int max)
        pkodev::pointer::CGuiPic__CGuiPic(pkodev::img_warning_mem, nullptr, 0);

        // Call bool CGuiPic::LoadImage(const char * file, int w, int h, int frame, int tx, int ty, float scalex, float scaley)
        pkodev::pointer::CGuiPic__LoadImage(pkodev::img_warning_mem, 
            "texture\\mods\\pkodev.mod.enemy\\warning.png", 32, 32, 0, 0, 0, 0.0f, 0.0f);
    }

    // Call original function void NetSwitchMap(stNetSwitchMap &switchmap)
    pkodev::pointer::NetSwitchMap(switchmap);
}

// void CGameScene::_RenderUI()
void __fastcall pkodev::hook::CGameScene___RenderUI(void* This, void* NotUsed)
{
    // Call the original function void CGameScene::_RenderUI()
    pkodev::pointer::CGameScene___RenderUI(This);

    // Check that warning icon is enabled
    if (pkodev::init == false || pkodev::settings.warning.show == false)
    {
        // Exit from the hook
        return;
    }

    // Check map
    if (pkodev::is_on_map == true)
    {
        // Check that we should draw ememy warning icon
        if (pkodev::enemies > 0)
        {
            // Draw the icon
            pkodev::pointer::CGuiPic__Render(
                reinterpret_cast<void*>(pkodev::address::MOD_EXE_VERSION::CGuiPic),
                pkodev::settings.warning.x,
                pkodev::settings.warning.y,
                (
                    *reinterpret_cast<unsigned int*>(pkodev::img_warning_mem)
                    +
                    *reinterpret_cast<unsigned int*>(
                        reinterpret_cast<unsigned int>(pkodev::img_warning_mem) + 4
                    )
                )
            );
        }

        // Reset the enemies counter
        pkodev::enemies = 0;
    }
}