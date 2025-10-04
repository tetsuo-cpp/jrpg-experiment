#include "shop.h"

Shop::Shop(const std::string& name, const std::string& greeting)
    : m_name(name), m_greeting(greeting)
{
}

Shop::~Shop() {
}

void Shop::addItem(const Item& item, int quantity) {
    // Create a copy of the item that the shop owns
    auto itemCopy = std::make_unique<Item>(item);
    Item* itemPtr = itemCopy.get();
    m_ownedItems.push_back(std::move(itemCopy));

    // Add to shop items list
    m_items.emplace_back(itemPtr, quantity);
}

void Shop::addEquipment(const Equipment& equipment, int quantity) {
    // Create a copy of the equipment that the shop owns
    auto equipCopy = std::make_unique<Equipment>(equipment);
    Item* itemPtr = equipCopy.get();  // Store as Item pointer
    m_ownedItems.push_back(std::move(equipCopy));

    // Add to shop items list
    m_items.emplace_back(itemPtr, quantity);
}

bool Shop::canBuy(int index, int quantity) const {
    if (index < 0 || index >= m_items.size()) {
        return false;
    }

    const ShopItem& shopItem = m_items[index];

    // Check if unlimited stock or enough quantity
    if (shopItem.quantity == -1) {
        return true;  // Unlimited stock
    }

    return shopItem.quantity >= quantity;
}

bool Shop::buy(int index, int quantity) {
    if (!canBuy(index, quantity)) {
        return false;
    }

    // Decrease stock if not unlimited
    if (m_items[index].quantity != -1) {
        m_items[index].quantity -= quantity;
    }

    return true;
}
