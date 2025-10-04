#include "dialog_scene.h"
#include "raylib.h"
#include <sstream>

DialogScene::DialogScene()
    : m_currentDialog(nullptr)
    , m_currentLineIndex(0)
    , m_showingChoices(false)
    , m_choiceSelection(0)
{
}

void DialogScene::onEnter() {
    m_currentLineIndex = 0;
    m_showingChoices = false;
    m_choiceSelection = 0;
}

void DialogScene::onExit() {
}

void DialogScene::update(float deltaTime) {
    handleInput();
}

void DialogScene::draw() {
    // Draw semi-transparent overlay
    DrawRectangle(0, 0, 800, 600, Color{0, 0, 0, 128});

    drawTextBox();

    if (m_currentDialog && m_currentLineIndex < m_currentDialog->getLineCount()) {
        drawDialogLine();
    }

    if (m_showingChoices) {
        drawChoices();
    }
}

void DialogScene::startDialog(int dialogId) {
    auto it = m_dialogs.find(dialogId);
    if (it != m_dialogs.end()) {
        m_currentDialog = it->second.get();
        m_currentLineIndex = 0;
        m_showingChoices = false;
        m_choiceSelection = 0;
    }
}

void DialogScene::setReturnCallback(std::function<void()> callback) {
    m_returnCallback = callback;
}

void DialogScene::registerDialog(std::unique_ptr<Dialog> dialog) {
    int id = dialog->getId();
    m_dialogs[id] = std::move(dialog);
}

void DialogScene::handleInput() {
    if (!m_currentDialog) return;

    if (m_showingChoices) {
        // Handle choice selection
        const auto& choices = m_currentDialog->getChoices();

        if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
            m_choiceSelection--;
            if (m_choiceSelection < 0) m_choiceSelection = choices.size() - 1;
        }
        if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
            m_choiceSelection++;
            if (m_choiceSelection >= choices.size()) m_choiceSelection = 0;
        }
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
            selectChoice(m_choiceSelection);
        }
    } else {
        // Handle line advancement
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
            advanceLine();
        }
    }

    // ESC to close dialog (only if no choices)
    if (IsKeyPressed(KEY_ESCAPE) && !m_showingChoices) {
        if (m_returnCallback) {
            m_returnCallback();
        }
    }
}

void DialogScene::advanceLine() {
    m_currentLineIndex++;

    // Check if we've reached the end of the dialog
    if (m_currentLineIndex >= m_currentDialog->getLineCount()) {
        // Check if this dialog has choices
        if (m_currentDialog->hasChoices()) {
            m_showingChoices = true;
            m_choiceSelection = 0;
        } else {
            // End of dialog, return to previous scene
            if (m_returnCallback) {
                m_returnCallback();
            }
        }
    }
}

void DialogScene::selectChoice(int choiceIndex) {
    const auto& choices = m_currentDialog->getChoices();
    if (choiceIndex >= 0 && choiceIndex < choices.size()) {
        int nextDialogId = choices[choiceIndex].nextDialogId;

        if (nextDialogId == -1) {
            // End dialog
            if (m_returnCallback) {
                m_returnCallback();
            }
        } else {
            // Jump to next dialog
            startDialog(nextDialogId);
        }
    }
}

void DialogScene::drawTextBox() {
    int boxY = 600 - TEXT_BOX_HEIGHT;

    // Draw box background
    DrawRectangle(0, boxY, 800, TEXT_BOX_HEIGHT, BLACK);

    // Draw box border
    DrawRectangleLines(0, boxY, 800, TEXT_BOX_HEIGHT, WHITE);
}

void DialogScene::drawDialogLine() {
    const auto& lines = m_currentDialog->getLines();
    if (m_currentLineIndex >= lines.size()) return;

    const DialogLine& line = lines[m_currentLineIndex];
    int boxY = 600 - TEXT_BOX_HEIGHT;
    int textY = boxY + TEXT_BOX_PADDING;

    // Draw speaker name if present
    if (!line.speakerName.empty()) {
        DrawText(line.speakerName.c_str(), TEXT_BOX_PADDING, textY, 20, YELLOW);
        textY += LINE_HEIGHT;
    }

    // Wrap and draw dialog text
    auto wrappedLines = wrapText(line.text, MAX_TEXT_WIDTH);
    for (const auto& wrappedLine : wrappedLines) {
        DrawText(wrappedLine.c_str(), TEXT_BOX_PADDING, textY, 18, WHITE);
        textY += LINE_HEIGHT;
    }

    // Draw continuation indicator if not at choices
    if (!m_showingChoices && m_currentLineIndex < m_currentDialog->getLineCount() - 1) {
        DrawText("▼", 760, boxY + TEXT_BOX_HEIGHT - 30, 20, WHITE);
    } else if (!m_showingChoices && !m_currentDialog->hasChoices()) {
        DrawText("[SPACE to close]", 600, boxY + TEXT_BOX_HEIGHT - 30, 14, GRAY);
    }
}

void DialogScene::drawChoices() {
    const auto& choices = m_currentDialog->getChoices();
    int boxY = 600 - TEXT_BOX_HEIGHT;
    int choiceY = boxY + TEXT_BOX_HEIGHT - TEXT_BOX_PADDING - (choices.size() * LINE_HEIGHT);

    DrawText("Choose:", TEXT_BOX_PADDING, choiceY - LINE_HEIGHT, 18, YELLOW);

    for (int i = 0; i < choices.size(); i++) {
        Color color = (i == m_choiceSelection) ? YELLOW : WHITE;
        const char* arrow = (i == m_choiceSelection) ? ">" : " ";

        DrawText(arrow, TEXT_BOX_PADDING + CHOICE_INDENT, choiceY, 18, color);
        DrawText(choices[i].text.c_str(), TEXT_BOX_PADDING + CHOICE_INDENT + 20, choiceY, 18, color);

        choiceY += LINE_HEIGHT;
    }
}

std::vector<std::string> DialogScene::wrapText(const std::string& text, int maxWidth) {
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
