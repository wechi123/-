#include "Equipment.h"
#include <iostream>
#include <sstream>

Equipment::Equipment(int id, const std::string& name, const std::string& desc,
                     int price, int attackBonus, int defenseBonus,
                     EquipmentSlot slot, int quantity)
    : Item(id, name, desc, ItemType::EQUIPMENT, price, quantity),
      m_attackBonus(attackBonus), m_defenseBonus(defenseBonus),
      m_slot(slot), m_equipped(false) {}

void Equipment::use(RPG::BaseCharacter& target) {
    if (m_equipped) {
        std::cout << "\n>>> 【" << m_name << "】已装备，无需重复装备。\n";
        return;
    }
    std::cout << "\n>>> 装备【" << m_name << "】到【" << slotToString(m_slot) << "】槽位\n";
    if (m_attackBonus > 0) { target.addAttack(m_attackBonus); std::cout << "攻击力 +" << m_attackBonus << "\n"; }
    if (m_defenseBonus > 0) { target.addDefense(m_defenseBonus); std::cout << "防御力 +" << m_defenseBonus << "\n"; }
    m_equipped = true;
    std::cout << "装备成功！\n";
}

void Equipment::unequip(RPG::BaseCharacter& target) {
    if (!m_equipped) {
        std::cout << "【" << m_name << "】未装备，无需卸下。\n";
        return;
    }
    std::cout << "\n>>> 卸下装备【" << m_name << "】\n";
    if (m_attackBonus > 0) { target.addAttack(-m_attackBonus); std::cout << "攻击力 -" << m_attackBonus << "\n"; }
    if (m_defenseBonus > 0) { target.addDefense(-m_defenseBonus); std::cout << "防御力 -" << m_defenseBonus << "\n"; }
    m_equipped = false;
    std::cout << "卸下装备成功！\n";
}

Item* Equipment::clone() const { return new Equipment(*this); }

std::string Equipment::getInfo() const {
    std::ostringstream oss;
    oss << Item::getInfo();
    oss << "\n  槽位：" << slotToString(m_slot)
        << " | 状态：" << (m_equipped ? "已装备" : "未装备");
    return oss.str();
}

std::string Equipment::getEffectDesc() const {
    std::ostringstream oss;
    if (m_attackBonus > 0) oss << "攻击+" << m_attackBonus << " ";
    if (m_defenseBonus > 0) oss << "防御+" << m_defenseBonus;
    return oss.str();
}

std::string Equipment::slotToString(EquipmentSlot slot) {
    switch (slot) {
        case EquipmentSlot::WEAPON:    return "武器";
        case EquipmentSlot::ARMOR:     return "防具";
        case EquipmentSlot::ACCESSORY: return "饰品";
        default:                       return "未知";
    }
}
