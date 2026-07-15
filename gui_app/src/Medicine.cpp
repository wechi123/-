#include "Medicine.h"
#include <sstream>

Medicine::Medicine(int id, const std::wstring& name, const std::wstring& desc, int price, int hpRestore, const std::wstring& cureEffect, int quantity)
    : Item(id, name, desc, ItemType::MEDICINE, price, quantity), m_hpRestore(hpRestore), m_cureEffect(cureEffect), m_hasCure(!cureEffect.empty()) {}
void Medicine::use(RPG::BaseCharacter& target) { target.addHP(m_hpRestore); m_quantity--; }
Item* Medicine::clone() const { return new Medicine(*this); }
std::wstring Medicine::getInfo() const { return Item::getInfo(); }
std::wstring Medicine::getEffectDesc() const { std::wostringstream oss; oss << L"恢复 " << m_hpRestore << L" 点生命值"; if(m_hasCure) oss << L"，治愈【" << m_cureEffect << L"】"; return oss.str(); }
