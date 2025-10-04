#pragma once

class CharacterStats {
public:
    CharacterStats(int level = 1);

    // Stat accessors (includes equipment bonuses)
    int getHP() const { return m_hp; }
    int getMaxHP() const { return m_maxHP + m_equipmentHPBonus; }
    int getMP() const { return m_mp; }
    int getMaxMP() const { return m_maxMP + m_equipmentMPBonus; }
    int getAttack() const { return m_attack + m_equipmentAttackBonus; }
    int getDefense() const { return m_defense + m_equipmentDefenseBonus; }
    int getLevel() const { return m_level; }
    int getExperience() const { return m_experience; }
    int getExperienceToNextLevel() const { return m_experienceToNextLevel; }

    // Base stats (without equipment)
    int getBaseMaxHP() const { return m_maxHP; }
    int getBaseMaxMP() const { return m_maxMP; }
    int getBaseAttack() const { return m_attack; }
    int getBaseDefense() const { return m_defense; }

    // Combat methods
    void takeDamage(int damage);
    void heal(int amount);
    void restoreMP(int amount);
    void useMP(int amount);

    // Progression methods
    void gainExperience(int exp);
    bool levelUp();

    // Status checks
    bool isDead() const { return m_hp <= 0; }
    bool isAlive() const { return m_hp > 0; }
    bool hasEnoughMP(int cost) const { return m_mp >= cost; }

    // Equipment bonuses (set by PartyMember when equipment changes)
    void setEquipmentBonuses(int hpBonus, int mpBonus, int attackBonus, int defenseBonus);
    void clearEquipmentBonuses();

private:
    void calculateStats();
    int calculateExperienceToNextLevel() const;

    // Core stats (base values, before equipment)
    int m_hp;
    int m_maxHP;
    int m_mp;
    int m_maxMP;
    int m_attack;
    int m_defense;
    int m_level;
    int m_experience;
    int m_experienceToNextLevel;

    // Equipment bonuses
    int m_equipmentHPBonus;
    int m_equipmentMPBonus;
    int m_equipmentAttackBonus;
    int m_equipmentDefenseBonus;

    // Stat growth formulas (can be customized per character class later)
    static constexpr int BASE_HP = 50;
    static constexpr int BASE_MP = 20;
    static constexpr int BASE_ATTACK = 10;
    static constexpr int BASE_DEFENSE = 8;
    static constexpr float HP_GROWTH = 8.0f;
    static constexpr float MP_GROWTH = 4.0f;
    static constexpr float ATTACK_GROWTH = 2.5f;
    static constexpr float DEFENSE_GROWTH = 1.8f;
};
