#ifndef EQUIPMENT_H
#define EQUIPMENT_H

#include "Item.h"

enum class EquipmentSlot { WEAPON, ARMOR, ACCESSORY };

// ============================================================
// Equipment ??? - ??? Item
// ?????????????/???
// ============================================================
class Equipment : public Item {
private:
    int             m_attackBonus;
    int             m_defenseBonus;
    EquipmentSlot   m_slot;
    bool            m_equipped;

public:
    Equipment(int id, const std::string& name, const std::string& desc,
              int price, int attackBonus, int defenseBonus,
              EquipmentSlot slot, int quantity = 1);

    void use(RPG::BaseCharacter& target) override;
    Item* clone() const override;
    std::string getTypeName() const override { return "装备"; }
    std::string getInfo() const override;
    std::string getEffectDesc() const override;

    int getAttackBonus() const      { return m_attackBonus; }
    int getDefenseBonus() const     { return m_defenseBonus; }
    EquipmentSlot getSlot() const   { return m_slot; }
    bool isEquipped() const         { return m_equipped; }
    void setEquipped(bool eq)       { m_equipped = eq; }
    void unequip(RPG::BaseCharacter& target);
    static std::string slotToString(EquipmentSlot slot);
};

#endif
