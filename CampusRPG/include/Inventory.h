#ifndef INVENTORY_H
#define INVENTORY_H

#include <vector>
#include <string>
#include "Item.h"
#include "Food.h"
#include "Medicine.h"
#include "Equipment.h"

// ============================================================
// Inventory ?????
// ?? vector<Item*> ???????????????
// ??????????????????????
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
    // ??????????????????
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
};

#endif
