#include "SaveManager.h"
#include <windows.h>
#include <fstream>
#include <iostream>
#include <sstream>

namespace RPG {

void SaveManager::initDataFolder() {
    CreateDirectoryA(SAVE_DIR, NULL);
}

std::string SaveManager::getSavePath() {
    initDataFolder();
    return std::string(SAVE_DIR) + "\\" + SAVE_FILE;
}

bool SaveManager::savePlayerData(const Player& player) {
    std::string path = getSavePath();
    std::ofstream ofs(path);
    if (!ofs.is_open()) {
        std::cerr << "[SaveManager] 错误：无法写入存档 " << path << "\n";
        return false;
    }
    ofs << player.serialize();
    ofs.close();
    std::cout << "[SaveManager] ✅ 存档已保存至 " << path << "\n";
    return true;
}

bool SaveManager::loadPlayerData(Player& player) {
    std::string path = getSavePath();
    std::ifstream ifs(path);
    if (!ifs.is_open()) {
        std::cout << "[SaveManager] 提示：无存档文件 " << path << "，将创建新角色\n";
        return false;
    }

    std::stringstream buffer;
    buffer << ifs.rdbuf();
    ifs.close();

    if (buffer.str().empty()) {
        std::cerr << "[SaveManager] 错误：存档文件为空\n";
        return false;
    }

    // 无 try/catch，异常已通过 -fno-exceptions 关闭
    player.deserialize(buffer.str());

    std::cout << "[SaveManager] ✅ 存档已加载：" << path << "\n";
    player.displayInfo();
    return true;
}

bool SaveManager::saveExists() {
    std::string path = getSavePath();
    DWORD attr = GetFileAttributesA(path.c_str());
    return (attr != INVALID_FILE_ATTRIBUTES && !(attr & FILE_ATTRIBUTE_DIRECTORY));
}

} // namespace RPG
