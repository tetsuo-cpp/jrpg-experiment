#pragma once

#include <string>

enum class EquipmentType {
    WEAPON,
    ARMOR,
    ACCESSORY
};

class Equipment {
public:
    Equipment(const std::string& name, const std::string& description,
              EquipmentType type,
              int attackBonus = 0, int defenseBonus = 0,
              int hpBonus = 0, int mpBonus = 0,
              int buyPrice = 0, int sellPrice = 0);

    // Accessors
    const std::string& getName() const { return m_name; }
    const std::string& getDescription() const { return m_description; }
    EquipmentType getType() const { return m_type; }

    // Stat bonuses
    int getAttackBonus() const { return m_attackBonus; }
    int getDefenseBonus() const { return m_defenseBonus; }
    int getHPBonus() const { return m_hpBonus; }
    int getMPBonus() const { return m_mpBonus; }

    // Shop prices
    int getBuyPrice() const { return m_buyPrice; }
    int getSellPrice() const { return m_sellPrice; }

private:
    std::string m_name;
    std::string m_description;
    EquipmentType m_type;

    // Stat modifiers
    int m_attackBonus;
    int m_defenseBonus;
    int m_hpBonus;
    int m_mpBonus;

    int m_buyPrice;
    int m_sellPrice;
};
