#include "LevelSystem.h"
#include <iostream>

namespace RPG {

int LevelSystem::expRequiredForLevel(int level) {
    if (level <= 1) return 100;
    return 100 + (level - 1) * 80;
}

void LevelSystem::gainExp(Player& player, int amount) {
    if (amount <= 0) return;
    player.addExp(amount);
}

void LevelSystem::showLevelUpResult(const Player& before, const Player& after) {
    std::cout << "\n────── 成长结果 ──────\n";
    std::cout << "  等级: " << before.getLevel() << " → " << after.getLevel() << "\n";
    std::cout << "  HP: " << before.getMaxHp() << " → " << after.getMaxHp() << "\n";
    std::cout << "  MP: " << before.getMaxMp() << " → " << after.getMaxMp() << "\n";
    std::cout << "  力量: " << before.getStr() << " → " << after.getStr() << "\n";
    std::cout << "  体质: " << before.getVit() << " → " << after.getVit() << "\n";
    std::cout << "  敏捷: " << before.getAgi() << " → " << after.getAgi() << "\n";
    std::cout << "  智力: " << before.getIntl() << " → " << after.getIntl() << "\n";
    std::cout << "────────────────────────\n\n";
}

} // namespace RPG
