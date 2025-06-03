//
// Created by Lenovo on 16.5.2025 г.
//

#include "C:/DandD/include/UI/panels/EquipmentPanel.h"
#include <cmath>
#include <iomanip>

// Constructor for the EquipmentPanel.
// Initializes with a pointer to the player, sets visibility to false,
// and sets initial item pointers to null.
EquipmentPanel::EquipmentPanel(Hero *_player)
    : player(_player), // Stores a pointer to the main player object.
      isVisible(false), // Panel is initially hidden.
      currentItem(nullptr), // Pointer to the currently equipped item being compared.
      newItem(nullptr), // Pointer to the new item being considered for equipment.
      panelBackgroundColor(BLACK) {
    // Default background color for the panels.
    // Add configurable background color
}

// Shows the equipment comparison panel.
// Takes pointers to the current item and the new item to display.
void EquipmentPanel::Show(const Item *_currentItem, const Item *_newItem) {
    currentItem = _currentItem; // Set the current item.
    newItem = _newItem; // Set the new item.

    // Determine the type of the new item if it exists.
    if (newItem) {
        itemType = newItem->GetType();
    }

    SetupButtons(); // Re-creates and positions the action buttons.
    isVisible = true; // Makes the panel visible.
}

// Shows the equipment comparison panel, specifically for a new treasure found.
// Automatically determines the currently equipped item of the same type for comparison.
void EquipmentPanel::ShowComparison(const Item *newTreasure) {
    if (!newTreasure) {
        // If no new treasure, do nothing.
        return;
    }

    const Inventory &inventory = player->GetInventory(); // Get the player's inventory.
    const Item *currentItem = nullptr; // Initialize current item pointer.
    const std::string typeStr = newTreasure->GetTypeStr(); // Get the string representation of the new item's type.

    // Based on the new item's type, retrieve the corresponding currently equipped item from inventory.
    if (typeStr == "ARMOR") {
        currentItem = &inventory.GetArmor();
        itemType = ItemType::ARMOR;
    } else if (typeStr == "WEAPON") {
        currentItem = &inventory.GetWeapon();
        itemType = ItemType::WEAPON;
    } else if (typeStr == "SPELL") {
        currentItem = &inventory.GetSpell();
        itemType = ItemType::SPELL;
    }

    // Call the overloaded Show method to display the comparison.
    Show(currentItem, newTreasure);
}

// Sets up the position, size, and callbacks for the "Keep Current" and "Equip New" buttons.
void EquipmentPanel::SetupButtons() {
    const int screenWidth = GetScreenWidth();
    const int screenHeight = GetScreenHeight();

    // Calculate total width and starting X/Y for positioning the two panels side-by-side.
    const int totalWidth = PANEL_WIDTH * 2 + PANEL_SPACING;
    const int startX = (screenWidth - totalWidth) / 2;
    const int startY = (screenHeight - PANEL_HEIGHT) / 2;

    const int leftPanelX = startX;
    const int rightPanelX = startX + PANEL_WIDTH + PANEL_SPACING;

    // Define dimensions and position for the buttons at the bottom of each panel.
    const int buttonHeight = 50; // Smaller button height
    const int buttonY = startY + PANEL_HEIGHT - buttonHeight - 10; // Position at bottom of panel

    const Rectangle leftButtonRect = {
        static_cast<float>(leftPanelX + 10), static_cast<float>(buttonY),
        static_cast<float>(PANEL_WIDTH - 20), static_cast<float>(buttonHeight)
    };
    const Rectangle rightButtonRect = {
        static_cast<float>(rightPanelX + 10), static_cast<float>(buttonY),
        static_cast<float>(PANEL_WIDTH - 20), static_cast<float>(buttonHeight)
    };

    // Initialize the "Keep Current" button.
    leftButton = Button(leftButtonRect, "Keep Current", [this]() {
        this->OnKeepCurrentEquipment(); // Callback to keep the current item.
    });

    // Initialize the "Equip New" button.
    rightButton = Button(rightButtonRect, "Equip New", [this]() {
        this->OnEquipNewItem(); // Callback to equip the new item.
    });

    // Set colors, borders, and hover animations for both buttons.
    const Color normalColor = {40, 40, 40, 255}; // Darker background for buttons
    const Color hoverColor = {60, 60, 60, 255}; // Slightly lighter for hover
    const Color pressedColor = {80, 80, 80, 255}; // Even lighter for pressed

    leftButton.SetColors(normalColor, hoverColor, pressedColor, WHITE);
    leftButton.SetBorder({255, 255, 255, 150}, 2);
    leftButton.EnableHoverAnimation(true, 1.05f, 8.0f);

    rightButton.SetColors(normalColor, hoverColor, pressedColor, WHITE);
    rightButton.SetBorder({255, 255, 255, 150}, 2);
    rightButton.EnableHoverAnimation(true, 1.05f, 8.0f);
}

// Sets the background color of the comparison panels.
void EquipmentPanel::SetPanelBackgroundColor(const Color color) {
    panelBackgroundColor = color;
}

// Hides the equipment comparison panel.
void EquipmentPanel::Hide() {
    isVisible = false;
}

// Checks if the equipment comparison panel is currently visible.
bool EquipmentPanel::IsVisible() const {
    return isVisible;
}

// Updates the state of the equipment panel, including button interactions and keyboard input.
void EquipmentPanel::Update() {
    if (!isVisible) return; // Only update if the panel is visible.

    Vector2 mousePos = GetMousePosition();

    // Update the state of both buttons based on mouse position.
    leftButton.Update(mousePos);
    rightButton.Update(mousePos);

    // Handle keyboard input for closing the panel (ESC) or selecting options (1/2).
    if (IsKeyPressed(KEY_ESCAPE)) {
        Hide(); // Hide the panel.
        return;
    }

    if (IsKeyPressed(KEY_ONE) || IsKeyPressed(KEY_KP_1)) {
        OnKeepCurrentEquipment(); // Call the "keep current" logic.
        return;
    }
    if (IsKeyPressed(KEY_TWO) || IsKeyPressed(KEY_KP_2)) {
        OnEquipNewItem(); // Call the "equip new" logic.
        return;
    }
}

// Draws the equipment comparison panel and all its elements.
void EquipmentPanel::Draw() const {
    if (!isVisible) return; // Only draw if the panel is visible.

    const int screenWidth = GetScreenWidth();
    const int screenHeight = GetScreenHeight();

    // Calculate positions for the two comparison panels.
    const int totalWidth = PANEL_WIDTH * 2 + PANEL_SPACING;
    const int startX = (screenWidth - totalWidth) / 2;
    const int startY = (screenHeight - PANEL_HEIGHT) / 2;

    const int leftPanelX = startX;
    const int rightPanelX = startX + PANEL_WIDTH + PANEL_SPACING;

    // Draw a semi-transparent overlay over the entire screen to focus on the panel.
    DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.5f));

    // Draw the background of the left (current item) and right (new item) panels.
    DrawRectangle(leftPanelX, startY, PANEL_WIDTH, PANEL_HEIGHT, panelBackgroundColor);
    DrawRectangle(rightPanelX, startY, PANEL_WIDTH, PANEL_HEIGHT, panelBackgroundColor);

    // Draw borders around the panels.
    DrawRectangleLines(leftPanelX, startY, PANEL_WIDTH, PANEL_HEIGHT, WHITE);
    DrawRectangleLines(rightPanelX, startY, PANEL_WIDTH, PANEL_HEIGHT, WHITE);

    // Draw the content (item details) within each panel.
    DrawPanelContent(leftPanelX, startY, currentItem, false); // No comparison details for the current item itself.
    DrawPanelContent(rightPanelX, startY, newItem, true); // Show comparison details for the new item.

    // Draw the "Keep Current" and "Equip New" buttons.
    leftButton.Draw();
    rightButton.Draw();

    // Draw titles above each panel.
    const std::string currentTitle = currentItem ? "Current Equipment" : "No Equipment";
    const std::string newTitle = "New Equipment";

    // Measure text width for centering titles.
    const int currentTitleWidth = MeasureText(currentTitle.c_str(), FONT_SIZE);
    const int newTitleWidth = MeasureText(newTitle.c_str(), FONT_SIZE);

    DrawText(currentTitle.c_str(), leftPanelX + (PANEL_WIDTH - currentTitleWidth) / 2, startY - 30, FONT_SIZE, WHITE);
    DrawText(newTitle.c_str(), rightPanelX + (PANEL_WIDTH - newTitleWidth) / 2, startY - 30, FONT_SIZE, WHITE);

    // Draw general instructions for interaction.
    const std::string instruction = "Click to choose equipment, press 1/2 for keyboard selection, or ESC to close";
    const int instructionWidth = MeasureText(instruction.c_str(), SMALL_FONT_SIZE);
    DrawText(instruction.c_str(), (screenWidth - instructionWidth) / 2, startY + PANEL_HEIGHT + 10, SMALL_FONT_SIZE,
             LIGHTGRAY);

    // Draw hover hints when buttons are hovered.
    if (leftButton.IsHovered()) {
        const std::string keepHint = currentItem ? "Keep your current equipment" : "Keep no equipment in this slot";
        const int hintWidth = MeasureText(keepHint.c_str(), SMALL_FONT_SIZE);
        DrawText(keepHint.c_str(), (screenWidth - hintWidth) / 2, startY + PANEL_HEIGHT + 35, SMALL_FONT_SIZE, YELLOW);
    } else if (rightButton.IsHovered()) {
        const std::string equipHint = "Equip the new item";
        int hintWidth = MeasureText(equipHint.c_str(), SMALL_FONT_SIZE);
        DrawText(equipHint.c_str(), (screenWidth - hintWidth) / 2, startY + PANEL_HEIGHT + 35, SMALL_FONT_SIZE, YELLOW);
    }
}

// Draws the content of a single item panel (icon, name, type, stats).
// `showComparison` determines if stat differences should be highlighted.
void EquipmentPanel::DrawPanelContent(const int x, const int y, const Item *item, const bool showComparison) const {
    if (!item) {
        // If there's no item to display (e.g., empty slot).
        const std::string noEquipText = "No Equipment";
        const int textWidth = MeasureText(noEquipText.c_str(), FONT_SIZE);
        DrawText(noEquipText.c_str(), x + (PANEL_WIDTH - textWidth) / 2, y + PANEL_HEIGHT / 2 - 10, FONT_SIZE,
                 DARKGRAY);

        // Draw a placeholder icon.
        Rectangle iconRect = {
            static_cast<float>(x + 10), static_cast<float>(y + 10), static_cast<float>(ICON_SIZE),
            static_cast<float>(ICON_SIZE)
        };
        DrawRectangleRec(iconRect, DARKGRAY);
        DrawRectangleLinesEx(iconRect, 2, GRAY);

        const std::string symbol = "?";
        const int symbolWidth = MeasureText(symbol.c_str(), 40);
        DrawText(symbol.c_str(), x + 10 + (ICON_SIZE - symbolWidth) / 2, y + 10 + (ICON_SIZE - 40) / 2, 40, GRAY);

        return;
    }

    // Draw the item's icon.
    DrawItemIcon(x + 10, y + 10, item);

    // Draw the item's name, truncating if too long.
    std::string name = item->GetName();
    const int nameWidth = MeasureText(name.c_str(), FONT_SIZE);
    if (nameWidth > PANEL_WIDTH - 20) {
        name = item->GetName().substr(0, PANEL_WIDTH - 20) + "..."; // Truncate and add ellipsis.
    }
    DrawText(name.c_str(), x + 10, y + 85, FONT_SIZE, WHITE);

    // Draw the item's type.
    const std::string typeStr = item->GetTypeStr();
    DrawText(typeStr.c_str(), x + 10, y + 110, SMALL_FONT_SIZE, LIGHTGRAY);

    // Draw the item's stats, with comparison highlights if enabled.
    DrawItemStats(x + 10, y + 140, item, showComparison);
}

// Draws the icon for a given item, using a symbol and color based on its type.
void EquipmentPanel::DrawItemIcon(int x, int y, const Item *item) const {
    const Rectangle iconRect = {
        static_cast<float>(x), static_cast<float>(y), static_cast<float>(ICON_SIZE), static_cast<float>(ICON_SIZE)
    };

    Color bgColor;
    std::string symbol;

    // Assign color and symbol based on item type.
    const std::string typeStr = item->GetTypeStr();
    if (typeStr == "ARMOR") {
        bgColor = BLUE;
        symbol = "A";
    } else if (typeStr == "WEAPON") {
        bgColor = RED;
        symbol = "W";
    } else if (typeStr == "SPELL") {
        bgColor = PURPLE;
        symbol = "S";
    } else {
        bgColor = GRAY;
        symbol = "?"; // Default for unknown types.
    }

    DrawRectangleRec(iconRect, bgColor); // Draw icon background.
    DrawRectangleLinesEx(iconRect, 2, WHITE); // Draw icon border.

    // Draw the symbol centered within the icon.
    const int symbolWidth = MeasureText(symbol.c_str(), 40);
    DrawText(symbol.c_str(), x + (ICON_SIZE - symbolWidth) / 2, y + (ICON_SIZE - 40) / 2, 40, WHITE);
}

// Draws the stats of an item, optionally showing a comparison to the current item.
void EquipmentPanel::DrawItemStats(int x, int y, const Item *item, bool showComparison) const {
    int lineHeight = 30;
    int currentY = y;

    // Display Level stat.
    std::string levelText = "Level: " + std::to_string(item->GetLevel());
    Color levelColor = WHITE;

    if (showComparison && currentItem) {
        // If comparing, calculate and display level difference.
        std::string diff = GetLevelDifference(currentItem->GetLevel(), item->GetLevel());
        if (!diff.empty()) {
            levelText += " (" + diff + ")";
            levelColor = GetLevelDifferenceColor(currentItem->GetLevel(), item->GetLevel());
        }
    } else if (showComparison && !currentItem) {
        // If there's no current item but a new one, show its level as a positive gain.
        levelText += " (+" + std::to_string(item->GetLevel()) + ")";
        levelColor = GREEN;
    }

    DrawText(levelText.c_str(), x, currentY, SMALL_FONT_SIZE, levelColor);
    currentY += lineHeight;

    // Display Bonus stat.
    std::stringstream ss;
    ss << std::fixed << std::setprecision(1) << item->GetBonus(); // Format bonus to one decimal place.
    std::string bonusText = "Bonus: " + ss.str() + "%";
    Color bonusColor = WHITE;

    if (showComparison && currentItem) {
        // If comparing, calculate and display bonus difference.
        std::string diff = GetBonusDifference(currentItem->GetBonus(), item->GetBonus());
        if (!diff.empty()) {
            bonusText += " (" + diff + ")";
            bonusColor = GetDifferenceColor(currentItem->GetBonus(), item->GetBonus());
        }
    } else if (showComparison && !currentItem) {
        // If no current item, show new item's bonus as a positive gain.
        std::stringstream diffStream;
        diffStream << std::fixed << std::setprecision(1) << item->GetBonus();
        bonusText += " (+" + diffStream.str() + "%)";
        bonusColor = GREEN;
    }

    DrawText(bonusText.c_str(), x, currentY, SMALL_FONT_SIZE, bonusColor);
    currentY += lineHeight;

    // Generate and display description based on item type.
    std::string typeStr = item->GetTypeStr();
    std::string description;
    if (typeStr == "WEAPON") {
        description = "Increases attack damage";
    } else if (typeStr == "ARMOR") {
        description = "Reduces damage taken";
    } else if (typeStr == "SPELL") {
        description = "Increases spell power";
    }

    if (!description.empty()) {
        currentY += 10; // Add some vertical spacing before description.

        const int maxWidth = PANEL_WIDTH - 20; // Max width for description text.
        std::string line;
        std::stringstream words(description);
        std::string word;

        // Simple word wrapping logic for the description.
        while (words >> word) {
            std::string testLine = line.empty() ? word : line + " " + word;
            if (MeasureText(testLine.c_str(), SMALL_FONT_SIZE) > maxWidth) {
                if (!line.empty()) {
                    DrawText(line.c_str(), x, currentY, SMALL_FONT_SIZE, LIGHTGRAY);
                    currentY += 20; // Move to next line.
                    line = word; // Start new line with current word.
                } else {
                    DrawText(word.c_str(), x, currentY, SMALL_FONT_SIZE, LIGHTGRAY); // Word itself is too long.
                    currentY += 20;
                }
            } else {
                line = testLine; // Add word to current line.
            }
        }

        if (!line.empty()) {
            DrawText(line.c_str(), x, currentY, SMALL_FONT_SIZE, LIGHTGRAY); // Draw any remaining text.
        }
    }
}

// Callback when the "Keep Current" button is pressed.
void EquipmentPanel::OnKeepCurrentEquipment() {
    isVisible = false; // Hide the panel.
}

// Callback when the "Equip New" button is pressed.
void EquipmentPanel::OnEquipNewItem() {
    if (!newItem) {
        // If there's no new item to equip, just hide the panel.
        Hide();
        return;
    }

    // Determine the type of the new item and call the appropriate inventory method to equip it.
    const std::string typeStr = newItem->GetTypeStr();
    if (typeStr == "ARMOR") {
        const Armor *armorItem = static_cast<const Armor *>(newItem);
        player->GetInventory().newArmor(*armorItem);
    } else if (typeStr == "WEAPON") {
        const Weapon *weaponItem = static_cast<const Weapon *>(newItem);
        player->GetInventory().newWeapon(*weaponItem);
    } else if (typeStr == "SPELL") {
        const Spell *spellItem = static_cast<const Spell *>(newItem);
        player->GetInventory().newSpell(*spellItem);
    }

    isVisible = false; // Hide the panel after equipping.
}

// Calculates and returns the string representation of the bonus difference.
// Returns an empty string if the difference is negligible.
std::string EquipmentPanel::GetBonusDifference(const double currentBonus, const double newBonus) const {
    const double difference = newBonus - currentBonus;
    if (fabs(difference) < 0.05) return ""; // Ignore very small differences.

    std::stringstream ss;
    ss << std::fixed << std::setprecision(1) << (difference > 0 ? "+" : "") << std::abs(difference);
    // Format with sign.

    return ss.str();
}

// Calculates and returns the string representation of the level difference.
// Returns an empty string if there is no difference.
std::string EquipmentPanel::GetLevelDifference(int currentLevel, int newLevel) const {
    int difference = newLevel - currentLevel;

    if (difference > 0) {
        return "+" + std::to_string(difference); // Positive difference.
    } else if (difference < 0) {
        return std::to_string(difference); // Negative difference.
    }

    return ""; // No difference.
}

// Returns a color (GREEN, RED, or WHITE) based on the numerical difference between two values.
Color EquipmentPanel::GetDifferenceColor(const double current, const double newValue) const {
    const double difference = newValue - current;

    if (difference > 0.05) {
        // Significant positive difference.
        return GREEN;
    } else if (difference < -0.05) {
        // Significant negative difference.
        return RED;
    }

    return WHITE; // No significant difference.
}

// Returns a color (GREEN, RED, or WHITE) based on the level difference.
Color EquipmentPanel::GetLevelDifferenceColor(const int currentLevel, const int newLevel) const {
    const int difference = newLevel - currentLevel;

    if (difference > 0) {
        // New level is higher.
        return GREEN;
    } else if (difference < 0) {
        // New level is lower.
        return RED;
    }

    return WHITE; // Levels are the same.
}
