#include "Item.h"
#include <sstream>

Item::Item(int id, const std::string& name, const std::string& desc,
           ItemType type, int price, int quantity)
    : m_id(id), m_name(name), m_description(desc),
      m_type(type), m_price(price), m_quantity(quantity) {}
Item::~Item() {}

std::string Item::getInfo() const {
    std::ostringstream oss;
    oss << "[" << getTypeName() << "] " << m_name
        << " (ID:" << m_id << ")"
        << "\n  描述：" << m_description
        << "\n  价格：" << m_price << " 金币"
        << "\n  数量：" << m_quantity
        << "\n  效果：" << getEffectDesc();
    return oss.str();
}
std::string Item::getEffectDesc() const { return "无特殊效果"; }
