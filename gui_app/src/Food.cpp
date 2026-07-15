#include "Food.h"
#include <sstream>

Food::Food(int id, const std::wstring& name, const std::wstring& desc, int price, int hpRestore, int quantity)
    : Item(id, name, desc, ItemType::FOOD, price, quantity), m_hpRestore(hpRestore) {}
void Food::use(RPG::BaseCharacter& target) { target.addHP(m_hpRestore); m_quantity--; }
Item* Food::clone() const { return new Food(*this); }
std::wstring Food::getInfo() const { return Item::getInfo(); }
std::wstring Food::getEffectDesc() const { std::wostringstream oss; oss << L"恢复 " << m_hpRestore << L" 点生命值"; return oss.str(); }
