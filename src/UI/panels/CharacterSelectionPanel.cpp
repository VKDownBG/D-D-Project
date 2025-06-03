//
// Created by Lenovo on 3.6.2025 г.
//

#include "C:/DandD/include/UI/panels/CharacterSelectionPanel.h"
#include <iostream>
#include <sstream>
#include <vector>

CharacterSelectionPanel::CharacterSelectionPanel(const int screenWidth, const int screenHeight)
    : screenWidth(screenWidth), screenHeight(screenHeight),
      backgroundTexture({0}), selectedRaceIndex(-1), isVisible(false),
      selectionConfirmed(false),
      panelColor({0, 51, 102, 255}),
      selectedColor({100, 150, 200, 255}),
      textColor(WHITE),
      headerColor({220, 220, 255, 255}),
      titleFont(GetFontDefault()),
      textFont(GetFontDefault()) {
    InitializeRaceData();
    CreateRaceButtons();
    LoadBackgroundImage();
}

void CharacterSelectionPanel::Initialize() {
    // Initialize fonts if custom ones are available
    titleFont = GetFontDefault();
    textFont = GetFontDefault();
    LoadBackgroundImage();
}

void CharacterSelectionPanel::LoadBackgroundImage() {
    // Load background image - replace "PATH HERE" with actual path
    backgroundTexture = LoadTexture("C:/DandD/assets/background/walls4.png");
    // If texture failed to load, create a simple fallback
    if (backgroundTexture.id == 0) {
        const Image img = GenImageGradientRadial(screenWidth, screenHeight, 0.0f, BLACK, DARKBLUE);
        backgroundTexture = LoadTextureFromImage(img);
        UnloadImage(img);
    }
}

void CharacterSelectionPanel::InitializeRaceData() {
    races = {
        {
            Race::Human, "Human", 30, 20, 50,
            "Balanced warrior with good all-around stats. Versatile in combat and magic."
        },
        {Race::Mage, "Mage", 10, 40, 50, "Master of magic with high mana but low strength. Excels in spellcasting."},
        {
            Race::Warrior, "Warrior", 40, 10, 50,
            "Mighty fighter with great strength but little magic. Dominates melee combat."
        }
    };
}

void CharacterSelectionPanel::CreateRaceButtons() {
    raceButtons.clear();

    // Create race selection buttons - made larger
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

    // Create back button with new color scheme (blue/gray instead of red)
    Rectangle backBounds = {
        20, static_cast<float>(screenHeight - 70), 100, BUTTON_HEIGHT
    };

    backButton = Button(backBounds, "Back", [this]() { OnBackClicked(); });
    backButton.SetColors({70, 130, 180, 200}, {90, 150, 200, 220}, {110, 170, 220, 240}, WHITE); // Steel blue colors
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

    // Draw background image
    if (backgroundTexture.id != 0) {
        // Scale background to fit screen while maintaining aspect ratio
        const float scaleX = static_cast<float>(screenWidth) / backgroundTexture.width;
        const float scaleY = static_cast<float>(screenHeight) / backgroundTexture.height;
        const float scale = (scaleX > scaleY) ? scaleX : scaleY; // Use larger scale to fill screen

        Vector2 origin = {
            (screenWidth - backgroundTexture.width * scale) / 2,
            (screenHeight - backgroundTexture.height * scale) / 2
        };

        DrawTextureEx(backgroundTexture, origin, 0.0f, scale, WHITE);
    }

    // Draw semi-transparent overlay to ensure text readability
    DrawRectangle(0, 0, screenWidth, screenHeight, {0, 0, 0, 120});

    // Draw title
    const char *title = "Choose Your Character";
    int titleFontSize = 48;
    Vector2 titleSize = MeasureTextEx(titleFont, title, titleFontSize, 2);
    Vector2 titlePos = {
        (screenWidth - titleSize.x) / 2,
        60
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
        DrawRectangleRoundedLines(selectedBounds, 12.0f, 12, 4, {255, 215, 0, 255}); // Gold border
    }
}

void CharacterSelectionPanel::DrawRacePanel(const RaceData &raceData, const int index, const Rectangle bounds) const {
    // Draw panel background
    Color bgColor = (selectedRaceIndex == index) ? selectedColor : panelColor;
    DrawRectangleRounded(bounds, 7.0f, 12, bgColor);
    DrawRectangleRoundedLines(bounds, 7, 12, 2, bgColor);

    float yOffset = bounds.y + 25;
    const float leftMargin = bounds.x + 20;
    const float rightMargin = bounds.x + bounds.width - 20;

    // Race name
    int nameFontSize = 28;
    Vector2 nameSize = MeasureTextEx(textFont, raceData.name.c_str(), nameFontSize, 1);
    Vector2 namePos = {bounds.x + (bounds.width - nameSize.x) / 2, yOffset};
    DrawTextEx(textFont, raceData.name.c_str(), namePos, nameFontSize, 1, headerColor);
    yOffset += nameSize.y + 25;

    // Stats section
    DrawTextEx(textFont, "Stats:", {leftMargin, yOffset}, 20, 1, textColor);
    yOffset += 30;

    // Strength bar
    Rectangle strBounds = {leftMargin, yOffset + 20, rightMargin - leftMargin, 25};
    DrawStatBar("Strength", raceData.strength, 50, strBounds, {220, 80, 80, 255});
    yOffset += 50;

    // Mana bar
    Rectangle manaBounds = {leftMargin, yOffset + 20, rightMargin - leftMargin, 25};
    DrawStatBar("Mana", raceData.mana, 50, manaBounds, {80, 80, 220, 255});
    yOffset += 50;

    // Health bar
    Rectangle healthBounds = {leftMargin, yOffset + 20, rightMargin - leftMargin, 25};
    DrawStatBar("Health", raceData.health, 50, healthBounds, {80, 220, 80, 255});
    yOffset += 60;

    // Description - Fixed positioning with manual word wrapping
    int descFontSize = 14;
    float maxWidth = rightMargin - leftMargin;
    float lineHeight = 18;

    // Simple word wrapping implementation
    std::string text = raceData.description;
    std::vector<std::string> lines;
    std::string currentLine = "";
    std::istringstream words(text);
    std::string word;

    while (words >> word) {
        std::string testLine = currentLine.empty() ? word : currentLine + " " + word;
        Vector2 testSize = MeasureTextEx(textFont, testLine.c_str(), descFontSize, 1);

        if (testSize.x <= maxWidth) {
            currentLine = testLine;
        } else {
            if (!currentLine.empty()) {
                lines.push_back(currentLine);
                currentLine = word;
            } else {
                lines.push_back(word); // Word too long, add anyway
                currentLine = "";
            }
        }
    }
    if (!currentLine.empty()) {
        lines.push_back(currentLine);
    }

    // Draw the wrapped text lines
    for (int i = 0; i < lines.size() && yOffset + (i * lineHeight) < bounds.y + bounds.height - 15; ++i) {
        DrawTextEx(textFont, lines[i].c_str(), {leftMargin, yOffset + (i * lineHeight)}, descFontSize, 1,
                   {200, 200, 200, 255});
    }
}

void CharacterSelectionPanel::DrawStatBar(const char *label, const int value, const int maxValue,
                                          const Rectangle bounds,
                                          const Color barColor) const {
    // Draw label
    DrawTextEx(textFont, label, {bounds.x, bounds.y - 22}, 16, 1, textColor);

    // Draw background bar
    DrawRectangleRounded(bounds, 6.0f, 8, {40, 40, 40, 255});

    // Draw value bar
    float fillRatio = static_cast<float>(value) / static_cast<float>(maxValue);
    Rectangle fillBounds = {bounds.x, bounds.y, bounds.width * fillRatio, bounds.height};
    DrawRectangleRounded(fillBounds, 6.0f, 8, barColor);

    // Draw value text
    std::string valueText = std::to_string(value) + "/" + std::to_string(maxValue);
    Vector2 textSize = MeasureTextEx(textFont, valueText.c_str(), 14, 1);
    Vector2 textPos = {
        bounds.x + (bounds.width - textSize.x) / 2,
        bounds.y + (bounds.height - textSize.y) / 2
    };
    DrawTextEx(textFont, valueText.c_str(), textPos, 14, 1, WHITE);
}

Rectangle CharacterSelectionPanel::GetRacePanelBounds(int index) const {
    // Made panels bigger - increased width and height
    const int RACE_PANEL_WIDTH_LARGE = 320; // Increased from original
    const int RACE_PANEL_HEIGHT_LARGE = 450; // Increased from original
    const int RACE_PANEL_SPACING_LARGE = 40; // Increased spacing

    int totalWidth = races.size() * RACE_PANEL_WIDTH_LARGE + (races.size() - 1) * RACE_PANEL_SPACING_LARGE;
    int startX = (screenWidth - totalWidth) / 2;

    return {
        static_cast<float>(startX + index * (RACE_PANEL_WIDTH_LARGE + RACE_PANEL_SPACING_LARGE)),
        static_cast<float>(screenHeight / 2 - RACE_PANEL_HEIGHT_LARGE / 2 + 20), // Slight offset for title
        static_cast<float>(RACE_PANEL_WIDTH_LARGE),
        static_cast<float>(RACE_PANEL_HEIGHT_LARGE)
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

// Destructor to cleanup texture
CharacterSelectionPanel::~CharacterSelectionPanel() {
    if (backgroundTexture.id != 0) {
        UnloadTexture(backgroundTexture);
    }
}
