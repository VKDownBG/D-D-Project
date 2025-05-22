#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include "raylib.h"
#include <string>
#include <optional>
#include <functional>

class ProgressBar {
public:
    ProgressBar() = default;

    explicit ProgressBar(Rectangle bounds, float maxValue = 100.0f);

    ProgressBar(const ProgressBar &other);

    ProgressBar &operator=(const ProgressBar &other);

    ~ProgressBar();

    void Update(float newValue, float deltaTime);

    void Draw() const;

    void SetValue(float newValue);

    float GetValue() const;

    void SetMaxValue(float newMaxValue);

    float GetMaxValue() const;

    float GetPercentage() const;

    void SetTransitionSpeed(float speed);

    void SetColors(Color background, Color foreground, Color border);

    void SetBackgroundTexture(Texture2D texture);

    void ClearBackgroundTexture();

    void SetRounding(float rounding);

    void SetBorderThickness(float thickness);

    void SetBorderColor(Color color);

    void SetFont(const Font &customFont, int fontSize, Color textColor = WHITE);

    void SetTextPrefix(const std::string &prefix);

    void SetTextSuffix(const std::string &suffix);

    void SetFontSize(int newFontSize);

    void ShowText(bool enabled);

    void SetTextFormat(const std::function<std::string(float current, float max)> &formatter);

    void EnablePulsatingEffect(bool enabled, float intensity = 0.2f, float speed = 1.0f);

    Rectangle GetBounds() const;

    void SetBounds(Rectangle newBounds);

private:
    Rectangle bounds;
    float displayedValue;
    float targetValue;
    float maxValue;
    float transitionSpeed;

    Color bgColor;
    Color fgColor;
    Color borderColor;
    float borderThickness;
    float rounding;

    std::optional<Texture2D> bgTexture;
    bool ownsTexture;

    bool drawText;
    std::string textPrefix;
    std::string textSuffix;
    Font font;
    int fontSize;
    Color textColor;
    std::function<std::string(float, float)> textFormatter;

    bool pulsatingEffect;
    float pulseIntensity;
    float pulseSpeed;
    float pulseTime;

    std::string GetFormattedText() const;
};

#endif // PROGRESSBAR_H
