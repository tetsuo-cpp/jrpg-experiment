#pragma once

#include <string>

enum class SkillType {
    OFFENSIVE_MAGIC,  // Damaging spells
    HEALING_MAGIC,    // HP restoration
    BUFF,             // Stat increases (for future)
    DEBUFF            // Stat decreases (for future)
};

enum class TargetType {
    SINGLE_ENEMY,
    ALL_ENEMIES,
    SINGLE_ALLY,
    ALL_ALLIES,
    SELF
};

class Skill {
public:
    Skill(const std::string& name, const std::string& description,
          SkillType type, TargetType targetType,
          int mpCost, int power);

    // Accessors
    const std::string& getName() const { return m_name; }
    const std::string& getDescription() const { return m_description; }
    SkillType getType() const { return m_type; }
    TargetType getTargetType() const { return m_targetType; }
    int getMPCost() const { return m_mpCost; }
    int getPower() const { return m_power; }

    // Usability
    bool isOffensive() const { return m_type == SkillType::OFFENSIVE_MAGIC; }
    bool isHealing() const { return m_type == SkillType::HEALING_MAGIC; }
    bool targetsEnemy() const {
        return m_targetType == TargetType::SINGLE_ENEMY ||
               m_targetType == TargetType::ALL_ENEMIES;
    }
    bool targetsAlly() const {
        return m_targetType == TargetType::SINGLE_ALLY ||
               m_targetType == TargetType::ALL_ALLIES ||
               m_targetType == TargetType::SELF;
    }
    bool isMultiTarget() const {
        return m_targetType == TargetType::ALL_ENEMIES ||
               m_targetType == TargetType::ALL_ALLIES;
    }

private:
    std::string m_name;
    std::string m_description;
    SkillType m_type;
    TargetType m_targetType;
    int m_mpCost;
    int m_power;  // Damage or healing amount
};
