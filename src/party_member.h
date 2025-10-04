#pragma once

#include "character_stats.h"
#include <string>

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
    CharacterStats& getStats() { return m_stats; }
    const CharacterStats& getStats() const { return m_stats; }

    // For future sprite/portrait support
    const std::string& getSpritePath() const { return m_spritePath; }
    void setSpritePath(const std::string& path) { m_spritePath = path; }

private:
    std::string m_name;
    CharacterClass m_characterClass;
    CharacterStats m_stats;
    std::string m_spritePath;
};
