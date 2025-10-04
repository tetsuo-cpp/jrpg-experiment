#pragma once

#include "character_stats.h"
#include "equipment.h"
#include "skill.h"
#include <string>
#include <memory>
#include <vector>

enum class CharacterClass {
    WARRIOR,
    MAGE,
    CLERIC,
    THIEF
};

class PartyMember {
public:
    PartyMember(const std::string& name, CharacterClass characterClass, int level = 1);

    // Accessors
    const std::string& getName() const { return m_name; }
    CharacterClass getCharacterClass() const { return m_characterClass; }
    std::string getClassName() const;
    CharacterStats& getStats() { return m_stats; }
    const CharacterStats& getStats() const { return m_stats; }

    // For future sprite/portrait support
    const std::string& getSpritePath() const { return m_spritePath; }
    void setSpritePath(const std::string& path) { m_spritePath = path; }

    // Equipment management
    bool equipWeapon(std::unique_ptr<Equipment> weapon);
    bool equipArmor(std::unique_ptr<Equipment> armor);
    bool equipAccessory(std::unique_ptr<Equipment> accessory);
    std::unique_ptr<Equipment> unequipWeapon();
    std::unique_ptr<Equipment> unequipArmor();
    std::unique_ptr<Equipment> unequipAccessory();

    // Equipment management (for menu system using raw pointers from inventory)
    bool equipWeapon(Equipment* weapon);
    bool equipArmor(Equipment* armor);
    bool equipAccessory(Equipment* accessory);

    const Equipment* getWeapon() const { return m_weapon.get(); }
    const Equipment* getArmor() const { return m_armor.get(); }
    const Equipment* getAccessory() const { return m_accessory.get(); }

    // Skill management
    void learnSkill(const Skill& skill);
    bool hasSkill(const std::string& skillName) const;
    const std::vector<Skill>& getSkills() const { return m_skills; }

private:
    void recalculateEquipmentBonuses();

    std::string m_name;
    CharacterClass m_characterClass;
    CharacterStats m_stats;
    std::string m_spritePath;

    // Equipment slots
    std::unique_ptr<Equipment> m_weapon;
    std::unique_ptr<Equipment> m_armor;
    std::unique_ptr<Equipment> m_accessory;

    // Known skills
    std::vector<Skill> m_skills;
};
