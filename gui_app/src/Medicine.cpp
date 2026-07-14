#include "Medicine.h"
#include <iostream>
#include <sstream>

Medicine::Medicine(int id, const std::string& name, const std::string& desc,
                   int price, int hpRestore, const std::string& cureEffect, int quantity)
    : Item(id, name, desc, ItemType::MEDICINE, price, quantity),
      m_hpRestore(hpRestore), m_cureEffect(cureEffect), m_hasCure(!cureEffect.empty()) {}

void Medicine::use(RPG::BaseCharacter& target) {
    std::cout << "\n>>> 使用药品【" << m_name << "】\n";
    int before = target.getHP();
    target.addHP(m_hpRestore);
    std::cout << "恢复了 " << (target.getHP() - before) << " 点生命值！\n";
    if (m_hasCure) std::cout << "治愈了【" << m_cureEffect << "】状态！\n";
    std::cout << "当前HP：" << target.getHP() << " / " << target.getMaxHP() << "\n";
    m_quantity--;
}
Item* Medicine::clone() const { return new Medicine(*this); }
std::string Medicine::getInfo() const { return Item::getInfo(); }
std::string Medicine::getEffectDesc() const {
    std::ostringstream oss;
    oss << "恢复 " << m_hpRestore << " 点生命值";
    if (m_hasCure) oss << "，治愈【" << m_cureEffect << "】";
    return oss.str();
}
