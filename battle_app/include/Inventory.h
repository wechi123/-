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
// 使用 vector<Item*> 管理物品指针???
// 支持物品的增删改查??????
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

    // ????
    bool addItem(Item* item);
    // ?????????ID???????????
    bool addItemOrStack(Item* item);
    // ?????????
    bool removeItem(int index);
    // ??????ID????????
    bool removeItemByID(int id, int quantity = 1);
    // 支持物品的增删改查??
    bool useItem(int index, Character& target);
    // ????
    void showInventory() const;
    // ????????ID?
    int findItemIndex(int id) const;
    // ??????
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
