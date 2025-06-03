//
// Created by Lenovo on 15.5.2025 г.
//

#include "C:/DandD/include/UI/widgets/ProgressBar.h"
#include <cmath> // Required for std::fabs and sin

// Constructor for ProgressBar with initial bounds and max value.
ProgressBar::ProgressBar(const Rectangle bounds, const float maxValue)
    : bounds(bounds), // Sets the bounding rectangle of the progress bar
      displayedValue(maxValue), // Initial displayed value (usually same as max)
      targetValue(maxValue), // Initial target value
      maxValue(maxValue > 0.0f ? maxValue : 50.0f), // Sets max value, defaults to 50 if 0 or less
      transitionSpeed(2.0f), // Speed at which the bar fills/empties
      bgColor(LIGHTGRAY), // Background color of the bar
      fgColor(GREEN), // Foreground (fill) color of the bar
      borderColor(DARKGRAY), // Border color
      borderThickness(1.0f), // Thickness of the border
      rounding(0.0f), // Corner rounding factor (0.0 for sharp corners)
      ownsTexture(false), // Flag to indicate if the ProgressBar owns its background texture
      drawText(false), // Whether to draw text on the bar
      font(GetFontDefault()), // Default font for text
      fontSize(16), // Default font size
      textColor(WHITE), // Default text color
      pulsatingEffect(false), // Pulsating effect disabled by default
      pulseIntensity(0.2f), // Default intensity for pulsating effect
      pulseSpeed(1.0f), // Default speed for pulsating effect
      pulseTime(0.0f) // Current time for pulse animation
{
    // Default text formatter: "current/max" as integers
    textFormatter = [](const float current, const float max) {
        return std::to_string(static_cast<int>(current)) + "/" + std::to_string(static_cast<int>(max));
    };
}

// Copy constructor for ProgressBar.
// Ensures that textures are not double-freed by setting ownsTexture to false for the copy.
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
      ownsTexture(false), // The copy does NOT own the texture; it's a shared reference if present
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
    // If the other ProgressBar has a background texture, copy the optional value.
    if (other.bgTexture.has_value()) {
        bgTexture = other.bgTexture;
    }
}

// Copy assignment operator for ProgressBar.
// Handles proper cleanup of existing resources before copying new state, especially textures.
ProgressBar &ProgressBar::operator=(const ProgressBar &other) {
    if (this != &other) {
        // Prevent self-assignment
        // If this ProgressBar owns a texture, unload it before copying.
        if (ownsTexture && bgTexture.has_value()) {
            UnloadTexture(bgTexture.value());
        }

        // Copy all member variables from 'other'.
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

        // The assigned object does not own the texture of 'other'.
        ownsTexture = false;
        if (other.bgTexture.has_value()) {
            bgTexture = other.bgTexture;
        } else {
            bgTexture.reset(); // Clear optional texture if 'other' doesn't have one.
        }
    }

    return *this; // Return reference to allow chaining.
}

// Destructor for ProgressBar.
// Unloads the background texture if the ProgressBar instance owns it.
ProgressBar::~ProgressBar() {
    // Clean up textures we own
    if (ownsTexture && bgTexture.has_value()) {
        UnloadTexture(bgTexture.value());
    }
}

// Updates the progress bar's displayed value and pulsating effect over time.
void ProgressBar::Update(float newValue, float deltaTime) {
    SetValue(newValue); // Set the new target value for the bar.

    float speed = maxValue * transitionSpeed; // Calculate the transition speed.
    // Smoothly transition the displayedValue towards the targetValue.
    if (fabs(displayedValue - targetValue) > 0.01f) {
        // Check if difference is significant enough to animate.
        if (displayedValue < targetValue) {
            displayedValue += speed * deltaTime;
            if (displayedValue > targetValue) displayedValue = targetValue; // Cap at target to prevent overshooting.
        } else {
            displayedValue -= speed * deltaTime;
            if (displayedValue < targetValue) displayedValue = targetValue; // Cap at target to prevent overshooting.
        }
    } else {
        displayedValue = targetValue; // Snap to target if very close.
    }

    // Update pulsating effect time.
    if (pulsatingEffect) {
        pulseTime += deltaTime * pulseSpeed; // Advance pulse time.
        if (pulseTime > 2.0f * PI) {
            // Reset pulse time after a full cycle (2*PI for sin function).
            pulseTime -= 2.0f * PI;
        }
    }
}

// Draws the progress bar on the screen.
void ProgressBar::Draw() const {
    // Draw the overall background of the bar. This line might be redundant if bgTexture or rounding is used.
    DrawRectangleRounded(bounds, 0.5f, 8, bgColor);

    // Draw the background texture if present.
    if (bgTexture.has_value()) {
        DrawTexturePro(
            bgTexture.value(),
            {0, 0, static_cast<float>(bgTexture->width), static_cast<float>(bgTexture->height)},
            // Source rectangle for the entire texture
            bounds, // Destination rectangle (the bounds of the progress bar)
            {0, 0}, // Origin
            0.0f, // Rotation
            WHITE); // Tint color
    } else if (rounding > 0.0f) {
        // If no texture, draw a rounded background.
        DrawRectangleRounded(bounds, rounding, 10, bgColor);
    } else {
        // Otherwise, draw a standard rectangular background.
        DrawRectangleRec(bounds, bgColor);
    }

    // Calculate the fill ratio based on displayed value.
    const float fillRatio = maxValue > 0.0f ? (displayedValue / maxValue) : 0.0f;
    Color currentFgColor = fgColor; // Start with the base foreground color.

    // Apply pulsating effect to foreground color if enabled and bar is not empty.
    if (pulsatingEffect && fillRatio > 0.0f) {
        const float pulse = 1.0f + pulseIntensity * sin(pulseTime); // Calculate pulse factor using sine wave.
        // Adjust R, G, B components based on pulse intensity, clamping at 255.
        currentFgColor.r = static_cast<unsigned char>(std::min(255, static_cast<int>(fgColor.r * pulse)));
        currentFgColor.g = static_cast<unsigned char>(std::min(255, static_cast<int>(fgColor.g * pulse)));
        currentFgColor.b = static_cast<unsigned char>(std::min(255, static_cast<int>(fgColor.b * pulse)));
    }

    // Calculate the rectangle for the filled portion of the bar.
    Rectangle fillRect = {
        bounds.x,
        bounds.y,
        bounds.width * fillRatio, // Width is proportional to fill ratio.
        bounds.height
    };

    // Draw the filled portion of the bar.
    if (fillRect.width > 0) {
        // Only draw if there's something to fill.
        if (rounding > 0.0f) {
            float effectiveRounding = rounding;
            // Adjust rounding for very short fill bars to prevent visual glitches.
            if (fillRect.width < bounds.height * rounding * 2) {
                effectiveRounding = fillRect.width / (bounds.height * 2);
            }
            DrawRectangleRounded(fillRect, effectiveRounding, 10, currentFgColor);
        } else {
            DrawRectangleRec(fillRect, currentFgColor);
        }
    }

    // Draw the border of the progress bar.
    if (borderThickness > 0.0f) {
        if (rounding > 0.0f) {
            DrawRectangleRoundedLines(bounds, rounding, 10, borderThickness, borderColor);
        } else {
            DrawRectangleLinesEx(bounds, borderThickness, borderColor);
        }
    }

    // Draw text on the progress bar if enabled.
    if (drawText) {
        const std::string text = GetFormattedText(); // Get the formatted text.
        const Vector2 textSize = MeasureTextEx(font, text.c_str(), fontSize, 1); // Measure text dimensions.
        const Vector2 textPos = {
            bounds.x + (bounds.width - textSize.x) * 0.5f, // Center text horizontally.
            bounds.y + (bounds.height - textSize.y) * 0.5f // Center text vertically.
        };

        DrawTextEx(font, text.c_str(), textPos, fontSize, 1, textColor); // Draw main text.
        // Draw a slightly offset, semi-transparent text for a subtle glow/shadow effect.
        DrawTextEx(font, text.c_str(), {
                       textPos.x - 1.5f, textPos.y + 1.5f
                   }, fontSize, 1, {textColor.r, textColor.g, textColor.b, 100});
    }
}

// Sets the target value of the progress bar, clamping it between 0 and maxValue.
void ProgressBar::SetValue(float newValue) {
    targetValue = (newValue < 0.0f) ? 0.0f : (newValue > maxValue ? maxValue : newValue);
}

// Returns the current target value of the progress bar.
float ProgressBar::GetValue() const {
    return targetValue;
}

// Sets a new maximum value for the progress bar.
// Adjusts target and displayed values proportionally to maintain current fill ratio.
void ProgressBar::SetMaxValue(const float newMaxValue) {
    if (newMaxValue <= 0.0f) return; // Prevent setting max value to zero or negative.

    if (maxValue <= 0.0f) {
        // If current max value is invalid, simply set values to new max.
        targetValue = displayedValue = newMaxValue;
    } else {
        // Otherwise, adjust proportionally.
        const float ratio = newMaxValue / maxValue; // Calculate the ratio of new max to old max.
        targetValue *= ratio; // Scale target value.
        displayedValue *= ratio; // Scale displayed value.
    }
    maxValue = newMaxValue; // Update the max value.
}

// Returns the maximum value of the progress bar.
float ProgressBar::GetMaxValue() const {
    return maxValue;
}

// Returns the current fill percentage of the progress bar.
float ProgressBar::GetPercentage() const {
    return (maxValue > 0.0f) ? (targetValue / maxValue * 100.0f) : 0.0f;
}

// Sets the speed at which the displayed value transitions to the target value.
void ProgressBar::SetTransitionSpeed(const float speed) {
    transitionSpeed = (speed < 0.0f) ? 0.0f : speed; // Ensure speed is non-negative.
}

// Sets the background, foreground, and border colors of the progress bar.
void ProgressBar::SetColors(Color background, Color foreground, Color border) {
    bgColor = background;
    fgColor = foreground;
    borderColor = border;
}

// Sets a background texture for the progress bar.
// If an existing texture was owned, it's unloaded first. The new texture is now owned by this instance.
void ProgressBar::SetBackgroundTexture(Texture2D texture) {
    // Unload existing owned texture before setting a new one.
    if (ownsTexture && bgTexture.has_value()) {
        UnloadTexture(bgTexture.value());
    }

    bgTexture = texture; // Assign the new texture.
    ownsTexture = true; // Mark as owned.
}

// Clears any set background texture and unloads it if owned by this instance.
void ProgressBar::ClearBackgroundTexture() {
    if (ownsTexture && bgTexture.has_value()) {
        UnloadTexture(bgTexture.value());
    }

    bgTexture.reset(); // Clear the optional texture.
    ownsTexture = false; // Mark as not owning a texture.
}

// Sets the corner rounding factor for the progress bar.
// Clamps the factor between 0.0f and 1.0f.
void ProgressBar::SetRounding(float roundingFactor) {
    rounding = (roundingFactor < 0.0f) ? 0.0f : (roundingFactor > 1.0f ? 1.0f : roundingFactor);
}

// Sets the thickness of the progress bar's border.
// Ensures thickness is non-negative.
void ProgressBar::SetBorderThickness(const float thickness) {
    borderThickness = (thickness < 0.0f) ? 0.0f : thickness;
}

// Sets the color of the progress bar's border.
void ProgressBar::SetBorderColor(const Color color) {
    borderColor = color;
}

// Sets the font, size, and color for the text displayed on the progress bar.
// Also enables text drawing.
void ProgressBar::SetFont(const Font &customFont, const int size, const Color color) {
    font = customFont;
    fontSize = (size > 0) ? size : fontSize; // Only update if size is positive.
    textColor = color;
    drawText = true; // Enable text drawing automatically when font is set.
}

// Sets only the font size for the text displayed on the progress bar.
void ProgressBar::SetFontSize(const int newFontSize) {
    fontSize = newFontSize;
}

// Sets a prefix string to be displayed before the formatted value text.
void ProgressBar::SetTextPrefix(const std::string &prefix) {
    textPrefix = prefix;
}

// Sets a suffix string to be displayed after the formatted value text.
void ProgressBar::SetTextSuffix(const std::string &suffix) {
    textSuffix = suffix;
}

// Enables or disables the drawing of text on the progress bar.
void ProgressBar::ShowText(bool enabled) {
    drawText = enabled;
}

// Sets a custom formatting function for the text displayed on the progress bar.
void ProgressBar::SetTextFormat(const std::function<std::string(float, float)> &formatter) {
    if (formatter) {
        // Only assign if the provided formatter is valid.
        textFormatter = formatter;
    }
}

// Enables or disables a pulsating effect for the foreground color.
// Sets intensity and speed for the pulse.
void ProgressBar::EnablePulsatingEffect(const bool enable, const float intensity, const float speed) {
    pulsatingEffect = enable;
    pulseIntensity = (intensity < 0.0f) ? 0.0f : (intensity > 1.0f ? 1.0f : intensity); // Clamp intensity.
    pulseSpeed = (speed <= 0.0f) ? 0.1f : speed; // Ensure positive speed.
    pulseTime = 0.0f; // Reset pulse time when enabling.
}

// Returns the bounding rectangle of the progress bar.
Rectangle ProgressBar::GetBounds() const {
    return bounds;
}

// Sets a new bounding rectangle for the progress bar.
void ProgressBar::SetBounds(const Rectangle newBounds) {
    bounds = newBounds;
}

// Returns the fully formatted text string, including prefix, formatted value, and suffix.
std::string ProgressBar::GetFormattedText() const {
    return textPrefix + textFormatter(targetValue, maxValue) + textSuffix;
}
