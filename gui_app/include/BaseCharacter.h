#pragma once

#include <string>
#include <iostream>
#include <sstream>

namespace RPG {

class BaseCharacter {
protected:
    std::string name;
    int level;
    int str, vit, agi, intl;
    int hp, maxHp, mp, maxMp;
    int def, mdef;

public:
    BaseCharacter();
    virtual ~BaseCharacter() = default;

    // 拷贝赋值（派生类深拷贝需要）
    BaseCharacter& operator=(const BaseCharacter& other);

    virtual void recalcStats();

    std::string getName() const;
    int getLevel() const;
    int getStr() const;
    int getVit() const;
    int getAgi() const;
    int getIntl() const;
    int getHp() const;
    int getMaxHp() const;
    int getMp() const;
    int getMaxMp() const;
    int getDef() const;
    int getMdef() const;
    // —— 文档接口（便捷访问）——
    int getAttack() const;      // atk = str + level
    int getDefense() const;     // 同 getDef()

    // —— 物品系统兼容接口 ——
    int getHP() const     { return hp; }        // 别名
    int getMaxHP() const  { return maxHp; }     // 别名
    void addHP(int amount) { healHp(amount); }  // 增加HP
    void addAttack(int delta) { str += delta; recalcStats(); }  // 增加攻击
    void addDefense(int delta) { def += delta; } // 增加防御

    void setName(const std::string& n);
    void setLevel(int lv);
    void setStr(int v);
    void setVit(int v);
    void setAgi(int v);
    void setIntl(int v);
    void setHp(int v);
    void setMaxHp(int v);
    void setMp(int v);
    void setMaxMp(int v);
    void setDef(int v);
    void setMdef(int v);

    void takeDamage(int damage, bool isMagic = false);
    void healHp(int amount);
    void healMp(int amount);
    void heal(int amount) { healHp(amount); }  // 文档接口
    bool isAlive() const;
    void fullRestore();

    virtual std::string serialize() const = 0;
    virtual void deserialize(const std::string& data) = 0;

    virtual void displayInfo() const;
    virtual void showInfo() const { displayInfo(); }  // 文档接口
};

} // namespace RPG
