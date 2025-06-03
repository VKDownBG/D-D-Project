//
// Created by Lenovo on 3.6.2025 г.
//

#include "C:/DandD/include/UI/panels/CharacterSelectionPanel.h"
#include <iostream>

CharacterSelectionPanel::CharacterSelectionPanel(int screenWidth, int screenHeight)
    : screenWidth(screenWidth), screenHeight(screenHeight),
      selectedRaceIndex(-1), isVisible(false), selectionConfirmed(false),
      panelColor({40, 40, 60, 220}),
      selectedColor({100, 150, 200, 255}),
      textColor(WHITE),
      headerColor({220, 220, 255, 255}),
      titleFont(GetFontDefault()),
      textFont(GetFontDefault()) {
    InitializeRaceData();
    CreateRaceButtons();
}

void CharacterSelectionPanel::Initialize() {
    // Initialize fonts if custom ones are available
    titleFont = GetFontDefault();
    textFont = GetFontDefault();
}

void CharacterSelectionPanel::InitializeRaceData() {
    races = {
        {Race::Human, "Human", 30, 20, 50, "Balanced warrior with good all-around stats"},
        {Race::Mage, "Mage", 10, 40, 50, "Master of magic with high mana but low strength"},
        {Race::Warrior, "Warrior", 40, 10, 50, "Mighty fighter with great strength but little magic"}
    };
}

void CharacterSelectionPanel::CreateRaceButtons() {
    raceButtons.clear();

    // Create race selection buttons
    for (int i = 0; i < races.size(); ++i) {
        Rectangle bounds = GetRacePanelBounds(i);

        auto button = std::make_unique<Button>(
            bounds,
            races[i].name,
            [this, i]() { OnRaceButtonClicked(i); }
        );

        button->SetColors(
            {60, 60, 80, 200}, // normal
            {80, 80, 120, 220}, // hover
            {100, 100, 140, 240}, // pressed
            WHITE // text
        );
        button->SetBorder(WHITE, 2);
        button->SetRoundedCorners(10.0f);
        button->EnableHoverAnimation(true, 1.05f, 8.0f);

        raceButtons.push_back(std::move(button));
    }

    // Create confirm button
    Rectangle confirmBounds = {
        static_cast<float>(screenWidth / 2 - 100),
        static_cast<float>(screenHeight - 120),
        200, BUTTON_HEIGHT
    };

    confirmButton = Button(confirmBounds, "Confirm Selection", [this]() { OnConfirmClicked(); });
    confirmButton.SetColors({50, 150, 50, 200}, {70, 170, 70, 220}, {90, 190, 90, 240}, WHITE);
    confirmButton.SetBorder(WHITE, 2);
    confirmButton.SetRoundedCorners(8.0f);
    confirmButton.EnableHoverAnimation(true, 1.1f, 6.0f);

    // Create back button
    Rectangle backBounds = {
        20, static_cast<float>(screenHeight - 70), 100, BUTTON_HEIGHT
    };

    backButton = Button(backBounds, "Back", [this]() { OnBackClicked(); });
    backButton.SetColors({150, 50, 50, 200}, {170, 70, 70, 220}, {190, 90, 90, 240}, WHITE);
    backButton.SetBorder(WHITE, 2);
    backButton.SetRoundedCorners(8.0f);
    backButton.EnableHoverAnimation(true, 1.1f, 6.0f);
}

void CharacterSelectionPanel::Show() {
    isVisible = true;
    selectionConfirmed = false;
    selectedRaceIndex = -1;
}

void CharacterSelectionPanel::Hide() {
    isVisible = false;
}

void CharacterSelectionPanel::Update() {
    if (!isVisible) return;

    Vector2 mousePos = GetMousePosition();

    // Update race buttons
    for (auto &button: raceButtons) {
        button->Update(mousePos);
    }

    // Update control buttons
    confirmButton.Update(mousePos);
    backButton.Update(mousePos);

    // Enable/disable confirm button based on selection
    confirmButton.SetActive(selectedRaceIndex != -1);
}

void CharacterSelectionPanel::Draw() const {
    if (!isVisible) return;

    // Draw semi-transparent background
    DrawRectangle(0, 0, screenWidth, screenHeight, {0, 0, 0, 180});

    // Draw title
    const char *title = "Choose Your Character";
    int titleFontSize = 48;
    Vector2 titleSize = MeasureTextEx(titleFont, title, titleFontSize, 2);
    Vector2 titlePos = {
        (screenWidth - titleSize.x) / 2,
        80
    };

    // Title shadow
    DrawTextEx(titleFont, title, {titlePos.x + 2, titlePos.y + 2}, titleFontSize, 2, BLACK);
    DrawTextEx(titleFont, title, titlePos, titleFontSize, 2, headerColor);

    // Draw race panels
    for (int i = 0; i < races.size(); ++i) {
        Rectangle bounds = GetRacePanelBounds(i);
        DrawRacePanel(races[i], i, bounds);
    }

    // Draw buttons
    for (const auto &button: raceButtons) {
        button->Draw();
    }

    confirmButton.Draw();
    backButton.Draw();

    // Draw selection indicator
    if (selectedRaceIndex != -1) {
        Rectangle selectedBounds = GetRacePanelBounds(selectedRaceIndex);
        DrawRectangleRoundedLines(selectedBounds, 10.0f, 10, 4, {255, 215, 0, 255}); // Gold border
    }
}

void CharacterSelectionPanel::DrawRacePanel(const RaceData &raceData, int index, Rectangle bounds) const {
    // Draw panel background
    Color bgColor = (selectedRaceIndex == index) ? selectedColor : panelColor;
    DrawRectangleRounded(bounds, 10.0f, 10, bgColor);
    DrawRectangleRoundedLines(bounds, 10.0f, 10, 2, WHITE);

    float yOffset = bounds.y + 20;
    const float leftMargin = bounds.x + 15;
    const float rightMargin = bounds.x + bounds.width - 15;

    // Race name
    int nameFontSize = 24;
    Vector2 nameSize = MeasureTextEx(textFont, raceData.name.c_str(), nameFontSize, 1);
    Vector2 namePos = {bounds.x + (bounds.width - nameSize.x) / 2, yOffset};
    DrawTextEx(textFont, raceData.name.c_str(), namePos, nameFontSize, 1, headerColor);
    yOffset += nameSize.y + 20;

    // Stats section
    DrawTextEx(textFont, "Stats:", {leftMargin, yOffset}, 18, 1, textColor);
    yOffset += 25;

    // Strength bar
    Rectangle strBounds = {leftMargin, yOffset, rightMargin - leftMargin, 20};
    DrawStatBar("Strength", raceData.strength, 50, strBounds, {220, 80, 80, 255});
    yOffset += 35;

    // Mana bar
    Rectangle manaBounds = {leftMargin, yOffset, rightMargin - leftMargin, 20};
    DrawStatBar("Mana", raceData.mana, 50, manaBounds, {80, 80, 220, 255});
    yOffset += 35;

    // Health bar
    Rectangle healthBounds = {leftMargin, yOffset, rightMargin - leftMargin, 20};
    DrawStatBar("Health", raceData.health, 50, healthBounds, {80, 220, 80, 255});
    yOffset += 45;

    // Description
    const std::string desc = raceData.description.c_str();
    int descFontSize = 12;
    Rectangle textBounds = {leftMargin, yOffset, rightMargin - leftMargin, bounds.y + bounds.height - yOffset - 10};

    // Simple word wrapping for description
    std::string wrappedText = raceData.description;
    // For simplicity, we'll just draw the text and let it wrap naturally
    DrawTextEx(textFont, desc.c_str(), {100, 150}, descFontSize, 1, {200, 200, 200, 255});
    // DrawTextRec(textFont, desc, textBounds, descFontSize, 1, true, {200, 200, 200, 255});
}

void CharacterSelectionPanel::DrawStatBar(const char *label, int value, int maxValue, Rectangle bounds,
                                          Color barColor) const {
    // Draw label
    DrawTextEx(textFont, label, {bounds.x, bounds.y - 18}, 14, 1, textColor);

    // Draw background bar
    DrawRectangleRounded(bounds, 5.0f, 6, {40, 40, 40, 255});

    // Draw value bar
    float fillRatio = static_cast<float>(value) / static_cast<float>(maxValue);
    Rectangle fillBounds = {bounds.x, bounds.y, bounds.width * fillRatio, bounds.height};
    DrawRectangleRounded(fillBounds, 5.0f, 6, barColor);

    // Draw value text
    std::string valueText = std::to_string(value) + "/" + std::to_string(maxValue);
    Vector2 textSize = MeasureTextEx(textFont, valueText.c_str(), 12, 1);
    Vector2 textPos = {
        bounds.x + (bounds.width - textSize.x) / 2,
        bounds.y + (bounds.height - textSize.y) / 2
    };
    DrawTextEx(textFont, valueText.c_str(), textPos, 12, 1, WHITE);
}

Rectangle CharacterSelectionPanel::GetRacePanelBounds(int index) const {
    int totalWidth = races.size() * RACE_PANEL_WIDTH + (races.size() - 1) * RACE_PANEL_SPACING;
    int startX = (screenWidth - totalWidth) / 2;

    return {
        static_cast<float>(startX + index * (RACE_PANEL_WIDTH + RACE_PANEL_SPACING)),
        static_cast<float>(screenHeight / 2 - RACE_PANEL_HEIGHT / 2),
        static_cast<float>(RACE_PANEL_WIDTH),
        static_cast<float>(RACE_PANEL_HEIGHT)
    };
}

void CharacterSelectionPanel::OnRaceButtonClicked(int raceIndex) {
    selectedRaceIndex = raceIndex;
}

void CharacterSelectionPanel::OnConfirmClicked() {
    if (selectedRaceIndex != -1) {
        selectionConfirmed = true;
        if (onRaceSelected) {
            onRaceSelected(races[selectedRaceIndex].race);
        }
    }
}

void CharacterSelectionPanel::OnBackClicked() {
    if (onBack) {
        onBack();
    }
}

bool CharacterSelectionPanel::IsVisible() const {
    return isVisible;
}

bool CharacterSelectionPanel::IsSelectionConfirmed() const {
    return selectionConfirmed;
}

Race CharacterSelectionPanel::GetSelectedRace() const {
    if (selectedRaceIndex != -1) {
        return races[selectedRaceIndex].race;
    }
    return Race::Human; // Default fallback
}

void CharacterSelectionPanel::SetOnRaceSelected(std::function<void(Race)> callback) {
    onRaceSelected = callback;
}

void CharacterSelectionPanel::SetOnBack(std::function<void()> callback) {
    onBack = callback;
}

void CharacterSelectionPanel::Reset() {
    selectedRaceIndex = -1;
    selectionConfirmed = false;
}
