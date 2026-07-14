#ifndef INVENTORY_H
#define INVENTORY_H

#include <vector>
#include <string>
#include "Item.h"
#include "Food.h"
#include "Medicine.h"
#include "Equipment.h"

// ============================================================
// Inventory 背包类
// 使用 vector<Item*> 管理物品指针
// 支持物品的增删改查
// ============================================================
class Inventory {
private:
    std::vector<Item*>  m_items;
    int                 m_maxCapacity;
    int                 m_ownerGold;

public:
    Inventory(int maxCapacity = 30);
    ~Inventory();

    Inventory(const Inventory&) = delete;
    Inventory& operator=(const Inventory&) = delete;

    // 添加物品
    bool addItem(Item* item);
    // 根据物品ID添加（自动堆叠）
    bool addItemOrStack(Item* item);
    // 删除物品（按索引）
    bool removeItem(int index);
    // 根据物品ID删除
    bool removeItemByID(int id, int quantity = 1);
    // 使用物品（多态调用）
    bool useItem(int index, RPG::BaseCharacter& target);
    // 显示背包
    void showInventory() const;
    // 查找物品索引
    int findItemIndex(int id) const;
    // 获取物品指针
    Item* getItem(int index) const;
    Item* getItemByID(int id) const;

    int getItemCount() const    { return (int)m_items.size(); }
    int getMaxCapacity() const  { return m_maxCapacity; }
    bool isFull() const         { return getItemCount() >= m_maxCapacity; }
    bool isEmpty() const        { return m_items.empty(); }
    const std::vector<Item*>& getItems() const { return m_items; }

    void setOwnerGold(int gold) { m_ownerGold = gold; }
    int getOwnerGold() const    { return m_ownerGold; }

    // 序列化（存档用）
    std::string serialize() const;
    void deserialize(const std::string& data);
};

#endif
