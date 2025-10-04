#include "skill.h"

Skill::Skill(const std::string& name, const std::string& description,
             SkillType type, TargetType targetType,
             int mpCost, int power)
    : m_name(name)
    , m_description(description)
    , m_type(type)
    , m_targetType(targetType)
    , m_mpCost(mpCost)
    , m_power(power)
{
}
