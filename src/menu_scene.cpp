#include "menu_scene.h"
#include <raylib.h>
#include <sstream>

MenuScene::MenuScene(Party* party, Inventory* inventory)
    : m_party(party)
    , m_inventory(inventory)
    , m_menuMode(MenuMode::MAIN_MENU)
    , m_mainMenuSelection(0)
    , m_statusPageIndex(0)
    , m_itemMenuMode(ItemMenuMode::BROWSE)
    , m_itemSelection(0)
    , m_itemScrollOffset(0)
    , m_itemTargetSelection(0)
    , m_equipmentMenuMode(EquipmentMenuMode::SELECT_MEMBER)
    , m_equipmentMemberSelection(0)
    , m_equipmentSlotSelection(EquipmentSlot::WEAPON)
    , m_equipmentItemSelection(0)
    , m_equipmentScrollOffset(0)
{
}

void MenuScene::onEnter() {
    // Reset to main menu
    m_menuMode = MenuMode::MAIN_MENU;
    m_mainMenuSelection = 0;
}

void MenuScene::onExit() {
}

void MenuScene::setReturnCallback(std::function<void()> callback) {
    m_returnCallback = callback;
}

void MenuScene::update(float deltaTime) {
    switch (m_menuMode) {
        case MenuMode::MAIN_MENU:
            handleMainMenuInput();
            break;
        case MenuMode::STATUS:
            handleStatusInput();
            break;
        case MenuMode::ITEMS:
            handleItemsInput();
            break;
        case MenuMode::EQUIPMENT:
            handleEquipmentInput();
            break;
        case MenuMode::SAVE:
            // For now, just allow returning
            if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE)) {
                m_menuMode = MenuMode::MAIN_MENU;
            }
            break;
    }
}

void MenuScene::draw() {
    ClearBackground(BLACK);

    switch (m_menuMode) {
        case MenuMode::MAIN_MENU:
            drawMainMenu();
            break;
        case MenuMode::STATUS:
            drawStatus();
            break;
        case MenuMode::ITEMS:
            drawItems();
            break;
        case MenuMode::EQUIPMENT:
            drawEquipment();
            break;
        case MenuMode::SAVE:
            drawSave();
            break;
    }
}

void MenuScene::handleMainMenuInput() {
    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE)) {
        returnToPreviousScene();
        return;
    }

    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        m_mainMenuSelection = (m_mainMenuSelection - 1 + 4) % 4;
    }
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        m_mainMenuSelection = (m_mainMenuSelection + 1) % 4;
    }

    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        switch (m_mainMenuSelection) {
            case 0:
                m_menuMode = MenuMode::STATUS;
                m_statusPageIndex = 0;
                break;
            case 1:
                m_menuMode = MenuMode::ITEMS;
                m_itemMenuMode = ItemMenuMode::BROWSE;
                m_itemSelection = 0;
                m_itemScrollOffset = 0;
                break;
            case 2:
                m_menuMode = MenuMode::EQUIPMENT;
                m_equipmentMenuMode = EquipmentMenuMode::SELECT_MEMBER;
                m_equipmentMemberSelection = 0;
                break;
            case 3:
                m_menuMode = MenuMode::SAVE;
                break;
        }
    }
}

void MenuScene::handleStatusInput() {
    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE)) {
        m_menuMode = MenuMode::MAIN_MENU;
        return;
    }

    const auto& activeParty = m_party->getActiveMembers();
    int partySize = static_cast<int>(activeParty.size());

    if (partySize > 0) {
        if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
            m_statusPageIndex = (m_statusPageIndex - 1 + partySize) % partySize;
        }
        if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
            m_statusPageIndex = (m_statusPageIndex + 1) % partySize;
        }
    }
}

void MenuScene::handleItemsInput() {
    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE)) {
        if (m_itemMenuMode == ItemMenuMode::SELECT_TARGET) {
            m_itemMenuMode = ItemMenuMode::BROWSE;
        } else {
            m_menuMode = MenuMode::MAIN_MENU;
        }
        return;
    }

    if (m_itemMenuMode == ItemMenuMode::BROWSE) {
        auto& slots = m_inventory->getItems();
        int itemCount = 0;
        for (const auto& slot : slots) {
            if (slot.quantity > 0) itemCount++;
        }

        if (itemCount == 0) return;

        if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
            m_itemSelection = (m_itemSelection - 1 + itemCount) % itemCount;
            if (m_itemSelection < m_itemScrollOffset) {
                m_itemScrollOffset = m_itemSelection;
            }
        }
        if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
            m_itemSelection = (m_itemSelection + 1) % itemCount;
            if (m_itemSelection >= m_itemScrollOffset + ITEMS_PER_PAGE) {
                m_itemScrollOffset = m_itemSelection - ITEMS_PER_PAGE + 1;
            }
        }

        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
            // Get the selected item
            int currentIndex = 0;
            for (size_t i = 0; i < slots.size(); i++) {
                if (slots[i].quantity > 0) {
                    if (currentIndex == m_itemSelection) {
                        // Check if item is usable in field
                        if (slots[i].item->isUsableInField()) {
                            m_itemMenuMode = ItemMenuMode::SELECT_TARGET;
                            m_itemTargetSelection = 0;
                        }
                        break;
                    }
                    currentIndex++;
                }
            }
        }
    } else if (m_itemMenuMode == ItemMenuMode::SELECT_TARGET) {
        const auto& activeParty = m_party->getActiveMembers();
        int partySize = static_cast<int>(activeParty.size());

        if (partySize > 0) {
            if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
                m_itemTargetSelection = (m_itemTargetSelection - 1 + partySize) % partySize;
            }
            if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
                m_itemTargetSelection = (m_itemTargetSelection + 1) % partySize;
            }

            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                useItemOnTarget(m_itemSelection, m_itemTargetSelection);
                m_itemMenuMode = ItemMenuMode::BROWSE;
            }
        }
    }
}

void MenuScene::handleEquipmentInput() {
    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE)) {
        if (m_equipmentMenuMode == EquipmentMenuMode::SELECT_EQUIPMENT) {
            m_equipmentMenuMode = EquipmentMenuMode::SELECT_SLOT;
        } else if (m_equipmentMenuMode == EquipmentMenuMode::SELECT_SLOT) {
            m_equipmentMenuMode = EquipmentMenuMode::SELECT_MEMBER;
        } else {
            m_menuMode = MenuMode::MAIN_MENU;
        }
        return;
    }

    const auto& activeParty = m_party->getActiveMembers();

    if (m_equipmentMenuMode == EquipmentMenuMode::SELECT_MEMBER) {
        int partySize = static_cast<int>(activeParty.size());
        if (partySize > 0) {
            if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
                m_equipmentMemberSelection = (m_equipmentMemberSelection - 1 + partySize) % partySize;
            }
            if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
                m_equipmentMemberSelection = (m_equipmentMemberSelection + 1) % partySize;
            }
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                m_equipmentMenuMode = EquipmentMenuMode::SELECT_SLOT;
                m_equipmentSlotSelection = EquipmentSlot::WEAPON;
            }
        }
    } else if (m_equipmentMenuMode == EquipmentMenuMode::SELECT_SLOT) {
        if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
            int slot = static_cast<int>(m_equipmentSlotSelection);
            slot = (slot - 1 + 3) % 3;
            m_equipmentSlotSelection = static_cast<EquipmentSlot>(slot);
        }
        if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
            int slot = static_cast<int>(m_equipmentSlotSelection);
            slot = (slot + 1) % 3;
            m_equipmentSlotSelection = static_cast<EquipmentSlot>(slot);
        }
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
            m_equipmentMenuMode = EquipmentMenuMode::SELECT_EQUIPMENT;
            m_equipmentItemSelection = 0;
            m_equipmentScrollOffset = 0;
        }
        if (IsKeyPressed(KEY_DELETE) || IsKeyPressed(KEY_X)) {
            // Unequip
            unequipItem(m_equipmentMemberSelection, m_equipmentSlotSelection);
        }
    } else if (m_equipmentMenuMode == EquipmentMenuMode::SELECT_EQUIPMENT) {
        // Build list of compatible equipment from inventory
        auto& slots = m_inventory->getItems();
        std::vector<Equipment*> compatibleEquipment;

        EquipmentType targetType;
        switch (m_equipmentSlotSelection) {
            case EquipmentSlot::WEAPON:
                targetType = EquipmentType::WEAPON;
                break;
            case EquipmentSlot::ARMOR:
                targetType = EquipmentType::ARMOR;
                break;
            case EquipmentSlot::ACCESSORY:
                targetType = EquipmentType::ACCESSORY;
                break;
        }

        for (const auto& slot : slots) {
            if (slot.quantity > 0 && slot.item->getType() == ItemType::EQUIPMENT) {
                Equipment* equip = static_cast<Equipment*>(slot.item);
                if (equip->getEquipmentType() == targetType) {
                    compatibleEquipment.push_back(equip);
                }
            }
        }

        int equipCount = static_cast<int>(compatibleEquipment.size());
        if (equipCount > 0) {
            if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
                m_equipmentItemSelection = (m_equipmentItemSelection - 1 + equipCount) % equipCount;
                if (m_equipmentItemSelection < m_equipmentScrollOffset) {
                    m_equipmentScrollOffset = m_equipmentItemSelection;
                }
            }
            if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
                m_equipmentItemSelection = (m_equipmentItemSelection + 1) % equipCount;
                if (m_equipmentItemSelection >= m_equipmentScrollOffset + ITEMS_PER_PAGE) {
                    m_equipmentScrollOffset = m_equipmentItemSelection - ITEMS_PER_PAGE + 1;
                }
            }
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                equipItem(m_equipmentMemberSelection, m_equipmentSlotSelection, compatibleEquipment[m_equipmentItemSelection]);
                m_equipmentMenuMode = EquipmentMenuMode::SELECT_SLOT;
            }
        }
    }
}

void MenuScene::drawMainMenu() {
    DrawText("MENU", MENU_X, MENU_Y, 40, WHITE);

    const char* menuItems[] = {"Status", "Items", "Equipment", "Save"};

    for (int i = 0; i < 4; i++) {
        Color color = (i == m_mainMenuSelection) ? YELLOW : WHITE;
        const char* prefix = (i == m_mainMenuSelection) ? "> " : "  ";
        DrawText(TextFormat("%s%s", prefix, menuItems[i]), MENU_X, MENU_Y + 80 + i * LINE_HEIGHT, 24, color);
    }

    DrawText("ESC: Close Menu", MENU_X, 550, 16, GRAY);
}

void MenuScene::drawStatus() {
    DrawText("STATUS", MENU_X, MENU_Y, 40, WHITE);

    const auto& activeParty = m_party->getActiveMembers();
    if (activeParty.empty()) {
        DrawText("No party members!", MENU_X, MENU_Y + 80, 20, RED);
        DrawText("ESC: Back", MENU_X, 550, 16, GRAY);
        return;
    }

    PartyMember* member = activeParty[m_statusPageIndex];
    const CharacterStats& stats = member->getStats();

    int yPos = MENU_Y + 80;

    // Name and class
    DrawText(TextFormat("%s - %s", member->getName().c_str(), member->getClassName().c_str()), MENU_X, yPos, 24, YELLOW);
    yPos += LINE_HEIGHT + 10;

    // Level and EXP
    DrawText(TextFormat("Level: %d", stats.getLevel()), MENU_X, yPos, 20, WHITE);
    yPos += LINE_HEIGHT;
    DrawText(TextFormat("EXP: %d / %d", stats.getExperience(), stats.getExperienceToNextLevel()), MENU_X, yPos, 20, WHITE);
    yPos += LINE_HEIGHT + 10;

    // HP and MP
    DrawText(TextFormat("HP: %d / %d", stats.getHP(), stats.getMaxHP()), MENU_X, yPos, 20, GREEN);
    yPos += LINE_HEIGHT;
    DrawText(TextFormat("MP: %d / %d", stats.getMP(), stats.getMaxMP()), MENU_X, yPos, 20, BLUE);
    yPos += LINE_HEIGHT + 10;

    // Stats
    DrawText(TextFormat("Attack: %d", stats.getAttack()), MENU_X, yPos, 20, WHITE);
    yPos += LINE_HEIGHT;
    DrawText(TextFormat("Defense: %d", stats.getDefense()), MENU_X, yPos, 20, WHITE);
    yPos += LINE_HEIGHT + 10;

    // Equipment
    DrawText("Equipment:", MENU_X, yPos, 20, YELLOW);
    yPos += LINE_HEIGHT;

    const Equipment* weapon = member->getWeapon();
    const Equipment* armor = member->getArmor();
    const Equipment* accessory = member->getAccessory();

    DrawText(TextFormat("Weapon: %s", weapon ? weapon->getName().c_str() : "None"), MENU_X + 20, yPos, 18, WHITE);
    yPos += LINE_HEIGHT;
    DrawText(TextFormat("Armor: %s", armor ? armor->getName().c_str() : "None"), MENU_X + 20, yPos, 18, WHITE);
    yPos += LINE_HEIGHT;
    DrawText(TextFormat("Accessory: %s", accessory ? accessory->getName().c_str() : "None"), MENU_X + 20, yPos, 18, WHITE);

    // Navigation hints
    if (activeParty.size() > 1) {
        DrawText(TextFormat("< Member %d/%d >", m_statusPageIndex + 1, (int)activeParty.size()), MENU_X, 520, 18, GRAY);
    }
    DrawText("ESC: Back", MENU_X, 550, 16, GRAY);
}

void MenuScene::drawItems() {
    DrawText("ITEMS", MENU_X, MENU_Y, 40, WHITE);

    auto& slots = m_inventory->getItems();
    std::vector<int> validIndices;

    // Build list of valid items
    for (size_t i = 0; i < slots.size(); i++) {
        if (slots[i].quantity > 0) {
            validIndices.push_back(static_cast<int>(i));
        }
    }

    if (validIndices.empty()) {
        DrawText("No items in inventory!", MENU_X, MENU_Y + 80, 20, RED);
        DrawText("ESC: Back", MENU_X, 550, 16, GRAY);
        return;
    }

    int yPos = MENU_Y + 80;
    int displayCount = std::min(ITEMS_PER_PAGE, static_cast<int>(validIndices.size()) - m_itemScrollOffset);

    for (int i = 0; i < displayCount; i++) {
        int listIndex = m_itemScrollOffset + i;
        int slotIndex = validIndices[listIndex];

        bool isSelected = (m_itemMenuMode == ItemMenuMode::BROWSE && listIndex == m_itemSelection);
        Color color = isSelected ? YELLOW : WHITE;
        const char* prefix = isSelected ? "> " : "  ";

        const Item* item = slots[slotIndex].item;
        const char* usable = item->isUsableInField() ? "" : " [Battle only]";

        DrawText(TextFormat("%s%s x%d%s", prefix, item->getName().c_str(), slots[slotIndex].quantity, usable),
                 MENU_X, yPos, 20, color);
        yPos += LINE_HEIGHT;
    }

    // Target selection overlay
    if (m_itemMenuMode == ItemMenuMode::SELECT_TARGET) {
        int overlayX = 400;
        int overlayY = 150;
        int overlayW = 350;
        int overlayH = 300;

        DrawRectangle(overlayX, overlayY, overlayW, overlayH, Fade(BLACK, 0.9f));
        DrawRectangleLines(overlayX, overlayY, overlayW, overlayH, WHITE);

        DrawText("Select Target:", overlayX + 10, overlayY + 10, 24, YELLOW);

        const auto& activeParty = m_party->getActiveMembers();
        int targetY = overlayY + 50;
        for (size_t i = 0; i < activeParty.size(); i++) {
            bool isSelected = (i == static_cast<size_t>(m_itemTargetSelection));
            Color color = isSelected ? YELLOW : WHITE;
            const char* prefix = isSelected ? "> " : "  ";

            PartyMember* member = activeParty[i];
            const CharacterStats& stats = member->getStats();

            DrawText(TextFormat("%s%s - HP:%d/%d MP:%d/%d",
                     prefix, member->getName().c_str(),
                     stats.getHP(), stats.getMaxHP(),
                     stats.getMP(), stats.getMaxMP()),
                     overlayX + 10, targetY, 18, color);
            targetY += LINE_HEIGHT;
        }

        DrawText("ENTER: Use  ESC: Cancel", overlayX + 10, overlayY + overlayH - 30, 14, GRAY);
    }

    DrawText("ESC: Back", MENU_X, 550, 16, GRAY);
}

void MenuScene::drawEquipment() {
    DrawText("EQUIPMENT", MENU_X, MENU_Y, 40, WHITE);

    const auto& activeParty = m_party->getActiveMembers();
    if (activeParty.empty()) {
        DrawText("No party members!", MENU_X, MENU_Y + 80, 20, RED);
        DrawText("ESC: Back", MENU_X, 550, 16, GRAY);
        return;
    }

    if (m_equipmentMenuMode == EquipmentMenuMode::SELECT_MEMBER) {
        DrawText("Select party member:", MENU_X, MENU_Y + 80, 24, YELLOW);

        int yPos = MENU_Y + 120;
        for (size_t i = 0; i < activeParty.size(); i++) {
            bool isSelected = (i == static_cast<size_t>(m_equipmentMemberSelection));
            Color color = isSelected ? YELLOW : WHITE;
            const char* prefix = isSelected ? "> " : "  ";

            PartyMember* member = activeParty[i];
            DrawText(TextFormat("%s%s - %s", prefix, member->getName().c_str(), member->getClassName().c_str()),
                     MENU_X, yPos, 20, color);
            yPos += LINE_HEIGHT;
        }

        DrawText("ESC: Back", MENU_X, 550, 16, GRAY);
    } else if (m_equipmentMenuMode == EquipmentMenuMode::SELECT_SLOT) {
        PartyMember* member = activeParty[m_equipmentMemberSelection];
        DrawText(TextFormat("%s - Select slot:", member->getName().c_str()), MENU_X, MENU_Y + 80, 24, YELLOW);

        const char* slotNames[] = {"Weapon", "Armor", "Accessory"};
        const Equipment* equipped[] = {member->getWeapon(), member->getArmor(), member->getAccessory()};

        int yPos = MENU_Y + 120;
        for (int i = 0; i < 3; i++) {
            bool isSelected = (static_cast<int>(m_equipmentSlotSelection) == i);
            Color color = isSelected ? YELLOW : WHITE;
            const char* prefix = isSelected ? "> " : "  ";

            const char* equipName = equipped[i] ? equipped[i]->getName().c_str() : "None";
            DrawText(TextFormat("%s%s: %s", prefix, slotNames[i], equipName),
                     MENU_X, yPos, 20, color);
            yPos += LINE_HEIGHT;
        }

        DrawText("ENTER: Change  X: Unequip  ESC: Back", MENU_X, 550, 16, GRAY);
    } else if (m_equipmentMenuMode == EquipmentMenuMode::SELECT_EQUIPMENT) {
        PartyMember* member = activeParty[m_equipmentMemberSelection];

        const char* slotName = "";
        EquipmentType targetType;
        switch (m_equipmentSlotSelection) {
            case EquipmentSlot::WEAPON:
                slotName = "Weapon";
                targetType = EquipmentType::WEAPON;
                break;
            case EquipmentSlot::ARMOR:
                slotName = "Armor";
                targetType = EquipmentType::ARMOR;
                break;
            case EquipmentSlot::ACCESSORY:
                slotName = "Accessory";
                targetType = EquipmentType::ACCESSORY;
                break;
        }

        DrawText(TextFormat("%s - Select %s:", member->getName().c_str(), slotName), MENU_X, MENU_Y + 80, 24, YELLOW);

        // Build compatible equipment list
        auto& slots = m_inventory->getItems();
        std::vector<Equipment*> compatibleEquipment;

        for (const auto& slot : slots) {
            if (slot.quantity > 0 && slot.item->getType() == ItemType::EQUIPMENT) {
                Equipment* equip = static_cast<Equipment*>(slot.item);
                if (equip->getEquipmentType() == targetType) {
                    compatibleEquipment.push_back(equip);
                }
            }
        }

        if (compatibleEquipment.empty()) {
            DrawText("No compatible equipment!", MENU_X, MENU_Y + 120, 20, RED);
        } else {
            int yPos = MENU_Y + 120;
            int displayCount = std::min(ITEMS_PER_PAGE, static_cast<int>(compatibleEquipment.size()) - m_equipmentScrollOffset);

            for (int i = 0; i < displayCount; i++) {
                int listIndex = m_equipmentScrollOffset + i;
                Equipment* equip = compatibleEquipment[listIndex];

                bool isSelected = (listIndex == m_equipmentItemSelection);
                Color color = isSelected ? YELLOW : WHITE;
                const char* prefix = isSelected ? "> " : "  ";

                std::stringstream statBonus;
                if (equip->getAttackBonus() > 0) statBonus << " ATK+" << equip->getAttackBonus();
                if (equip->getDefenseBonus() > 0) statBonus << " DEF+" << equip->getDefenseBonus();
                if (equip->getHPBonus() > 0) statBonus << " HP+" << equip->getHPBonus();
                if (equip->getMPBonus() > 0) statBonus << " MP+" << equip->getMPBonus();

                DrawText(TextFormat("%s%s%s", prefix, equip->getName().c_str(), statBonus.str().c_str()),
                         MENU_X, yPos, 20, color);
                yPos += LINE_HEIGHT;
            }
        }

        DrawText("ESC: Back", MENU_X, 550, 16, GRAY);
    }
}

void MenuScene::drawSave() {
    DrawText("SAVE", MENU_X, MENU_Y, 40, WHITE);
    DrawText("Save system not yet implemented.", MENU_X, MENU_Y + 80, 24, YELLOW);
    DrawText("This will be added in Phase 6: Persistence & Polish", MENU_X, MENU_Y + 120, 20, GRAY);
    DrawText("ESC: Back", MENU_X, 550, 16, GRAY);
}

void MenuScene::returnToPreviousScene() {
    if (m_returnCallback) {
        m_returnCallback();
    }
}

void MenuScene::useItemOnTarget(int itemIndex, int targetIndex) {
    auto& slots = m_inventory->getItems();
    std::vector<int> validIndices;

    for (size_t i = 0; i < slots.size(); i++) {
        if (slots[i].quantity > 0) {
            validIndices.push_back(static_cast<int>(i));
        }
    }

    if (itemIndex < 0 || itemIndex >= static_cast<int>(validIndices.size())) return;

    int slotIndex = validIndices[itemIndex];
    Item* item = slots[slotIndex].item;

    const auto& activeParty = m_party->getActiveMembers();
    if (targetIndex < 0 || targetIndex >= static_cast<int>(activeParty.size())) return;

    PartyMember* target = activeParty[targetIndex];
    CharacterStats& stats = target->getStats();

    // Apply item effect
    ItemEffect effect = item->getEffect();
    switch (effect) {
        case ItemEffect::RESTORE_HP:
            stats.heal(item->getEffectPower());
            break;
        case ItemEffect::RESTORE_MP:
            stats.restoreMP(item->getEffectPower());
            break;
        case ItemEffect::REVIVE:
            if (stats.getHP() == 0) {
                stats.heal(stats.getMaxHP() / 2);
            }
            break;
        default:
            break;
    }

    // Remove item from inventory
    m_inventory->removeItem(item, 1);

    // Adjust selection if we ran out of items
    if (m_inventory->getItemCount(item) == 0) {
        validIndices.clear();
        for (size_t i = 0; i < slots.size(); i++) {
            if (slots[i].quantity > 0) {
                validIndices.push_back(static_cast<int>(i));
            }
        }
        if (m_itemSelection >= static_cast<int>(validIndices.size())) {
            m_itemSelection = std::max(0, static_cast<int>(validIndices.size()) - 1);
        }
    }
}

void MenuScene::equipItem(int memberIndex, EquipmentSlot slot, Equipment* equipment) {
    const auto& activeParty = m_party->getActiveMembers();
    if (memberIndex < 0 || memberIndex >= static_cast<int>(activeParty.size())) return;

    PartyMember* member = activeParty[memberIndex];

    // Create a copy of the new equipment before removing from inventory
    Equipment* newEquipment = new Equipment(*equipment);

    // Unequip current item if any (returns to inventory)
    const Equipment* currentEquip = nullptr;
    switch (slot) {
        case EquipmentSlot::WEAPON:
            currentEquip = member->getWeapon();
            break;
        case EquipmentSlot::ARMOR:
            currentEquip = member->getArmor();
            break;
        case EquipmentSlot::ACCESSORY:
            currentEquip = member->getAccessory();
            break;
    }

    if (currentEquip) {
        m_inventory->addItem(*currentEquip, 1);
    }

    // Remove from inventory (this deletes the original)
    m_inventory->removeItem(equipment, 1);

    // Equip the copy (transfers ownership to party member)
    switch (slot) {
        case EquipmentSlot::WEAPON:
            member->equipWeapon(newEquipment);
            break;
        case EquipmentSlot::ARMOR:
            member->equipArmor(newEquipment);
            break;
        case EquipmentSlot::ACCESSORY:
            member->equipAccessory(newEquipment);
            break;
    }
}

void MenuScene::unequipItem(int memberIndex, EquipmentSlot slot) {
    const auto& activeParty = m_party->getActiveMembers();
    if (memberIndex < 0 || memberIndex >= static_cast<int>(activeParty.size())) return;

    PartyMember* member = activeParty[memberIndex];

    const Equipment* currentEquip = nullptr;
    switch (slot) {
        case EquipmentSlot::WEAPON:
            currentEquip = member->getWeapon();
            if (currentEquip) {
                m_inventory->addItem(*currentEquip, 1);
                member->unequipWeapon();
            }
            break;
        case EquipmentSlot::ARMOR:
            currentEquip = member->getArmor();
            if (currentEquip) {
                m_inventory->addItem(*currentEquip, 1);
                member->unequipArmor();
            }
            break;
        case EquipmentSlot::ACCESSORY:
            currentEquip = member->getAccessory();
            if (currentEquip) {
                m_inventory->addItem(*currentEquip, 1);
                member->unequipAccessory();
            }
            break;
    }
}
