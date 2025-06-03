//
// Created by Lenovo on 15.5.2025 г.
//

#include "C:/DandD/include/UI/screens/GameHUD.h"
#include <cmath>
#include <iomanip>

// Constructor for GameHUD, initializes UI elements and their properties
GameHUD::GameHUD(const int _screenWidth, const int _screenHeight)
    : hero(nullptr), armorItem(nullptr), weaponItem(nullptr),
      spellItem(nullptr), screenWidth(_screenWidth), screenHeight(_screenHeight),
      hpBar(CalculateHPBarBounds(), 50.0f), // Initializes HP bar with calculated bounds and initial value
      xpBar(CalculateXPBarBounds(), 0.0f), // Initializes XP bar with calculated bounds and initial value
      inventoryHoverIndex(-1),
      currentLevel(1),
      monstersRemaining(0),
      treasuresRemaining(0),
      backgroundLoaded(false) {
    // Define UI colors
    frameColor = {40, 40, 70, 230};
    textColor = {220, 220, 250, 255};
    darkPurple = {30, 20, 50, 150};
    mysticBlue = {60, 80, 120, 255};
    deepRed = {150, 30, 50, 255};
    stoneGray = {80, 80, 90, 200};

    // armorButton = Button(CalculateArmorButtonBounds(), "", nullptr);
    // weaponButton = Button(CalculateWeaponButtonBounds(), "", nullptr);
    // spellButton = Button(CalculateSpellButtonBounds(), "", nullptr);

    // Alternative color scheme (commented out)
    // frameColor = {30, 30, 45, 255};
    // textColor = {220, 200, 235, 255};
    // darkPurple = {60, 50, 90, 255};
    // mysticBlue = {80, 120, 200, 255};
    // deepRed = {180, 40, 40, 255};
    // stoneGray = {90, 90, 110, 255};

    // Initialize equipment buttons with calculated bounds and default properties
    armorButton = Button(CalculateArmorButtonBounds(), "", nullptr);
    weaponButton = Button(CalculateWeaponButtonBounds(), "", nullptr);
    spellButton = Button(CalculateSpellButtonBounds(), "", nullptr);

    // Define colors for equipment buttons
    const Color equipButtonColor = {60, 60, 85, 255};
    const Color equipHoverColor = {75, 75, 110, 255};
    const Color equipPressColor = {90, 90, 130, 255};
    const Color equipBorderColor = {110, 110, 170, 255};

    // Set colors, borders, and hover animations for armor button
    armorButton.SetColors(equipButtonColor, equipHoverColor, equipPressColor, textColor);
    armorButton.SetBorder(equipBorderColor, 1);
    armorButton.EnableHoverAnimation(true, 1.1f, 8.0f);

    // Set colors, borders, and hover animations for weapon button
    weaponButton.SetColors(equipButtonColor, equipHoverColor, equipPressColor, textColor);
    weaponButton.SetBorder(equipBorderColor, 1);
    weaponButton.EnableHoverAnimation(true, 1.1f, 8.0f);

    // Set colors, borders, and hover animations for spell button
    spellButton.SetColors(equipButtonColor, equipHoverColor, equipPressColor, textColor);
    spellButton.SetBorder(equipBorderColor, 1);
    spellButton.EnableHoverAnimation(true, 1.1f, 8.0f);

    // Configure HP bar properties
    hpBar.SetColors({40, 40, 50, 255}, deepRed, {78, 15, 15, 255});
    hpBar.SetBorderThickness(3.0f);
    hpBar.SetRounding(0.2f);
    hpBar.ShowText(true);
    hpBar.SetTextPrefix("HP: ");
    // Lambda function to format HP text
    hpBar.SetTextFormat([](float current, float max) {
        return std::to_string(static_cast<int>(current)) + "/" + std::to_string(static_cast<int>(max));
    });
    hpBar.EnablePulsatingEffect(true, 0.2f, 0.8f);

    // Configure XP bar properties
    xpBar.SetColors(mysticBlue, {35, 152, 80, 255}, {9, 9, 125, 255});
    xpBar.SetBorderThickness(3.0f);
    xpBar.SetRounding(0.2f);
    xpBar.ShowText(true);
    xpBar.SetTextPrefix("XP: ");
    // Lambda function to format XP text as a percentage
    xpBar.SetTextFormat([](const float current, const float max) {
        const int percentage = static_cast<int>((current / max) * 100.0f);
        return std::to_string(percentage) + "%";
    });
    xpBar.EnablePulsatingEffect(true, 0.1f, 0.6f);
}

// Destructor for GameHUD, unloads resources
GameHUD::~GameHUD() {
    Unload();
}

// Initializes the GameHUD with a reference to the hero object
void GameHUD::Initialize(Hero *heroRef) {
    hero = heroRef;

    // Set initial active state of equipment buttons based on whether an item is equipped
    armorButton.SetActive(armorItem != nullptr);
    weaponButton.SetActive(weaponItem != nullptr);
    spellButton.SetActive(spellItem != nullptr);
}

// Sets the armor item and updates the armor button's active state
void GameHUD::SetArmor(const Armor *armor) {
    armorItem = armor;
    armorButton.SetActive(armorItem != nullptr);
}

// Sets the weapon item and updates the weapon button's active state
void GameHUD::SetWeapon(const Weapon *weapon) {
    weaponItem = weapon;
    weaponButton.SetActive(weaponItem != nullptr);
}

// Sets the spell item and updates the spell button's active state
void GameHUD::SetSpell(const Spell *spell) {
    spellItem = spell;
    spellButton.SetActive(spellItem != nullptr);
}

// Sets the current level
void GameHUD::SetLevel(const int level) {
    currentLevel = level;
}

// Sets the count of remaining monsters
void GameHUD::SetMonstersRemaining(const int count) {
    monstersRemaining = count;
}

// Sets the count of remaining treasures
void GameHUD::SetTreasuresRemaining(const int count) {
    treasuresRemaining = count;
}

// Loads necessary textures and fonts for the HUD
void GameHUD::LoadResources() {
    hudFont = LoadFont("C:/DandD/assets/fonts/.TTF"); // Loads the HUD font

    hudTexture = LoadTexture("C:/DandD/assets/background/walls4"); // Loads background texture
    frameTexture = LoadTexture("C:/DandD/assets/border/testFrame.png"); // Loads frame texture
    armorIconTexture = LoadTexture("C:/DandD/assets/equipment/testArmor.png"); // Loads armor icon
    weaponIconTexture = LoadTexture("C:/DandD/assets/equipment/testWeapon.png"); // Loads weapon icon
    spellIconTexture = LoadTexture("C:/DandD/assets/equipment/testSpell.png"); // Loads spell icon

    // Set font for HP and XP bars
    hpBar.SetFont(hudFont, 21, textColor);
    xpBar.SetFont(hudFont, 21, textColor);

    // Set textures for equipment buttons
    armorButton.SetTexture(armorIconTexture);
    weaponButton.SetTexture(weaponIconTexture);
    spellButton.SetTexture(spellIconTexture);

    // Set rounded corners for equipment buttons
    float cornerRadius = 0.3f;
    armorButton.SetRoundedCorners(cornerRadius);
    weaponButton.SetRoundedCorners(cornerRadius);
    spellButton.SetRoundedCorners(cornerRadius);
}

// Updates the HUD state based on game logic and user input
void GameHUD::Update(const float deltaTime) {
    if (!hero) return; // If no hero, do nothing

    // Check for screen size changes and recalculate bounds if necessary
    const int newWidth = GetScreenWidth();
    const int newHeight = GetScreenHeight();
    if (newWidth != screenWidth || newHeight != screenHeight) {
        screenWidth = newWidth;
        screenHeight = newHeight;

        // Recalculate and set bounds for HP and XP bars
        hpBar.SetBounds(CalculateHPBarBounds());
        xpBar.SetBounds(CalculateXPBarBounds());

        // Reinitialize buttons with new calculated bounds
        armorButton = Button(CalculateArmorButtonBounds(), "", nullptr);
        weaponButton = Button(CalculateWeaponButtonBounds(), "", nullptr);
        spellButton = Button(CalculateSpellButtonBounds(), "", nullptr);
    }

    // Update HP bar with hero's current health and set max health
    hpBar.Update(hero->GetHealth(), deltaTime);
    hpBar.SetMaxValue(hero->GetMaxHealth());

    // Update XP bar with hero's current XP and set max XP
    xpBar.Update(hero->GetXP(), deltaTime);
    xpBar.SetMaxValue(100);

    // Get mouse position for hover detection
    const Vector2 mousePos = GetMousePosition();
    inventoryHoverIndex = -1; // Reset hover index

    // Check if mouse is hovering over equipment buttons and set inventoryHoverIndex
    if (armorButton.IsActive() && CheckCollisionPointRec(mousePos, armorButton.GetBounds())) {
        inventoryHoverIndex = 0;
    } else if (weaponButton.IsActive() && CheckCollisionPointRec(mousePos, weaponButton.GetBounds())) {
        inventoryHoverIndex = 1;
    } else if (spellButton.IsActive() && CheckCollisionPointRec(mousePos, spellButton.GetBounds())) {
        inventoryHoverIndex = 2;
    }

    // Update equipment buttons based on mouse interaction
    armorButton.Update(mousePos);
    weaponButton.Update(mousePos);
    spellButton.Update(mousePos);
}

// Draws all HUD elements
void GameHUD::Draw() const {
    if (!hero) return; // If no hero, do nothing

    ClearBackground(darkPurple); // Clear background with a dark purple color

    if (backgroundLoaded)
        DrawBackground(); // Draw background if loaded

    DrawFrame(); // Draw the HUD frame
    DrawStats(); // Draw hero stats
    DrawLevelInfo(); // Draw level and remaining info
    DrawInventory(); // Draw inventory items
    hpBar.Draw(); // Draw HP bar
    xpBar.Draw(); // Draw XP bar
}

// Draws the background texture for the HUD
void GameHUD::DrawBackground() const {
    if (!backgroundLoaded) return; // Only draw if background is loaded

    const Rectangle backgroundRect = {0, 0, static_cast<float>(screenWidth), static_cast<float>(screenHeight)};
    DrawTexturePro(
        hudTexture,
        {0, 0, static_cast<float>(hudTexture.width), static_cast<float>(hudTexture.height)}, // Source rectangle
        {0, 0, static_cast<float>(screenWidth), static_cast<float>(screenHeight)}, // Destination rectangle
        {0, 0}, // Origin for rotation
        0.0f, // Rotation
        WHITE // Tint color
    );
}

// Unloads all loaded textures and fonts
void GameHUD::Unload() {
    UnloadFont(hudFont); // Unload HUD font
    UnloadTexture(frameTexture); // Unload frame texture
    UnloadTexture(characterIconTexture); // Unload character icon (might be a placeholder, not used in code)
    UnloadTexture(armorIconTexture); // Unload armor icon
    UnloadTexture(weaponIconTexture); // Unload weapon icon
    UnloadTexture(spellIconTexture); // Unload spell icon

    if (backgroundLoaded) {
        UnloadTexture(hudTexture); // Unload background texture if it was loaded
        backgroundLoaded = false;
    }
}

// Draws the main frame and borders of the HUD
void GameHUD::DrawFrame() const {
    const Rectangle frameRect = {0, 0, static_cast<float>(screenWidth), 120};

    // Draw the main background rectangle for the HUD frame
    DrawRectangleRec(frameRect, ColorAlpha(frameColor, 0.9f));

    // Draw outer borders with different opacities for a glow effect
    DrawRectangleLinesEx({0, 0, static_cast<float>(screenWidth), static_cast<float>(screenHeight - 2)}, 2, stoneGray);
    DrawRectangleLinesEx({0, 1, static_cast<float>(screenWidth), static_cast<float>(screenHeight - 1)}, 3,
                         ColorAlpha(mysticBlue, 0.6f));
    DrawRectangleLinesEx({0, 2, static_cast<float>(screenWidth), static_cast<float>(screenHeight)}, 4,
                         ColorAlpha(mysticBlue, 0.3f));

    // Draw lines separating the stat area with different opacities for a glow effect
    DrawLine(0, 120, screenWidth, 120, stoneGray);
    DrawLineEx({0, 118}, {static_cast<float>(screenWidth), 118}, 2, ColorAlpha(mysticBlue, 0.6f));
    DrawLineEx({0, 119}, {static_cast<float>(screenWidth), 119}, 3, ColorAlpha(mysticBlue, 0.3f));
}

// Helper function to draw a stat with a subtle glow effect
void DrawStatWithGlow(const Font &font, const char *label, const char *value,
                      const Vector2 position, const Color textColor, const Color valueColor) {
    // Draw the main text for the label
    DrawTextEx(font, label, position, 24, 1, textColor);
    // Draw a slightly offset, more transparent version for the glow
    DrawTextEx(font, label, {position.x - 1.5f, position.y + 1.5f}, 24, 1,
               {textColor.r, textColor.g, textColor.b, 100});

    // Calculate position for the value text
    const Vector2 valuePos = {position.x + (label == "Strength:" ? 110 : 67), position.y};
    // Draw the main text for the value
    DrawTextEx(font, value, {valuePos.x - 1.5f, valuePos.y + 1.5f}, 24, 1,
               {valueColor.r, valueColor.g, valueColor.b, 100});
    // Draw a slightly offset, more transparent version for the glow
    DrawTextEx(font, value, valuePos, 24, 1, valueColor);
}

// Draws the hero's strength and mana stats
void GameHUD::DrawStats() const {
    // Base position for stats, relative to the HP bar
    const Vector2 basePos = {
        hpBar.GetBounds().x + 10,
        hpBar.GetBounds().y + hpBar.GetBounds().height + 25
    };

    // Draw Strength stat
    DrawStatWithGlow(hudFont, "Strength:", std::to_string(hero->GetStrength()).c_str(),
                     basePos, textColor, {220, 220, 100, 255});

    // Draw Mana stat, positioned relative to Strength
    const Vector2 manaPos = {basePos.x + hpBar.GetBounds().width / 2 + 55, basePos.y};
    DrawStatWithGlow(hudFont, "Mana:", std::to_string(hero->GetMana()).c_str(),
                     manaPos, textColor, {120, 180, 255, 255});
}

// Draws the inventory equipment buttons and their tooltips
void GameHUD::DrawInventory() const {
    armorButton.Draw(); // Draw armor button
    weaponButton.Draw(); // Draw weapon button
    spellButton.Draw(); // Draw spell button

    // Draw tooltip if an item is hovered over
    if (inventoryHoverIndex == 0 && armorItem) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(1) << armorItem->GetBonus(); // Format bonus to one decimal place
        DrawItemTooltip(armorItem->GetName().c_str(), armorItem->GetLevel(), ss.str().c_str(),
                        armorButton.GetBounds());
    } else if (inventoryHoverIndex == 1 && weaponItem) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(1) << weaponItem->GetBonus(); // Format bonus to one decimal place
        DrawItemTooltip(weaponItem->GetName().c_str(), weaponItem->GetLevel(), ss.str().c_str(),
                        weaponButton.GetBounds());
    } else if (inventoryHoverIndex == 2 && spellItem) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(1) << spellItem->GetBonus(); // Format bonus to one decimal place
        DrawItemTooltip(spellItem->GetName().c_str(), spellItem->GetLevel(), ss.str().c_str(),
                        spellButton.GetBounds());
    }
}

// Draws the current level, monsters remaining, and treasures remaining
void GameHUD::DrawLevelInfo() const {
    const int fontSize = 24;
    const float startX = screenWidth - 330; // X-coordinate for level info
    const float startY = 18; // Y-coordinate for level info
    const float lineHeight = 30; // Vertical spacing between lines

    const std::string levelText = "LEVEL " + std::to_string(currentLevel);

    // Draw LEVEL text with a glow effect
    DrawTextEx(hudFont, levelText.c_str(),
               {startX, startY},
               fontSize + 2, 2,
               {255, 255, 150, 255});
    DrawTextEx(hudFont, levelText.c_str(),
               {startX - 1.5f, startY + 1.5f},
               fontSize + 2, 2,
               {255, 255, 150, 100});

    const std::string monstersText = "Current number of monsters: " + std::to_string(monstersRemaining);

    // Draw monsters remaining text with a glow effect
    DrawTextEx(hudFont, monstersText.c_str(),
               {startX - 110, startY + lineHeight},
               fontSize, 1,
               {220, 100, 100, 255});
    DrawTextEx(hudFont, monstersText.c_str(),
               {startX - 110 - 1.5f, startY + lineHeight + 1.5f},
               fontSize, 1,
               {220, 100, 100, 100});

    const std::string treasuresText = "Current number of treasures: " + std::to_string(treasuresRemaining);

    // Draw treasures remaining text with a glow effect
    DrawTextEx(hudFont, treasuresText.c_str(),
               {startX - 118, startY + lineHeight * 2},
               fontSize, 1,
               {220, 100, 100, 255});
    DrawTextEx(hudFont, treasuresText.c_str(),
               {startX - 118 - 1.5f, startY + lineHeight * 2 + 1.5f},
               fontSize, 1,
               {220, 100, 100, 100});
}

// Draws a tooltip for an item, displaying its name, level, and bonus
void GameHUD::DrawItemTooltip(const char *name, int level, const char *bonus, Rectangle itemBounds) const {
    std::vector<std::string> nameLines;
    std::string nameStr(name);

    // Logic to wrap the item name into multiple lines if it's too long
    while (!nameStr.empty()) {
        if (nameStr.length() <= 13) {
            // If remaining string is short enough, add it as a line
            nameLines.push_back(nameStr);
            nameStr.clear();
        } else {
            size_t breakPoint = 13;
            // Find a suitable breakpoint (space) within the first 13 characters
            while (breakPoint > 0 &&
                   nameStr[breakPoint] != ' ') {
                breakPoint--;
            }

            if (breakPoint == 0) breakPoint = 13; // If no space found, just cut at 13
            else if (nameStr[breakPoint] == ' ') breakPoint++; // Move past the space

            nameLines.push_back(nameStr.substr(0, breakPoint)); // Add the line
            nameStr = nameStr.substr(breakPoint); // Update remaining string
        }
    }

    const int tooltipWidth = 200;
    const int lineHeight = 22;
    const int baseHeight = 75;
    // Calculate total tooltip height based on number of name lines
    const int tooltipHeight = baseHeight + nameLines.size() * lineHeight;

    // Calculate initial position for the tooltip (left of the item, centered vertically)
    Vector2 position = {
        itemBounds.x - tooltipWidth - 10,
        itemBounds.y + itemBounds.height / 2 - tooltipHeight / 2
    };

    // Adjust tooltip position if it goes off-screen (move to the right of the item)
    if (position.x < 10)
        position.x = itemBounds.x + itemBounds.width + 1;
    // Adjust tooltip position if it goes off the bottom of the screen
    if (position.y + tooltipHeight > screenHeight)
        position.y = screenHeight - tooltipHeight - 10;

    Rectangle tooltipRect = {
        position.x,
        position.y,
        static_cast<float>(tooltipWidth),
        static_cast<float>(tooltipHeight)
    };

    // Draw the tooltip background rectangle
    DrawRectangleRec(tooltipRect, ColorAlpha(darkPurple, 0.9f));

    // Draw borders for the tooltip with different opacities for a glow effect
    DrawRectangleLinesEx(tooltipRect, 1, ColorAlpha(mysticBlue, 0.9f));
    DrawRectangleLinesEx({tooltipRect.x + 1, tooltipRect.y + 1, tooltipRect.width - 2, tooltipRect.height - 2}, 1,
                         ColorAlpha(mysticBlue, 0.6f));
    DrawRectangleLinesEx({tooltipRect.x + 2, tooltipRect.y + 2, tooltipRect.width - 4, tooltipRect.height - 4}, 1,
                         ColorAlpha(mysticBlue, 0.3f));

    float textY = position.y + 15; // Starting Y position for text within the tooltip

    // Draw each line of the item name
    for (const auto &line: nameLines) {
        const float textWidth = MeasureTextEx(hudFont, line.c_str(), 24, 1).x;
        const float textX = position.x + (tooltipWidth - textWidth) / 2.0f; // Center text horizontally

        // Draw main text and glow effect
        DrawTextEx(hudFont, line.c_str(), {textX, textY}, 24, 1, textColor);
        DrawTextEx(hudFont, line.c_str(), {textX - 1.5f, textY + 1}, 24, 1,
                   {textColor.r, textColor.g, textColor.b, 100});
        textY += lineHeight; // Move to the next line
    }

    std::string levelText = "Level: " + std::to_string(level);
    float levelTextWidth = MeasureTextEx(hudFont, levelText.c_str(), 20, 1).x;
    float levelTextX = position.x + (tooltipWidth - levelTextWidth) / 2.0f;

    // Draw item level text with glow
    DrawTextEx(hudFont, levelText.c_str(), {levelTextX, textY + 5}, 22, 1, {200, 220, 100, 255});
    DrawTextEx(hudFont, levelText.c_str(), {levelTextX - 1.5f, textY + 6.5f}, 22, 1, {180, 180, 100, 80});
    textY += 22;

    std::string bonusText = "Bonus: " + std::string(bonus) + "%";
    float bonusTextWidth = MeasureTextEx(hudFont, bonusText.c_str(), 20, 1).x;
    float bonusTextX = position.x + (tooltipWidth - bonusTextWidth) / 2.0f;

    // Draw item bonus text with glow
    DrawTextEx(hudFont, bonusText.c_str(), {bonusTextX, textY + 5}, 22, 1, {120, 240, 120, 255});
    DrawTextEx(hudFont, bonusText.c_str(), {bonusTextX - 2, textY + 6.5f}, 22, 1, {100, 200, 100, 80});
}

// Calculates the bounding rectangle for the HP bar
Rectangle GameHUD::CalculateHPBarBounds() const {
    const float centerX = screenWidth / 2.0f;
    return {centerX - (centerX - 30), 20, 360, 28}; // Positioned on the left side of the screen
}

// Calculates the bounding rectangle for the XP bar
Rectangle GameHUD::CalculateXPBarBounds() const {
    // Positioned to the right of the HP bar
    return {
        hpBar.GetBounds().x + hpBar.GetBounds().width + 35,
        hpBar.GetBounds().height / 2 + 35, 320, 25
    };
}

// Calculates the bounding rectangle for the armor button
Rectangle GameHUD::CalculateArmorButtonBounds() const {
    const float buttonSize = 60.0f;
    const float inventoryAreaX = screenWidth - buttonSize - 20.0f; // Positioned on the right side of the screen

    return {
        inventoryAreaX,
        screenHeight / 4.0f, // Vertically positioned at 1/4th of the screen height
        buttonSize,
        buttonSize
    };
}

// Calculates the bounding rectangle for the weapon button
Rectangle GameHUD::CalculateWeaponButtonBounds() const {
    const Rectangle weaponBounds = CalculateArmorButtonBounds(); // Get armor button bounds as reference
    const float buttonSize = 60.0f;
    const float spacing = 20.0f; // Vertical spacing between buttons

    // Positioned below the armor button
    return {
        weaponBounds.x,
        weaponBounds.y + weaponBounds.height + spacing,
        buttonSize,
        buttonSize
    };
}

// Calculates the bounding rectangle for the spell button
Rectangle GameHUD::CalculateSpellButtonBounds() const {
    const Rectangle spellBounds = CalculateWeaponButtonBounds(); // Get weapon button bounds as reference
    const float buttonSize = 60.0f;
    const float spacing = 20.0f; // Vertical spacing between buttons

    // Positioned below the weapon button
    return {
        spellBounds.x,
        spellBounds.y + spellBounds.height + spacing,
        buttonSize,
        buttonSize
    };
}
