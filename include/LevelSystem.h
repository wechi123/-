#pragma once

#include "Player.h"

namespace RPG {

// 等级成长辅助（调用 Player 内建逻辑）
struct LevelSystem {
    // 计算指定等级所需的经验值
    static int expRequiredForLevel(int level);
    // 给玩家增加经验（自动检测升级）
    static void gainExp(Player& player, int amount);
    // 显示升级结果对比
    static void showLevelUpResult(const Player& before, const Player& after);
};

} // namespace RPG
