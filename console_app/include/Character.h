#ifndef CHARACTER_H
#define CHARACTER_H

#include <string>

// ============================================================
// Character ? - ????
// ?????? Item/Inventory/Shop ?????????
// ?????? Character ????????
// ============================================================
class Character {
private:
    std::string name;
    int level, hp, maxHp, exp, gold, attack, defense;

public:
    Character(std::string name = "Player", int level = 1,
              int maxHp = 100, int attack = 10, int defense = 5);

    std::string getName() const;
    int getLevel() const;
    int getHP() const;
    int getMaxHP() const;
    int getExp() const;
    int getGold() const;
    int getAttack() const;
    int getDefense() const;

    void setHP(int hp);
    void addHP(int delta);
    void setMaxHP(int maxHp);
    void addExp(int delta);
    void addGold(int delta);
    bool spendGold(int amount);
    void addAttack(int delta);
    void addDefense(int delta);

    void showStatus() const;
};

#endif
