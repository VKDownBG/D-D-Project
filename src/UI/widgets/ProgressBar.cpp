//
// Created by Lenovo on 15.5.2025 г.
//

#include "C:/DandD/include/UI/widgets/ProgressBar.h"
#include <cmath>

ProgressBar::ProgressBar(const Rectangle bounds, const float maxValue)
    : bounds(bounds),
      displayedValue(maxValue),
      targetValue(maxValue),
      maxValue(maxValue > 0.0f ? maxValue : 50.0f),
      transitionSpeed(2.0f),
      bgColor(LIGHTGRAY),
      fgColor(GREEN),
      borderColor(DARKGRAY),
      borderThickness(1.0f),
      rounding(0.0f),
      ownsTexture(false),
      drawText(false),
      font(GetFontDefault()),
      fontSize(16),
      textColor(WHITE),
      pulsatingEffect(false),
      pulseIntensity(0.2f),
      pulseSpeed(1.0f),
      pulseTime(0.0f) {
    textFormatter = [](const float current, const float max) {
        return std::to_string(static_cast<int>(current)) + "/" + std::to_string(static_cast<int>(max));
    };
}

ProgressBar::ProgressBar(const ProgressBar &other)
    : bounds(other.bounds),
      displayedValue(other.displayedValue),
      targetValue(other.targetValue),
      maxValue(other.maxValue),
      transitionSpeed(other.transitionSpeed),
      bgColor(other.bgColor),
      fgColor(other.fgColor),
      borderColor(other.borderColor),
      borderThickness(other.borderThickness),
      rounding(other.rounding),
      ownsTexture(false),
      drawText(other.drawText),
      textPrefix(other.textPrefix),
      textSuffix(other.textSuffix),
      font(other.font),
      fontSize(other.fontSize),
      textColor(other.textColor),
      textFormatter(other.textFormatter),
      pulsatingEffect(other.pulsatingEffect),
      pulseIntensity(other.pulseIntensity),
      pulseSpeed(other.pulseSpeed),
      pulseTime(other.pulseTime) {
    if (other.bgTexture.has_value()) {
        bgTexture = other.bgTexture;
    }
}

ProgressBar &ProgressBar::operator=(const ProgressBar &other) {
    if (this != &other) {
        if (ownsTexture && bgTexture.has_value()) {
            UnloadTexture(bgTexture.value());
        }

        bounds = other.bounds;
        displayedValue = other.displayedValue;
        targetValue = other.targetValue;
        maxValue = other.maxValue;
        transitionSpeed = other.transitionSpeed;
        bgColor = other.bgColor;
        fgColor = other.fgColor;
        borderColor = other.borderColor;
        borderThickness = other.borderThickness;
        rounding = other.rounding;
        drawText = other.drawText;
        textPrefix = other.textPrefix;
        textSuffix = other.textSuffix;
        font = other.font;
        fontSize = other.fontSize;
        textColor = other.textColor;
        textFormatter = other.textFormatter;
        pulsatingEffect = other.pulsatingEffect;
        pulseIntensity = other.pulseIntensity;
        pulseSpeed = other.pulseSpeed;
        pulseTime = other.pulseTime;

        ownsTexture = false;
        if (other.bgTexture.has_value()) {
            bgTexture = other.bgTexture;
        } else {
            bgTexture.reset();
        }
    }

    return *this;
}

ProgressBar::~ProgressBar() {
    // Clean up textures we own
    if (ownsTexture && bgTexture.has_value()) {
        UnloadTexture(bgTexture.value());
    }
}

void ProgressBar::Update(float newValue, float deltaTime) {
    SetValue(newValue);

    float speed = maxValue * transitionSpeed;
    if (fabs(displayedValue - targetValue) > 0.01f) {
        if (displayedValue < targetValue) {
            displayedValue += speed * deltaTime;
            if (displayedValue > targetValue) displayedValue = targetValue;
        } else {
            displayedValue -= speed * deltaTime;
            if (displayedValue < targetValue) displayedValue = targetValue;
        }
    } else {
        displayedValue = targetValue;
    }

    if (pulsatingEffect) {
        pulseTime += deltaTime * pulseSpeed;
        if (pulseTime > 2.0f * PI) {
            pulseTime -= 2.0f * PI;
        }
    }
}

void ProgressBar::Draw() const {
    DrawRectangleRounded(bounds, 0.5f, 8, bgColor);

    if (bgTexture.has_value()) {
        DrawTexturePro(
            bgTexture.value(),
            {0, 0, static_cast<float>(bgTexture->width), static_cast<float>(bgTexture->height)},
            bounds,
            {0, 0},
            0.0f,
            WHITE);
    } else if (rounding > 0.0f) {
        DrawRectangleRounded(bounds, rounding, 10, bgColor);
    } else {
        DrawRectangleRec(bounds, bgColor);
    }

    const float fillRatio = maxValue > 0.0f ? (displayedValue / maxValue) : 0.0f;
    Color currentFgColor = fgColor;
    if (pulsatingEffect && fillRatio > 0.0f) {
        const float pulse = 1.0f + pulseIntensity * sin(pulseTime);
        currentFgColor.r = static_cast<unsigned char>(std::min(255, static_cast<int>(fgColor.r * pulse)));
        currentFgColor.g = static_cast<unsigned char>(std::min(255, static_cast<int>(fgColor.g * pulse)));
        currentFgColor.b = static_cast<unsigned char>(std::min(255, static_cast<int>(fgColor.b * pulse)));
    }

    Rectangle fillRect = {
        bounds.x,
        bounds.y,
        bounds.width * fillRatio,
        bounds.height
    };

    if (fillRect.width > 0) {
        if (rounding > 0.0f) {
            float effectiveRounding = rounding;
            if (fillRect.width < bounds.height * rounding * 2) {
                effectiveRounding = fillRect.width / (bounds.height * 2);
            }
            DrawRectangleRounded(fillRect, effectiveRounding, 10, currentFgColor);
        } else {
            DrawRectangleRec(fillRect, currentFgColor);
        }
    }

    if (borderThickness > 0.0f) {
        if (rounding > 0.0f) {
            DrawRectangleRoundedLines(bounds, rounding, 10, borderThickness, borderColor);
        } else {
            DrawRectangleLinesEx(bounds, borderThickness, borderColor);
        }
    }

    if (drawText) {
        const std::string text = GetFormattedText();
        Vector2 textSize = MeasureTextEx(font, text.c_str(), fontSize, 1);
        Vector2 textPos = {
            bounds.x + (bounds.width - textSize.x) * 0.5f,
            bounds.y + (bounds.height - textSize.y) * 0.5f
        };

        DrawTextEx(font, text.c_str(), textPos, fontSize, 1, textColor);
        DrawTextEx(font, text.c_str(), {
                       textPos.x - 1.5f, textPos.y + 1.5f
                   }, fontSize, 1, {textColor.r, textColor.g, textColor.b, 100});
    }
}

void ProgressBar::SetValue(float newValue) {
    targetValue = (newValue < 0.0f) ? 0.0f : (newValue > maxValue ? maxValue : newValue);
}

float ProgressBar::GetValue() const {
    return targetValue;
}

void ProgressBar::SetMaxValue(const float newMaxValue) {
    if (newMaxValue <= 0.0f) return;

    if (maxValue <= 0.0f) {
        targetValue = displayedValue = newMaxValue;
    } else {
        const float ratio = newMaxValue / maxValue;
        targetValue *= ratio;
        displayedValue *= ratio;
    }
    maxValue = newMaxValue;
}

float ProgressBar::GetMaxValue() const {
    return maxValue;
}

float ProgressBar::GetPercentage() const {
    return (maxValue > 0.0f) ? (targetValue / maxValue * 100.0f) : 0.0f;
}

void ProgressBar::SetTransitionSpeed(float speed) {
    transitionSpeed = (speed < 0.0f) ? 0.0f : speed;
}

void ProgressBar::SetColors(Color background, Color foreground, Color border) {
    bgColor = background;
    fgColor = foreground;
    borderColor = border;
}

void ProgressBar::SetBackgroundTexture(Texture2D texture) {
    if (ownsTexture && bgTexture.has_value()) {
        UnloadTexture(bgTexture.value());
    }

    bgTexture = texture;
    ownsTexture = true;
}

void ProgressBar::ClearBackgroundTexture() {
    if (ownsTexture && bgTexture.has_value()) {
        UnloadTexture(bgTexture.value());
    }

    bgTexture.reset();
    ownsTexture = false;
}

void ProgressBar::SetRounding(float roundingFactor) {
    rounding = (roundingFactor < 0.0f) ? 0.0f : (roundingFactor > 1.0f ? 1.0f : roundingFactor);
}

void ProgressBar::SetBorderThickness(const float thickness) {
    borderThickness = (thickness < 0.0f) ? 0.0f : thickness;
}

void ProgressBar::SetBorderColor(const Color color) {
    borderColor = color;
}

void ProgressBar::SetFont(const Font &customFont, int size, Color color) {
    font = customFont;
    fontSize = (size > 0) ? size : fontSize;
    textColor = color;
    drawText = true;
}

void ProgressBar::SetFontSize(const int newFontSize) {
    fontSize = newFontSize;
}

void ProgressBar::SetTextPrefix(const std::string &prefix) {
    textPrefix = prefix;
}

void ProgressBar::SetTextSuffix(const std::string &suffix) {
    textSuffix = suffix;
}

void ProgressBar::ShowText(bool enabled) {
    drawText = enabled;
}

void ProgressBar::SetTextFormat(const std::function<std::string(float, float)> &formatter) {
    if (formatter) {
        textFormatter = formatter;
    }
}

void ProgressBar::EnablePulsatingEffect(bool enable, float intensity, float speed) {
    pulsatingEffect = enable;
    pulseIntensity = (intensity < 0.0f) ? 0.0f : (intensity > 1.0f ? 1.0f : intensity);
    pulseSpeed = (speed <= 0.0f) ? 0.1f : speed;
    pulseTime = 0.0f;
}

Rectangle ProgressBar::GetBounds() const {
    return bounds;
}

void ProgressBar::SetBounds(const Rectangle newBounds) {
    bounds = newBounds;
}

std::string ProgressBar::GetFormattedText() const {
    return textPrefix + textFormatter(targetValue, maxValue) + textSuffix;
}
