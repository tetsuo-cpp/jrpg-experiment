#include "party_member.h"

PartyMember::PartyMember(const std::string& name, CharacterClass characterClass, int level)
    : m_name(name)
    , m_characterClass(characterClass)
    , m_stats(level)
    , m_spritePath("assets/player.png") // Default for now
{
}
