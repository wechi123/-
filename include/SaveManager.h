#pragma once

#include "Player.h"
#include <string>

namespace RPG {

class SaveManager {
public:
    static constexpr const char* SAVE_DIR  = "data";
    static constexpr const char* SAVE_FILE = "player_save.txt";

    static void initDataFolder();
    // —— 文档接口 ——
    static bool savePlayer(const Player& player);
    static bool loadPlayer(Player& player);
    // —— 兼容旧接口 ——
    static bool savePlayerData(const Player& player) { return savePlayer(player); }
    static bool loadPlayerData(Player& player) { return loadPlayer(player); }
    // —— 全量存读（预留集成接口）——
    static bool saveAll(const Player& player);
    static bool loadAll(Player& player);
    // —— 工具 ——
    static bool saveExists();
    static std::string getSavePath();
};

} // namespace RPG
