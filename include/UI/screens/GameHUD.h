//
// Created by Lenovo on 15.5.2025 г.
//

#ifndef GAMEHUD_H
#define GAMEHUD_H

#include "raylib.h"
#include "C:/DandD/include/UI/widgets/ProgressBar.h"
#include "C:/DandD/include/UI/widgets/Button.h"
#include "../../Entities/Hero.h"
#include "C:/DandD/include/Items/Types/Weapon.h"
#include "C:/DandD/include/Items/Types/Armor.h"
#include "C:/DandD/include/Items/Types/Spell.h"

class GameHUD {
public:
    GameHUD(int _screenWidth, int _screenHeight);

    ~GameHUD();

    void Initialize(Hero *heroRef);

    void SetArmor(const Armor *armor);

    void SetWeapon(const Weapon *weapon);

    void SetSpell(const Spell *spell);

    void SetLevel(int level);

    void SetMonstersRemaining(int count);

    void SetTreasuresRemaining(int count);

    void LoadResources();

    void Update(float deltaTime);

    void Draw() const;

    void Unload();

    void RefreshInventoryDisplay();

private:
    Hero *hero;
    const Armor *armorItem;
    const Weapon *weaponItem;
    const Spell *spellItem;

    int screenWidth, screenHeight;

    const float baseWidth = 1920.0f;
    const float baseHeight = 1080.0f;

    ProgressBar hpBar, xpBar;

    Button characterButton;
    bool inventoryExpanded;

    Button armorButton;
    Button weaponButton;
    Button spellButton;
    int inventoryHoverIndex;

    Font hudFont{};

    Texture2D hudTexture{};
    Texture2D frameTexture{};
    Texture2D characterIconTexture{};
    Texture2D armorIconTexture{};
    Texture2D weaponIconTexture{};
    Texture2D spellIconTexture{};

    Color frameColor{};
    Color textColor{};
    Color darkPurple{};
    Color mysticBlue{};
    Color deepRed{};
    Color stoneGray{};

    float inventoryAnimProgress;
    float animSpeed;

    int currentLevel;
    int monstersRemaining;
    int treasuresRemaining;

    bool backgroundLoaded;

private:
    void DrawBackground() const;

    void DrawFrame() const;

    void DrawStats() const;

    void DrawInventory() const;

    void DrawLevelInfo() const;

    void DrawItemTooltip(const char *name, int level, const char *bonus, Rectangle itemBounds) const;

    Rectangle CalculateHPBarBounds() const;

    Rectangle CalculateXPBarBounds() const;

    Rectangle CalculateArmorButtonBounds() const;

    Rectangle CalculateWeaponButtonBounds() const;

    Rectangle CalculateSpellButtonBounds() const;
};

#endif //GAMEHUD_H
