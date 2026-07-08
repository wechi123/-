#include "Shop.h"
#include "Food.h"
#include "Medicine.h"
#include "Equipment.h"
#include <iostream>

Shop::Shop(const std::string& name) : m_name(name) {}
Shop::~Shop() { for (Item* item : m_shopItems) delete item; m_shopItems.clear(); }
void Shop::addItem(Item* item) { m_shopItems.push_back(item); }

void Shop::showShop() const {
    std::cout << "\n========== " << m_name << " ==========\n";
    if (m_shopItems.empty()) std::cout << "（商店暂无商品）\n";
    else for (size_t i = 0; i < m_shopItems.size(); ++i) {
        std::cout << "[" << i << "] " << m_shopItems[i]->getInfo() << "\n";
        std::cout << "----------------------------------------\n";
    }
    std::cout << "========================================\n";
}

bool Shop::buyItem(int index, Inventory& inventory) {
    if (index < 0 || index >= getItemCount()) { std::cout << "【错误】无效的商品索引！\n"; return false; }
    Item* shopItem = m_shopItems[index];
    if (inventory.isFull()) { std::cout << "【提示】背包已满，无法购买！\n"; return false; }
    int price = shopItem->getPrice();
    if (inventory.getOwnerGold() < price) {
        std::cout << "【提示】金币不足！需要 " << price
                  << " 金币，当前拥有 " << inventory.getOwnerGold() << " 金币。\n"; return false;
    }
    inventory.setOwnerGold(inventory.getOwnerGold() - price);
    Item* purchased = shopItem->clone();
    purchased->setQuantity(1);
    inventory.addItemOrStack(purchased);
    std::cout << "【购买成功】" << shopItem->getName() << "，花费 " << price << " 金币！\n";
    std::cout << "剩余金币：" << inventory.getOwnerGold() << "\n";
    return true;
}

bool Shop::sellItem(int inventoryIndex, Inventory& inventory) {
    Item* invItem = inventory.getItem(inventoryIndex);
    if (!invItem) { std::cout << "【错误】无效的物品索引！\n"; return false; }
    if (invItem->getType() == ItemType::EQUIPMENT) {
        Equipment* eq = dynamic_cast<Equipment*>(invItem);
        if (eq && eq->isEquipped()) {
            std::cout << "【提示】装备【" << invItem->getName()
                      << "】正在使用中，请先卸下再出售！\n"; return false;
        }
    }
    int sellPrice = invItem->getPrice() * 6 / 10;
    if (sellPrice < 1) sellPrice = 1;
    inventory.setOwnerGold(inventory.getOwnerGold() + sellPrice);
    std::cout << "【出售成功】" << invItem->getName() << "，获得 " << sellPrice << " 金币！\n";
    std::cout << "当前金币：" << inventory.getOwnerGold() << "\n";
    return inventory.removeItemByID(invItem->getID(), 1);
}

Item* Shop::getItem(int index) const {
    if (index < 0 || index >= getItemCount()) return nullptr;
    return m_shopItems[index];
}

void Shop::initDefaultShop() {
    addItem(new Food(1001, "面包", "松软的白面包，能恢复少量体力", 5, 20));
    addItem(new Food(1002, "鸡腿", "香喷喷的烤鸡腿，恢复较多体力", 15, 50));
    addItem(new Food(1003, "蛋糕", "精美的奶油蛋糕，恢复大量体力", 30, 100));
    addItem(new Medicine(2001, "红药水", "恢复100点HP的基础药水", 20, 100, ""));
    addItem(new Medicine(2002, "解毒剂", "解除中毒状态，并恢复30点HP", 25, 30, "中毒"));
    addItem(new Medicine(2003, "万能药", "解除所有异常状态，恢复200点HP", 80, 200, "全异常"));
    addItem(new Equipment(3001, "木剑", "新手武器，略微提升攻击力", 30, 5, 0, EquipmentSlot::WEAPON));
    addItem(new Equipment(3002, "铁剑", "坚固的铁剑，提升攻击力", 80, 15, 0, EquipmentSlot::WEAPON));
    addItem(new Equipment(3003, "布甲", "基础防具，略微提升防御力", 25, 0, 5, EquipmentSlot::ARMOR));
    addItem(new Equipment(3004, "铁甲", "坚固的铁甲，大幅提升防御力", 70, 0, 15, EquipmentSlot::ARMOR));
    addItem(new Equipment(3005, "力量戒指", "提升攻击力的饰品", 50, 10, 0, EquipmentSlot::ACCESSORY));
    addItem(new Equipment(3006, "守护项链", "提升防御力的饰品", 50, 0, 10, EquipmentSlot::ACCESSORY));
}
