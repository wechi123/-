#include "Character.h"
#include <algorithm>

namespace RPG {

Attributes Character::getBaseAttributes(CharacterClass cls) {
    switch (cls) {
        case CharacterClass::WARRIOR:
            return {150, 15, 10, 3};
        case CharacterClass::MAGE:
            return {80, 25, 3, 5};
        case CharacterClass::ARCHER:
            return {100, 18, 6, 8};
        default:
            return {100, 10, 5, 5};
    }
}

std::string Character::classNameToString(CharacterClass cls) {
    switch (cls) {
        case CharacterClass::WARRIOR: return "战士";
        case CharacterClass::MAGE:    return "法师";
        case CharacterClass::ARCHER:  return "弓箭手";
        default:                      return "无职业";
    }
}

CharacterClass Character::stringToClass(const std::string& str) {
    if (str == "战士")   return CharacterClass::WARRIOR;
    if (str == "法师")   return CharacterClass::MAGE;
    if (str == "弓箭手") return CharacterClass::ARCHER;
    return CharacterClass::NONE;
}

Character::Character()
    : name("未命名"), level(1), hp(100), maxHp(100),
      exp(0), maxExp(100), gold(0),
      attack(10), defense(5), speed(5),
      charClass(CharacterClass::NONE) {}

Character::Character(const std::string& name, CharacterClass charClass)
    : name(name), level(1), exp(0), gold(0),
      charClass(charClass)
{
    Attributes base = getBaseAttributes(charClass);
    maxHp   = base.maxHp;
    hp      = maxHp;
    attack  = base.attack;
    defense = base.defense;
    speed   = base.speed;
    maxExp  = 100;
}

std::string Character::getName() const { return name; }
int Character::getLevel()    const { return level; }
int Character::getHp()       const { return hp; }
int Character::getMaxHp()    const { return maxHp; }
int Character::getExp()      const { return exp; }
int Character::getMaxExp()   const { return maxExp; }
int Character::getGold()     const { return gold; }
int Character::getAttack()   const { return attack; }
int Character::getDefense()  const { return defense; }
int Character::getSpeed()    const { return speed; }
CharacterClass Character::getCharClass() const { return charClass; }

void Character::setName(const std::string& n)   { name = n; }
void Character::setHp(int h)                    { hp = clamp(h, 0, maxHp); }
void Character::setMaxHp(int m)                 { maxHp = m; hp = (std::min)(hp, maxHp); }
void Character::setExp(int e)                   { exp = (std::max)(0, e); }
void Character::setMaxExp(int m)                { maxExp = (std::max)(1, m); }
void Character::setGold(int g)                  { gold = (std::max)(0, g); }
void Character::setLevel(int l)                 { level = (std::max)(1, l); }
void Character::setAttack(int a)                { attack = (std::max)(0, a); }
void Character::setDefense(int d)               { defense = (std::max)(0, d); }
void Character::setSpeed(int s)                 { speed = (std::max)(0, s); }
void Character::setCharClass(CharacterClass c)  { charClass = c; }

void Character::takeDamage(int damage) {
    int actualDamage = (std::max)(1, damage - defense);
    hp = (std::max)(0, hp - actualDamage);
}

void Character::heal(int amount) {
    hp = (std::min)(maxHp, hp + amount);
}

bool Character::isAlive() const {
    return hp > 0;
}

void Character::addGold(int amount) {
    gold += (std::max)(0, amount);
}

bool Character::spendGold(int amount) {
    if (amount < 0 || gold < amount) return false;
    gold -= amount;
    return true;
}

std::string Character::serialize() const {
    std::ostringstream oss;
    oss << name << "\n"
        << level << " " << hp << " " << maxHp << " "
        << exp << " " << maxExp << " "
        << gold << " "
        << attack << " " << defense << " " << speed << "\n"
        << classNameToString(charClass) << "\n";
    return oss.str();
}

Character Character::deserialize(const std::string& data) {
    std::istringstream iss(data);
    Character ch;
    std::string clsStr;

    std::getline(iss, ch.name);
    iss >> ch.level >> ch.hp >> ch.maxHp
        >> ch.exp >> ch.maxExp
        >> ch.gold
        >> ch.attack >> ch.defense >> ch.speed;
    iss.ignore();
    std::getline(iss, clsStr);
    ch.charClass = stringToClass(clsStr);

    return ch;
}

void Character::displayInfo() const {
    std::cout << "\n══════════════════════════════════════\n";
    std::cout << "             角色信息\n";
    std::cout << "══════════════════════════════════════\n";
    std::cout << "  名称: " << name << "\n";
    std::cout << "  职业: " << classNameToString(charClass) << "\n";
    std::cout << "  等级: " << level << "\n";
    std::cout << "  生命: " << hp << " / " << maxHp << "\n";
    std::cout << "  经验: " << exp << " / " << maxExp << "\n";
    std::cout << "  金币: " << gold << "\n";
    std::cout << "  攻击: " << attack << "  防御: " << defense << "  速度: " << speed << "\n";
    std::cout << "══════════════════════════════════════\n\n";
}

} // namespace RPG
