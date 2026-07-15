#include "Item.h"
#include <sstream>

Item::Item(int id, const std::wstring& name, const std::wstring& desc, ItemType type, int price, int quantity)
    : m_id(id), m_name(name), m_description(desc), m_type(type), m_price(price), m_quantity(quantity) {}
Item::~Item() {}

std::wstring Item::getInfo() const {
    std::wostringstream oss;
    oss << L"[" << getTypeName() << L"] " << m_name << L" (ID:" << m_id << L")"
        << L"\n  描述：" << m_description << L"\n  价格：" << m_price << L" 金币"
        << L"\n  数量：" << m_quantity << L"\n  效果：" << getEffectDesc();
    return oss.str();
}
std::wstring Item::getEffectDesc() const { return L"无特殊效果"; }
