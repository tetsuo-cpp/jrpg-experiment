#pragma once

#include "item.h"
#include <string>

enum class EquipmentType {
    WEAPON,
    ARMOR,
    ACCESSORY
};

class Equipment : public Item {
public:
    Equipment(const std::string& name, const std::string& description,
              EquipmentType type,
              int attackBonus = 0, int defenseBonus = 0,
              int hpBonus = 0, int mpBonus = 0,
              int buyPrice = 0, int sellPrice = 0);

    // Equipment-specific accessors
    EquipmentType getEquipmentType() const { return m_equipmentType; }

    // Stat bonuses
    int getAttackBonus() const { return m_attackBonus; }
    int getDefenseBonus() const { return m_defenseBonus; }
    int getHPBonus() const { return m_hpBonus; }
    int getMPBonus() const { return m_mpBonus; }

private:
    EquipmentType m_equipmentType;

    // Stat modifiers
    int m_attackBonus;
    int m_defenseBonus;
    int m_hpBonus;
    int m_mpBonus;
};
