#include "Food.h"
#include <iostream>
#include <sstream>

Food::Food(int id, const std::string& name, const std::string& desc,
           int price, int hpRestore, int quantity)
    : Item(id, name, desc, ItemType::FOOD, price, quantity), m_hpRestore(hpRestore) {}

void Food::use(RPG::BaseCharacter& target) {
    int before = target.getHP();
    target.addHP(m_hpRestore);
    int healed = target.getHP() - before;
    std::cout << "\n>>> 使用食物【" << m_name << "】\n";
    std::cout << "恢复了 " << healed << " 点生命值！\n";
    std::cout << "当前HP：" << target.getHP() << " / " << target.getMaxHP() << "\n";
    m_quantity--;
}
Item* Food::clone() const { return new Food(*this); }
std::string Food::getInfo() const { return Item::getInfo(); }
std::string Food::getEffectDesc() const {
    std::ostringstream oss;
    oss << "恢复 " << m_hpRestore << " 点生命值";
    return oss.str();
}
