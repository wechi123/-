#ifndef SHOP_H
#define SHOP_H

#include <vector>
#include <string>
#include "Item.h"
#include "Inventory.h"

// ============================================================
// Shop 商店类
// 实现物品的买卖交易功能
// ============================================================
class Shop {
private:
    std::string         m_name;
    std::vector<Item*>  m_shopItems;

public:
    Shop(const std::string& name = "校园商店");
    ~Shop();

    Shop(const Shop&) = delete;
    Shop& operator=(const Shop&) = delete;

    void addItem(Item* item);
    void showShop() const;
    bool buyItem(int index, Inventory& inventory);
    bool sellItem(int inventoryIndex, Inventory& inventory);

    std::string getName() const { return m_name; }
    int getItemCount() const    { return (int)m_shopItems.size(); }
    const std::vector<Item*>& getItems() const { return m_shopItems; }
    Item* getItem(int index) const;

    void initDefaultShop();
};

#endif
