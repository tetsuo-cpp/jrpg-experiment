#include "character_stats.h"
#include <algorithm>
#include <cmath>

CharacterStats::CharacterStats(int level)
    : m_level(level)
    , m_experience(0)
    , m_equipmentHPBonus(0)
    , m_equipmentMPBonus(0)
    , m_equipmentAttackBonus(0)
    , m_equipmentDefenseBonus(0)
{
    calculateStats();
    m_hp = m_maxHP;
    m_mp = m_maxMP;
    m_experienceToNextLevel = calculateExperienceToNextLevel();
}

void CharacterStats::calculateStats() {
    // Calculate stats based on level using growth formulas
    m_maxHP = BASE_HP + static_cast<int>(HP_GROWTH * (m_level - 1));
    m_maxMP = BASE_MP + static_cast<int>(MP_GROWTH * (m_level - 1));
    m_attack = BASE_ATTACK + static_cast<int>(ATTACK_GROWTH * (m_level - 1));
    m_defense = BASE_DEFENSE + static_cast<int>(DEFENSE_GROWTH * (m_level - 1));
}

int CharacterStats::calculateExperienceToNextLevel() const {
    // Experience formula: 100 * level^1.5
    return static_cast<int>(100.0f * std::pow(m_level, 1.5f));
}

void CharacterStats::takeDamage(int damage) {
    m_hp = std::max(0, m_hp - damage);
}

void CharacterStats::heal(int amount) {
    m_hp = std::min(m_maxHP, m_hp + amount);
}

void CharacterStats::restoreMP(int amount) {
    m_mp = std::min(m_maxMP, m_mp + amount);
}

void CharacterStats::useMP(int amount) {
    m_mp = std::max(0, m_mp - amount);
}

void CharacterStats::gainExperience(int exp) {
    m_experience += exp;

    // Check for level up
    while (m_experience >= m_experienceToNextLevel && m_level < 99) {
        levelUp();
    }
}

bool CharacterStats::levelUp() {
    if (m_level >= 99) {
        return false;
    }

    m_level++;

    // Store old max values
    int oldMaxHP = m_maxHP;
    int oldMaxMP = m_maxMP;

    // Recalculate stats
    calculateStats();

    // Restore HP/MP based on the increase
    m_hp += (m_maxHP - oldMaxHP);
    m_mp += (m_maxMP - oldMaxMP);

    // Update experience requirement
    m_experienceToNextLevel = calculateExperienceToNextLevel();

    return true;
}

void CharacterStats::setEquipmentBonuses(int hpBonus, int mpBonus, int attackBonus, int defenseBonus) {
    m_equipmentHPBonus = hpBonus;
    m_equipmentMPBonus = mpBonus;
    m_equipmentAttackBonus = attackBonus;
    m_equipmentDefenseBonus = defenseBonus;
}

void CharacterStats::clearEquipmentBonuses() {
    m_equipmentHPBonus = 0;
    m_equipmentMPBonus = 0;
    m_equipmentAttackBonus = 0;
    m_equipmentDefenseBonus = 0;
}
