#pragma once

#include "party_member.h"
#include <vector>
#include <memory>

class Party {
public:
    Party();

    // Party management
    bool addMember(std::unique_ptr<PartyMember> member);
    bool removeMember(int index);
    bool swapMembers(int index1, int index2);
    bool moveToReserve(int activeIndex);
    bool moveToActive(int reserveIndex);

    // Accessors
    PartyMember* getActiveMember(int index);
    const PartyMember* getActiveMember(int index) const;
    PartyMember* getReserveMember(int index);
    const PartyMember* getReserveMember(int index) const;

    int getActiveCount() const { return static_cast<int>(m_activeMembers.size()); }
    int getReserveCount() const { return static_cast<int>(m_reserveMembers.size()); }
    int getTotalCount() const { return getActiveCount() + getReserveCount(); }

    // Party status
    bool isAllDead() const;
    bool hasAliveMembers() const;

    // Utility
    void healAll(int amount);
    void restoreAllMP(int amount);
    void gainExperienceAll(int exp);

    static constexpr int MAX_ACTIVE_MEMBERS = 4;
    static constexpr int MAX_RESERVE_MEMBERS = 8;

private:
    std::vector<std::unique_ptr<PartyMember>> m_activeMembers;
    std::vector<std::unique_ptr<PartyMember>> m_reserveMembers;
};
