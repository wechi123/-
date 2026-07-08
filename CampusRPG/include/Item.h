#ifndef ITEM_H
#define ITEM_H

#include <string>
#include "Character.h"

// ============================================================
// ItemType ?? - ????
// ============================================================
enum class ItemType {
    FOOD,       // ??
    MEDICINE,   // ??
    EQUIPMENT   // ??
};

// ============================================================
// Item ????
// ??: protected ???? + public ??
// ??: Food/Medicine/Equipment ??
// ??: ???? use() / clone() / getTypeName()
// ============================================================
class Item {
protected:
    int         m_id;
    std::string m_name;
    std::string m_description;
    ItemType    m_type;
    int         m_price;
    int         m_quantity;

public:
    Item(int id, const std::string& name, const std::string& desc,
         ItemType type, int price, int quantity = 1);
    virtual ~Item();

    // ???? - ????
    virtual void use(Character& target) = 0;
    virtual Item* clone() const = 0;
    virtual std::string getTypeName() const = 0;

    // ??? - ??????
    virtual std::string getInfo() const;
    virtual std::string getEffectDesc() const;

    // ????
    int getID() const           { return m_id; }
    std::string getName() const { return m_name; }
    ItemType getType() const    { return m_type; }
    int getPrice() const        { return m_price; }
    int getQuantity() const     { return m_quantity; }
    void setQuantity(int qty)   { m_quantity = qty; }
    void addQuantity(int delta) { m_quantity += delta; }
    void setPrice(int price)    { m_price = price; }
};

#endif
