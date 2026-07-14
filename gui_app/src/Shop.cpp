#include "Shop.h"
#include "Food.h"
#include "Medicine.h"
#include "Equipment.h"
#include <iostream>

Shop::Shop(const std::string& name) : m_name(name) {}

Shop::~Shop() { for (auto* item : m_shopItems) delete item; }

void Shop::addItem(Item* item) { m_shopItems.push_back(item); }

void Shop::showShop() const {
    std::cout << "\n===== " << m_name << " =====" << std::endl;
    std::cout << "序号 | 类型 | 名称 | 价格 | 效果" << std::endl;
    std::cout << "--------------------------------" << std::endl;
    for (size_t i = 0; i < m_shopItems.size(); ++i) {
        Item* it = m_shopItems[i];
        std::cout << (i+1) << "     [" << it->getTypeName() << "] "
                  << it->getName() << "  " << it->getPrice() << "G  "
                  << it->getEffectDesc() << std::endl;
    }
}

Item* Shop::getItem(int index) const {
    if (index < 0 || index >= (int)m_shopItems.size()) return nullptr;
    return m_shopItems[index];
}

bool Shop::buyItem(int index, Inventory& inventory) {
    Item* item = getItem(index);
    if (!item) { std::cout << "商品不存在！" << std::endl; return false; }
    int price = item->getPrice();
    if (inventory.getOwnerGold() < price) {
        std::cout << "金币不足！需要 " << price << " 金币（当前 " << inventory.getOwnerGold() << "）" << std::endl;
        return false;
    }
    if (inventory.isFull()) { std::cout << "背包已满！" << std::endl; return false; }
    inventory.setOwnerGold(inventory.getOwnerGold() - price);
    Item* purchased = item->clone();
    inventory.addItemOrStack(purchased);
    std::cout << "购买成功！获得【" << purchased->getName() << "】，剩余金币：" << inventory.getOwnerGold() << std::endl;
    return true;
}

bool Shop::sellItem(int inventoryIndex, Inventory& inventory) {
    Item* invItem = inventory.getItem(inventoryIndex);
    if (!invItem) { std::cout << "物品不存在！" << std::endl; return false; }
    int sellPrice = invItem->getPrice() / 2;
    inventory.setOwnerGold(inventory.getOwnerGold() + sellPrice);
    std::cout << "出售【" << invItem->getName() << "】，获得 " << sellPrice << " 金币" << std::endl;
    return inventory.removeItemByID(invItem->getID(), 1);
}

void Shop::initDefaultShop() {
    // === 食物类 ===
    addItem(new Food(1001, "校园面包",  "校园食堂基础面包，充饥回血，性价比高", 15, 25));
    addItem(new Food(1002, "清甜牛奶",  "营养牛奶，快速恢复少量体力", 25, 40));
    addItem(new Food(1003, "能量饭团",  "高能量饭团，适合冒险战斗后快速回血", 40, 65));
    // === 药品类 ===
    addItem(new Medicine(2001, "基础创可贴", "简易医疗用品，应对轻微受伤", 30, 50, ""));
    addItem(new Medicine(2002, "校园急救喷雾", "校园医务室常备药品，回血效果优秀", 60, 100, ""));
    addItem(new Medicine(2003, "全能恢复药剂", "高级医疗药剂，绝境翻盘道具", 120, 9999, ""));
    // === 装备类 ===
    addItem(new Equipment(3001, "新手校服护甲", "冒险者初始基础防具", 80,  0,  8, EquipmentSlot::ARMOR));
    addItem(new Equipment(3002, "运动护腕",    "锻炼专用护腕，提升战斗输出", 150, 12, 0, EquipmentSlot::WEAPON));
    addItem(new Equipment(3003, "学霸护身徽章", "校园高级装备，综合提升战斗能力", 280, 15, 10, EquipmentSlot::ACCESSORY));
}