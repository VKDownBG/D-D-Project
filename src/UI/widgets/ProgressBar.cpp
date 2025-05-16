//
// Created by Lenovo on 15.5.2025 г.
//

#include "C:/DandD/include/UI/widgets/ProgressBar.h"

ProgressBar::ProgressBar(Rectangle _bounds, float _maxValue)
    : bounds(_bounds), displayedValue(_maxValue), targetValue(_maxValue), maxValue(_maxValue) {
}

void ProgressBar::Update(float newValue, float deltaTime) {
    targetValue = (newValue < 0) ? 0 : (newValue > maxValue ? maxValue : newValue);

    float speed = maxValue * 2.0f;

    if (displayedValue < targetValue) {
        displayedValue += speed * deltaTime;
        if (displayedValue > targetValue) displayedValue = targetValue;
    } else if (displayedValue > targetValue) {
        displayedValue -= speed * deltaTime;
        if (displayedValue < targetValue) displayedValue = targetValue;
    }
}

void ProgressBar::SetColors(Color bg, Color fg, Color border) {
    bgColor = bg;
    fgColor = fg;
    borderColor = border;
}

void ProgressBar::SetBackgroundTexture(Texture2D tex) {
    bgTexture = tex;
}

void ProgressBar::SetFont(const Font &customFont, const int size, const Color color) {
    font = customFont;
    fontSize = size;
    textColor = color;
    drawText = true;
}

void ProgressBar::SetTextPrefix(const std::string &prefix) {
    textPrefix = prefix;
}

void ProgressBar::ShowText(bool enabled) {
    drawText = enabled;
}

void ProgressBar::SetMaxValue(const float newMaxValue) {
    maxValue = (newMaxValue > 0) ? newMaxValue : maxValue;
    if (targetValue > maxValue) targetValue = maxValue;
    if (displayedValue > maxValue) displayedValue = maxValue;
}

float ProgressBar::GetMaxValue() const {
    return maxValue;
}

void ProgressBar::Draw() const {
    if (bgTexture) {
        DrawTexturePro(bgTexture.value(),
                       {0, 0, static_cast<float>(bgTexture->width), static_cast<float>(bgTexture->height)},
                       {bounds.x, bounds.y, bounds.width, bounds.height},
                       {0, 0}, 0.0f, WHITE);
    } else {
        DrawRectangleRec(bounds, bgColor);
    }

    float fillRatio = (maxValue > 0 ? (displayedValue / maxValue) : 0);
    Rectangle fillRect = {bounds.x, bounds.y, bounds.width * fillRatio, bounds.height};
    DrawRectangleRec(fillRect, fgColor);

    DrawRectangleLinesEx(bounds, 1, borderColor);

    if (drawText) {
        const std::string text = textPrefix + std::to_string(static_cast<int>(targetValue))
                           + "/" + std::to_string(static_cast<int>(maxValue));
        Vector2 sz = MeasureTextEx(font, text.c_str(), fontSize, 1);
        Vector2 pos = {bounds.x + (bounds.width - sz.x) / 2, bounds.y + (bounds.height - sz.y) / 2};
        DrawTextEx(font, text.c_str(), pos, fontSize, 1, textColor);
    }
}
