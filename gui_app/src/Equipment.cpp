#include "Equipment.h"
#include <sstream>

Equipment::Equipment(int id, const std::wstring& name, const std::wstring& desc, int price, int attackBonus, int defenseBonus, EquipmentSlot slot, int quantity)
    : Item(id, name, desc, ItemType::EQUIPMENT, price, quantity), m_attackBonus(attackBonus), m_defenseBonus(defenseBonus), m_slot(slot), m_equipped(false) {}
void Equipment::use(RPG::BaseCharacter& target) { if(m_equipped) return; if(m_attackBonus>0) target.addAttack(m_attackBonus); if(m_defenseBonus>0) target.addDefense(m_defenseBonus); m_equipped=true; }
void Equipment::unequip(RPG::BaseCharacter& target) { if(!m_equipped) return; if(m_attackBonus>0) target.addAttack(-m_attackBonus); if(m_defenseBonus>0) target.addDefense(-m_defenseBonus); m_equipped=false; }
Item* Equipment::clone() const { return new Equipment(*this); }
std::wstring Equipment::getInfo() const { std::wostringstream oss; oss << Item::getInfo() << L"\n  槽位：" << slotToString(m_slot) << L" | 状态：" << (m_equipped?L"已装备":L"未装备"); return oss.str(); }
std::wstring Equipment::getEffectDesc() const { std::wostringstream oss; if(m_attackBonus>0) oss << L"攻击+" << m_attackBonus << L" "; if(m_defenseBonus>0) oss << L"防御+" << m_defenseBonus; return oss.str(); }
std::wstring Equipment::slotToString(EquipmentSlot slot) { switch(slot){ case EquipmentSlot::WEAPON: return L"武器"; case EquipmentSlot::ARMOR: return L"防具"; case EquipmentSlot::ACCESSORY: return L"饰品"; default: return L"未知"; } }
