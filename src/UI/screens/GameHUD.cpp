//
// Created by Lenovo on 15.5.2025 г.
//

#include "C:/DandD/include/UI/screens/GameHUD.h"
#include <cmath>
#include <iomanip>

GameHUD::GameHUD(const int _screenWidth, const int _screenHeight)
    : hero(nullptr), armorItem(nullptr), weaponItem(nullptr),
      spellItem(nullptr), screenWidth(_screenWidth), screenHeight(_screenHeight),
      hpBar(CalculateHPBarBounds(), 50.0f),
      xpBar(CalculateXPBarBounds(), 0.0f),
      inventoryHoverIndex(-1),
      currentLevel(1),
      monstersRemaining(0),
      treasuresRemaining(0),
      backgroundLoaded(false) {
    frameColor = {40, 40, 70, 230};
    textColor = {220, 220, 250, 255};
    darkPurple = {30, 20, 50, 255};
    mysticBlue = {60, 80, 120, 255};
    deepRed = {150, 30, 50, 255};
    stoneGray = {80, 80, 90, 200};

    // frameColor = {30, 30, 45, 255};
    // textColor = {220, 200, 235, 255};
    // darkPurple = {60, 50, 90, 255};
    // mysticBlue = {80, 120, 200, 255};
    // deepRed = {180, 40, 40, 255};
    // stoneGray = {90, 90, 110, 255};

    armorButton = Button(CalculateArmorButtonBounds(), "", nullptr);
    weaponButton = Button(CalculateWeaponButtonBounds(), "", nullptr);
    spellButton = Button(CalculateSpellButtonBounds(), "", nullptr);

    const Color equipButtonColor = {60, 60, 85, 255};
    const Color equipHoverColor = {75, 75, 110, 255};
    const Color equipPressColor = {90, 90, 130, 255};
    const Color equipBorderColor = {110, 110, 170, 255};

    armorButton.SetColors(equipButtonColor, equipHoverColor, equipPressColor, textColor);
    armorButton.SetBorder(equipBorderColor, 1);
    armorButton.EnableHoverAnimation(true, 1.1f, 8.0f);

    weaponButton.SetColors(equipButtonColor, equipHoverColor, equipPressColor, textColor);
    weaponButton.SetBorder(equipBorderColor, 1);
    weaponButton.EnableHoverAnimation(true, 1.1f, 8.0f);

    spellButton.SetColors(equipButtonColor, equipHoverColor, equipPressColor, textColor);
    spellButton.SetBorder(equipBorderColor, 1);
    spellButton.EnableHoverAnimation(true, 1.1f, 8.0f);

    hpBar.SetColors({40, 40, 50, 255}, deepRed, {78, 15, 15, 255});
    hpBar.SetBorderThickness(3.0f);
    hpBar.SetRounding(0.2f);
    hpBar.ShowText(true);
    hpBar.SetTextPrefix("HP: ");
    hpBar.SetTextFormat([](float current, float max) {
        return std::to_string(static_cast<int>(current)) + "/" + std::to_string(static_cast<int>(max));
    });
    hpBar.EnablePulsatingEffect(true, 0.2f, 0.8f);

    xpBar.SetColors({58, 181, 105, 70}, {35, 152, 80, 255}, {33, 49, 40, 255});
    xpBar.SetBorderThickness(3.0f);
    xpBar.SetRounding(0.2f);
    xpBar.ShowText(true);
    xpBar.SetTextPrefix("XP: ");
    xpBar.SetTextFormat([](const float current, const float max) {
        const int percentage = static_cast<int>((current / max) * 100.0f);
        return std::to_string(percentage) + "%";
    });
    xpBar.EnablePulsatingEffect(true, 0.1f, 0.6f);
}

GameHUD::~GameHUD() {
    Unload();
}

void GameHUD::Initialize(Hero *heroRef) {
    hero = heroRef;

    armorButton.SetActive(armorItem != nullptr);
    weaponButton.SetActive(weaponItem != nullptr);
    spellButton.SetActive(spellItem != nullptr);
}

void GameHUD::SetArmor(Armor *armor) {
    armorItem = armor;
    armorButton.SetActive(armorItem != nullptr);
}

void GameHUD::SetWeapon(Weapon *weapon) {
    weaponItem = weapon;
    weaponButton.SetActive(weaponItem != nullptr);
}

void GameHUD::SetSpell(Spell *spell) {
    spellItem = spell;
    spellButton.SetActive(spellItem != nullptr);
}

void GameHUD::SetLevel(const int level) {
    currentLevel = level;
}

void GameHUD::SetMonstersRemaining(const int count) {
    monstersRemaining = count;
}

void GameHUD::SetTreasuresRemaining(const int count) {
    treasuresRemaining = count;
}

void GameHUD::LoadResources() {
    hudFont = LoadFont("C:/DandD/assets/fonts/.TTF");

    frameTexture = LoadTexture("C:/DandD/assets/border/testFrame.png");
    armorIconTexture = LoadTexture("C:/DandD/assets/equipment/testArmor.png");
    weaponIconTexture = LoadTexture("C:/DandD/assets/equipment/testWeapon.png");
    spellIconTexture = LoadTexture("C:/DandD/assets/equipment/testSpell.png");

    hpBar.SetFont(hudFont, 21, textColor);
    xpBar.SetFont(hudFont, 21, textColor);

    armorButton.SetTexture(armorIconTexture);
    weaponButton.SetTexture(weaponIconTexture);
    spellButton.SetTexture(spellIconTexture);

    float cornerRadius = 0.3f;
    armorButton.SetRoundedCorners(cornerRadius);
    weaponButton.SetRoundedCorners(cornerRadius);
    spellButton.SetRoundedCorners(cornerRadius);
}

void GameHUD::Update(const float deltaTime) {
    if (!hero) return;

    const int newWidth = GetScreenWidth();
    const int newHeight = GetScreenHeight();
    if (newWidth != screenWidth || newHeight != screenHeight) {
        screenWidth = newWidth;
        screenHeight = newHeight;

        hpBar.SetBounds(CalculateHPBarBounds());
        xpBar.SetBounds(CalculateXPBarBounds());

        armorButton = Button(CalculateArmorButtonBounds(), "", nullptr);
        weaponButton = Button(CalculateWeaponButtonBounds(), "", nullptr);
        spellButton = Button(CalculateSpellButtonBounds(), "", nullptr);
    }

    hpBar.Update(hero->GetHealth(), deltaTime);
    hpBar.SetMaxValue(hero->GetMaxHealth());

    xpBar.Update(hero->GetXP(), deltaTime);
    xpBar.SetMaxValue(100);

    const Vector2 mousePos = GetMousePosition();
    inventoryHoverIndex = -1;

    if (armorButton.IsActive() && CheckCollisionPointRec(mousePos, armorButton.GetBounds())) {
        inventoryHoverIndex = 0;
    } else if (weaponButton.IsActive() && CheckCollisionPointRec(mousePos, weaponButton.GetBounds())) {
        inventoryHoverIndex = 1;
    } else if (spellButton.IsActive() && CheckCollisionPointRec(mousePos, spellButton.GetBounds())) {
        inventoryHoverIndex = 2;
    }

    armorButton.Update(mousePos);
    weaponButton.Update(mousePos);
    spellButton.Update(mousePos);
}

void GameHUD::Draw() const {
    if (!hero) return;

    ClearBackground(darkPurple);

    if (backgroundLoaded)
        DrawBackground();

    DrawFrame();
    DrawStats();
    DrawLevelInfo();
    DrawInventory();
    hpBar.Draw();
    xpBar.Draw();
}

void GameHUD::DrawBackground() const {
    if (!backgroundLoaded) return;

    const Rectangle backgroundRect = {0, 0, static_cast<float>(screenWidth), static_cast<float>(screenHeight)};
    DrawTexturePro(
        hudTexture,
        {0, 0, static_cast<float>(hudTexture.width), static_cast<float>(hudTexture.height)},
        {0, 0, static_cast<float>(screenWidth), static_cast<float>(screenHeight)},
        {0, 0},
        0.0f,
        WHITE
    );
}

void GameHUD::Unload() {
    UnloadFont(hudFont);
    UnloadTexture(frameTexture);
    UnloadTexture(characterIconTexture);
    UnloadTexture(armorIconTexture);
    UnloadTexture(weaponIconTexture);
    UnloadTexture(spellIconTexture);

    if (backgroundLoaded) {
        UnloadTexture(hudTexture);
        backgroundLoaded = false;
    }
}

void GameHUD::DrawFrame() const {
    Rectangle frameRect = {0, 0, static_cast<float>(screenWidth), 120};

    DrawRectangleRec(frameRect, ColorAlpha(frameColor, 0.9f));

    //OuterBorder
    DrawRectangleLinesEx({0, 0, static_cast<float>(screenWidth), static_cast<float>(screenHeight - 2)}, 2, stoneGray);
    DrawRectangleLinesEx({0, 1, static_cast<float>(screenWidth), static_cast<float>(screenHeight - 1)}, 3,
                         ColorAlpha(mysticBlue, 0.6f));
    DrawRectangleLinesEx({0, 2, static_cast<float>(screenWidth), static_cast<float>(screenHeight)}, 4,
                         ColorAlpha(mysticBlue, 0.3f));

    //StatBorderLine
    DrawLine(0, 120, screenWidth, 120, stoneGray);
    DrawLineEx({0, 118}, {static_cast<float>(screenWidth), 118}, 2, ColorAlpha(mysticBlue, 0.6f));
    DrawLineEx({0, 119}, {static_cast<float>(screenWidth), 119}, 3, ColorAlpha(mysticBlue, 0.3f));
}

void DrawStatWithGlow(const Font &font, const char *label, const char *value,
                      const Vector2 position, const Color textColor, const Color valueColor) {
    DrawTextEx(font, label, position, 24, 1, textColor);
    DrawTextEx(font, label, {position.x - 1.5f, position.y + 1.5f}, 24, 1,
               {textColor.r, textColor.g, textColor.b, 100});

    const Vector2 valuePos = {position.x + (label == "Strength:" ? 110 : 67), position.y};
    DrawTextEx(font, value, {valuePos.x - 1.5f, valuePos.y + 1.5f}, 24, 1,
               {valueColor.r, valueColor.g, valueColor.b, 100});
    DrawTextEx(font, value, valuePos, 24, 1, valueColor);
}

void GameHUD::DrawStats() const {
    const Vector2 basePos = {
        hpBar.GetBounds().x + 7,
        hpBar.GetBounds().y + hpBar.GetBounds().height + 25
    };

    // Strength
    DrawStatWithGlow(hudFont, "Strength:", std::to_string(hero->GetStrength()).c_str(),
                     basePos, textColor, {220, 220, 100, 255});

    // Mana
    const Vector2 manaPos = {basePos.x + hpBar.GetBounds().width / 2 + 25, basePos.y};
    DrawStatWithGlow(hudFont, "Mana:", std::to_string(hero->GetMana()).c_str(),
                     manaPos, textColor, {120, 180, 255, 255});
}

void GameHUD::DrawInventory() const {
    armorButton.Draw();
    weaponButton.Draw();
    spellButton.Draw();

    if (inventoryHoverIndex == 0 && armorItem) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(1) << armorItem->GetBonus();
        DrawItemTooltip(armorItem->GetName().c_str(), armorItem->GetLevel(), ss.str().c_str(),
                        armorButton.GetBounds());
    } else if (inventoryHoverIndex == 1 && weaponItem) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(1) << weaponItem->GetBonus();
        DrawItemTooltip(weaponItem->GetName().c_str(), weaponItem->GetLevel(), ss.str().c_str(),
                        weaponButton.GetBounds());
    } else if (inventoryHoverIndex == 2 && spellItem) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(1) << spellItem->GetBonus();
        DrawItemTooltip(spellItem->GetName().c_str(), spellItem->GetLevel(), ss.str().c_str(),
                        spellButton.GetBounds());
    }
}

void GameHUD::DrawLevelInfo() const {
    const int fontSize = 24;
    const float startX = screenWidth - 330;
    const float startY = 18;
    const float lineHeight = 30;

    std::string levelText = "LEVEL " + std::to_string(currentLevel);

    //TextDisplay
    DrawTextEx(hudFont, levelText.c_str(),
               {startX, startY},
               fontSize + 2, 2,
               {255, 255, 150, 255});
    //TextGlow
    DrawTextEx(hudFont, levelText.c_str(),
               {startX - 1.5f, startY + 1.5f},
               fontSize + 2, 2,
               {255, 255, 150, 100});

    const std::string monstersText = "Current number of monsters: " + std::to_string(monstersRemaining);

    //TextDisplay
    DrawTextEx(hudFont, monstersText.c_str(),
               {startX - 110, startY + lineHeight},
               fontSize, 1,
               {220, 100, 100, 255});
    //TextGlow
    DrawTextEx(hudFont, monstersText.c_str(),
               {startX - 110 - 1.5f, startY + lineHeight + 1.5f},
               fontSize, 1,
               {220, 100, 100, 100});

    const std::string treasuresText = "Current number of treasures: " + std::to_string(treasuresRemaining);

    //TextDisplay
    DrawTextEx(hudFont, treasuresText.c_str(),
               {startX - 118, startY + lineHeight * 2},
               fontSize, 1,
               {220, 100, 100, 255});
    //TextGlow
    DrawTextEx(hudFont, treasuresText.c_str(),
               {startX - 118 - 1.5f, startY + lineHeight * 2 + 1.5f},
               fontSize, 1,
               {220, 100, 100, 100});
}

void GameHUD::DrawItemTooltip(const char *name, int level, const char *bonus, Rectangle itemBounds) const {
    std::vector<std::string> nameLines;
    std::string nameStr(name);

    while (!nameStr.empty()) {
        if (nameStr.length() <= 13) {
            nameLines.push_back(nameStr);
            nameStr.clear();
        } else {
            size_t breakPoint = 13;
            while (breakPoint > 0 &&
                   nameStr[breakPoint] != ' ') {
                breakPoint--;
            }

            if (breakPoint == 0) breakPoint = 13;
            else if (nameStr[breakPoint] == ' ') breakPoint++;

            nameLines.push_back(nameStr.substr(0, breakPoint));
            nameStr = nameStr.substr(breakPoint);
        }
    }

    const int tooltipWidth = 200;
    const int lineHeight = 22;
    const int baseHeight = 75;
    const int tooltipHeight = baseHeight + nameLines.size() * lineHeight;

    Vector2 position = {
        itemBounds.x - tooltipWidth - 10,
        itemBounds.y + itemBounds.height / 2 - tooltipHeight / 2
    };

    if (position.x < 10)
        position.x = itemBounds.x + itemBounds.width + 1;
    if (position.y + tooltipHeight > screenHeight)
        position.y = screenHeight - tooltipHeight - 10;

    Rectangle tooltipRect = {
        position.x,
        position.y,
        static_cast<float>(tooltipWidth),
        static_cast<float>(tooltipHeight)
    };

    DrawRectangleRec(tooltipRect, ColorAlpha(darkPurple, 0.9f));

    DrawRectangleLinesEx(tooltipRect, 1, ColorAlpha(mysticBlue, 0.9f));
    DrawRectangleLinesEx({tooltipRect.x + 1, tooltipRect.y + 1, tooltipRect.width - 2, tooltipRect.height - 2}, 1,
                         ColorAlpha(mysticBlue, 0.6f));
    DrawRectangleLinesEx({tooltipRect.x + 2, tooltipRect.y + 2, tooltipRect.width - 4, tooltipRect.height - 4}, 1,
                         ColorAlpha(mysticBlue, 0.3f));

    float textY = position.y + 15;

    for (const auto &line: nameLines) {
        float textWidth = MeasureTextEx(hudFont, line.c_str(), 24, 1).x;
        float textX = position.x + (tooltipWidth - textWidth) / 2.0f;

        DrawTextEx(hudFont, line.c_str(), {textX, textY}, 24, 1, textColor);
        DrawTextEx(hudFont, line.c_str(), {textX - 1.5f, textY + 1}, 24, 1,
                   {textColor.r, textColor.g, textColor.b, 100});
        textY += lineHeight;
    }

    std::string levelText = "Level: " + std::to_string(level);
    float levelTextWidth = MeasureTextEx(hudFont, levelText.c_str(), 20, 1).x;
    float levelTextX = position.x + (tooltipWidth - levelTextWidth) / 2.0f;

    DrawTextEx(hudFont, levelText.c_str(), {levelTextX, textY + 5}, 22, 1, {200, 220, 100, 255});
    DrawTextEx(hudFont, levelText.c_str(), {levelTextX - 1.5f, textY + 6.5f}, 22, 1, {180, 180, 100, 80});
    textY += 22;

    std::string bonusText = "Bonus: " + std::string(bonus) + "%";
    float bonusTextWidth = MeasureTextEx(hudFont, bonusText.c_str(), 20, 1).x;
    float bonusTextX = position.x + (tooltipWidth - bonusTextWidth) / 2.0f;

    DrawTextEx(hudFont, bonusText.c_str(), {bonusTextX, textY + 5}, 22, 1, {120, 240, 120, 255});
    DrawTextEx(hudFont, bonusText.c_str(), {bonusTextX - 2, textY + 6.5f}, 22, 1, {100, 200, 100, 80});
}

Rectangle GameHUD::CalculateHPBarBounds() const {
    const float centerX = screenWidth / 2.0f;
    return {centerX - (centerX - 30), 20, 360, 28};
}

Rectangle GameHUD::CalculateXPBarBounds() const {
    return {
        hpBar.GetBounds().x + hpBar.GetBounds().width + 35,
        hpBar.GetBounds().height / 2 + 25, 320, 25
    };
}

Rectangle GameHUD::CalculateArmorButtonBounds() const {
    const float buttonSize = 60.0f;
    const float inventoryAreaX = screenWidth - buttonSize - 20.0f;

    return {
        inventoryAreaX,
        screenHeight / 4.0f,
        buttonSize,
        buttonSize
    };
}

Rectangle GameHUD::CalculateWeaponButtonBounds() const {
    Rectangle armorBounds = CalculateArmorButtonBounds();
    const float buttonSize = 60.0f;
    const float spacing = 20.0f;

    return {
        armorBounds.x,
        armorBounds.y + armorBounds.height + spacing,
        buttonSize,
        buttonSize
    };
}

Rectangle GameHUD::CalculateSpellButtonBounds() const {
    Rectangle spellBounds = CalculateWeaponButtonBounds();
    const float buttonSize = 60.0f;
    const float spacing = 20.0f;

    return {
        spellBounds.x,
        spellBounds.y + spellBounds.height + spacing,
        buttonSize,
        buttonSize
    };
}
