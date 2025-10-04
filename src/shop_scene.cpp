#include "shop_scene.h"
#include "raylib.h"
#include <sstream>

ShopScene::ShopScene(Party* party, Inventory* inventory)
    : m_shop(nullptr)
    , m_party(party)
    , m_inventory(inventory)
    , m_shopState(ShopState::MAIN_MENU)
    , m_mainMenuSelection(0)
    , m_buySelection(0)
    , m_sellSelection(0)
    , m_buyScrollOffset(0)
    , m_sellScrollOffset(0)
    , m_quantity(1)
{
}

void ShopScene::onEnter() {
    m_shopState = ShopState::MAIN_MENU;
    m_mainMenuSelection = 0;
    m_buySelection = 0;
    m_sellSelection = 0;
    m_buyScrollOffset = 0;
    m_sellScrollOffset = 0;
    m_quantity = 1;
}

void ShopScene::onExit() {
}

void ShopScene::update(float deltaTime) {
    switch (m_shopState) {
        case ShopState::MAIN_MENU:
            handleMainMenuInput();
            break;
        case ShopState::BUYING:
            handleBuyingInput();
            break;
        case ShopState::BUY_CONFIRM:
            handleBuyConfirmInput();
            break;
        case ShopState::SELLING:
            handleSellingInput();
            break;
        case ShopState::SELL_CONFIRM:
            handleSellConfirmInput();
            break;
    }
}

void ShopScene::draw() {
    ClearBackground(BLACK);

    // Draw shop background
    DrawRectangle(0, 0, 800, 600, Color{20, 20, 40, 255});

    // Draw shop name and greeting
    if (m_shop) {
        DrawText(m_shop->getName().c_str(), 20, 20, 24, YELLOW);
        DrawText(m_shop->getGreeting().c_str(), 20, 50, 18, LIGHTGRAY);
    }

    drawGoldDisplay();

    switch (m_shopState) {
        case ShopState::MAIN_MENU:
            drawMainMenu();
            break;
        case ShopState::BUYING:
            drawBuyingScreen();
            break;
        case ShopState::BUY_CONFIRM:
            drawBuyConfirmScreen();
            break;
        case ShopState::SELLING:
            drawSellingScreen();
            break;
        case ShopState::SELL_CONFIRM:
            drawSellConfirmScreen();
            break;
    }
}

void ShopScene::setShop(Shop* shop) {
    m_shop = shop;
}

void ShopScene::setReturnCallback(std::function<void()> callback) {
    m_returnCallback = callback;
}

void ShopScene::handleMainMenuInput() {
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        m_mainMenuSelection--;
        if (m_mainMenuSelection < 0) m_mainMenuSelection = 2;
    }
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        m_mainMenuSelection++;
        if (m_mainMenuSelection > 2) m_mainMenuSelection = 0;
    }

    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        switch (m_mainMenuSelection) {
            case 0: // Buy
                m_shopState = ShopState::BUYING;
                m_buySelection = 0;
                m_buyScrollOffset = 0;
                break;
            case 1: // Sell
                m_shopState = ShopState::SELLING;
                m_sellSelection = 0;
                m_sellScrollOffset = 0;
                break;
            case 2: // Leave
                if (m_returnCallback) {
                    m_returnCallback();
                }
                break;
        }
    }

    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE)) {
        if (m_returnCallback) {
            m_returnCallback();
        }
    }
}

void ShopScene::handleBuyingInput() {
    if (!m_shop || m_shop->getItems().empty()) {
        m_shopState = ShopState::MAIN_MENU;
        return;
    }

    const auto& items = m_shop->getItems();

    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        m_buySelection--;
        if (m_buySelection < 0) m_buySelection = items.size() - 1;

        // Adjust scroll offset
        if (m_buySelection < m_buyScrollOffset) {
            m_buyScrollOffset = m_buySelection;
        } else if (m_buySelection >= m_buyScrollOffset + ITEMS_PER_PAGE) {
            m_buyScrollOffset = m_buySelection - ITEMS_PER_PAGE + 1;
        }
    }

    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        m_buySelection++;
        if (m_buySelection >= items.size()) m_buySelection = 0;

        // Adjust scroll offset
        if (m_buySelection < m_buyScrollOffset) {
            m_buyScrollOffset = m_buySelection;
        } else if (m_buySelection >= m_buyScrollOffset + ITEMS_PER_PAGE) {
            m_buyScrollOffset = m_buySelection - ITEMS_PER_PAGE + 1;
        }
    }

    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        m_quantity = 1;
        m_shopState = ShopState::BUY_CONFIRM;
    }

    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE)) {
        m_shopState = ShopState::MAIN_MENU;
    }
}

void ShopScene::handleBuyConfirmInput() {
    const auto& items = m_shop->getItems();
    if (m_buySelection >= items.size()) {
        m_shopState = ShopState::BUYING;
        return;
    }

    const ShopItem& shopItem = items[m_buySelection];
    int maxQuantity = 99;
    if (shopItem.quantity != -1) {
        maxQuantity = shopItem.quantity;
    }

    // Adjust quantity with left/right
    if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
        m_quantity--;
        if (m_quantity < 1) m_quantity = 1;
    }
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
        m_quantity++;
        if (m_quantity > maxQuantity) m_quantity = maxQuantity;
        if (m_quantity > 99) m_quantity = 99;
    }

    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        int totalCost = shopItem.item->getBuyPrice() * m_quantity;

        // Check if player has enough gold
        if (m_party->getGold() >= totalCost) {
            // Check if shop has stock
            if (m_shop->buy(m_buySelection, m_quantity)) {
                // Complete purchase
                m_party->spendGold(totalCost);

                // Add to inventory
                if (shopItem.item->getType() == ItemType::EQUIPMENT) {
                    const Equipment* equip = static_cast<const Equipment*>(shopItem.item);
                    m_inventory->addItem(*equip, m_quantity);
                } else {
                    m_inventory->addItem(*shopItem.item, m_quantity);
                }

                m_shopState = ShopState::BUYING;
            }
        }
    }

    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE)) {
        m_shopState = ShopState::BUYING;
    }
}

void ShopScene::handleSellingInput() {
    const auto& items = m_inventory->getItems();

    if (items.empty()) {
        m_shopState = ShopState::MAIN_MENU;
        return;
    }

    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        m_sellSelection--;
        if (m_sellSelection < 0) m_sellSelection = items.size() - 1;

        // Adjust scroll offset
        if (m_sellSelection < m_sellScrollOffset) {
            m_sellScrollOffset = m_sellSelection;
        } else if (m_sellSelection >= m_sellScrollOffset + ITEMS_PER_PAGE) {
            m_sellScrollOffset = m_sellSelection - ITEMS_PER_PAGE + 1;
        }
    }

    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        m_sellSelection++;
        if (m_sellSelection >= items.size()) m_sellSelection = 0;

        // Adjust scroll offset
        if (m_sellSelection < m_sellScrollOffset) {
            m_sellScrollOffset = m_sellSelection;
        } else if (m_sellSelection >= m_sellScrollOffset + ITEMS_PER_PAGE) {
            m_sellScrollOffset = m_sellSelection - ITEMS_PER_PAGE + 1;
        }
    }

    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        m_quantity = 1;
        m_shopState = ShopState::SELL_CONFIRM;
    }

    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE)) {
        m_shopState = ShopState::MAIN_MENU;
    }
}

void ShopScene::handleSellConfirmInput() {
    const auto& items = m_inventory->getItems();
    if (m_sellSelection >= items.size()) {
        m_shopState = ShopState::SELLING;
        return;
    }

    const ItemSlot& slot = items[m_sellSelection];
    int maxQuantity = slot.quantity;

    // Adjust quantity with left/right
    if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
        m_quantity--;
        if (m_quantity < 1) m_quantity = 1;
    }
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
        m_quantity++;
        if (m_quantity > maxQuantity) m_quantity = maxQuantity;
    }

    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        int totalSellValue = slot.item->getSellPrice() * m_quantity;

        // Remove items from inventory
        if (m_inventory->removeItem(m_sellSelection, m_quantity)) {
            // Add gold to party
            m_party->addGold(totalSellValue);

            // If we sold all, adjust selection
            const auto& updatedItems = m_inventory->getItems();
            if (updatedItems.empty()) {
                m_shopState = ShopState::MAIN_MENU;
            } else {
                if (m_sellSelection >= updatedItems.size()) {
                    m_sellSelection = updatedItems.size() - 1;
                }
                m_shopState = ShopState::SELLING;
            }
        }
    }

    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE)) {
        m_shopState = ShopState::SELLING;
    }
}

void ShopScene::drawMainMenu() {
    DrawText("What would you like to do?", MENU_X, MENU_Y - 40, 20, WHITE);

    const char* options[] = {"Buy", "Sell", "Leave"};
    for (int i = 0; i < 3; i++) {
        Color color = (i == m_mainMenuSelection) ? YELLOW : WHITE;
        const char* arrow = (i == m_mainMenuSelection) ? ">" : " ";

        DrawText(arrow, MENU_X, MENU_Y + i * LINE_HEIGHT, 20, color);
        DrawText(options[i], MENU_X + 30, MENU_Y + i * LINE_HEIGHT, 20, color);
    }

    DrawText("[ENTER/SPACE] Select  [ESC] Leave", 20, 560, 16, GRAY);
}

void ShopScene::drawBuyingScreen() {
    if (!m_shop) return;

    const auto& items = m_shop->getItems();
    DrawText("Buy Items", 20, 90, 20, YELLOW);
    DrawText("[UP/DOWN] Navigate  [ENTER] Buy  [ESC] Back", 20, 560, 16, GRAY);

    int y = 120;
    int endIndex = std::min(m_buyScrollOffset + ITEMS_PER_PAGE, static_cast<int>(items.size()));

    for (int i = m_buyScrollOffset; i < endIndex; i++) {
        const ShopItem& shopItem = items[i];
        Color color = (i == m_buySelection) ? YELLOW : WHITE;
        const char* arrow = (i == m_buySelection) ? ">" : " ";

        DrawText(arrow, 20, y, 18, color);
        DrawText(shopItem.item->getName().c_str(), 50, y, 18, color);

        // Draw price
        char priceText[32];
        snprintf(priceText, sizeof(priceText), "%dG", shopItem.item->getBuyPrice());
        DrawText(priceText, 400, y, 18, color);

        // Draw stock
        if (shopItem.quantity == -1) {
            DrawText("∞", 500, y, 18, LIGHTGRAY);
        } else {
            char stockText[32];
            snprintf(stockText, sizeof(stockText), "x%d", shopItem.quantity);
            DrawText(stockText, 500, y, 18, LIGHTGRAY);
        }

        y += 25;
    }

    // Draw description of selected item
    if (m_buySelection < items.size()) {
        const Item* item = items[m_buySelection].item;
        DrawText(item->getDescription().c_str(), 20, 480, 16, LIGHTGRAY);
    }
}

void ShopScene::drawBuyConfirmScreen() {
    if (!m_shop) return;

    const auto& items = m_shop->getItems();
    if (m_buySelection >= items.size()) return;

    const ShopItem& shopItem = items[m_buySelection];
    int totalCost = shopItem.item->getBuyPrice() * m_quantity;

    DrawText("Confirm Purchase", MENU_X - 50, MENU_Y - 60, 24, YELLOW);

    DrawText(shopItem.item->getName().c_str(), MENU_X - 50, MENU_Y, 20, WHITE);
    DrawText(shopItem.item->getDescription().c_str(), MENU_X - 50, MENU_Y + 30, 16, LIGHTGRAY);

    char quantityText[64];
    snprintf(quantityText, sizeof(quantityText), "Quantity: %d", m_quantity);
    DrawText(quantityText, MENU_X - 50, MENU_Y + 70, 18, WHITE);

    char totalText[64];
    snprintf(totalText, sizeof(totalText), "Total: %dG", totalCost);
    DrawText(totalText, MENU_X - 50, MENU_Y + 100, 18, WHITE);

    Color priceColor = (m_party->getGold() >= totalCost) ? GREEN : RED;
    DrawText(totalCost <= m_party->getGold() ? "Can afford" : "Not enough gold!",
             MENU_X - 50, MENU_Y + 130, 18, priceColor);

    DrawText("[LEFT/RIGHT] Adjust Quantity  [ENTER] Confirm  [ESC] Cancel", 20, 560, 16, GRAY);
}

void ShopScene::drawSellingScreen() {
    const auto& items = m_inventory->getItems();
    DrawText("Sell Items", 20, 90, 20, YELLOW);
    DrawText("[UP/DOWN] Navigate  [ENTER] Sell  [ESC] Back", 20, 560, 16, GRAY);

    int y = 120;
    int endIndex = std::min(m_sellScrollOffset + ITEMS_PER_PAGE, static_cast<int>(items.size()));

    for (int i = m_sellScrollOffset; i < endIndex; i++) {
        const ItemSlot& slot = items[i];
        Color color = (i == m_sellSelection) ? YELLOW : WHITE;
        const char* arrow = (i == m_sellSelection) ? ">" : " ";

        DrawText(arrow, 20, y, 18, color);
        DrawText(slot.item->getName().c_str(), 50, y, 18, color);

        // Draw sell price
        char priceText[32];
        snprintf(priceText, sizeof(priceText), "%dG", slot.item->getSellPrice());
        DrawText(priceText, 400, y, 18, color);

        // Draw quantity
        char qtyText[32];
        snprintf(qtyText, sizeof(qtyText), "x%d", slot.quantity);
        DrawText(qtyText, 500, y, 18, LIGHTGRAY);

        y += 25;
    }

    // Draw description of selected item
    if (m_sellSelection < items.size()) {
        const Item* item = items[m_sellSelection].item;
        DrawText(item->getDescription().c_str(), 20, 480, 16, LIGHTGRAY);
    }
}

void ShopScene::drawSellConfirmScreen() {
    const auto& items = m_inventory->getItems();
    if (m_sellSelection >= items.size()) return;

    const ItemSlot& slot = items[m_sellSelection];
    int totalValue = slot.item->getSellPrice() * m_quantity;

    DrawText("Confirm Sale", MENU_X - 50, MENU_Y - 60, 24, YELLOW);

    DrawText(slot.item->getName().c_str(), MENU_X - 50, MENU_Y, 20, WHITE);
    DrawText(slot.item->getDescription().c_str(), MENU_X - 50, MENU_Y + 30, 16, LIGHTGRAY);

    char quantityText[64];
    snprintf(quantityText, sizeof(quantityText), "Quantity: %d", m_quantity);
    DrawText(quantityText, MENU_X - 50, MENU_Y + 70, 18, WHITE);

    char totalText[64];
    snprintf(totalText, sizeof(totalText), "You'll receive: %dG", totalValue);
    DrawText(totalText, MENU_X - 50, MENU_Y + 100, 18, GREEN);

    DrawText("[LEFT/RIGHT] Adjust Quantity  [ENTER] Confirm  [ESC] Cancel", 20, 560, 16, GRAY);
}

void ShopScene::drawGoldDisplay() {
    char goldText[64];
    snprintf(goldText, sizeof(goldText), "Gold: %d", m_party->getGold());
    DrawText(goldText, 600, 20, 20, GOLD);
}

std::vector<std::string> ShopScene::wrapText(const std::string& text, int maxWidth) {
    std::vector<std::string> lines;
    std::istringstream words(text);
    std::string word;
    std::string currentLine;

    while (words >> word) {
        std::string testLine = currentLine.empty() ? word : currentLine + " " + word;
        int textWidth = MeasureText(testLine.c_str(), 18);

        if (textWidth > maxWidth && !currentLine.empty()) {
            lines.push_back(currentLine);
            currentLine = word;
        } else {
            currentLine = testLine;
        }
    }

    if (!currentLine.empty()) {
        lines.push_back(currentLine);
    }

    return lines;
}
