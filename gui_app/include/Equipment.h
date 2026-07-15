#ifndef EQUIPMENT_H
#define EQUIPMENT_H
#include "Item.h"

enum class EquipmentSlot { WEAPON, ARMOR, ACCESSORY };

class Equipment : public Item {
private:
    int m_attackBonus;
    int m_defenseBonus;
    EquipmentSlot m_slot;
    bool m_equipped;
public:
    Equipment(int id, const std::wstring& name, const std::wstring& desc, int price, int attackBonus, int defenseBonus, EquipmentSlot slot, int quantity=1);
    void use(RPG::BaseCharacter& target) override;
    Item* clone() const override;
    std::wstring getTypeName() const override { return L"装备"; }
    std::wstring getInfo() const override;
    std::wstring getEffectDesc() const override;
    int getAttackBonus() const { return m_attackBonus; }
    int getDefenseBonus() const { return m_defenseBonus; }
    EquipmentSlot getSlot() const { return m_slot; }
    bool isEquipped() const { return m_equipped; }
    void setEquipped(bool eq) { m_equipped = eq; }
    void unequip(RPG::BaseCharacter& target);
    static std::wstring slotToString(EquipmentSlot slot);
};
#endif
