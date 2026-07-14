#ifndef GAMECONFIG_H
#define GAMECONFIG_H

#include <string>

namespace GameConfig {

    constexpr int INITIAL_LEVEL     = 1;
    constexpr int INITIAL_HP        = 100;
    constexpr int INITIAL_MAX_HP    = 100;
    constexpr int INITIAL_EXP       = 0;
    constexpr int INITIAL_GOLD      = 50;
    constexpr int INITIAL_ATTACK    = 15;
    constexpr int INITIAL_DEFENSE   = 5;

    constexpr int EXP_PER_LEVEL_BASE = 100;
    constexpr int EXP_PER_LEVEL_MID  = 150;
    constexpr int EXP_PER_LEVEL_HIGH = 200;

    constexpr int LEVEL_THRESHOLD_1  = 10;
    constexpr int LEVEL_THRESHOLD_2  = 20;

    constexpr int HP_GAIN_LOW    = 20;
    constexpr int ATK_GAIN_LOW   = 5;
    constexpr int DEF_GAIN_LOW   = 3;

    constexpr int HP_GAIN_MID    = 25;
    constexpr int ATK_GAIN_MID   = 7;
    constexpr int DEF_GAIN_MID   = 4;

    constexpr int HP_GAIN_HIGH   = 30;
    constexpr int ATK_GAIN_HIGH  = 10;
    constexpr int DEF_GAIN_HIGH  = 5;

    constexpr int DAMAGE_MIN      = 1;
    constexpr int DEFENSE_DIVISOR = 2;
    constexpr int PLAYER_RAND_MAX = 10;
    constexpr int ENEMY_RAND_MAX  = 5;

    constexpr int MAX_CAPACITY = 20;
    constexpr double SELL_PRICE_RATIO = 0.5;

    const std::string PLAYER_SAVE_FILE  = "data/player_save.txt";
    const std::string SHOP_ITEMS_FILE   = "data/shop_items.txt";
    const std::string TASKS_FILE        = "data/tasks.txt";
    const std::string ENEMIES_FILE      = "data/enemies.txt";

    inline int expToLevelUp(int level) {
        if (level <= LEVEL_THRESHOLD_1)
            return level * EXP_PER_LEVEL_BASE;
        else if (level <= LEVEL_THRESHOLD_2)
            return level * EXP_PER_LEVEL_MID;
        else
            return level * EXP_PER_LEVEL_HIGH;
    }

    inline int hpGainPerLevel(int level) {
        if (level <= LEVEL_THRESHOLD_1) return HP_GAIN_LOW;
        else if (level <= LEVEL_THRESHOLD_2) return HP_GAIN_MID;
        else return HP_GAIN_HIGH;
    }

    inline int atkGainPerLevel(int level) {
        if (level <= LEVEL_THRESHOLD_1) return ATK_GAIN_LOW;
        else if (level <= LEVEL_THRESHOLD_2) return ATK_GAIN_MID;
        else return ATK_GAIN_HIGH;
    }

    inline int defGainPerLevel(int level) {
        if (level <= LEVEL_THRESHOLD_1) return DEF_GAIN_LOW;
        else if (level <= LEVEL_THRESHOLD_2) return DEF_GAIN_MID;
        else return DEF_GAIN_HIGH;
    }

}

#endif
