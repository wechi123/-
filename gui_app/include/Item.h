#ifndef ITEM_H
#define ITEM_H

#include <string>
#include "BaseCharacter.h"

// ============================================================
// ItemType 枚举 - 物品类型
// ============================================================
enum class ItemType {
    FOOD,       // 食物
    MEDICINE,   // 食物
    EQUIPMENT   // 食物
};

// ============================================================
// Item 基类 - 所有物品的抽象父类
// 基类：protected 属性 + public 虚函数接口
// 子类：Food(食物) / Medicine(药品) / Equipment(装备)
// 虚方法：use() / clone() / getTypeName() / getInfo()
// ============================================================
class Item {
protected:
    int         m_id;
    std::string m_name;
    std::string m_description;
    ItemType    m_type;
    int         m_price;
    int         m_quantity;

public:
    Item(int id, const std::string& name, const std::string& desc,
         ItemType type, int price, int quantity = 1);
    virtual ~Item();

    // 构造 - 创建物品
    virtual void use(RPG::BaseCharacter& target) = 0;
    virtual Item* clone() const = 0;
    virtual std::string getTypeName() const = 0;

    // 访问器 - 获取属性
    virtual std::string getInfo() const;
    virtual std::string getEffectDesc() const;

    // 操作接口
    int getID() const           { return m_id; }
    std::string getName() const { return m_name; }
    std::string getDescription() const { return m_description; }
    ItemType getType() const    { return m_type; }
    int getPrice() const        { return m_price; }
    int getQuantity() const     { return m_quantity; }
    void setQuantity(int qty)   { m_quantity = qty; }
    void addQuantity(int delta) { m_quantity += delta; }
    void setPrice(int price)    { m_price = price; }
};

#endif
