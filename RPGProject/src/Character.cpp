#include "Character.h"
#include "GameConfig.h"
#include <sstream>
#include <algorithm>

Character::Character()
    : name("未命名"), level(GameConfig::INITIAL_LEVEL)
    , hp(GameConfig::INITIAL_HP), maxHp(GameConfig::INITIAL_MAX_HP)
    , exp(GameConfig::INITIAL_EXP), gold(GameConfig::INITIAL_GOLD)
    , attack(GameConfig::INITIAL_ATTACK), defense(GameConfig::INITIAL_DEFENSE)
{
}

void Character::create(const std::string& characterName) {
    name = characterName;
    level = GameConfig::INITIAL_LEVEL;
    hp = GameConfig::INITIAL_HP;
    maxHp = GameConfig::INITIAL_MAX_HP;
    exp = GameConfig::INITIAL_EXP;
    gold = GameConfig::INITIAL_GOLD;
    attack = GameConfig::INITIAL_ATTACK;
    defense = GameConfig::INITIAL_DEFENSE;
}

std::vector<std::string> Character::getInfoLines() const {
    std::vector<std::string> lines;
    lines.push_back("名称：" + name);
    lines.push_back("等级：Lv." + std::to_string(level));
    lines.push_back("HP  ：["
        + std::to_string(hp) + "/" + std::to_string(maxHp) + "]");
    lines.push_back("EXP ：" + std::to_string(exp)
        + " / " + std::to_string(expToNextLevel()));
    lines.push_back("金币：" + std::to_string(gold) + " G");
    lines.push_back("攻击：" + std::to_string(attack));
    lines.push_back("防御：" + std::to_string(defense));
    return lines;
}

void Character::gainExp(int amount, std::vector<LevelUpEvent>& outLevelUps) {
    exp += amount;
    levelUpIfNeeded(outLevelUps);
}

void Character::gainGold(int amount) {
    gold += amount;
}

bool Character::costGold(int amount) {
    if (gold >= amount) {
        gold -= amount;
        return true;
    }
    return false;
}

int Character::takeDamage(int damage) {
    int actualDamage = std::max(GameConfig::DAMAGE_MIN,
                                damage - defense / GameConfig::DEFENSE_DIVISOR);
    hp -= actualDamage;
    if (hp < 0) hp = 0;
    return actualDamage;
}

void Character::heal(int amount) {
    hp += amount;
    if (hp > maxHp) hp = maxHp;
}

int Character::getHealAmount(int healAmount) const {
    int need = maxHp - hp;
    return std::min(healAmount, need);
}

bool Character::isAlive() const {
    return hp > 0;
}

int Character::expToNextLevel() const {
    return GameConfig::expToLevelUp(level);
}

void Character::levelUpIfNeeded(std::vector<LevelUpEvent>& outLevelUps) {
    while (exp >= expToNextLevel()) {
        exp -= expToNextLevel();
        level++;

        LevelUpEvent evt;
        evt.newLevel = level;
        evt.hpGain   = GameConfig::hpGainPerLevel(level);
        evt.atkGain  = GameConfig::atkGainPerLevel(level);
        evt.defGain  = GameConfig::defGainPerLevel(level);

        maxHp += evt.hpGain;
        hp = maxHp;
        attack += evt.atkGain;
        defense += evt.defGain;

        outLevelUps.push_back(evt);
    }
}

std::string Character::serialize() const {
    std::ostringstream oss;
    oss << name << "|" << level << "|" << hp << "|" << maxHp << "|"
        << exp << "|" << gold << "|" << attack << "|" << defense;
    return oss.str();
}

void Character::deserialize(const std::string& data) {
    std::stringstream ss(data);
    std::string token;
    std::vector<std::string> parts;
    while (std::getline(ss, token, '|')) {
        parts.push_back(token);
    }
    if (parts.size() >= 8) {
        name    = parts[0];
        level   = std::stoi(parts[1]);
        hp      = std::stoi(parts[2]);
        maxHp   = std::stoi(parts[3]);
        exp     = std::stoi(parts[4]);
        gold    = std::stoi(parts[5]);
        attack  = std::stoi(parts[6]);
        defense = std::stoi(parts[7]);
    }
}
