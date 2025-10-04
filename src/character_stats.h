#pragma once

class CharacterStats {
public:
    CharacterStats(int level = 1);

    // Stat accessors
    int getHP() const { return m_hp; }
    int getMaxHP() const { return m_maxHP; }
    int getMP() const { return m_mp; }
    int getMaxMP() const { return m_maxMP; }
    int getAttack() const { return m_attack; }
    int getDefense() const { return m_defense; }
    int getLevel() const { return m_level; }
    int getExperience() const { return m_experience; }
    int getExperienceToNextLevel() const { return m_experienceToNextLevel; }

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

private:
    void calculateStats();
    int calculateExperienceToNextLevel() const;

    // Core stats
    int m_hp;
    int m_maxHP;
    int m_mp;
    int m_maxMP;
    int m_attack;
    int m_defense;
    int m_level;
    int m_experience;
    int m_experienceToNextLevel;

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
