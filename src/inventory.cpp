#include "inventory.h"
#include "equipment.h"
#include <cstring>

Inventory::Inventory(int maxSlots)
    : m_maxSlots(maxSlots)
{
    m_items.resize(maxSlots);
}

Inventory::~Inventory() {
    // Clean up dynamically allocated items
    for (auto& slot : m_items) {
        if (slot.item) {
            delete slot.item;
            slot.item = nullptr;
        }
    }
}

bool Inventory::addItem(const Item& item, int quantity) {
    if (quantity <= 0) return false;

    // Check if item already exists in inventory
    int existingSlot = findSlot(item.getName());

    if (existingSlot != -1) {
        // Item exists, add to stack
        int newQuantity = m_items[existingSlot].quantity + quantity;
        if (newQuantity > MAX_STACK) {
            newQuantity = MAX_STACK;
        }
        m_items[existingSlot].quantity = newQuantity;
        return true;
    } else {
        // Find empty slot
        for (size_t i = 0; i < m_items.size(); i++) {
            if (m_items[i].item == nullptr) {
                // Create a copy of the item
                if (item.getType() == ItemType::EQUIPMENT) {
                    const Equipment* equip = static_cast<const Equipment*>(&item);
                    m_items[i].item = new Equipment(*equip);
                } else {
                    m_items[i].item = new Item(item);
                }

                int addQuantity = (quantity > MAX_STACK) ? MAX_STACK : quantity;
                m_items[i].quantity = addQuantity;
                return true;
            }
        }
        // No empty slots
        return false;
    }
}

bool Inventory::removeItem(Item* item, int quantity) {
    if (quantity <= 0 || !item) return false;

    int slot = findSlot(item);
    if (slot == -1) return false;

    if (m_items[slot].quantity < quantity) {
        return false;  // Not enough quantity
    }

    m_items[slot].quantity -= quantity;

    if (m_items[slot].quantity == 0) {
        delete m_items[slot].item;
        m_items[slot].item = nullptr;
    }

    return true;
}

bool Inventory::removeItem(int slotIndex, int quantity) {
    if (quantity <= 0) return false;
    if (slotIndex < 0 || slotIndex >= m_items.size()) return false;
    if (!m_items[slotIndex].item) return false;

    if (m_items[slotIndex].quantity < quantity) {
        return false;  // Not enough quantity
    }

    m_items[slotIndex].quantity -= quantity;

    if (m_items[slotIndex].quantity == 0) {
        delete m_items[slotIndex].item;
        m_items[slotIndex].item = nullptr;
    }

    return true;
}

bool Inventory::hasItem(const std::string& itemName) const {
    return findSlot(itemName) != -1;
}

int Inventory::getItemCount(Item* item) const {
    if (!item) return 0;

    int slot = findSlot(item);
    return (slot != -1) ? m_items[slot].quantity : 0;
}

Item* Inventory::getItem(const std::string& itemName) {
    int slot = findSlot(itemName);
    return (slot != -1) ? m_items[slot].item : nullptr;
}

int Inventory::getUsedSlots() const {
    int count = 0;
    for (const auto& slot : m_items) {
        if (slot.item != nullptr) {
            count++;
        }
    }
    return count;
}

int Inventory::findSlot(const std::string& itemName) const {
    for (size_t i = 0; i < m_items.size(); i++) {
        if (m_items[i].item && m_items[i].item->getName() == itemName) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

int Inventory::findSlot(Item* item) const {
    if (!item) return -1;

    for (size_t i = 0; i < m_items.size(); i++) {
        if (m_items[i].item == item) {
            return static_cast<int>(i);
        }
    }
    return -1;
}
