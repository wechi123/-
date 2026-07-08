#ifndef MEDICINE_H
#define MEDICINE_H

#include "Item.h"

// ============================================================
// Medicine ??? - ??? Item
// ???????? + ??????????
// ============================================================
class Medicine : public Item {
private:
    int         m_hpRestore;
    std::string m_cureEffect;
    bool        m_hasCure;

public:
    Medicine(int id, const std::string& name, const std::string& desc,
             int price, int hpRestore,
             const std::string& cureEffect = "", int quantity = 1);

    void use(Character& target) override;
    Item* clone() const override;
    std::string getTypeName() const override { return "??"; }
    std::string getInfo() const override;
    std::string getEffectDesc() const override;

    int getHpRestore() const         { return m_hpRestore; }
    std::string getCureEffect() const{ return m_cureEffect; }
    bool hasCure() const             { return m_hasCure; }
};

#endif
