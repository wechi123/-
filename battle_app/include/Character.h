#ifndef CHARACTER_H
#define CHARACTER_H

#include <string>
#include <vector>

struct LevelUpEvent {
    int newLevel;
    int hpGain;
    int atkGain;
    int defGain;
};

class Character {
private:
    std::string name;
    int level;
    int hp;
    int maxHp;
    int exp;
    int gold;
    int attack;
    int defense;

public:
    Character();

    void create(const std::string& characterName);

    std::vector<std::string> getInfoLines() const;

    void gainExp(int amount, std::vector<LevelUpEvent>& outLevelUps);
    void gainExp(int amount);  // 便捷重载（忽略升级事件）
    void gainGold(int amount);
    bool costGold(int amount);
    int takeDamage(int damage);
    void heal(int amount);
    bool isAlive() const;

    void levelUpIfNeeded(std::vector<LevelUpEvent>& outLevelUps);
    int expToNextLevel() const;

    int getHealAmount(int healAmount) const;

    std::string serialize() const;
    void deserialize(const std::string& data);

    const std::string& getName() const { return name; }
    int getLevel() const { return level; }
    int getHp() const { return hp; }
    int getMaxHp() const { return maxHp; }
    int getExp() const { return exp; }
    int getGold() const { return gold; }
    int getAttack() const { return attack; }
    int getDefense() const { return defense; }

    void setName(const std::string& n) { name = n; }
    void setHp(int value) { hp = value; }

    // ---- 物品系统兼容接口 ----
    int getHP() const { return hp; }
    int getMaxHP() const { return maxHp; }
    void addHP(int amount) { heal(amount); }
    void addAttack(int delta) { attack += delta; }
    void addDefense(int delta) { defense += delta; }
    void setMaxHp(int value) { maxHp = value; }
};

#endif
