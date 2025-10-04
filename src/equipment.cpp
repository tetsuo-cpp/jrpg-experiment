#include "equipment.h"

Equipment::Equipment(const std::string& name, const std::string& description,
                     EquipmentType type,
                     int attackBonus, int defenseBonus,
                     int hpBonus, int mpBonus,
                     int buyPrice, int sellPrice)
    : Item(name, description, ItemType::EQUIPMENT, ItemEffect::NONE, 0, buyPrice, sellPrice)
    , m_equipmentType(type)
    , m_attackBonus(attackBonus)
    , m_defenseBonus(defenseBonus)
    , m_hpBonus(hpBonus)
    , m_mpBonus(mpBonus)
{
}
