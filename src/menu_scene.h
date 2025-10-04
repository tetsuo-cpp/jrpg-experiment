#pragma once

#include "scene.h"
#include "party.h"
#include "inventory.h"
#include <functional>

enum class MenuMode {
    MAIN_MENU,
    STATUS,
    ITEMS,
    EQUIPMENT,
    SAVE
};

enum class ItemMenuMode {
    BROWSE,
    SELECT_TARGET
};

enum class EquipmentMenuMode {
    SELECT_MEMBER,
    SELECT_SLOT,
    SELECT_EQUIPMENT
};

enum class EquipmentSlot {
    WEAPON,
    ARMOR,
    ACCESSORY
};

class MenuScene : public Scene {
public:
    MenuScene(Party* party, Inventory* inventory);

    const std::string& getName() const override { return m_name; }
    void onEnter() override;
    void onExit() override;
    void update(float deltaTime) override;
    void draw() override;

    void setReturnCallback(std::function<void()> callback);

private:
    // Input handling
    void handleMainMenuInput();
    void handleStatusInput();
    void handleItemsInput();
    void handleEquipmentInput();

    // Drawing functions
    void drawMainMenu();
    void drawStatus();
    void drawItems();
    void drawEquipment();
    void drawSave();

    // Helper functions
    void returnToPreviousScene();
    void useItemOnTarget(int itemIndex, int targetIndex);
    void equipItem(int memberIndex, EquipmentSlot slot, Equipment* equipment);
    void unequipItem(int memberIndex, EquipmentSlot slot);

    // Systems
    Party* m_party;
    Inventory* m_inventory;

    // Name
    std::string m_name = "menu";

    // Menu state
    MenuMode m_menuMode;
    int m_mainMenuSelection;
    int m_statusPageIndex;

    // Items submenu state
    ItemMenuMode m_itemMenuMode;
    int m_itemSelection;
    int m_itemScrollOffset;
    int m_itemTargetSelection;

    // Equipment submenu state
    EquipmentMenuMode m_equipmentMenuMode;
    int m_equipmentMemberSelection;
    EquipmentSlot m_equipmentSlotSelection;
    int m_equipmentItemSelection;
    int m_equipmentScrollOffset;

    // Callback
    std::function<void()> m_returnCallback;

    // UI constants
    static constexpr int MENU_X = 50;
    static constexpr int MENU_Y = 50;
    static constexpr int LINE_HEIGHT = 25;
    static constexpr int ITEMS_PER_PAGE = 15;
};
