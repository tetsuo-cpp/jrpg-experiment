#pragma once

#include "item.h"
#include <memory>
#include <string>
#include <vector>

struct ItemSlot {
    Item* item = nullptr;
    int quantity = 0;
};

class Inventory {
public:
    Inventory(int maxSlots = 64);
    ~Inventory();

    // Item management
    bool addItem(const Item& item, int quantity = 1);
    bool removeItem(Item* item, int quantity = 1);
    bool hasItem(const std::string& itemName) const;
    int getItemCount(Item* item) const;

    // Retrieve item data
    Item* getItem(const std::string& itemName);

    // Get all items (for UI display)
    const std::vector<ItemSlot>& getItems() const { return m_items; }

    // Inventory info
    int getUsedSlots() const;
    int getMaxSlots() const { return m_maxSlots; }
    bool isFull() const { return getUsedSlots() >= m_maxSlots; }

private:
    static constexpr int MAX_STACK = 99;  // Max quantity per item type

    std::vector<ItemSlot> m_items;
    int m_maxSlots;

    // Find existing slot for item
    int findSlot(const std::string& itemName) const;
    int findSlot(Item* item) const;
};
