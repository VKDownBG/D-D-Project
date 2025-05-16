//
// Created by Lenovo on 15.5.2025 г..
//

#ifndef BUTTON_H
#define BUTTON_H

#include <functional>
#include <string>
#include "raylib.h"

class Button {
private:
    Rectangle bounds;
    std::string label;
    Font font = GetFontDefault();
    Texture2D texture = {0};
    std::function<void()> action;

    bool isHovered = false;
    bool isPressed = false;

public:
    Button(Rectangle rect, std::string text, std::function<void()> onClick);

    void SetFont(Font customFont);

    void SetTexture(Texture2D customTexture);

    void Update();

    void Draw() const;

    [[nodiscard]] bool IsPressed() const;
};

#endif //BUTTON_H
