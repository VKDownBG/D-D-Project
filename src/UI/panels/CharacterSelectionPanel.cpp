//
// Created by Lenovo on 3.6.2025 г.
//

#include "C:/DandD/include/UI/panels/CharacterSelectionPanel.h"
#include <iostream>
#include <sstream>
#include <vector>

// Constructor for CharacterSelectionPanel.
// Initializes screen dimensions, default states, colors, and loads initial data.
CharacterSelectionPanel::CharacterSelectionPanel(const int screenWidth, const int screenHeight)
    : screenWidth(screenWidth), screenHeight(screenHeight),
      backgroundTexture({0}), // Initialize texture to empty
      selectedRaceIndex(-1), // No race selected initially
      isVisible(false), // Panel is not visible by default
      selectionConfirmed(false), // Selection not confirmed initially
      panelColor({0, 51, 102, 255}), // Dark blue for panel backgrounds
      selectedColor({100, 150, 200, 255}), // Lighter blue for selected panel
      textColor(WHITE), // White text color
      headerColor({220, 220, 255, 255}), // Light blue for headers
      titleFont(GetFontDefault()), // Default font for titles
      textFont(GetFontDefault()) // Default font for general text
{
    InitializeRaceData(); // Populate race data
    CreateRaceButtons(); // Create UI buttons for race selection and controls
    LoadBackgroundImage(); // Load the panel's background image
}

// Initializes fonts and loads the background image.
// This method might be called separately after construction for re-initialization.
void CharacterSelectionPanel::Initialize() {
    // Initialize fonts with default Raylib fonts.
    // Ideally, custom fonts would be loaded here if available.
    titleFont = GetFontDefault();
    textFont = GetFontDefault();
    LoadBackgroundImage(); // Reloads or loads the background image.
}

// Loads the background image for the panel.
// Includes a fallback to a generated gradient image if the texture fails to load.
void CharacterSelectionPanel::LoadBackgroundImage() {
    // Load background image from a specified path.
    backgroundTexture = LoadTexture("C:/DandD/assets/background/walls4.png");
    // If texture loading fails (id is 0), generate a simple gradient image as a fallback.
    if (backgroundTexture.id == 0) {
        const Image img = GenImageGradientRadial(screenWidth, screenHeight, 0.0f, BLACK, DARKBLUE);
        backgroundTexture = LoadTextureFromImage(img);
        UnloadImage(img); // Unload the CPU-side image data after converting to GPU texture.
    }
}

// Populates the 'races' vector with predefined character race data.
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

// Creates the interactive buttons for race selection, confirm, and back.
// Clears existing buttons and re-creates them, allowing for dynamic resizing or re-layout.
void CharacterSelectionPanel::CreateRaceButtons() {
    raceButtons.clear(); // Clear existing buttons to avoid duplicates on re-creation.

    // Create a button for each race defined in the 'races' vector.
    for (int i = 0; i < races.size(); ++i) {
        const Rectangle bounds = GetRacePanelBounds(i); // Calculate bounds for each race panel.

        // Create a unique pointer to a Button object.
        auto button = std::make_unique<Button>(
            bounds, // Button position and size
            races[i].name, // Text label for the button
            [this, i]() { OnRaceButtonClicked(i); } // Lambda function as click action
        );

        // Set visual styles for the race buttons.
        button->SetColors(
            {60, 60, 80, 200}, // Normal state color
            {80, 80, 120, 220}, // Hover state color
            {100, 100, 140, 240}, // Pressed state color
            WHITE // Text color
        );
        button->SetBorder(WHITE, 2); // White border with 2 thickness
        button->SetRoundedCorners(10.0f); // Apply rounded corners
        button->EnableHoverAnimation(true, 1.05f, 8.0f); // Enable subtle hover scaling animation

        raceButtons.push_back(std::move(button)); // Add the unique_ptr to the vector.
    }

    // Create the "Confirm Selection" button.
    const Rectangle confirmBounds = {
        static_cast<float>(screenWidth / 2 - 100),
        static_cast<float>(screenHeight - 120),
        200, BUTTON_HEIGHT // BUTTON_HEIGHT is a defined constant
    };

    confirmButton = Button(confirmBounds, "Confirm Selection", [this]() { OnConfirmClicked(); });
    // Set colors and styling for the confirm button.
    confirmButton.SetColors({50, 150, 50, 200}, {70, 170, 70, 220}, {90, 190, 90, 240}, WHITE); // Greenish colors
    confirmButton.SetBorder(WHITE, 2);
    confirmButton.SetRoundedCorners(8.0f);
    confirmButton.EnableHoverAnimation(true, 1.1f, 6.0f);

    // Create the "Back" button.
    Rectangle backBounds = {
        20, static_cast<float>(screenHeight - 70), 100, BUTTON_HEIGHT
    };

    backButton = Button(backBounds, "Back", [this]() { OnBackClicked(); });
    // Set colors and styling for the back button (steel blue scheme).
    backButton.SetColors({70, 130, 180, 200}, {90, 150, 200, 220}, {110, 170, 220, 240}, WHITE);
    backButton.SetBorder(WHITE, 2);
    backButton.SetRoundedCorners(8.0f);
    backButton.EnableHoverAnimation(true, 1.1f, 6.0f);
}

// Makes the character selection panel visible and resets its state.
void CharacterSelectionPanel::Show() {
    isVisible = true;
    selectionConfirmed = false; // Reset confirmation state
    selectedRaceIndex = -1; // Clear any previous selection
}

// Hides the character selection panel.
void CharacterSelectionPanel::Hide() {
    isVisible = false;
}

// Updates the state of the panel's elements, primarily handling button interactions.
void CharacterSelectionPanel::Update() {
    if (!isVisible) return; // Only update if the panel is visible.

    Vector2 mousePos = GetMousePosition(); // Get current mouse cursor position.

    // Update all race selection buttons.
    for (auto &button: raceButtons) {
        button->Update(mousePos);
    }

    // Update the confirm and back control buttons.
    confirmButton.Update(mousePos);
    backButton.Update(mousePos);

    // Enable or disable the confirm button based on whether a race has been selected.
    confirmButton.SetActive(selectedRaceIndex != -1);
}

// Draws all elements of the character selection panel to the screen.
void CharacterSelectionPanel::Draw() const {
    if (!isVisible) return; // Only draw if the panel is visible.

    // Draw background image, scaled to cover the screen while maintaining aspect ratio.
    if (backgroundTexture.id != 0) {
        const float scaleX = static_cast<float>(screenWidth) / backgroundTexture.width;
        const float scaleY = static_cast<float>(screenHeight) / backgroundTexture.height;
        const float scale = (scaleX > scaleY) ? scaleX : scaleY; // Choose larger scale to ensure coverage.

        // Calculate origin to center the scaled texture.
        Vector2 origin = {
            (screenWidth - backgroundTexture.width * scale) / 2,
            (screenHeight - backgroundTexture.height * scale) / 2
        };

        DrawTextureEx(backgroundTexture, origin, 0.0f, scale, WHITE); // Draw the texture.
    }

    // Draw a semi-transparent black overlay for readability over the background.
    DrawRectangle(0, 0, screenWidth, screenHeight, {0, 0, 0, 120});

    // Draw the panel title.
    const char *title = "Choose Your Character";
    const int titleFontSize = 48;
    const Vector2 titleSize = MeasureTextEx(titleFont, title, titleFontSize, 2); // Measure text size.
    const Vector2 titlePos = {
        (screenWidth - titleSize.x) / 2, // Center title horizontally.
        60
    };

    // Draw title with a slight shadow effect.
    DrawTextEx(titleFont, title, {titlePos.x + 2, titlePos.y + 2}, titleFontSize, 2, BLACK);
    DrawTextEx(titleFont, title, titlePos, titleFontSize, 2, headerColor);

    // Draw each race panel, displaying race-specific information.
    for (int i = 0; i < races.size(); ++i) {
        Rectangle bounds = GetRacePanelBounds(i); // Get calculated bounds for the panel.
        DrawRacePanel(races[i], i, bounds); // Draw the panel content.
    }

    // Draw the interactive buttons over the panels.
    for (const auto &button: raceButtons) {
        button->Draw();
    }

    confirmButton.Draw();
    backButton.Draw();

    // Draw a visual indicator (gold border) around the currently selected race panel.
    if (selectedRaceIndex != -1) {
        Rectangle selectedBounds = GetRacePanelBounds(selectedRaceIndex);
        DrawRectangleRoundedLines(selectedBounds, 12.0f, 12, 4, {255, 215, 0, 255}); // Gold border
    }
}

// Draws a single race panel, including its background, name, stats, and description.
void CharacterSelectionPanel::DrawRacePanel(const RaceData &raceData, const int index, const Rectangle bounds) const {
    // Determine background color: selected color if chosen, otherwise normal panel color.
    Color bgColor = (selectedRaceIndex == index) ? selectedColor : panelColor;
    DrawRectangleRounded(bounds, 7.0f, 12, bgColor); // Draw rounded background.
    DrawRectangleRoundedLines(bounds, 7, 12, 2, bgColor); // Draw rounded border with same color (subtle effect).

    float yOffset = bounds.y + 25; // Initial Y position for drawing content within the panel.
    const float leftMargin = bounds.x + 20; // Left margin for content.
    const float rightMargin = bounds.x + bounds.width - 20; // Right margin for content.

    // Draw Race name, centered.
    int nameFontSize = 28;
    Vector2 nameSize = MeasureTextEx(textFont, raceData.name.c_str(), nameFontSize, 1);
    Vector2 namePos = {bounds.x + (bounds.width - nameSize.x) / 2, yOffset};
    DrawTextEx(textFont, raceData.name.c_str(), namePos, nameFontSize, 1, headerColor);
    yOffset += nameSize.y + 25; // Move Y offset down.

    // Draw "Stats:" label.
    DrawTextEx(textFont, "Stats:", {leftMargin, yOffset}, 20, 1, textColor);
    yOffset += 30;

    // Draw Strength stat bar.
    Rectangle strBounds = {leftMargin, yOffset + 20, rightMargin - leftMargin, 25};
    DrawStatBar("Strength", raceData.strength, 50, strBounds, {220, 80, 80, 255}); // Red color for strength.
    yOffset += 50;

    // Draw Mana stat bar.
    Rectangle manaBounds = {leftMargin, yOffset + 20, rightMargin - leftMargin, 25};
    DrawStatBar("Mana", raceData.mana, 50, manaBounds, {80, 80, 220, 255}); // Blue color for mana.
    yOffset += 50;

    // Draw Health stat bar.
    Rectangle healthBounds = {leftMargin, yOffset + 20, rightMargin - leftMargin, 25};
    DrawStatBar("Health", raceData.health, 50, healthBounds, {80, 220, 80, 255}); // Green color for health.
    yOffset += 60;

    // Description section with manual word wrapping.
    int descFontSize = 14;
    float maxWidth = rightMargin - leftMargin; // Max width for text lines.
    const float lineHeight = 18; // Vertical spacing between lines.

    // Simple word wrapping logic:
    std::string text = raceData.description;
    std::vector<std::string> lines;
    std::string currentLine = "";
    std::istringstream words(text); // Use stringstream to iterate words.
    std::string word;

    while (words >> word) {
        // Read word by word.
        std::string testLine = currentLine.empty() ? word : currentLine + " " + word;
        Vector2 testSize = MeasureTextEx(textFont, testLine.c_str(), descFontSize, 1);

        if (testSize.x <= maxWidth) {
            // If adding word doesn't exceed max width, extend current line.
            currentLine = testLine;
        } else {
            // If it exceeds, push current line and start a new one.
            if (!currentLine.empty()) {
                lines.push_back(currentLine);
                currentLine = word; // Start new line with the current word.
            } else {
                lines.push_back(word); // If single word exceeds width, add it anyway.
                currentLine = "";
            }
        }
    }
    if (!currentLine.empty()) {
        // Add any remaining text as the last line.
        lines.push_back(currentLine);
    }

    // Draw the wrapped text lines for the description.
    for (int i = 0; i < lines.size() && yOffset + (i * lineHeight) < bounds.y + bounds.height - 15; ++i) {
        DrawTextEx(textFont, lines[i].c_str(), {leftMargin, yOffset + (i * lineHeight)}, descFontSize, 1,
                   {200, 200, 200, 255}); // Slightly gray text.
    }
}

// Draws a single stat bar with its label, current value, and maximum value.
void CharacterSelectionPanel::DrawStatBar(const char *label, const int value, const int maxValue,
                                          const Rectangle bounds,
                                          const Color barColor) const {
    // Draw the stat label (e.g., "Strength").
    DrawTextEx(textFont, label, {bounds.x, bounds.y - 22}, 16, 1, textColor);

    // Draw the background of the stat bar (e.g., empty part of the bar).
    DrawRectangleRounded(bounds, 6.0f, 8, {40, 40, 40, 255});

    // Draw the filled portion of the stat bar based on the value.
    float fillRatio = static_cast<float>(value) / static_cast<float>(maxValue);
    Rectangle fillBounds = {bounds.x, bounds.y, bounds.width * fillRatio, bounds.height};
    DrawRectangleRounded(fillBounds, 6.0f, 8, barColor);

    // Draw the numeric value text (e.g., "30/50") centered on the bar.
    std::string valueText = std::to_string(value) + "/" + std::to_string(maxValue);
    Vector2 textSize = MeasureTextEx(textFont, valueText.c_str(), 14, 1);
    Vector2 textPos = {
        bounds.x + (bounds.width - textSize.x) / 2,
        bounds.y + (bounds.height - textSize.y) / 2
    };
    DrawTextEx(textFont, valueText.c_str(), textPos, 14, 1, WHITE);
}

// Calculates the bounding rectangle for a race panel based on its index.
// This ensures panels are dynamically positioned and sized.
Rectangle CharacterSelectionPanel::GetRacePanelBounds(int index) const {
    const int RACE_PANEL_WIDTH_LARGE = 320; // Increased width for larger panels.
    const int RACE_PANEL_HEIGHT_LARGE = 450; // Increased height for larger panels.
    const int RACE_PANEL_SPACING_LARGE = 40; // Increased spacing between panels.

    // Calculate total width occupied by all panels and their spacing.
    int totalWidth = races.size() * RACE_PANEL_WIDTH_LARGE + (races.size() - 1) * RACE_PANEL_SPACING_LARGE;
    int startX = (screenWidth - totalWidth) / 2; // Calculate starting X to center the block of panels.

    return {
        static_cast<float>(startX + index * (RACE_PANEL_WIDTH_LARGE + RACE_PANEL_SPACING_LARGE)),
        // X position for this panel.
        static_cast<float>(screenHeight / 2 - RACE_PANEL_HEIGHT_LARGE / 2 + 20),
        // Y position, slightly offset from vertical center.
        static_cast<float>(RACE_PANEL_WIDTH_LARGE),
        static_cast<float>(RACE_PANEL_HEIGHT_LARGE)
    };
}

// Callback function executed when a race button is clicked.
// Sets the 'selectedRaceIndex' to the index of the clicked race.
void CharacterSelectionPanel::OnRaceButtonClicked(int raceIndex) {
    selectedRaceIndex = raceIndex;
}

// Callback function executed when the "Confirm Selection" button is clicked.
// Confirms the selection and triggers the 'onRaceSelected' callback if a race is chosen.
void CharacterSelectionPanel::OnConfirmClicked() {
    if (selectedRaceIndex != -1) {
        // Only confirm if a race has been selected.
        selectionConfirmed = true;
        if (onRaceSelected) {
            onRaceSelected(races[selectedRaceIndex].race); // Invoke the callback with the selected Race enum.
        }
    }
}

// Callback function executed when the "Back" button is clicked.
// Triggers the 'onBack' callback.
void CharacterSelectionPanel::OnBackClicked() {
    if (onBack) {
        onBack(); // Invoke the back callback.
    }
}

// Checks if the character selection panel is currently visible.
bool CharacterSelectionPanel::IsVisible() const {
    return isVisible;
}

// Checks if a race selection has been confirmed.
bool CharacterSelectionPanel::IsSelectionConfirmed() const {
    return selectionConfirmed;
}

// Returns the currently selected race enum.
// Defaults to Race::Human if no race is selected.
Race CharacterSelectionPanel::GetSelectedRace() const {
    if (selectedRaceIndex != -1) {
        return races[selectedRaceIndex].race;
    }
    return Race::Human; // Default fallback if no race is selected.
}

// Sets the callback function to be invoked when a race is successfully selected and confirmed.
void CharacterSelectionPanel::SetOnRaceSelected(std::function<void(Race)> callback) {
    onRaceSelected = callback;
}

// Sets the callback function to be invoked when the "Back" button is clicked.
void CharacterSelectionPanel::SetOnBack(std::function<void()> callback) {
    onBack = callback;
}

// Resets the panel's state, clearing any selection and confirmation.
void CharacterSelectionPanel::Reset() {
    selectedRaceIndex = -1;
    selectionConfirmed = false;
}

// Destructor for CharacterSelectionPanel.
// Responsible for unloading the background texture to prevent memory leaks.
CharacterSelectionPanel::~CharacterSelectionPanel() {
    if (backgroundTexture.id != 0) {
        // Only unload if the texture was actually loaded.
        UnloadTexture(backgroundTexture);
    }
}
