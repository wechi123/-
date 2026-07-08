#include "Inventory.h"
#include <iostream>
#include <algorithm>

Inventory::Inventory(int maxCapacity) : m_maxCapacity(maxCapacity), m_ownerGold(0) {}
Inventory::~Inventory() { for (Item* item : m_items) delete item; m_items.clear(); }

bool Inventory::addItem(Item* item) {
    if (isFull()) {
        std::cout << "【提示】背包已满（" << m_maxCapacity << "），无法获得【"
                  << item->getName() << "】！\n";
        return false;
    }
    m_items.push_back(item);
    std::cout << "【获得物品】" << item->getName() << " x" << item->getQuantity() << "\n";
    return true;
}

bool Inventory::addItemOrStack(Item* item) {
    int index = findItemIndex(item->getID());
    if (index != -1) {
        m_items[index]->addQuantity(item->getQuantity());
        std::cout << "【获得物品】" << item->getName()
                  << " x" << item->getQuantity()
                  << "（堆叠后：" << m_items[index]->getQuantity() << "）\n";
        delete item; return true;
    }
    return addItem(item);
}

bool Inventory::removeItem(int index) {
    if (index < 0 || index >= getItemCount()) {
        std::cout << "【错误】无效的物品索引！\n"; return false;
    }
    std::string name = m_items[index]->getName();
    delete m_items[index]; m_items.erase(m_items.begin() + index);
    std::cout << "【删除物品】" << name << "\n";
    return true;
}

bool Inventory::removeItemByID(int id, int quantity) {
    int index = findItemIndex(id);
    if (index == -1) { std::cout << "【错误】背包中没有该物品！\n"; return false; }
    Item* item = m_items[index];
    if (item->getQuantity() <= quantity) return removeItem(index);
    item->addQuantity(-quantity);
    std::cout << "【使用物品】" << item->getName() << "，剩余 " << item->getQuantity() << " 个\n";
    return true;
}

bool Inventory::useItem(int index, Character& target) {
    if (index < 0 || index >= getItemCount()) {
        std::cout << "【错误】无效的物品索引！\n"; return false;
    }
    Item* item = m_items[index];
    item->use(target);  // 多态调用
    if (item->getQuantity() <= 0) { delete m_items[index]; m_items.erase(m_items.begin() + index); }
    return true;
}

void Inventory::showInventory() const {
    std::cout << "\n========== 背包（" << getItemCount() << " / " << m_maxCapacity << "）==========\n";
    std::cout << "金币：" << m_ownerGold << "\n";
    if (isEmpty()) {
        std::cout << "（空）\n";
    } else {
        for (size_t i = 0; i < m_items.size(); ++i) {
            std::cout << "[" << i << "] " << m_items[i]->getInfo() << "\n";
            std::cout << "----------------------------------------\n";
        }
    }
    std::cout << "========================================\n";
}

int Inventory::findItemIndex(int id) const {
    for (size_t i = 0; i < m_items.size(); ++i)
        if (m_items[i]->getID() == id) return (int)i;
    return -1;
}

Item* Inventory::getItem(int index) const {
    if (index < 0 || index >= getItemCount()) return nullptr;
    return m_items[index];
}

Item* Inventory::getItemByID(int id) const {
    int idx = findItemIndex(id);
    return (idx != -1) ? m_items[idx] : nullptr;
}
