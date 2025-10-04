#pragma once

#include "item.h"
#include "equipment.h"
#include <vector>
#include <memory>
#include <string>

struct ShopItem {
    Item* item;          // Raw pointer to item (shop doesn't own the item)
    int quantity;        // -1 means unlimited stock

    ShopItem(Item* itm, int qty = -1) : item(itm), quantity(qty) {}
};

class Shop {
public:
    Shop(const std::string& name, const std::string& greeting);
    ~Shop();

    // Accessors
    const std::string& getName() const { return m_name; }
    const std::string& getGreeting() const { return m_greeting; }

    // Shop inventory management
    void addItem(const Item& item, int quantity = -1);        // Add consumable/key item
    void addEquipment(const Equipment& equipment, int quantity = -1);  // Add equipment

    // Shop items access
    const std::vector<ShopItem>& getItems() const { return m_items; }

    // Purchase handling
    bool canBuy(int index, int quantity = 1) const;
    bool buy(int index, int quantity = 1);  // Returns true if purchase successful

private:
    std::string m_name;
    std::string m_greeting;
    std::vector<ShopItem> m_items;
    std::vector<std::unique_ptr<Item>> m_ownedItems;  // Shop owns copies of items
};
