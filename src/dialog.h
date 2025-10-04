#pragma once
#include <string>
#include <vector>
#include <functional>

// Represents a single line of dialog
struct DialogLine {
    std::string text;
    std::string speakerName;  // Optional speaker name (empty = narrator)
    std::string portraitPath; // Optional portrait image path

    DialogLine(const std::string& txt, const std::string& speaker = "", const std::string& portrait = "")
        : text(txt), speakerName(speaker), portraitPath(portrait) {}
};

// Represents a choice in a dialog
struct DialogChoice {
    std::string text;
    int nextDialogId; // ID of dialog to jump to (-1 = end dialog)

    DialogChoice(const std::string& txt, int nextId = -1)
        : text(txt), nextDialogId(nextId) {}
};

// Represents a complete dialog sequence
class Dialog {
private:
    int m_id;
    std::vector<DialogLine> m_lines;
    std::vector<DialogChoice> m_choices;
    bool m_hasChoices;

public:
    Dialog(int id) : m_id(id), m_hasChoices(false) {}

    void addLine(const std::string& text, const std::string& speaker = "", const std::string& portrait = "") {
        m_lines.emplace_back(text, speaker, portrait);
    }

    void addChoice(const std::string& text, int nextDialogId = -1) {
        m_choices.emplace_back(text, nextDialogId);
        m_hasChoices = true;
    }

    int getId() const { return m_id; }
    const std::vector<DialogLine>& getLines() const { return m_lines; }
    const std::vector<DialogChoice>& getChoices() const { return m_choices; }
    bool hasChoices() const { return m_hasChoices; }
    size_t getLineCount() const { return m_lines.size(); }
};
