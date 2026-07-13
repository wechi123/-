#include "Character.h"
#include <iostream>
#include <algorithm>

Character::Character(std::string name, int level, int maxHp, int attack, int defense)
    : name(name), level(level), maxHp(maxHp), hp(maxHp),
      exp(0), gold(100), attack(attack), defense(defense) {}

std::string Character::getName() const { return name; }
int Character::getLevel() const { return level; }
int Character::getHP() const { return hp; }
int Character::getMaxHP() const { return maxHp; }
int Character::getExp() const { return exp; }
int Character::getGold() const { return gold; }
int Character::getAttack() const { return attack; }
int Character::getDefense() const { return defense; }

void Character::setHP(int hp) { this->hp = std::max(0, std::min(hp, maxHp)); }
void Character::addHP(int delta) {
    hp += delta;
    if (hp > maxHp) hp = maxHp;
    if (hp < 0) hp = 0;
}
void Character::setMaxHP(int maxHp) {
    this->maxHp = std::max(1, maxHp);
    if (hp > this->maxHp) hp = this->maxHp;
}
void Character::addExp(int delta) { exp += delta; }
void Character::addGold(int delta) { gold += delta; }
bool Character::spendGold(int amount) {
    if (gold >= amount) { gold -= amount; return true; }
    return false;
}
void Character::addAttack(int delta) { attack += delta; }
void Character::addDefense(int delta) { defense += delta; }

void Character::showStatus() const {
    std::cout << "\n========== 角色状态 ==========\n";
    std::cout << "名称：" << name << "\n";
    std::cout << "等级：" << level << "\n";
    std::cout << "生命：" << hp << " / " << maxHp << "\n";
    std::cout << "经验：" << exp << "\n";
    std::cout << "金币：" << gold << "\n";
    std::cout << "攻击：" << attack << "\n";
    std::cout << "防御：" << defense << "\n";
    std::cout << "===============================\n";
}
