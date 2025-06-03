#ifndef BUTTON_H
#define BUTTON_H

#include "raylib.h"
#include <functional>
#include <string>

class Button {
public:
    Rectangle bounds;

    Button();

    Button(Rectangle rect, std::string text, std::function<void()> onClick);

    Button(Rectangle rect, const Texture2D &buttonTexture, std::function<void()> onClick);

    void SetFont(const Font &customFont);

    void SetTexture(const Texture2D &customTexture);

    void SetColors(Color normal, Color hover, Color pressed, Color text);

    void SetBorder(Color color, int thickness = 1);

    void SetRoundedCorners(float radius);

    void EnableHoverAnimation(bool enable = true, float scale = 1.1f, float speed = 5.0f);

    void Update(Vector2 mousePosition = {0, 0});

    void Draw() const;

    bool IsPressed() const;

    bool IsHovered() const;

    bool IsActive() const;

    void SetActive(bool active);

    Rectangle GetBounds() const;

    ~Button() = default;

private:
    std::string label;
    Font font;
    Texture2D texture;
    std::function<void()> action;

    bool isHovered;
    bool isPressed;
    bool isActive;
    float animationProgress;

    Color normalColor;
    Color hoverColor;
    Color pressedColor;
    Color textColor;
    Color borderColor;
    int borderThickness;
    float cornerRadius;

    bool useHoverAnimation;
    float hoverScale;
    float animationSpeed;
};

#endif //BUTTON_H
