//
// Created by Lenovo on 3.6.2025 г.
//

#ifndef CHARACTERSELECTIONPANEL_H
#define CHARACTERSELECTIONPANEL_H

#include "raylib.h"
#include "C:/DandD/include/Entities/Hero.h"
#include "C:/DandD/include/UI/widgets/Button.h"
#include <functional>
#include <vector>
#include <memory>

struct RaceData {
    Race race;
    std::string name;
    int strength;
    int mana;
    int health;
    std::string description;
};

class CharacterSelectionPanel {
public:
    CharacterSelectionPanel(int screenWidth, int screenHeight);

    ~CharacterSelectionPanel();

    void Initialize();

    void Show();

    void Hide();

    void Update();

    void Draw() const;

    bool IsVisible() const;

    bool IsSelectionConfirmed() const;

    Race GetSelectedRace() const;

    void SetOnRaceSelected(std::function<void(Race)> callback);

    void SetOnBack(std::function<void()> callback);

    void Reset();

private:
    int screenWidth;
    int screenHeight;

    Texture2D backgroundTexture;

    std::vector<RaceData> races;

    std::vector<std::unique_ptr<Button> > raceButtons;
    Button confirmButton;
    Button backButton;

    int selectedRaceIndex;
    bool isVisible;
    bool selectionConfirmed;

    std::function<void(Race)> onRaceSelected;
    std::function<void()> onBack;

    Color panelColor;
    Color selectedColor;
    Color textColor;
    Color headerColor;
    Font titleFont;
    Font textFont;

    static constexpr int PANEL_PADDING = 40;
    static constexpr int RACE_PANEL_WIDTH = 200;
    static constexpr int RACE_PANEL_HEIGHT = 300;
    static constexpr int RACE_PANEL_SPACING = 50;
    static constexpr int BUTTON_HEIGHT = 50;

private:
    void LoadBackgroundImage();

    void InitializeRaceData();

    void CreateRaceButtons();

    void DrawRacePanel(const RaceData &raceData, int index, Rectangle bounds) const;

    void DrawStatBar(const char *label, int value, int maxValue, Rectangle bounds, Color barColor) const;

    Rectangle GetRacePanelBounds(int index) const;

    void OnRaceButtonClicked(int raceIndex);

    void OnConfirmClicked();

    void OnBackClicked();
};

#endif //CHARACTERSELECTIONPANEL_H
