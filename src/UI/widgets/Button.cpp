//
// Created by Lenovo on 15.5.2025 г.
//

#include "C:/DandD/include/UI/widgets/Button.h"

#include <algorithm> // Required for std::clamp
#include <utility>   // Required for std::move

// Default constructor for the Button class.
// Initializes a button with default size, font, colors, and no action or texture.
Button::Button()
    : bounds({0, 0, 100, 50}), // Default button size and position
      font(GetFontDefault()), // Sets the default Raylib font
      texture({0}), // Initializes texture to an empty (unloaded) state
      action(nullptr), // No click action assigned by default
      isHovered(false), // Not hovered initially
      isPressed(false), // Not pressed initially
      isActive(true), // Button is active by default
      animationProgress(0.0f), // Initial animation progress for hover effect
      normalColor(DARKGRAY), // Default normal color
      hoverColor(GRAY), // Default hover color
      pressedColor(LIGHTGRAY), // Default pressed color
      textColor(WHITE), // Default text color
      borderColor(WHITE), // Default border color
      borderThickness(0), // No border by default
      cornerRadius(0.0f), // No rounded corners by default
      useHoverAnimation(false), // Hover animation is disabled by default
      hoverScale(1.1f), // Default scale factor for hover animation
      animationSpeed(5.0f) // Default speed for hover animation
{
}

// Constructor for a text-based button.
// Initializes a button with specified bounds, text label, and a click action.
Button::Button(const Rectangle rect, std::string text, std::function<void()> onClick)
    : bounds(rect), // Sets the button's bounding rectangle
      label(std::move(text)), // Sets the button's text label
      font(GetFontDefault()), // Sets the default Raylib font
      texture({0}), // No texture assigned
      action(std::move(onClick)), // Assigns the provided click action
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

// Constructor for an image-based button.
// Initializes a button with specified bounds, a texture, and a click action.
Button::Button(const Rectangle rect, const Texture2D &buttonTexture, std::function<void()> onClick)
    : bounds(rect),
      font(GetFontDefault()),
      texture(buttonTexture), // Sets the button's texture
      action(std::move(onClick)), // Assigns the provided click action
      isHovered(false),
      isPressed(false),
      isActive(true),
      animationProgress(0.0f),
      normalColor(WHITE), // Default colors are set for a texture-based button
      hoverColor(WHITE),
      pressedColor(GRAY),
      textColor(WHITE), // Text color still white, but might not be used if texture is dominant
      borderColor(WHITE),
      borderThickness(0),
      cornerRadius(0.0f),
      useHoverAnimation(false),
      hoverScale(1.1f),
      animationSpeed(5.0f) {
}

// Sets a custom font for the button's label.
void Button::SetFont(const Font &customFont) {
    font = customFont;
}

// Sets a custom texture for the button's background.
void Button::SetTexture(const Texture2D &customTexture) {
    texture = customTexture;
}

// Sets the colors for the button in its normal, hovered, and pressed states, as well as the text color.
void Button::SetColors(const Color normal, const Color hover, const Color pressed, const Color text) {
    normalColor = normal;
    hoverColor = hover;
    pressedColor = pressed;
    textColor = text;
}

// Sets the border color and thickness for the button.
void Button::SetBorder(const Color color, const int thickness) {
    borderColor = color;
    borderThickness = thickness;
}

// Sets the corner radius for rounded rectangles when drawing the button.
void Button::SetRoundedCorners(const float radius) {
    cornerRadius = radius;
}

// Enables or disables the hover animation effect, and sets its scale factor and speed.
void Button::EnableHoverAnimation(const bool enable, const float scale, const float speed) {
    useHoverAnimation = enable;
    hoverScale = scale;
    animationSpeed = speed;
}

// Updates the button's state based on mouse position and time.
void Button::Update(Vector2 mousePosition) {
    if (!isActive) return; // If the button is not active, skip update.

    // Check if the mouse cursor is currently over the button's bounds.
    isHovered = CheckCollisionPointRec(mousePosition, bounds);

    // Handle hover animation progress.
    if (useHoverAnimation) {
        // If hovered, increase animation progress; otherwise, decrease it.
        // std::clamp ensures the progress stays between 0.0f and 1.0f.
        animationProgress = std::clamp(animationProgress +
                                       (isHovered
                                            ? GetFrameTime() * animationSpeed // Increase progress when hovered
                                            : -GetFrameTime() * animationSpeed), // Decrease progress when not hovered
                                       0.0f, 1.0f);
    }

    // Reset pressed state before checking for new press.
    isPressed = false;
    // Check if the button is hovered and the left mouse button is pressed.
    if (isHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        isPressed = true; // Set pressed state to true.
        if (action) action(); // Execute the assigned click action if it exists.
    }
}

// Draws the button on the screen based on its current state and properties.
void Button::Draw() const {
    if (!isActive) return; // If the button is not active, do not draw.

    Rectangle drawBounds = bounds; // Start with the original bounds for drawing.

    // Apply hover animation if enabled and progress is greater than 0.
    if (useHoverAnimation && animationProgress > 0.0f) {
        // Calculate the current scale factor based on animation progress.
        const float scaleFactor = 1.0f + (hoverScale - 1.0f) * animationProgress;

        // Calculate the center of the original bounds to scale from the center.
        const float centerX = bounds.x + bounds.width / 2;
        const float centerY = bounds.y + bounds.height / 2;

        // Apply scaling to width and height.
        drawBounds.width *= scaleFactor;
        drawBounds.height *= scaleFactor;
        // Adjust x and y to keep the button centered after scaling.
        drawBounds.x = centerX - drawBounds.width / 2;
        drawBounds.y = centerY - drawBounds.height / 2;
    }

    // Determine the current drawing color based on the button's state (pressed > hovered > normal).
    Color currentColor;
    if (isPressed) {
        currentColor = pressedColor;
    } else if (isHovered) {
        currentColor = hoverColor;
    } else {
        currentColor = normalColor;
    }

    // Draw the button's background, either with a texture or a solid color/rounded rectangle.
    if (texture.id != 0) {
        // If a texture is assigned and loaded.
        DrawTexturePro(texture,
                       {0, 0, static_cast<float>(texture.width), static_cast<float>(texture.height)},
                       // Source rect (whole texture)
                       drawBounds, // Destination rect (current button bounds)
                       {0, 0}, // Origin for drawing
                       0.0f, // Rotation
                       currentColor); // Tint with current color
    } else {
        // If no texture, draw a colored rectangle.
        if (cornerRadius > 0) {
            // If rounded corners are specified.
            DrawRectangleRounded(drawBounds, cornerRadius, 10, currentColor); // Draw rounded rectangle.
        } else {
            DrawRectangleRec(drawBounds, currentColor); // Draw standard rectangle.
        }
    }

    // Draw the button's border if borderThickness is greater than 0.
    if (borderThickness > 0) {
        if (cornerRadius > 0) {
            // If rounded corners, draw rounded border lines.
            DrawRectangleRoundedLines(drawBounds, cornerRadius, 10, borderThickness, borderColor);
        } else {
            // Otherwise, draw standard rectangle lines.
            // Loop to draw multiple lines for thicker borders, creating a layered effect.
            for (int i = 0; i < borderThickness; i++) {
                DrawRectangleLinesEx({
                                         drawBounds.x + i / 2.0f, // Offset x to keep border centered
                                         drawBounds.y + i / 2.0f, // Offset y to keep border centered
                                         drawBounds.width - i, // Reduce width for inner lines
                                         drawBounds.height - i // Reduce height for inner lines
                                     }, 1.0f, borderColor); // Draw a 1-pixel thick line for each iteration
            }
        }
    }

    // Draw the button's text label if it's not empty.
    if (!label.empty()) {
        const int fontSize = 20;
        // Measure text size to center it within the button.
        const Vector2 textSize = MeasureTextEx(font, label.c_str(), fontSize, 1);
        const Vector2 textPos = {
            drawBounds.x + (drawBounds.width - textSize.x) / 2, // Center text horizontally
            drawBounds.y + (drawBounds.height - textSize.y) / 2 // Center text vertically
        };

        // Draw a shadow effect for the text.
        DrawTextEx(font, label.c_str(), {textPos.x + 1, textPos.y + 1}, fontSize, 1, BLACK);
        // Draw the main text.
        DrawTextEx(font, label.c_str(), textPos, fontSize, 1, textColor);
    }

    // Draw a subtle white highlight when hovered and no custom hover animation is used.
    if (isHovered && !useHoverAnimation) {
        DrawRectangleLinesEx(drawBounds, 2, {255, 255, 255, 100}); // Semi-transparent white border.
    }
}

// Checks if the button is currently in a pressed state and is active.
bool Button::IsPressed() const {
    return isPressed && isActive;
}

// Checks if the button is currently in a hovered state and is active.
bool Button::IsHovered() const {
    return isHovered && isActive;
}

// Checks if the button is currently active.
bool Button::IsActive() const {
    return isActive;
}

// Sets the active state of the button. An inactive button doesn't update or draw.
void Button::SetActive(bool active) {
    isActive = active;
}

// Returns the bounding rectangle of the button.
Rectangle Button::GetBounds() const {
    return bounds;
}
