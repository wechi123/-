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
    if (m_shopItems.empty()) std::cout << "（商店暫無商品）\n";
    else for (size_t i = 0; i < m_shopItems.size(); ++i) {
        std::cout << "[" << i << "] " << m_shopItems[i]->getInfo() << "\n";
        std::cout << "----------------------------------------\n";
    }
    std::cout << "========================================\n";
}

bool Shop::buyItem(int index, Inventory& inventory) {
    if (index < 0 || index >= getItemCount()) {
        std::cout << "【错误】无效的商品索引！\n"; return false;
    }
    Item* shopItem = m_shopItems[index];
    if (inventory.isFull()) {
        std::cout << "【提示】背包背包已满，无法购买！\n"; return false;
    }
    int price = shopItem->getPrice();
    if (inventory.getOwnerGold() < price) {
        std::cout << "【提示】金币不足！需要 " << price
                  << " 金币，当前拥有 " << inventory.getOwnerGold() << " 金币。\n";
        return false;
    }
    inventory.setOwnerGold(inventory.getOwnerGold() - price);
    Item* purchased = shopItem->clone();
    purchased->setQuantity(1);
    inventory.addItemOrStack(purchased);
    std::cout << "【购买成功】" << shopItem->getName()
              << "，花費 " << price << " 金币！\n";
    std::cout << "剩余金币：" << inventory.getOwnerGold() << "\n";
    return true;
}

bool Shop::sellItem(int inventoryIndex, Inventory& inventory) {
    Item* invItem = inventory.getItem(inventoryIndex);
    if (!invItem) {
        std::cout << "【错误】无效的物品索引！\n"; return false;
    }
    if (invItem->getType() == ItemType::EQUIPMENT) {
        Equipment* eq = dynamic_cast<Equipment*>(invItem);
        if (eq && eq->isEquipped()) {
            std::cout << "【提示】装备【" << invItem->getName()
                      << "】正在使用中，請先卸下再出售！\n"; return false;
        }
    }
    int sellPrice = invItem->getPrice() * 6 / 10;
    if (sellPrice < 1) sellPrice = 1;
    inventory.setOwnerGold(inventory.getOwnerGold() + sellPrice);
    std::cout << "【出售成功】" << invItem->getName()
              << "，获得 " << sellPrice << " 金币！\n";
    std::cout << "当前金币：" << inventory.getOwnerGold() << "\n";
    return inventory.removeItemByID(invItem->getID(), 1);
}

Item* Shop::getItem(int index) const {
    if (index < 0 || index >= getItemCount()) return nullptr;
    return m_shopItems[index];
}

void Shop::initDefaultShop() {
    addItem(new Food(1001, "面包", "松軟的白面包，能恢復少量體力", 5, 20));
    addItem(new Food(1002, "雞腿", "香噴噴的烤雞腿，恢復較多體力", 15, 50));
    addItem(new Food(1003, "蛋糕", "精美的奶油蛋糕，恢復大量體力", 30, 100));
    addItem(new Medicine(2001, "紅藥水", "恢復100點HP的基礎藥水", 20, 100, ""));
    addItem(new Medicine(2002, "解毒劑", "解除中毒狀態，並恢復30點HP", 25, 30, "中毒"));
    addItem(new Medicine(2003, "萬能藥", "解除所有異常狀態，恢復200點HP", 80, 200, "全异常"));
    addItem(new Equipment(3001, "木劍", "新手武器，略微提升攻擊力", 30, 5, 0, EquipmentSlot::WEAPON));
    addItem(new Equipment(3002, "鐵劍", "堅固的鐵劍，提升攻擊力", 80, 15, 0, EquipmentSlot::WEAPON));
    addItem(new Equipment(3003, "布甲", "基礎防具，略微提升防禦力", 25, 0, 5, EquipmentSlot::ARMOR));
    addItem(new Equipment(3004, "鐵甲", "堅固的鐵甲，大幅提升防禦力", 70, 0, 15, EquipmentSlot::ARMOR));
    addItem(new Equipment(3005, "力量戒指", "提升攻擊力的飾品", 50, 10, 0, EquipmentSlot::ACCESSORY));
    addItem(new Equipment(3006, "守護項鏈", "提升防禦力的飾品", 50, 0, 10, EquipmentSlot::ACCESSORY));
}
