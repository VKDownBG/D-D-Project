//
// Created by Lenovo on 16.5.2025 г.
//

#include "C:/DandD/include/UI/panels/EquipmentPanel.h"
#include <cmath>
#include <iomanip>

EquipmentPanel::EquipmentPanel(Hero *_player)
    : player(_player),
      isVisible(false),
      currentItem(nullptr),
      newItem(nullptr),
      panelBackgroundColor(BLACK) {
    // Add configurable background color
}

void EquipmentPanel::Show(const Item *_currentItem, const Item *_newItem) {
    currentItem = _currentItem;
    newItem = _newItem;

    if (newItem) {
        itemType = newItem->GetType();
    }

    SetupButtons();
    isVisible = true;
}

void EquipmentPanel::ShowComparison(const Item *newTreasure) {
    if (!newTreasure) {
        return;
    }

    const Inventory &inventory = player->GetInventory();
    const Item *currentItem = nullptr;
    std::string typeStr = newTreasure->GetTypeStr();

    if (typeStr == "ARMOR") {
        if (inventory.hasArmor()) {
            currentItem = &inventory.GetArmor();
        }
        itemType = ItemType::ARMOR;
    } else if (typeStr == "WEAPON") {
        currentItem = &inventory.GetWeapon();
        itemType = ItemType::WEAPON;
    } else if (typeStr == "SPELL") {
        currentItem = &inventory.GetSpell();
        itemType = ItemType::SPELL;
    }

    Show(currentItem, newTreasure);
}

void EquipmentPanel::SetupButtons() {
    const int screenWidth = GetScreenWidth();
    const int screenHeight = GetScreenHeight();

    const int totalWidth = PANEL_WIDTH * 2 + PANEL_SPACING;
    const int startX = (screenWidth - totalWidth) / 2;
    const int startY = (screenHeight - PANEL_HEIGHT) / 2;

    const int leftPanelX = startX;
    const int rightPanelX = startX + PANEL_WIDTH + PANEL_SPACING;

    // Create smaller buttons at the bottom of each panel instead of covering the entire panel
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

    leftButton = Button(leftButtonRect, "Keep Current", [this]() {
        this->OnKeepCurrentEquipment();
    });

    rightButton = Button(rightButtonRect, "Equip New", [this]() {
        this->OnEquipNewItem();
    });

    // Set solid background colors instead of transparent
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

void EquipmentPanel::SetPanelBackgroundColor(const Color color) {
    panelBackgroundColor = color;
}

void EquipmentPanel::Hide() {
    isVisible = false;
}

bool EquipmentPanel::IsVisible() const {
    return isVisible;
}

void EquipmentPanel::Update() {
    if (!isVisible) return;

    // Only update buttons when panel is visible and handle input carefully
    Vector2 mousePos = GetMousePosition();

    leftButton.Update(mousePos);
    rightButton.Update(mousePos);

    // Handle keyboard input
    if (IsKeyPressed(KEY_ESCAPE)) {
        Hide();
    }

    // Alternative keyboard controls
    if (IsKeyPressed(KEY_ONE) || IsKeyPressed(KEY_KP_1)) {
        OnKeepCurrentEquipment();
    }
    if (IsKeyPressed(KEY_TWO) || IsKeyPressed(KEY_KP_2)) {
        OnEquipNewItem();
    }
}

void EquipmentPanel::Draw() const {
    if (!isVisible) return;

    const int screenWidth = GetScreenWidth();
    const int screenHeight = GetScreenHeight();

    const int totalWidth = PANEL_WIDTH * 2 + PANEL_SPACING;
    const int startX = (screenWidth - totalWidth) / 2;
    const int startY = (screenHeight - PANEL_HEIGHT) / 2;

    const int leftPanelX = startX;
    const int rightPanelX = startX + PANEL_WIDTH + PANEL_SPACING;

    // Draw semi-transparent overlay
    DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.5f));

    // Draw panel backgrounds
    DrawRectangle(leftPanelX, startY, PANEL_WIDTH, PANEL_HEIGHT, panelBackgroundColor);
    DrawRectangle(rightPanelX, startY, PANEL_WIDTH, PANEL_HEIGHT, panelBackgroundColor);

    // Draw panel borders
    DrawRectangleLines(leftPanelX, startY, PANEL_WIDTH, PANEL_HEIGHT, WHITE);
    DrawRectangleLines(rightPanelX, startY, PANEL_WIDTH, PANEL_HEIGHT, WHITE);

    // Draw panel content
    DrawPanelContent(leftPanelX, startY, currentItem, false);
    DrawPanelContent(rightPanelX, startY, newItem, true);

    // Draw buttons
    leftButton.Draw();
    rightButton.Draw();

    // Draw titles
    const std::string currentTitle = currentItem ? "Current Equipment" : "No Equipment";
    const std::string newTitle = "New Equipment";

    const int currentTitleWidth = MeasureText(currentTitle.c_str(), FONT_SIZE);
    const int newTitleWidth = MeasureText(newTitle.c_str(), FONT_SIZE);

    DrawText(currentTitle.c_str(), leftPanelX + (PANEL_WIDTH - currentTitleWidth) / 2, startY - 30, FONT_SIZE, WHITE);
    DrawText(newTitle.c_str(), rightPanelX + (PANEL_WIDTH - newTitleWidth) / 2, startY - 30, FONT_SIZE, WHITE);

    // Draw instructions
    const std::string instruction = "Click to choose equipment, press 1/2 for keyboard selection, or ESC to close";
    const int instructionWidth = MeasureText(instruction.c_str(), SMALL_FONT_SIZE);
    DrawText(instruction.c_str(), (screenWidth - instructionWidth) / 2, startY + PANEL_HEIGHT + 10, SMALL_FONT_SIZE,
             LIGHTGRAY);

    // Draw hover hints
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

void EquipmentPanel::DrawPanelContent(const int x, const int y, const Item *item, const bool showComparison) const {
    if (!item) {
        const std::string noEquipText = "No Equipment";
        const int textWidth = MeasureText(noEquipText.c_str(), FONT_SIZE);
        DrawText(noEquipText.c_str(), x + (PANEL_WIDTH - textWidth) / 2, y + PANEL_HEIGHT / 2 - 10, FONT_SIZE,
                 DARKGRAY);

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

    DrawItemIcon(x + 10, y + 10, item);

    std::string name = item->GetName();
    const int nameWidth = MeasureText(name.c_str(), FONT_SIZE);
    if (nameWidth > PANEL_WIDTH - 20) {
        name = item->GetName().substr(0, PANEL_WIDTH - 20) + "...";
    }
    DrawText(name.c_str(), x + 10, y + 85, FONT_SIZE, WHITE);

    const std::string typeStr = item->GetTypeStr();
    DrawText(typeStr.c_str(), x + 10, y + 110, SMALL_FONT_SIZE, LIGHTGRAY);

    DrawItemStats(x + 10, y + 140, item, showComparison);
}

void EquipmentPanel::DrawItemIcon(int x, int y, const Item *item) const {
    const Rectangle iconRect = {static_cast<float>(x), static_cast<float>(y), static_cast<float>(ICON_SIZE)};

    Color bgColor;
    std::string symbol;

    // Use the item's actual type instead of relying on member variable
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
        symbol = "?";
    }

    DrawRectangleRec(iconRect, bgColor);
    DrawRectangleLinesEx(iconRect, 2, WHITE);

    const int symbolWidth = MeasureText(symbol.c_str(), 40);
    DrawText(symbol.c_str(), x + (ICON_SIZE - symbolWidth) / 2, y + (ICON_SIZE - 40) / 2, 40, WHITE);
}

void EquipmentPanel::DrawItemStats(int x, int y, const Item *item, bool showComparison) const {
    int lineHeight = 30;
    int currentY = y;

    std::string levelText = "Level: " + std::to_string(item->GetLevel());
    Color levelColor = WHITE;

    if (showComparison && currentItem) {
        std::string diff = GetLevelDifference(currentItem->GetLevel(), item->GetLevel());
        if (!diff.empty()) {
            levelText += " (" + diff + ")";
            levelColor = GetLevelDifferenceColor(currentItem->GetLevel(), item->GetLevel());
        }
    } else if (showComparison && !currentItem) {
        levelText += " (+" + std::to_string(item->GetLevel()) + ")";
        levelColor = GREEN;
    }

    DrawText(levelText.c_str(), x, currentY, SMALL_FONT_SIZE, levelColor);
    currentY += lineHeight;

    std::stringstream ss;
    ss << std::fixed << std::setprecision(1) << item->GetBonus();
    std::string bonusText = "Bonus: " + ss.str() + "%";
    Color bonusColor = WHITE;

    if (showComparison && currentItem) {
        std::string diff = GetBonusDifference(currentItem->GetBonus(), item->GetBonus());
        if (!diff.empty()) {
            bonusText += " (" + diff + ")";
            bonusColor = GetDifferenceColor(currentItem->GetBonus(), item->GetBonus());
        }
    } else if (showComparison && !currentItem) {
        std::stringstream diffStream;
        diffStream << std::fixed << std::setprecision(1) << item->GetBonus();
        bonusText += " (+" + diffStream.str() + "%)";
        bonusColor = GREEN;
    }

    DrawText(bonusText.c_str(), x, currentY, SMALL_FONT_SIZE, bonusColor);
    currentY += lineHeight;

    // Generate description based on the actual item type
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
        currentY += 10;

        const int maxWidth = PANEL_WIDTH - 20;
        std::string line;
        std::stringstream words(description);
        std::string word;

        while (words >> word) {
            std::string testLine = line.empty() ? word : line + " " + word;
            if (MeasureText(testLine.c_str(), SMALL_FONT_SIZE) > maxWidth) {
                if (!line.empty()) {
                    DrawText(line.c_str(), x, currentY, SMALL_FONT_SIZE, LIGHTGRAY);
                    currentY += 20;
                    line = word;
                } else {
                    DrawText(word.c_str(), x, currentY, SMALL_FONT_SIZE, LIGHTGRAY);
                    currentY += 20;
                }
            } else {
                line = testLine;
            }
        }

        if (!line.empty()) {
            DrawText(line.c_str(), x, currentY, SMALL_FONT_SIZE, LIGHTGRAY);
        }
    }
}

void EquipmentPanel::OnKeepCurrentEquipment() {
    Hide();
}

void EquipmentPanel::OnEquipNewItem() {
    if (!newItem) {
        Hide();
        return;
    }

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

    Hide();
}

std::string EquipmentPanel::GetBonusDifference(const double currentBonus, const double newBonus) const {
    const double difference = newBonus - currentBonus;
    if (fabs(difference) < 0.05) return "";

    std::stringstream ss;
    ss << std::fixed << std::setprecision(1) << (difference > 0 ? "+" : "") << std::abs(difference);

    return ss.str();
}

std::string EquipmentPanel::GetLevelDifference(int currentLevel, int newLevel) const {
    int difference = newLevel - currentLevel;

    if (difference > 0) {
        return "+" + std::to_string(difference);
    } else if (difference < 0) {
        return std::to_string(difference);
    }

    return "";
}

Color EquipmentPanel::GetDifferenceColor(const double current, const double newValue) const {
    const double difference = newValue - current;

    if (difference > 0.05) {
        return GREEN;
    } else if (difference < -0.05) {
        return RED;
    }

    return WHITE;
}

Color EquipmentPanel::GetLevelDifferenceColor(const int currentLevel, const int newLevel) const {
    const int difference = newLevel - currentLevel;

    if (difference > 0) {
        return GREEN;
    } else if (difference < 0) {
        return RED;
    }

    return WHITE;
}
