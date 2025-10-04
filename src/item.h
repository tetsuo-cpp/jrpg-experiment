#pragma once

#include <string>

enum class ItemType {
    CONSUMABLE,  // Usable items like potions
    KEY_ITEM,    // Quest/story items
    MATERIAL     // Crafting materials (for future)
};

enum class ItemEffect {
    RESTORE_HP,
    RESTORE_MP,
    RESTORE_BOTH,
    REVIVE,      // Resurrect fallen ally (for future)
    NONE
};

class Item {
public:
    Item() = default;  // Default constructor for map storage
    Item(const std::string& name, const std::string& description,
         ItemType type, ItemEffect effect = ItemEffect::NONE,
         int effectPower = 0, int buyPrice = 0, int sellPrice = 0);

    // Accessors
    const std::string& getName() const { return m_name; }
    const std::string& getDescription() const { return m_description; }
    ItemType getType() const { return m_type; }
    ItemEffect getEffect() const { return m_effect; }
    int getEffectPower() const { return m_effectPower; }
    int getBuyPrice() const { return m_buyPrice; }
    int getSellPrice() const { return m_sellPrice; }

    // Usability
    bool isUsableInBattle() const;
    bool isUsableInField() const;

private:
    std::string m_name = "";
    std::string m_description = "";
    ItemType m_type = ItemType::CONSUMABLE;
    ItemEffect m_effect = ItemEffect::NONE;
    int m_effectPower = 0;  // Amount of HP/MP restored, etc.
    int m_buyPrice = 0;
    int m_sellPrice = 0;
};
