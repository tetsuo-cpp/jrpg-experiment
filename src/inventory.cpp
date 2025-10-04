#include "inventory.h"

Inventory::Inventory(int maxSlots)
    : m_maxSlots(maxSlots)
{
}

bool Inventory::addItem(const Item& item, int quantity) {
    if (quantity <= 0) return false;

    auto it = m_items.find(item.getName());

    if (it != m_items.end()) {
        // Item already exists, add to stack
        int newQuantity = it->second.second + quantity;
        if (newQuantity > MAX_STACK) {
            newQuantity = MAX_STACK;
        }
        it->second.second = newQuantity;
        return true;
    } else {
        // New item
        if (isFull()) {
            return false;  // Inventory full
        }

        int addQuantity = (quantity > MAX_STACK) ? MAX_STACK : quantity;
        m_items[item.getName()] = {item, addQuantity};
        return true;
    }
}

bool Inventory::removeItem(const std::string& itemName, int quantity) {
    if (quantity <= 0) return false;

    auto it = m_items.find(itemName);
    if (it == m_items.end()) {
        return false;  // Item not found
    }

    if (it->second.second < quantity) {
        return false;  // Not enough quantity
    }

    it->second.second -= quantity;

    if (it->second.second == 0) {
        m_items.erase(it);  // Remove item if quantity reaches 0
    }

    return true;
}

bool Inventory::hasItem(const std::string& itemName) const {
    return m_items.find(itemName) != m_items.end();
}

int Inventory::getItemCount(const std::string& itemName) const {
    auto it = m_items.find(itemName);
    return (it != m_items.end()) ? it->second.second : 0;
}

const Item* Inventory::getItem(const std::string& itemName) const {
    auto it = m_items.find(itemName);
    return (it != m_items.end()) ? &it->second.first : nullptr;
}

std::vector<std::pair<Item, int>> Inventory::getAllItems() const {
    std::vector<std::pair<Item, int>> result;
    for (const auto& pair : m_items) {
        result.push_back({pair.second.first, pair.second.second});
    }
    return result;
}
