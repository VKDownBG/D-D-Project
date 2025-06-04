//
// Created by Lenovo on 25.5.2025 г..
//

#ifndef LEVELUPPANEL_H
#define LEVELUPPANEL_H

#include "raylib.h"
#include "C:/DandD/include/Entities/Hero.h"
#include "C:/DandD/include/UI/widgets/Button.h"
#include <string>
#include <functional>

class LevelUpPanel {
public:
    LevelUpPanel();

    LevelUpPanel(Rectangle bounds, const Font &customFont);

    void Update();

    void Draw() const;

    void Show();

    void Hide();

    bool IsVisible() const;

    void SetFont(const Font &customFont);

    void SetColors(Color panel, Color border, Color text, Color inputBg, Color inputActive);

    void SetMaxPoints(int maxPts);

    void SetBounds(Rectangle bounds);

    void SetOnConfirm(const std::function<void(int str, int mana, int health)> &callback);

    void SetOnCancel(const std::function<void()> &callback);

    void SetHeroReference(Hero *hero);

    void ApplyStatsToHero() const;

    void ShowForHero(Hero *hero);

    int GetStrengthPoints() const;

    int GetManaPoints() const;

    int GetHealthPoints() const;

    int GetTotalPointsAllocated() const;

    int GetRemainingPoints() const;

    void Reset();

private:
    Rectangle panelBounds;
    Font font;
    Color panelColor;
    Color borderColor;
    Color textColor;
    Color inputBgColor;
    Color inputActiveColor;
    bool isVisible;

    int strengthPoints;
    int manaPoints;
    int healthPoints;
    int totalPointsAllocated;
    int maxPoints;

    Rectangle strengthInputBounds;
    Rectangle manaInputBounds;
    Rectangle healthInputBounds;

    bool strengthInputActive;
    bool manaInputActive;
    bool healthInputActive;

    std::string strengthInputText;
    std::string manaInputText;
    std::string healthInputText;

    Button confirmButton;
    Button cancelButton;
    Button strengthUpButton;
    Button strengthDownButton;
    Button manaUpButton;
    Button manaDownButton;
    Button healthUpButton;
    Button healthDownButton;

    std::function<void(int str, int mana, int health)> onConfirm;
    std::function<void()> onCancel;

    Hero *heroRef;

private:
    void UpdateButtonPositions();

    void SetupArrowButtons();

    void UpdateInputField(Rectangle inputBounds, bool &isActive, std::string &inputText, int &pointValue);

    void ValidateAndUpdatePoints();

    void DrawInputField(Rectangle bounds, const std::string &text, bool isActive, const std::string &level) const;

    bool IsPointInRect(Vector2 point, Rectangle rect) const;

    void ValidateIndividualInput(int &pointValue, const std::string &inputText);
};

#endif //LEVELUPPANEL_H
