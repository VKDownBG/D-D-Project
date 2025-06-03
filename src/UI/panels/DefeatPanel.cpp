//
// Created by Lenovo on 3.6.2025 г..
//

#include "C:/DandD/include/UI/panels/DefeatPanel.h"

void DefeatPanel::Show() {
    isActive = true;
    quitSelected = false;

    const int screenWidth = GetScreenWidth();
    const int screenHeight = GetScreenHeight();

    quitButton = Button(
        {static_cast<float>(screenWidth / 2 - 110), static_cast<float>(screenHeight / 2 + 120), 200, 70},
        "QUIT",
        [this]() { CloseWindow(); });
    quitButton.Draw();
    quitButton.SetColors({
                             85, 0, 0, 255
                         }, {75, 0, 0, 255}, LIGHTGRAY, WHITE);
    quitButton.SetRoundedCorners(4.0f);
    quitButton.SetActive(true);
}


void DefeatPanel::Update() {
    if (!isActive) return;


    quitButton.Update(GetMousePosition());
}

void DefeatPanel::Draw() const {
    if (!isActive) return;

    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), {0, 0, 0, 100});

    // "DEFEAT" text
    const char *text = "DEFEAT";
    const int fontSize = 250;
    const int textWidth = MeasureText(text, fontSize);
    DrawTextEx(GetFontDefault(), text,
               {
                   static_cast<float>(GetScreenWidth() / 2 - 450), static_cast<float>(GetScreenHeight() / 2 - 150),
               }, fontSize, 2,
               {85, 0, 0, 255});

    quitButton.Draw();
}

bool DefeatPanel::ShouldQuit() const {
    return quitSelected;
}
