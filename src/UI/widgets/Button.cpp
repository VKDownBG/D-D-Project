//
// Created by Lenovo on 15.5.2025 г.
//

#include <utility>

#include "C:/DandD/include/UI/widgets/Button.h"

Button::Button(const Rectangle rect, std::string text, std::function<void()> onClick)
    : bounds(rect), label(std::move(text)), action(std::move(onClick)) {
}

void Button::SetFont(Font customFont) {
    font = customFont;
}

void Button::SetTexture(Texture2D customTexture) {
    texture = customTexture;
}

void Button::Update() {
    Vector2 mousePos = GetMousePosition();
    isHovered = CheckCollisionPointRec(mousePos, bounds);
    isPressed = false;

    if (isHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        isPressed = true;
        if (action) action();
    }
}

void Button::Draw() const {
    if (texture.id != 0) {
        DrawTexturePro(texture,
                       {0, 0, (float) texture.width, (float) texture.height},
                       bounds,
                       {0, 0},
                       0.0f,
                       WHITE);
    } else {
        Color bgColor = isHovered ? Color{150, 150, 150, 255} : Color{100, 100, 100, 255};
        DrawRectangleRec(bounds, bgColor);
    }

    int fontSize = 20;
    Vector2 textSize = MeasureTextEx(GetFontDefault(), label.c_str(), fontSize, 1);
    Vector2 textPos = {
        bounds.x + (bounds.width - textSize.x) / 2,
        bounds.y + (bounds.height - textSize.y) / 2
    };

    DrawTextEx(font, label.c_str(), textPos, fontSize, 1, WHITE);
}

bool Button::IsPressed() const {
    return isPressed;
}
