#include "BaseCharacter.h"
#include <algorithm>
#include <cmath>

namespace RPG {

template<typename T>
static inline const T& clamp(const T& v, const T& lo, const T& hi) {
    return (v < lo) ? lo : (v > hi) ? hi : v;
}

BaseCharacter::BaseCharacter()
    : name("未命名"), level(1),
      str(0), vit(0), agi(0), intl(0),
      hp(100), maxHp(100), mp(60), maxMp(60),
      def(0), mdef(0) {}

// 拷贝赋值
BaseCharacter& BaseCharacter::operator=(const BaseCharacter& other) {
    if (this != &other) {
        name = other.name;
        level = other.level;
        str = other.str; vit = other.vit; agi = other.agi; intl = other.intl;
        hp = other.hp; maxHp = other.maxHp; mp = other.mp; maxMp = other.maxMp;
        def = other.def; mdef = other.mdef;
    }
    return *this;
}

void BaseCharacter::recalcStats() {
    maxHp = 100 + vit * 15 + level * 10;
    maxMp = 60 + intl * 8 + level * 6;
    def = (int)std::round(vit * 1.2);
    mdef = (int)std::round(intl * 1.0);
    hp = (std::min)(hp, maxHp);
    mp = (std::min)(mp, maxMp);
}

// ---- 原有 getter ----
std::string BaseCharacter::getName() const { return name; }
int BaseCharacter::getLevel() const { return level; }
int BaseCharacter::getStr()   const { return str; }
int BaseCharacter::getVit()   const { return vit; }
int BaseCharacter::getAgi()   const { return agi; }
int BaseCharacter::getIntl()  const { return intl; }
int BaseCharacter::getHp()    const { return hp; }
int BaseCharacter::getMaxHp() const { return maxHp; }
int BaseCharacter::getMp()    const { return mp; }
int BaseCharacter::getMaxMp() const { return maxMp; }
int BaseCharacter::getDef()   const { return def; }
int BaseCharacter::getMdef()  const { return mdef; }

// ---- 文档接口 getter ----
int BaseCharacter::getAttack()  const { return str + level; }
int BaseCharacter::getDefense() const { return def; }

void BaseCharacter::setName(const std::string& n) { name = n; }
void BaseCharacter::setLevel(int lv)   { level = (std::max)(1, lv); }
void BaseCharacter::setStr(int v)      { str = (std::max)(0, v); }
void BaseCharacter::setVit(int v)      { vit = (std::max)(0, v); }
void BaseCharacter::setAgi(int v)      { agi = (std::max)(0, v); }
void BaseCharacter::setIntl(int v)     { intl = (std::max)(0, v); }
void BaseCharacter::setHp(int v)       { hp = clamp(v, 0, maxHp); }
void BaseCharacter::setMaxHp(int v)    { maxHp = (std::max)(1, v); hp = (std::min)(hp, maxHp); }
void BaseCharacter::setMp(int v)       { mp = clamp(v, 0, maxMp); }
void BaseCharacter::setMaxMp(int v)    { maxMp = (std::max)(1, v); mp = (std::min)(mp, maxMp); }
void BaseCharacter::setDef(int v)      { def = (std::max)(0, v); }
void BaseCharacter::setMdef(int v)     { mdef = (std::max)(0, v); }

void BaseCharacter::takeDamage(int damage, bool isMagic) {
    int reduction = isMagic ? mdef : def;
    int actual = (std::max)(1, damage - reduction);
    hp = (std::max)(0, hp - actual);
}

void BaseCharacter::healHp(int amount) {
    hp = (std::min)(maxHp, hp + amount);
}

void BaseCharacter::healMp(int amount) {
    mp = (std::min)(maxMp, mp + amount);
}

bool BaseCharacter::isAlive() const { return hp > 0; }

void BaseCharacter::fullRestore() { hp = maxHp; mp = maxMp; }

void BaseCharacter::displayInfo() const {
    std::cout << "\n════════════════════════════════\n";
    std::cout << "  角色: " << name << "\n";
    std::cout << "  等级: " << level << "\n";
    std::cout << "  力量: " << str << "  体质: " << vit
              << "  敏捷: " << agi << "  智力: " << intl << "\n";
    std::cout << "  HP: " << hp << "/" << maxHp << "\n";
    std::cout << "  MP: " << mp << "/" << maxMp << "\n";
    std::cout << "  物防: " << def << "  魔防: " << mdef << "\n";
    std::cout << "════════════════════════════════\n\n";
}

} // namespace RPG
