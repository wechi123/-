#include "Inventory.h"
#include <algorithm>

Inventory::Inventory(int maxCapacity) : m_maxCapacity(maxCapacity), m_ownerGold(0) {}
Inventory::~Inventory() { for(auto* item:m_items) delete item; }

bool Inventory::addItem(Item* item) {
    if(isFull()) return false;
    m_items.push_back(item);
    return true;
}
bool Inventory::addItemOrStack(Item* item) {
    if(item->getType()==ItemType::EQUIPMENT||isFull()) return addItem(item);
    for(auto* it:m_items) {
        if(it->getID()==item->getID()&&it->getType()!=ItemType::EQUIPMENT) {
            it->addQuantity(item->getQuantity());
            delete item;
            return true;
        }
    }
    return addItem(item);
}
bool Inventory::removeItem(int index) {
    if(index<0||index>=(int)m_items.size()) return false;
    delete m_items[index];
    m_items.erase(m_items.begin()+index);
    return true;
}
bool Inventory::removeItemByID(int id, int quantity) {
    for(size_t i=0;i<m_items.size();++i) {
        if(m_items[i]->getID()==id) {
            m_items[i]->addQuantity(-quantity);
            if(m_items[i]->getQuantity()<=0) return removeItem((int)i);
            return true;
        }
    }
    return false;
}
bool Inventory::useItem(int index, RPG::BaseCharacter& target) {
    Item* item=getItem(index);
    if(!item) return false;
    item->use(target);
    if(item->getType()!=ItemType::EQUIPMENT&&item->getQuantity()<=0) removeItem(index);
    return true;
}
int Inventory::findItemIndex(int id) const {
    for(size_t i=0;i<m_items.size();++i) if(m_items[i]->getID()==id) return (int)i;
    return -1;
}
Item* Inventory::getItem(int index) const {
    if(index<0||index>=(int)m_items.size()) return nullptr;
    return m_items[index];
}
Item* Inventory::getItemByID(int id) const {
    int idx=findItemIndex(id);
    return idx>=0?m_items[idx]:nullptr;
}
