#include "item.h"

Item::Item(const std::string& name, const std::string& description,
           ItemType type, ItemEffect effect, int effectPower,
           int buyPrice, int sellPrice)
    : m_name(name)
    , m_description(description)
    , m_type(type)
    , m_effect(effect)
    , m_effectPower(effectPower)
    , m_buyPrice(buyPrice)
    , m_sellPrice(sellPrice)
{
}

bool Item::isUsableInBattle() const {
    // Consumables can be used in battle, key items cannot
    return m_type == ItemType::CONSUMABLE && m_effect != ItemEffect::NONE;
}

bool Item::isUsableInField() const {
    // Only healing consumables can be used in the field
    return m_type == ItemType::CONSUMABLE &&
           (m_effect == ItemEffect::RESTORE_HP ||
            m_effect == ItemEffect::RESTORE_MP ||
            m_effect == ItemEffect::RESTORE_BOTH);
}
