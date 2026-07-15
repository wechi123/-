#ifndef MEDICINE_H
#define MEDICINE_H
#include "Item.h"

class Medicine : public Item {
private:
    int m_hpRestore;
    std::wstring m_cureEffect;
    bool m_hasCure;
public:
    Medicine(int id, const std::wstring& name, const std::wstring& desc, int price, int hpRestore, const std::wstring& cureEffect=L"", int quantity=1);
    void use(RPG::BaseCharacter& target) override;
    Item* clone() const override;
    std::wstring getTypeName() const override { return L"药品"; }
    std::wstring getInfo() const override;
    std::wstring getEffectDesc() const override;
    int getHpRestore() const { return m_hpRestore; }
    std::wstring getCureEffect() const { return m_cureEffect; }
    bool hasCure() const { return m_hasCure; }
};
#endif
