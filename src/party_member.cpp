#include "party_member.h"
#include <algorithm>

PartyMember::PartyMember(const std::string& name, CharacterClass characterClass, int level)
    : m_name(name)
    , m_characterClass(characterClass)
    , m_stats(level)
    , m_spritePath("assets/player.png") // Default for now
{
}

std::string PartyMember::getClassName() const {
    switch (m_characterClass) {
        case CharacterClass::WARRIOR:
            return "Warrior";
        case CharacterClass::MAGE:
            return "Mage";
        case CharacterClass::CLERIC:
            return "Cleric";
        case CharacterClass::THIEF:
            return "Thief";
        default:
            return "Unknown";
    }
}

bool PartyMember::equipWeapon(std::unique_ptr<Equipment> weapon) {
    if (!weapon || weapon->getEquipmentType() != EquipmentType::WEAPON) {
        return false;
    }
    m_weapon = std::move(weapon);
    recalculateEquipmentBonuses();
    return true;
}

bool PartyMember::equipArmor(std::unique_ptr<Equipment> armor) {
    if (!armor || armor->getEquipmentType() != EquipmentType::ARMOR) {
        return false;
    }
    m_armor = std::move(armor);
    recalculateEquipmentBonuses();
    return true;
}

bool PartyMember::equipAccessory(std::unique_ptr<Equipment> accessory) {
    if (!accessory || accessory->getEquipmentType() != EquipmentType::ACCESSORY) {
        return false;
    }
    m_accessory = std::move(accessory);
    recalculateEquipmentBonuses();
    return true;
}

std::unique_ptr<Equipment> PartyMember::unequipWeapon() {
    auto weapon = std::move(m_weapon);
    recalculateEquipmentBonuses();
    return weapon;
}

std::unique_ptr<Equipment> PartyMember::unequipArmor() {
    auto armor = std::move(m_armor);
    recalculateEquipmentBonuses();
    return armor;
}

std::unique_ptr<Equipment> PartyMember::unequipAccessory() {
    auto accessory = std::move(m_accessory);
    recalculateEquipmentBonuses();
    return accessory;
}

void PartyMember::recalculateEquipmentBonuses() {
    int totalHP = 0;
    int totalMP = 0;
    int totalAttack = 0;
    int totalDefense = 0;

    if (m_weapon) {
        totalHP += m_weapon->getHPBonus();
        totalMP += m_weapon->getMPBonus();
        totalAttack += m_weapon->getAttackBonus();
        totalDefense += m_weapon->getDefenseBonus();
    }

    if (m_armor) {
        totalHP += m_armor->getHPBonus();
        totalMP += m_armor->getMPBonus();
        totalAttack += m_armor->getAttackBonus();
        totalDefense += m_armor->getDefenseBonus();
    }

    if (m_accessory) {
        totalHP += m_accessory->getHPBonus();
        totalMP += m_accessory->getMPBonus();
        totalAttack += m_accessory->getAttackBonus();
        totalDefense += m_accessory->getDefenseBonus();
    }

    m_stats.setEquipmentBonuses(totalHP, totalMP, totalAttack, totalDefense);
}

void PartyMember::learnSkill(const Skill& skill) {
    if (!hasSkill(skill.getName())) {
        m_skills.push_back(skill);
    }
}

bool PartyMember::hasSkill(const std::string& skillName) const {
    return std::find_if(m_skills.begin(), m_skills.end(),
        [&skillName](const Skill& s) { return s.getName() == skillName; }) != m_skills.end();
}

bool PartyMember::equipWeapon(Equipment* weapon) {
    if (!weapon || weapon->getEquipmentType() != EquipmentType::WEAPON) {
        return false;
    }
    m_weapon.reset(weapon);
    recalculateEquipmentBonuses();
    return true;
}

bool PartyMember::equipArmor(Equipment* armor) {
    if (!armor || armor->getEquipmentType() != EquipmentType::ARMOR) {
        return false;
    }
    m_armor.reset(armor);
    recalculateEquipmentBonuses();
    return true;
}

bool PartyMember::equipAccessory(Equipment* accessory) {
    if (!accessory || accessory->getEquipmentType() != EquipmentType::ACCESSORY) {
        return false;
    }
    m_accessory.reset(accessory);
    recalculateEquipmentBonuses();
    return true;
}
