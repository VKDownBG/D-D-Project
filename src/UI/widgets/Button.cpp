//
// Created by Lenovo on 15.5.2025 г.
//

#include "C:/DandD/include/UI/widgets/Button.h"
#include <utility>

Button::Button()
    : bounds({0, 0, 100, 50}),
      font(GetFontDefault()),
      texture({0}),
      action(nullptr),
      isHovered(false),
      isPressed(false),
      isActive(true),
      animationProgress(0.0f),
      normalColor(DARKGRAY),
      hoverColor(GRAY),
      pressedColor(LIGHTGRAY),
      textColor(WHITE),
      borderColor(WHITE),
      borderThickness(0),
      cornerRadius(0.0f),
      useHoverAnimation(false),
      hoverScale(1.1f),
      animationSpeed(5.0f) {
}

Button::Button(const Rectangle rect, std::string text, std::function<void()> onClick)
    : bounds(rect),
      label(std::move(text)),
      font(GetFontDefault()),
      texture({0}),
      action(std::move(onClick)),
      isHovered(false),
      isPressed(false),
      isActive(true),
      animationProgress(0.0f),
      normalColor(DARKGRAY),
      hoverColor(GRAY),
      pressedColor(LIGHTGRAY),
      textColor(WHITE),
      borderColor(WHITE),
      borderThickness(0),
      cornerRadius(0.0f),
      useHoverAnimation(false),
      hoverScale(1.1f),
      animationSpeed(5.0f) {
}

Button::Button(const Rectangle rect, const Texture2D &buttonTexture, std::function<void()> onClick)
    : bounds(rect),
      font(GetFontDefault()),
      texture(buttonTexture),
      action(std::move(onClick)),
      isHovered(false),
      isPressed(false),
      isActive(true),
      animationProgress(0.0f),
      normalColor(WHITE),
      hoverColor(WHITE),
      pressedColor(GRAY),
      textColor(WHITE),
      borderColor(WHITE),
      borderThickness(0),
      cornerRadius(0.0f),
      useHoverAnimation(false),
      hoverScale(1.1f),
      animationSpeed(5.0f) {
}

void Button::SetFont(const Font &customFont) {
    font = customFont;
}

void Button::SetTexture(const Texture2D &customTexture) {
    texture = customTexture;
}

void Button::SetColors(Color normal, Color hover, Color pressed, Color text) {
    normalColor = normal;
    hoverColor = hover;
    pressedColor = pressed;
    textColor = text;
}

void Button::SetBorder(Color color, int thickness) {
    borderColor = color;
    borderThickness = thickness;
}

void Button::SetRoundedCorners(float radius) {
    cornerRadius = radius;
}

void Button::EnableHoverAnimation(bool enable, float scale, float speed) {
    useHoverAnimation = enable;
    hoverScale = scale;
    animationSpeed = speed;
}

void Button::Update(Vector2 mousePosition) {
    if (!isActive) return;

    if (mousePosition.x == 0 && mousePosition.y == 0) {
        mousePosition = GetMousePosition();
    }

    isHovered = CheckCollisionPointRec(mousePosition, bounds);

    if (useHoverAnimation) {
        if (isHovered) {
            animationProgress += GetFrameTime() * animationSpeed;
            if (animationProgress > 1.0f) animationProgress = 1.0f;
        } else {
            animationProgress -= GetFrameTime() * animationSpeed;
            if (animationProgress < 0.0f) animationProgress = 0.0f;
        }
    }

    isPressed = false;
    if (isHovered) {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            isPressed = true;

            if (action) {
                action();
            }
        }
    }
}

void Button::Draw() const {
    if (!isActive) return;

    Rectangle drawBounds = bounds;
    if (useHoverAnimation && animationProgress > 0.0f) {
        const float scaleFactor = 1.0f + (hoverScale - 1.0f) * animationProgress;

        const float centerX = bounds.x + bounds.width / 2;
        const float centerY = bounds.y + bounds.height / 2;

        drawBounds.width *= scaleFactor;
        drawBounds.height *= scaleFactor;
        drawBounds.x = centerX - drawBounds.width / 2;
        drawBounds.y = centerY - drawBounds.height / 2;
    }

    // Determine the current color based on state
    Color currentColor;
    if (isPressed) {
        currentColor = pressedColor;
    } else if (isHovered) {
        currentColor = hoverColor;
    } else {
        currentColor = normalColor;
    }

    if (texture.id != 0) {
        DrawTexturePro(texture,
                       {0, 0, static_cast<float>(texture.width), static_cast<float>(texture.height)},
                       drawBounds,
                       {0, 0},
                       0.0f,
                       currentColor);
    } else {
        if (cornerRadius > 0) {
            DrawRectangleRounded(drawBounds, cornerRadius, 10, currentColor);
        } else {
            DrawRectangleRec(drawBounds, currentColor);
        }
    }

    if (borderThickness > 0) {
        if (cornerRadius > 0) {
            DrawRectangleRoundedLines(drawBounds, cornerRadius, 10, borderThickness, borderColor);
        } else {
            for (int i = 0; i < borderThickness; i++) {
                DrawRectangleLinesEx({
                                         drawBounds.x + i / 2.0f,
                                         drawBounds.y + i / 2.0f,
                                         drawBounds.width - i,
                                         drawBounds.height - i
                                     }, 1.0f, borderColor);
            }
        }
    }

    if (!label.empty()) {
        int fontSize = 20;
        Vector2 textSize = MeasureTextEx(font, label.c_str(), fontSize, 1);
        Vector2 textPos = {
            drawBounds.x + (drawBounds.width - textSize.x) / 2,
            drawBounds.y + (drawBounds.height - textSize.y) / 2
        };

        DrawTextEx(font, label.c_str(), {textPos.x + 1, textPos.y + 1}, fontSize, 1, BLACK);
        DrawTextEx(font, label.c_str(), textPos, fontSize, 1, textColor);
    }

    if (isHovered && !useHoverAnimation) {
        DrawRectangleLinesEx(drawBounds, 2, {255, 255, 255, 100});
    }
}

bool Button::IsPressed() const {
    return isPressed && isActive;
}

bool Button::IsHovered() const {
    return isHovered && isActive;
}

bool Button::IsActive() const {
    return isActive;
}

void Button::SetActive(bool active) {
    isActive = active;
}

Rectangle Button::GetBounds() const {
    return bounds;
}

// const Texture2D &Button::GetTexture() const {
//     return texture;
// }
