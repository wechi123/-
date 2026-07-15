#include "Shop.h"
#include "Food.h"
#include "Medicine.h"
#include "Equipment.h"

Shop::Shop(const std::string& name) : m_name(name) {}
Shop::~Shop() { for (auto* item : m_shopItems) delete item; }
void Shop::addItem(Item* item) { m_shopItems.push_back(item); }
Item* Shop::getItem(int index) const {
    if (index < 0 || index >= (int)m_shopItems.size()) return nullptr;
    return m_shopItems[index];
}
bool Shop::buyItem(int index, Inventory& inventory) {
    Item* item = getItem(index);
    if (!item) return false;
    int price = item->getPrice();
    if (inventory.getOwnerGold() < price) return false;
    if (inventory.isFull()) return false;
    inventory.setOwnerGold(inventory.getOwnerGold() - price);
    Item* purchased = item->clone();
    inventory.addItemOrStack(purchased);
    return true;
}
bool Shop::sellItem(int inventoryIndex, Inventory& inventory) {
    Item* invItem = inventory.getItem(inventoryIndex);
    if (!invItem) return false;
    int sellPrice = invItem->getPrice() / 2;
    inventory.setOwnerGold(inventory.getOwnerGold() + sellPrice);
    return inventory.removeItemByID(invItem->getID(), 1);
}
void Shop::initDefaultShop() {
    addItem(new Food(1001, L"校园面包",  L"校园食堂基础面包，充饥回血，性价比高", 15, 25));
    addItem(new Food(1002, L"清甜牛奶",  L"营养牛奶，快速恢复少量体力", 25, 40));
    addItem(new Food(1003, L"能量饭团",  L"高能量饭团，适合冒险战斗后快速回血", 40, 65));
    addItem(new Medicine(2001, L"基础创可贴", L"简易医疗用品，应对轻微受伤", 30, 50, L""));
    addItem(new Medicine(2002, L"校园急救喷雾", L"校园医务室常备药品，回血效果优秀", 60, 100, L""));
    addItem(new Medicine(2003, L"全能恢复药剂", L"高级医疗药剂，绝境翻盘道具", 120, 9999, L""));
    addItem(new Equipment(3001, L"新手校服护甲", L"冒险者初始基础防具", 80, 0, 8, EquipmentSlot::ARMOR));
    addItem(new Equipment(3002, L"运动护腕", L"锻炼专用护腕，提升战斗输出", 150, 12, 0, EquipmentSlot::WEAPON));
    addItem(new Equipment(3003, L"学霸护身徽章", L"校园高级装备，综合提升战斗能力", 280, 15, 10, EquipmentSlot::ACCESSORY));
}
