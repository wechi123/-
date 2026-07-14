#ifndef FOOD_H
#define FOOD_H

#include "Item.h"

// ============================================================
// Food ??? - ??? Item
// ???????????HP?
// ============================================================
class Food : public Item {
private:
    int m_hpRestore;    // ???HP?

public:
    Food(int id, const std::string& name, const std::string& desc,
         int price, int hpRestore, int quantity = 1);

    void use(RPG::BaseCharacter& target) override;
    Item* clone() const override;
    std::string getTypeName() const override { return "食物"; }
    std::string getInfo() const override;
    std::string getEffectDesc() const override;

    int getHpRestore() const { return m_hpRestore; }
};

#endif
