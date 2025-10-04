#include "equipment.h"

Equipment::Equipment(const std::string& name, const std::string& description,
                     EquipmentType type,
                     int attackBonus, int defenseBonus,
                     int hpBonus, int mpBonus,
                     int buyPrice, int sellPrice)
    : m_name(name)
    , m_description(description)
    , m_type(type)
    , m_attackBonus(attackBonus)
    , m_defenseBonus(defenseBonus)
    , m_hpBonus(hpBonus)
    , m_mpBonus(mpBonus)
    , m_buyPrice(buyPrice)
    , m_sellPrice(sellPrice)
{
}
