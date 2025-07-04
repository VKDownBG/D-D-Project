//
// Created by Lenovo on 16.5.2025 г..
//

#ifndef EQUIPMENTPANEL_H
#define EQUIPMENTPANEL_H

#include "raylib.h"
#include "C:/DandD/include/Items/Item.h"
#include "C:/DandD/include/UI/widgets/Button.h"
#include "C:/DandD/include/Entities/Hero.h"
#include <string>

class EquipmentPanel {
public:
    EquipmentPanel(Hero *_player);

    ~EquipmentPanel() = default;

    void Show(const Item *_currentItem, const Item *_newItem);

    void Hide();

    void Update();

    void Draw() const;

    bool IsVisible() const;

    void ShowComparison(const Item *newTreasure);

    void SetOnEquipCallback(std::function<void()> callback) {
        onEquipCallback = callback;
    }

    void SetOnKeepCallback(std::function<void()> callback) {
        onKeepCallback = callback;
    }

private:
    const int PANEL_WIDTH = 300;
    const int PANEL_HEIGHT = 420;
    const int PANEL_SPACING = 40;
    const int ICON_SIZE = 80;
    const int FONT_SIZE = 22;
    const int SMALL_FONT_SIZE = 35;

    Hero *player;

    bool isVisible;
    const Item *currentItem;
    const Item *newItem;
    ItemType itemType;

    Button leftButton;
    Button rightButton;

    Color panelBackgroundColor;

    std::function<void()> onEquipCallback;
    std::function<void()> onKeepCallback;

private:
    void DrawItemIcon(int x, int y, const Item *item) const;

    void DrawItemStats(int x, int y, const Item *item, bool showComparison = false) const;

    void SetupButtons();

    void SetPanelBackgroundColor(Color color);

    void DrawPanelContent(int x, int y, const Item *item, bool showComparison) const;

    void OnKeepCurrentEquipment();

    void OnEquipNewItem();

    std::string GetBonusDifference(double currentBonus, double newBonus) const;

    std::string GetLevelDifference(int currentLevel, int newLevel) const;

    Color GetDifferenceColor(double current, double newValue) const;

    Color GetLevelDifferenceColor(int currentLevel, int newLevel) const;
};

#endif //EQUIPMENTPANEL_H
