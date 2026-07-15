#ifndef ITEM_H
#define ITEM_H

#include <string>
#include "BaseCharacter.h"

enum class ItemType { FOOD, MEDICINE, EQUIPMENT };

class Item {
protected:
    int m_id;
    std::wstring m_name;
    std::wstring m_description;
    ItemType m_type;
    int m_price;
    int m_quantity;
public:
    Item(int id, const std::wstring& name, const std::wstring& desc, ItemType type, int price, int quantity=1);
    virtual ~Item();
    virtual void use(RPG::BaseCharacter& target)=0;
    virtual Item* clone() const=0;
    virtual std::wstring getTypeName() const=0;
    virtual std::wstring getInfo() const;
    virtual std::wstring getEffectDesc() const;
    int getID() const{return m_id;}
    std::wstring getName() const{return m_name;}
    std::wstring getDescription() const{return m_description;}
    ItemType getType() const{return m_type;}
    int getPrice() const{return m_price;}
    int getQuantity() const{return m_quantity;}
    void setQuantity(int q){m_quantity=q;}
    void addQuantity(int d){m_quantity+=d;}
    void setPrice(int p){m_price=p;}
};
#endif
