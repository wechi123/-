#ifndef ITEM_H
#define ITEM_H
#include <string>
#include <iostream>

enum class ItemType { FOOD, MEDICINE, EQUIPMENT };

// ============================================================
// Character 最小结构体 - 本模块编译需要
// 队友实现完整 Character 类后，替换此结构体即可对接
// 接口需求：addHP, addGold, addAttack, addDefense, spendGold, getHP 等
// ============================================================
struct Character {
    std::string name;
    int hp = 100, maxHp = 100, gold = 100;
    int attack = 10, defense = 5;
    int level = 1, exp = 0;

    void addHP(int d)        { hp += d; if (hp > maxHp) hp = maxHp; if (hp < 0) hp = 0; }
    void addGold(int d)      { gold += d; }
    void addAttack(int d)    { attack += d; }
    void addDefense(int d)   { defense += d; }
    bool spendGold(int amt)  { if (gold >= amt) { gold -= amt; return true; } return false; }

    int getHP() const        { return hp; }
    int getMaxHP() const     { return maxHp; }
    int getGold() const      { return gold; }
    int getAttack() const    { return attack; }
    int getDefense() const   { return defense; }
    int getLevel() const     { return level; }
    int getExp() const       { return exp; }
    std::string getName() const { return name; }
    void setMaxHP(int mh)    { maxHp = mh; }
    void addExp(int d)       { exp += d; }

    void showStatus() const {
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
};

// ============================================================
// Item 抽象基类 - 所有物品的公共接口
// 封装: protected 数据成员 + public 接口
// 继承: Food / Medicine / Equipment 派生自 Item
// 多态: 纯虚函数 use() / clone() / getTypeName()
// ============================================================
class Item {
protected:
    int         m_id, m_price, m_quantity;
    std::string m_name, m_description;
    ItemType    m_type;

public:
    Item(int id, const std::string& name, const std::string& desc,
         ItemType type, int price, int quantity = 1);
    virtual ~Item();

    // 纯虚函数 - 多态核心
    virtual void use(Character& target) = 0;
    virtual Item* clone() const = 0;
    virtual std::string getTypeName() const = 0;

    // 虚函数 - 可被子类重写
    virtual std::string getInfo() const;
    virtual std::string getEffectDesc() const;

    // 通用接口
    int         getID() const       { return m_id; }
    std::string getName() const     { return m_name; }
    ItemType    getType() const     { return m_type; }
    int         getPrice() const    { return m_price; }
    int         getQuantity() const { return m_quantity; }
    void        setQuantity(int q)  { m_quantity = q; }
    void        addQuantity(int d)  { m_quantity += d; }
    void        setPrice(int p)     { m_price = p; }
};
#endif
