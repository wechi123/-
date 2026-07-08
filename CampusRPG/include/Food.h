#ifndef FOOD_H
#define FOOD_H
#include "Item.h"
// Food 食物 - 继承自 Item，效果：恢复角色HP
class Food : public Item {
    int m_hpRestore;
public:
    Food(int id, const std::string& name, const std::string& desc,
         int price, int hpRestore, int quantity = 1);
    void use(Character& target) override;
    Item* clone() const override;
    std::string getTypeName() const override { return "食物"; }
    std::string getInfo() const override;
    std::string getEffectDesc() const override;
    int getHpRestore() const { return m_hpRestore; }
};
#endif
