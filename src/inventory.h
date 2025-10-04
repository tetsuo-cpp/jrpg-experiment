#pragma once

#include "item.h"
#include <map>
#include <string>
#include <vector>

class Inventory {
public:
    Inventory(int maxSlots = 64);

    // Item management
    bool addItem(const Item& item, int quantity = 1);
    bool removeItem(const std::string& itemName, int quantity = 1);
    bool hasItem(const std::string& itemName) const;
    int getItemCount(const std::string& itemName) const;

    // Retrieve item data
    const Item* getItem(const std::string& itemName) const;

    // Get all items (for UI display)
    std::vector<std::pair<Item, int>> getAllItems() const;

    // Inventory info
    int getUsedSlots() const { return m_items.size(); }
    int getMaxSlots() const { return m_maxSlots; }
    bool isFull() const { return getUsedSlots() >= m_maxSlots; }

private:
    static constexpr int MAX_STACK = 99;  // Max quantity per item type

    std::map<std::string, std::pair<Item, int>> m_items;  // Item name -> (Item, quantity)
    int m_maxSlots;
};
