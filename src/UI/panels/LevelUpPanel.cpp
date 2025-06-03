//
// Created by Lenovo on 25.5.2025 г.
//

#include "C:/DandD/include/UI/panels/LevelUpPanel.h"
#include <algorithm>

LevelUpPanel::LevelUpPanel()
    : panelBounds({static_cast<float>(GetScreenWidth()) / 2 - 150, static_cast<float>(GetScreenHeight()) / 2 - 100, 450, 300}),
      font(GetFontDefault()),
      panelColor({40, 40, 40, 240}),
      borderColor(WHITE),
      textColor(WHITE),
      inputBgColor({60, 60, 60, 255}),
      inputActiveColor({80, 80, 120, 255}),
      isVisible(false),
      strengthPoints(0),
      manaPoints(0),
      healthPoints(0),
      totalPointsAllocated(0),
      maxPoints(30),
      strengthInputActive(false),
      manaInputActive(false),
      healthInputActive(false),
      strengthInputText("0"),
      manaInputText("0"),
      healthInputText("0"),
      onConfirm(nullptr),
      onCancel(nullptr),
      heroRef(nullptr) {
    UpdateButtonPositions();
    SetupArrowButtons();
}

LevelUpPanel::LevelUpPanel(const Rectangle bounds, const Font &customFont)
    : LevelUpPanel() {
    SetBounds(bounds);
    SetFont(customFont);
}

void LevelUpPanel::Update() {
    if (!isVisible) return;

    const Vector2 mousePos = GetMousePosition();

    confirmButton.SetActive(totalPointsAllocated == maxPoints);
    confirmButton.Update(mousePos);
    // cancelButton.Update(mousePos);

    strengthUpButton.Update(mousePos);
    strengthDownButton.Update(mousePos);
    manaUpButton.Update(mousePos);
    manaDownButton.Update(mousePos);
    healthUpButton.Update(mousePos);
    healthDownButton.Update(mousePos);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        strengthInputActive = IsPointInRect(mousePos, strengthInputBounds);
        manaInputActive = IsPointInRect(mousePos, manaInputBounds);
        healthInputActive = IsPointInRect(mousePos, healthInputBounds);
    }

    if (strengthInputActive) {
        UpdateInputField(strengthInputBounds, strengthInputActive, strengthInputText, strengthPoints);
    }
    if (manaInputActive) {
        UpdateInputField(manaInputBounds, manaInputActive, manaInputText, manaPoints);
    }
    if (healthInputActive) {
        UpdateInputField(healthInputBounds, healthInputActive, healthInputText, healthPoints);
    }

    ValidateAndUpdatePoints();
}

void LevelUpPanel::Draw() const {
    if (!isVisible) return;

    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), {0, 0, 0, 100});

    DrawRectangleRounded(panelBounds, 0.1f, 10, panelColor);
    DrawRectangleRoundedLines(panelBounds, 0.1f, 10, 2, borderColor);

    const std::string title = "Level Up!";
    const Vector2 titleSize = MeasureTextEx(font, title.c_str(), 24, 1);
    const Vector2 titlePos = {
        panelBounds.x + (panelBounds.width - titleSize.x) / 2,
        panelBounds.y + 15
    };
    DrawTextEx(font, title.c_str(), titlePos, 24, 1, textColor);

    DrawTextEx(font, "Strength:", {panelBounds.x + 20, panelBounds.y + 65}, 18, 1, textColor);
    DrawInputField(strengthInputBounds, strengthInputText, strengthInputActive, "Strength");

    DrawTextEx(font, "Mana:", {panelBounds.x + 20, panelBounds.y + 115}, 18, 1, textColor);
    DrawInputField(manaInputBounds, manaInputText, manaInputActive, "Mana");

    DrawTextEx(font, "Health:", {panelBounds.x + 20, panelBounds.y + 165}, 18, 1, textColor);
    DrawInputField(healthInputBounds, healthInputText, healthInputActive, "Health");

    const std::string remainingText = "Points remaining: " + std::to_string(GetRemainingPoints());
    DrawTextEx(font, remainingText.c_str(), {panelBounds.x + 20, panelBounds.y + 205}, 16, 1, textColor);

    confirmButton.Draw();
    //cancelButton.Draw();

    strengthUpButton.Draw();
    strengthDownButton.Draw();
    manaUpButton.Draw();
    manaDownButton.Draw();
    healthUpButton.Draw();
    healthDownButton.Draw();
}

void LevelUpPanel::UpdateInputField(Rectangle inputBounds, bool &isActive, std::string &inputText, int &pointValue) {
    int key = GetCharPressed();

    while (key > 0) {
        if (key >= '0' && key <= '9' && inputText.length() < 2) {
            if (inputText == "0") {
                inputText = static_cast<char>(key);
            } else {
                inputText += static_cast<char>(key);
            }
        }
        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE) && !inputText.empty()) {
        inputText.pop_back();
        if (inputText.empty()) {
            inputText = "0";
        }
    }

    if (IsKeyPressed(KEY_ENTER)) {
        isActive = false;
    }

    try {
        pointValue = std::stoi(inputText);
        ValidateAndUpdatePoints(); // Enforce limits after input
    } catch (const std::exception &) {
        pointValue = 0;
        inputText = "0";
    }
}

// void LevelUpPanel::ValidateAndUpdatePoints() {
//     totalPointsAllocated = strengthPoints + manaPoints + healthPoints;
//
//     if (totalPointsAllocated > maxPoints) {
//         totalPointsAllocated = strengthPoints + manaPoints + healthPoints;
//
//         if (totalPointsAllocated > maxPoints) {
//             strengthPoints = std::min(strengthPoints, maxPoints);
//             manaPoints = std::min(manaPoints, maxPoints - strengthPoints);
//             healthPoints = std::min(healthPoints, maxPoints - strengthPoints - manaPoints);
//
//             strengthInputText = std::to_string(strengthPoints);
//             manaInputText = std::to_string(manaPoints);
//             healthInputText = std::to_string(healthPoints);
//         }
//     }
// }

void LevelUpPanel::ValidateAndUpdatePoints() {
    // First ensure no negative values
    strengthPoints = std::max(0, strengthPoints);
    manaPoints = std::max(0, manaPoints);
    healthPoints = std::max(0, healthPoints);

    // Calculate total allocated points
    totalPointsAllocated = strengthPoints + manaPoints + healthPoints;

    // If total exceeds max, reduce the most recently changed field
    if (totalPointsAllocated > maxPoints) {
        int excess = totalPointsAllocated - maxPoints;

        if (healthPoints >= excess) {
            healthPoints -= excess;
        } else if (manaPoints >= excess) {
            manaPoints -= excess;
        } else {
            strengthPoints -= excess;
        }

        // Recalculate after adjustment
        totalPointsAllocated = strengthPoints + manaPoints + healthPoints;
    }

    // Update text fields to match clamped values
    strengthInputText = std::to_string(strengthPoints);
    manaInputText = std::to_string(manaPoints);
    healthInputText = std::to_string(healthPoints);
}

void LevelUpPanel::DrawInputField(const Rectangle bounds, const std::string &text, const bool isActive,
                                  const std::string &level) const {
    Color bgColor = isActive ? inputActiveColor : inputBgColor;

    DrawRectangleRec(bounds, bgColor);
    DrawRectangleLinesEx(bounds, 1, isActive ? WHITE : GRAY);

    Vector2 textSize = MeasureTextEx(font, text.c_str(), 16, 1);
    Vector2 textPos = {
        bounds.x + (bounds.width - textSize.x) / 2,
        bounds.y + (bounds.height - textSize.y) / 2
    };
    DrawTextEx(font, text.c_str(), textPos, 16, 1, WHITE);

    if (isActive) {
        const float cursorX = textPos.x + textSize.x + 2;
        if (static_cast<int>(GetTime() * 2) % 2) {
            DrawLine(static_cast<int>(cursorX), static_cast<int>(bounds.y + 5),
                     static_cast<int>(cursorX), static_cast<int>(bounds.y + bounds.height - 5),WHITE);
        }
    }
}

bool LevelUpPanel::IsPointInRect(Vector2 point, Rectangle rect) const {
    return CheckCollisionPointRec(point, rect);
}

void LevelUpPanel::Show() {
    isVisible = true;
    Reset();
}

void LevelUpPanel::Hide() {
    isVisible = false;
    strengthInputActive = false;
    manaInputActive = false;
    healthInputActive = false;
}

bool LevelUpPanel::IsVisible() const {
    return isVisible;
}

void LevelUpPanel::SetFont(const Font &customFont) {
    font = customFont;
}

void LevelUpPanel::SetColors(const Color panel, const Color border, const Color text, const Color inputBg,
                             const Color inputActive) {
    panelColor = panel;
    borderColor = border;
    textColor = text;
    inputBgColor = inputBg;
    inputActiveColor = inputActive;
}

void LevelUpPanel::SetMaxPoints(const int maxPts) {
    maxPoints = std::max(1, maxPts);
    ValidateAndUpdatePoints();
}

void LevelUpPanel::SetBounds(const Rectangle bounds) {
    panelBounds = bounds;
    UpdateButtonPositions();
}

void LevelUpPanel::UpdateButtonPositions() {
    const float panelX = panelBounds.x;
    const float panelY = panelBounds.y;
    float panelW = panelBounds.width;

    strengthInputBounds = {panelX + 150, panelY + 60, 80, 30};
    manaInputBounds = {panelX + 150, panelY + 110, 80, 30};
    healthInputBounds = {panelX + 150, panelY + 160, 80, 30};

    const float arrowSize = 25;
    const float arrowSpacing = 5;

    strengthUpButton = Button({
                                  strengthInputBounds.x + strengthInputBounds.width + arrowSpacing,
                                  strengthInputBounds.y, arrowSize, arrowSize
                              }, "+", nullptr);
    strengthDownButton = Button({
                                    strengthInputBounds.x + strengthInputBounds.width + arrowSpacing + arrowSize + 2,
                                    strengthInputBounds.y, arrowSize, arrowSize
                                }, "-", nullptr);

    manaUpButton = Button({
                              manaInputBounds.x + manaInputBounds.width + arrowSpacing,
                              manaInputBounds.y, arrowSize, arrowSize
                          }, "+", nullptr);
    manaDownButton = Button({
                                manaInputBounds.x + manaInputBounds.width + arrowSpacing + arrowSize + 2,
                                manaInputBounds.y, arrowSize, arrowSize
                            }, "-", nullptr);

    healthUpButton = Button({
                                healthInputBounds.x + healthInputBounds.width + arrowSpacing,
                                healthInputBounds.y, arrowSize, arrowSize
                            }, "+", nullptr);
    healthDownButton = Button({
                                  healthInputBounds.x + healthInputBounds.width + arrowSpacing + arrowSize + 2,
                                  healthInputBounds.y, arrowSize, arrowSize
                              }, "-", nullptr);

    confirmButton = Button({panelX + 150, panelY + 240, 100, 40}, "Confirm", nullptr);
    //cancelButton = Button({panelX + confirmButton.GetBounds().x + 75, panelY + 240, 100, 40}, "Cancel", nullptr);

    SetupArrowButtons();
}

void LevelUpPanel::SetupArrowButtons() {
    strengthUpButton = Button(strengthUpButton.GetBounds(), "+", [this]() {
        if (totalPointsAllocated < maxPoints) {
            strengthPoints++;
            strengthInputText = std::to_string(strengthPoints);
            ValidateAndUpdatePoints();
        }
    });

    strengthDownButton = Button(strengthDownButton.GetBounds(), "-", [this]() {
        if (strengthPoints > 0) {
            strengthPoints--;
            strengthInputText = std::to_string(strengthPoints);
            ValidateAndUpdatePoints();
        }
    });

    manaUpButton = Button(manaUpButton.GetBounds(), "+", [this]() {
        if (totalPointsAllocated < maxPoints) {
            manaPoints++;
            manaInputText = std::to_string(manaPoints);
            ValidateAndUpdatePoints();
        }
    });

    manaDownButton = Button(manaDownButton.GetBounds(), "-", [this]() {
        if (manaPoints > 0) {
            manaPoints--;
            manaInputText = std::to_string(manaPoints);
            ValidateAndUpdatePoints();
        }
    });

    healthUpButton = Button(healthUpButton.GetBounds(), "+", [this]() {
        if (totalPointsAllocated < maxPoints) {
            healthPoints++;
            healthInputText = std::to_string(healthPoints);
            ValidateAndUpdatePoints();
        }
    });

    healthDownButton = Button(healthDownButton.GetBounds(), "-", [this]() {
        if (healthPoints > 0) {
            healthPoints--;
            healthInputText = std::to_string(healthPoints);
            ValidateAndUpdatePoints();
        }
    });

    confirmButton = Button(confirmButton.GetBounds(), "Confirm", [this]() {
        if (heroRef) {
            ApplyStatsToHero();
        }


        if (onConfirm) {
            onConfirm(strengthPoints, manaPoints, healthPoints);
        }
        Hide();
    });
}

void LevelUpPanel::SetOnConfirm(const std::function<void(int str, int mana, int health)> &callback) {
    onConfirm = callback;
}

void LevelUpPanel::SetOnCancel(const std::function<void()> &callback) {
    onCancel = callback;
}

void LevelUpPanel::SetHeroReference(Hero *hero) {
    heroRef = hero;
}

void LevelUpPanel::ApplyStatsToHero() const {
    if (!heroRef) return;

    heroRef->levelUp(strengthPoints, manaPoints, healthPoints);
}

void LevelUpPanel::ShowForHero(Hero *hero) {
    SetHeroReference(hero);
    Show();
}

int LevelUpPanel::GetStrengthPoints() const {
    return strengthPoints;
}

int LevelUpPanel::GetManaPoints() const {
    return manaPoints;
}

int LevelUpPanel::GetHealthPoints() const {
    return healthPoints;
}

int LevelUpPanel::GetTotalPointsAllocated() const {
    return totalPointsAllocated;
}

int LevelUpPanel::GetRemainingPoints() const {
    return maxPoints - totalPointsAllocated;
}

void LevelUpPanel::Reset() {
    strengthPoints = 0;
    manaPoints = 0;
    healthPoints = 0;
    totalPointsAllocated = 0;

    strengthInputText = "0";
    manaInputText = "0";
    healthInputText = "0";

    strengthInputActive = false;
    manaInputActive = false;
    healthInputActive = false;
}
