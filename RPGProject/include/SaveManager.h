#pragma once

#include "Player.h"
#include <string>

namespace RPG {

class SaveManager {
public:
    static constexpr const char* SAVE_DIR  = "D:\\Codex\\RPGProject\\data";
    static constexpr const char* SAVE_FILE = "player_save.txt";

    static void initDataFolder();
    static bool savePlayerData(const Player& player);
    static bool loadPlayerData(Player& player);
    static bool saveExists();
    static std::string getSavePath();
};

} // namespace RPG
