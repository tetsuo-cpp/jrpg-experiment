#include "party.h"

Party::Party() : m_gold(500) {
    m_activeMembers.reserve(MAX_ACTIVE_MEMBERS);
    m_reserveMembers.reserve(MAX_RESERVE_MEMBERS);
}

bool Party::addMember(std::unique_ptr<PartyMember> member) {
    if (!member) {
        return false;
    }

    // Try to add to active party first
    if (m_activeMembers.size() < MAX_ACTIVE_MEMBERS) {
        m_activeMembers.push_back(std::move(member));
        return true;
    }

    // Otherwise add to reserve
    if (m_reserveMembers.size() < MAX_RESERVE_MEMBERS) {
        m_reserveMembers.push_back(std::move(member));
        return true;
    }

    return false; // Party is full
}

bool Party::removeMember(int index) {
    if (index < 0 || index >= getTotalCount()) {
        return false;
    }

    if (index < getActiveCount()) {
        m_activeMembers.erase(m_activeMembers.begin() + index);
    } else {
        int reserveIndex = index - getActiveCount();
        m_reserveMembers.erase(m_reserveMembers.begin() + reserveIndex);
    }

    return true;
}

bool Party::swapMembers(int index1, int index2) {
    if (index1 < 0 || index1 >= getActiveCount() ||
        index2 < 0 || index2 >= getActiveCount()) {
        return false;
    }

    std::swap(m_activeMembers[index1], m_activeMembers[index2]);
    return true;
}

bool Party::moveToReserve(int activeIndex) {
    if (activeIndex < 0 || activeIndex >= getActiveCount() ||
        m_reserveMembers.size() >= MAX_RESERVE_MEMBERS) {
        return false;
    }

    m_reserveMembers.push_back(std::move(m_activeMembers[activeIndex]));
    m_activeMembers.erase(m_activeMembers.begin() + activeIndex);
    return true;
}

bool Party::moveToActive(int reserveIndex) {
    if (reserveIndex < 0 || reserveIndex >= getReserveCount() ||
        m_activeMembers.size() >= MAX_ACTIVE_MEMBERS) {
        return false;
    }

    m_activeMembers.push_back(std::move(m_reserveMembers[reserveIndex]));
    m_reserveMembers.erase(m_reserveMembers.begin() + reserveIndex);
    return true;
}

PartyMember* Party::getActiveMember(int index) {
    if (index < 0 || index >= getActiveCount()) {
        return nullptr;
    }
    return m_activeMembers[index].get();
}

const PartyMember* Party::getActiveMember(int index) const {
    if (index < 0 || index >= getActiveCount()) {
        return nullptr;
    }
    return m_activeMembers[index].get();
}

PartyMember* Party::getReserveMember(int index) {
    if (index < 0 || index >= getReserveCount()) {
        return nullptr;
    }
    return m_reserveMembers[index].get();
}

const PartyMember* Party::getReserveMember(int index) const {
    if (index < 0 || index >= getReserveCount()) {
        return nullptr;
    }
    return m_reserveMembers[index].get();
}

std::vector<PartyMember*> Party::getActiveMembers() {
    std::vector<PartyMember*> members;
    for (auto& member : m_activeMembers) {
        members.push_back(member.get());
    }
    return members;
}

std::vector<const PartyMember*> Party::getActiveMembers() const {
    std::vector<const PartyMember*> members;
    for (const auto& member : m_activeMembers) {
        members.push_back(member.get());
    }
    return members;
}

bool Party::isAllDead() const {
    for (const auto& member : m_activeMembers) {
        if (member->getStats().isAlive()) {
            return false;
        }
    }
    return !m_activeMembers.empty(); // Empty party is not "all dead"
}

bool Party::hasAliveMembers() const {
    return !isAllDead();
}

void Party::healAll(int amount) {
    for (auto& member : m_activeMembers) {
        member->getStats().heal(amount);
    }
}

void Party::restoreAllMP(int amount) {
    for (auto& member : m_activeMembers) {
        member->getStats().restoreMP(amount);
    }
}

void Party::gainExperienceAll(int exp) {
    for (auto& member : m_activeMembers) {
        member->getStats().gainExperience(exp);
    }
}

void Party::addGold(int amount) {
    if (amount > 0) {
        m_gold += amount;
    }
}

bool Party::spendGold(int amount) {
    if (amount <= 0 || amount > m_gold) {
        return false;
    }
    m_gold -= amount;
    return true;
}
