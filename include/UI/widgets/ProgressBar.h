//
// Created by Lenovo on 15.5.2025 г..
//

#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include "raylib.h"
#include <string>
#include <optional>

class ProgressBar {
public:
    explicit ProgressBar(Rectangle _bounds, float _maxValue);

    void Update(float newValue, float deltaTime);

    void SetColors(Color bg, Color fg, Color border);

    void SetBackgroundTexture(Texture2D tex);

    void SetFont(const Font &customFont, int size, Color color = WHITE);

    void SetTextPrefix(const std::string &prefix);

    void ShowText(bool enabled);

    void SetMaxValue(float newMaxValue);

    float GetMaxValue() const;

    void Draw() const;

private:
    Rectangle bounds;
    float displayedValue;
    float targetValue;
    float maxValue;

    Color bgColor = GRAY;
    Color fgColor = GREEN;
    Color borderColor = BLACK;

    std::optional<Texture2D> bgTexture;

    bool drawText = false;
    std::string textPrefix;
    Font font = GetFontDefault();
    int fontSize = 16;
    Color textColor = WHITE;
};

#endif //PROGRESSBAR_H
