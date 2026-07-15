#ifndef FOOD_H
#define FOOD_H
#include "Item.h"

class Food : public Item {
private:
    int m_hpRestore;
public:
    Food(int id, const std::wstring& name, const std::wstring& desc, int price, int hpRestore, int quantity=1);
    void use(RPG::BaseCharacter& target) override;
    Item* clone() const override;
    std::wstring getTypeName() const override { return L"食物"; }
    std::wstring getInfo() const override;
    std::wstring getEffectDesc() const override;
    int getHpRestore() const { return m_hpRestore; }
};
#endif
