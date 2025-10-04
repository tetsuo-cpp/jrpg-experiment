#pragma once

#include "scene.h"
#include "shop.h"
#include "party.h"
#include "inventory.h"
#include <functional>
#include <memory>

enum class ShopState {
    MAIN_MENU,      // Buy / Sell / Leave
    BUYING,         // Browse shop items
    BUY_CONFIRM,    // Confirm purchase
    SELLING,        // Browse inventory items
    SELL_CONFIRM    // Confirm sale
};

class ShopScene : public Scene {
public:
    ShopScene(Party* party, Inventory* inventory);

    const std::string& getName() const override { return m_name; }
    void onEnter() override;
    void onExit() override;
    void update(float deltaTime) override;
    void draw() override;

    // Shop setup
    void setShop(Shop* shop);
    void setReturnCallback(std::function<void()> callback);

private:
    // Input handling
    void handleMainMenuInput();
    void handleBuyingInput();
    void handleBuyConfirmInput();
    void handleSellingInput();
    void handleSellConfirmInput();

    // Drawing
    void drawMainMenu();
    void drawBuyingScreen();
    void drawBuyConfirmScreen();
    void drawSellingScreen();
    void drawSellConfirmScreen();
    void drawGoldDisplay();

    // Helper for text wrapping
    std::vector<std::string> wrapText(const std::string& text, int maxWidth);

    // State
    Shop* m_shop;
    Party* m_party;
    Inventory* m_inventory;
    ShopState m_shopState;

    // Menu selections
    int m_mainMenuSelection;
    int m_buySelection;
    int m_sellSelection;
    int m_buyScrollOffset;
    int m_sellScrollOffset;
    int m_quantity;

    // Callback
    std::function<void()> m_returnCallback;

    // Name
    std::string m_name = "shop";

    // UI constants
    static constexpr int ITEMS_PER_PAGE = 12;
    static constexpr int MENU_X = 250;
    static constexpr int MENU_Y = 200;
    static constexpr int LINE_HEIGHT = 30;
};
