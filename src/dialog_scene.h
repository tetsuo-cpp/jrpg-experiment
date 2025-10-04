#pragma once

#include "scene.h"
#include "dialog.h"
#include <functional>
#include <memory>
#include <unordered_map>

class DialogScene : public Scene {
public:
    DialogScene();

    const std::string& getName() const override { return m_name; }
    void onEnter() override;
    void onExit() override;
    void update(float deltaTime) override;
    void draw() override;

    // Dialog management
    void startDialog(int dialogId);
    void setReturnCallback(std::function<void()> callback);
    void registerDialog(std::unique_ptr<Dialog> dialog);

private:
    // Input handling
    void handleInput();
    void advanceLine();
    void selectChoice(int choiceIndex);

    // Drawing
    void drawTextBox();
    void drawDialogLine();
    void drawChoices();

    // Text wrapping helper
    std::vector<std::string> wrapText(const std::string& text, int maxWidth);

    // Dialog state
    std::unordered_map<int, std::unique_ptr<Dialog>> m_dialogs;
    Dialog* m_currentDialog;
    int m_currentLineIndex;
    bool m_showingChoices;
    int m_choiceSelection;

    // Callback
    std::function<void()> m_returnCallback;

    // Name
    std::string m_name = "dialog";

    // UI constants
    static constexpr int TEXT_BOX_HEIGHT = 150;
    static constexpr int TEXT_BOX_PADDING = 20;
    static constexpr int LINE_HEIGHT = 25;
    static constexpr int CHOICE_INDENT = 40;
    static constexpr int MAX_TEXT_WIDTH = 720; // 800 - 2*TEXT_BOX_PADDING - some margin
};
