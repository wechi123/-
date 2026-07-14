#pragma once

#include <algorithm>  // for min/max
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>

namespace RPG {

// GCC 6.3 兼容 clamp
template <typename T>
const T& clamp(const T& value, const T& lo, const T& hi) {
    return (value < lo) ? lo : (value > hi) ? hi : value;
}

enum class CharacterClass {
    WARRIOR,
    MAGE,
    ARCHER,
    NONE
};

struct Attributes {
    int maxHp = 100;
    int attack = 10;
    int defense = 5;
    int speed = 5;
};

class Character {
private:
    std::string name;
    int level;
    int hp;
    int maxHp;
    int exp;
    int maxExp;
    int gold;
    int attack;
    int defense;
    int speed;
    CharacterClass charClass;

    static Attributes getBaseAttributes(CharacterClass cls);

public:
    Character();
    explicit Character(const std::string& name, CharacterClass charClass = CharacterClass::WARRIOR);

    std::string getName() const;
    int getLevel() const;
    int getHp() const;
    int getMaxHp() const;
    int getExp() const;
    int getMaxExp() const;
    int getGold() const;
    int getAttack() const;
    int getDefense() const;
    int getSpeed() const;
    CharacterClass getCharClass() const;

    void setName(const std::string& name);
    void setHp(int hp);
    void setMaxHp(int mhp);
    void setExp(int exp);
    void setMaxExp(int mexp);
    void setGold(int gold);
    void setLevel(int level);
    void setAttack(int atk);
    void setDefense(int def);
    void setSpeed(int spd);
    void setCharClass(CharacterClass cls);

    void takeDamage(int damage);
    void heal(int amount);
    bool isAlive() const;
    void addGold(int amount);
    bool spendGold(int amount);

    std::string serialize() const;
    static Character deserialize(const std::string& data);

    void displayInfo() const;
    static std::string classNameToString(CharacterClass cls);
    static CharacterClass stringToClass(const std::string& str);
};

} // namespace RPG
