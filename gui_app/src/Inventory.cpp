#include "Inventory.h"
#include <iostream>
#include <algorithm>


#include <sstream>

namespace {
    std::vector<std::string> split(const std::string& s, char delim) {
        std::vector<std::string> tokens;
        std::stringstream ss(s);
        std::string token;
        while (std::getline(ss, token, delim)) {
            tokens.push_back(token);
        }
        return tokens;
    }
}

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

bool Inventory::useItem(int index, RPG::BaseCharacter& target) {
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


// ========== 序列化 ==========
std::string Inventory::serialize() const {
    std::string result;
    for (size_t i = 0; i < m_items.size(); ++i) {
        if (i > 0) result += ",";
        result += std::to_string(m_items[i]->getID())
               + ":" + std::to_string(m_items[i]->getQuantity());
    }
    result += "|GOLD:" + std::to_string(m_ownerGold);
    return result;
}

void Inventory::deserialize(const std::string& data) {
    // 清空背包
    while (!m_items.empty()) {
        delete m_items.back();
        m_items.pop_back();
    }

    auto parts = split(data, '|');
    if (parts.empty()) return;

    // 解析物品
    auto items = split(parts[0], ',');
    for (const auto& itemStr : items) {
        if (itemStr.empty()) continue;
        auto kv = split(itemStr, ':');
        if (kv.size() >= 2) {
            int id = std::stoi(kv[0]);
            int qty = std::stoi(kv[1]);
            // 根据 ID 重建物品
            Item* item = nullptr;
            if (id >= 1001 && id <= 1999)    item = new Food(id, "", "", 0, 0, qty);
            else if (id >= 2001 && id <= 2999) item = new Medicine(id, "", "", 0, 0, "", qty);
            else if (id >= 3001 && id <= 3999) item = new Equipment(id, "", "", 0, 0, 0, EquipmentSlot::WEAPON, qty);
            if (item) m_items.push_back(item);
        }
    }

    // 解析金币
    if (parts.size() >= 2) {
        auto goldPart = split(parts[1], ':');
        if (goldPart.size() >= 2) {
            m_ownerGold = std::stoi(goldPart[1]);
        }
    }
}
