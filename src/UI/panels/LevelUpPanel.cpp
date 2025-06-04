//
// Created by Lenovo on 25.5.2025 г.
//

#include "C:/DandD/include/UI/panels/LevelUpPanel.h"
#include <algorithm> // Required for std::max

// Default constructor for the LevelUpPanel.
// Initializes the panel's position, size, colors, and default state.
LevelUpPanel::LevelUpPanel()
    : panelBounds({55, static_cast<float>(GetScreenHeight()) / 2 - 100, 350, 300}), // Default panel dimensions
      font(GetFontDefault()), // Sets the default Raylib font
      panelColor({40, 40, 40, 240}), // Dark gray, semi-transparent panel background
      borderColor(WHITE), // White border color
      textColor(WHITE), // White text color
      inputBgColor({60, 60, 60, 255}), // Darker gray for input field background
      inputActiveColor({80, 80, 120, 255}), // Lighter blue-gray for active input field
      isVisible(false), // Panel is hidden by default
      strengthPoints(0), // Initial points for strength
      manaPoints(0), // Initial points for mana
      healthPoints(0), // Initial points for health
      totalPointsAllocated(0), // Initial total allocated points
      maxPoints(30), // Maximum points that can be allocated
      strengthInputActive(false), // Strength input field not active initially
      manaInputActive(false), // Mana input field not active initially
      healthInputActive(false), // Health input field not active initially
      strengthInputText("0"), // Initial text for strength input
      manaInputText("0"), // Initial text for mana input
      healthInputText("0"), // Initial text for health input
      onConfirm(nullptr), // No confirm action assigned
      onCancel(nullptr), // No cancel action assigned
      heroRef(nullptr) // No hero reference assigned
{
    UpdateButtonPositions(); // Sets up the positions of internal UI elements (buttons, input fields)
    SetupArrowButtons(); // Configures the actions for the arrow buttons
}

// Overloaded constructor that allows setting custom bounds and font.
// Calls the default constructor for initial setup, then applies custom settings.
LevelUpPanel::LevelUpPanel(const Rectangle bounds, const Font &customFont)
    : LevelUpPanel() // Calls the default constructor for common initialization
{
    SetBounds(bounds); // Set custom panel bounds
    SetFont(customFont); // Set custom font
}

// Updates the state of the LevelUpPanel, handling user input and button interactions.
void LevelUpPanel::Update() {
    if (!isVisible) return; // Only update if the panel is visible

    const Vector2 mousePos = GetMousePosition(); // Get current mouse position

    // Enable or disable the confirm button based on whether all points are allocated.
    confirmButton.SetActive(totalPointsAllocated == maxPoints);
    confirmButton.Update(mousePos); // Update confirm button state (hover, pressed)
    // cancelButton.Update(mousePos); // Update cancel button state (currently commented out)

    // Update the state of all arrow buttons.
    strengthUpButton.Update(mousePos);
    strengthDownButton.Update(mousePos);
    manaUpButton.Update(mousePos);
    manaDownButton.Update(mousePos);
    healthUpButton.Update(mousePos);
    healthDownButton.Update(mousePos);

    // Check if any input field is clicked to activate it.
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        strengthInputActive = IsPointInRect(mousePos, strengthInputBounds);
        manaInputActive = IsPointInRect(mousePos, manaInputBounds);
        healthInputActive = IsPointInRect(mousePos, healthInputBounds);
    }

    // Update active input fields based on keyboard input.
    if (strengthInputActive) {
        UpdateInputField(strengthInputBounds, strengthInputActive, strengthInputText, strengthPoints);
    }
    if (manaInputActive) {
        UpdateInputField(manaInputBounds, manaInputActive, manaInputText, manaPoints);
    }
    if (healthInputActive) {
        UpdateInputField(healthInputBounds, healthInputActive, healthInputText, healthPoints);
    }

    ValidateAndUpdatePoints(); // Ensure point allocation rules are followed.
}

// Draws all elements of the LevelUpPanel to the screen.
void LevelUpPanel::Draw() const {
    if (!isVisible) return; // Only draw if the panel is visible

    // Draw a semi-transparent overlay over the entire screen to dim the background.
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), {0, 0, 0, 100});

    // Draw the main panel background and border.
    DrawRectangleRounded(panelBounds, 0.1f, 10, panelColor);
    DrawRectangleRoundedLines(panelBounds, 0.1f, 10, 2, borderColor);

    // Draw the "Level Up!" title.
    const std::string title = "Level Up!";
    const Vector2 titleSize = MeasureTextEx(font, title.c_str(), 24, 1);
    const Vector2 titlePos = {
        panelBounds.x + (panelBounds.width - titleSize.x) / 2, // Center title horizontally
        panelBounds.y + 15
    };
    DrawTextEx(font, title.c_str(), titlePos, 24, 1, textColor);

    // Draw "Strength:" label and its input field.
    DrawTextEx(font, "Strength:", {panelBounds.x + 20, panelBounds.y + 65}, 18, 1, textColor);
    DrawInputField(strengthInputBounds, strengthInputText, strengthInputActive, "Strength");

    // Draw "Mana:" label and its input field.
    DrawTextEx(font, "Mana:", {panelBounds.x + 20, panelBounds.y + 115}, 18, 1, textColor);
    DrawInputField(manaInputBounds, manaInputText, manaInputActive, "Mana");

    // Draw "Health:" label and its input field.
    DrawTextEx(font, "Health:", {panelBounds.x + 20, panelBounds.y + 165}, 18, 1, textColor);
    DrawInputField(healthInputBounds, healthInputText, healthInputActive, "Health");

    // Draw the "Points remaining:" text.
    const std::string remainingText = "Points remaining: " + std::to_string(GetRemainingPoints());
    DrawTextEx(font, remainingText.c_str(), {panelBounds.x + 20, panelBounds.y + 205}, 16, 1, textColor);

    // Draw the confirm and arrow buttons.
    confirmButton.Draw();
    //cancelButton.Draw();

    strengthUpButton.Draw();
    strengthDownButton.Draw();
    manaUpButton.Draw();
    manaDownButton.Draw();
    healthUpButton.Draw();
    healthDownButton.Draw();
}

void LevelUpPanel::ValidateIndividualInput(int &pointValue, const std::string &inputText) {
    int requestedValue = 0;

    try {
        requestedValue = std::stoi(inputText);
    } catch (const std::exception &) {
        requestedValue = 0;
    }

    // Calculate the sum of the OTHER two stats (not the one being modified)
    int otherTwoStats = 0;
    if (&pointValue == &strengthPoints) {
        otherTwoStats = manaPoints + healthPoints;
    } else if (&pointValue == &manaPoints) {
        otherTwoStats = strengthPoints + healthPoints;
    } else if (&pointValue == &healthPoints) {
        otherTwoStats = strengthPoints + manaPoints;
    }

    // Cap the requested value so total doesn't exceed maxPoints
    int maxAllowed = maxPoints - otherTwoStats;
    pointValue = std::max(0, std::min(requestedValue, maxAllowed));
}

// Handles keyboard input for an active text input field.
void LevelUpPanel::UpdateInputField(Rectangle inputBounds, bool &isActive, std::string &inputText, int &pointValue) {
    int key = GetCharPressed();

    while (key > 0) {
        if (key >= '0' && key <= '9' && inputText.length() < 2) {
            if (inputText == "0") {
                inputText = static_cast<char>(key);
            } else {
                inputText += static_cast<char>(key);
            }
        }
        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE) && !inputText.empty()) {
        inputText.pop_back();
        if (inputText.empty()) {
            inputText = "0";
        }
    }

    if (IsKeyPressed(KEY_ENTER)) {
        isActive = false;
    }

    // Only use the new validation method
    ValidateIndividualInput(pointValue, inputText);
    inputText = std::to_string(pointValue);

    // Just recalculate total - no validation
    totalPointsAllocated = strengthPoints + manaPoints + healthPoints;
}

// Validates the allocated points, ensuring they don't exceed maxPoints and adjusting if necessary.
void LevelUpPanel::ValidateAndUpdatePoints() {
    // Ensure no negative point values
    strengthPoints = std::max(0, strengthPoints);
    manaPoints = std::max(0, manaPoints);
    healthPoints = std::max(0, healthPoints);

    // Calculate what the total would be with current values
    int potentialTotal = strengthPoints + manaPoints + healthPoints;

    // If the total exceeds maxPoints, we need to reduce the values
    if (potentialTotal > maxPoints) {
        // Calculate how much we need to reduce
        int excess = potentialTotal - maxPoints;

        // Reduce each stat proportionally, but ensure no stat goes below 0
        // This maintains the relative distribution the user intended
        int totalBeforeReduction = potentialTotal;

        strengthPoints = std::max(0, strengthPoints - (excess * strengthPoints / totalBeforeReduction));
        manaPoints = std::max(0, manaPoints - (excess * manaPoints / totalBeforeReduction));
        healthPoints = std::max(0, healthPoints - (excess * healthPoints / totalBeforeReduction));

        // Handle any remaining excess due to integer division
        int newTotal = strengthPoints + manaPoints + healthPoints;
        int remainingExcess = maxPoints - newTotal;

        if (remainingExcess < 0) {
            // Still over limit, reduce from highest value
            if (strengthPoints >= manaPoints && strengthPoints >= healthPoints && strengthPoints > 0) {
                strengthPoints += remainingExcess; // remainingExcess is negative
            } else if (manaPoints >= healthPoints && manaPoints > 0) {
                manaPoints += remainingExcess;
            } else if (healthPoints > 0) {
                healthPoints += remainingExcess;
            }
        }
    }

    // Recalculate total allocated points
    totalPointsAllocated = strengthPoints + manaPoints + healthPoints;

    // Update the text in the input fields to reflect the corrected values
    strengthInputText = std::to_string(strengthPoints);
    manaInputText = std::to_string(manaPoints);
    healthInputText = std::to_string(healthPoints);
}

// Draws a single text input field, including its background, text, and active state indicator (cursor).
void LevelUpPanel::DrawInputField(const Rectangle bounds, const std::string &text, const bool isActive,
                                  const std::string &level) const {
    const Color bgColor = isActive ? inputActiveColor : inputBgColor; // Use active color if field is selected.

    DrawRectangleRec(bounds, bgColor); // Draw the input field background.
    DrawRectangleLinesEx(bounds, 1, isActive ? WHITE : GRAY); // Draw border, white if active, gray otherwise.

    const Vector2 textSize = MeasureTextEx(font, text.c_str(), 16, 1); // Measure text size for centering.
    const Vector2 textPos = {
        bounds.x + (bounds.width - textSize.x) / 2, // Center text horizontally.
        bounds.y + (bounds.height - textSize.y) / 2 // Center text vertically.
    };
    DrawTextEx(font, text.c_str(), textPos, 16, 1, WHITE); // Draw the actual text.

    // Draw a blinking cursor if the input field is active.
    if (isActive) {
        const float cursorX = textPos.x + textSize.x + 2; // Position cursor just after the text.
        // Blinking effect: cursor drawn every other half-second.
        if (static_cast<int>(GetTime() * 2) % 2) {
            DrawLine(static_cast<int>(cursorX), static_cast<int>(bounds.y + 5),
                     static_cast<int>(cursorX), static_cast<int>(bounds.y + bounds.height - 5),WHITE);
        }
    }
}

// Checks if a given point is within a specified rectangle.
bool LevelUpPanel::IsPointInRect(Vector2 point, Rectangle rect) const {
    return CheckCollisionPointRec(point, rect);
}

// Makes the level-up panel visible and resets its allocated points.
void LevelUpPanel::Show() {
    isVisible = true;
    Reset(); // Reset points and input fields when showing the panel.
}

// Hides the level-up panel and deactivates all input fields.
void LevelUpPanel::Hide() {
    isVisible = false;
    strengthInputActive = false;
    manaInputActive = false;
    healthInputActive = false;
}

// Checks if the level-up panel is currently visible.
bool LevelUpPanel::IsVisible() const {
    return isVisible;
}

// Sets a custom font for the panel's text.
void LevelUpPanel::SetFont(const Font &customFont) {
    font = customFont;
}

// Sets the various color schemes for the panel.
void LevelUpPanel::SetColors(const Color panel, const Color border, const Color text, const Color inputBg,
                             const Color inputActive) {
    panelColor = panel;
    borderColor = border;
    textColor = text;
    inputBgColor = inputBg;
    inputActiveColor = inputActive;
}

// Sets the maximum number of points that can be allocated.
// Ensures maxPoints is at least 1 and re-validates current allocation.
void LevelUpPanel::SetMaxPoints(const int maxPts) {
    maxPoints = std::max(1, maxPts); // Ensure maxPoints is at least 1.
    ValidateAndUpdatePoints(); // Re-validate points against the new max.
}

// Sets the bounding rectangle for the entire panel and updates positions of its internal elements.
void LevelUpPanel::SetBounds(const Rectangle bounds) {
    panelBounds = bounds;
    UpdateButtonPositions(); // Recalculate positions of buttons and input fields relative to new panel bounds.
}

// Recalculates and sets the positions and sizes of input fields and buttons based on panelBounds.
void LevelUpPanel::UpdateButtonPositions() {
    const float panelX = panelBounds.x;
    const float panelY = panelBounds.y;
    float panelW = panelBounds.width; // Not directly used in current calculations but could be for dynamic sizing.

    // Set bounds for the numeric input fields.
    strengthInputBounds = {panelX + 150, panelY + 60, 80, 30};
    manaInputBounds = {panelX + 150, panelY + 110, 80, 30};
    healthInputBounds = {panelX + 150, panelY + 160, 80, 30};

    const float arrowSize = 25; // Size of the arrow buttons.
    const float arrowSpacing = 5; // Spacing between input field and first arrow button.

    // Define the initial (dummy) buttons with calculated positions.
    // Their actions are set in SetupArrowButtons().
    strengthUpButton = Button({
                                  strengthInputBounds.x + strengthInputBounds.width + arrowSpacing,
                                  strengthInputBounds.y, arrowSize, arrowSize
                              }, "+", nullptr);
    strengthDownButton = Button({
                                    strengthInputBounds.x + strengthInputBounds.width + arrowSpacing + arrowSize + 2,
                                    strengthInputBounds.y, arrowSize, arrowSize
                                }, "-", nullptr);

    manaUpButton = Button({
                              manaInputBounds.x + manaInputBounds.width + arrowSpacing,
                              manaInputBounds.y, arrowSize, arrowSize
                          }, "+", nullptr);
    manaDownButton = Button({
                                manaInputBounds.x + manaInputBounds.width + arrowSpacing + arrowSize + 2,
                                manaInputBounds.y, arrowSize, arrowSize
                            }, "-", nullptr);

    healthUpButton = Button({
                                healthInputBounds.x + healthInputBounds.width + arrowSpacing,
                                healthInputBounds.y, arrowSize, arrowSize
                            }, "+", nullptr);
    healthDownButton = Button({
                                  healthInputBounds.x + healthInputBounds.width + arrowSpacing + arrowSize + 2,
                                  healthInputBounds.y, arrowSize, arrowSize
                              }, "-", nullptr);

    confirmButton = Button({panelX + 150, panelY + 240, 100, 40}, "Confirm", nullptr);
    //cancelButton = Button({panelX + confirmButton.GetBounds().x + 75, panelY + 240, 100, 40}, "Cancel", nullptr);

    SetupArrowButtons(); // Re-assign the click actions for the buttons.
}

// Sets up the click actions (lambdas) for all the increment/decrement arrow buttons and the confirm button.
void LevelUpPanel::SetupArrowButtons() {
    strengthUpButton = Button(strengthUpButton.GetBounds(), "+", [this]() {
        if (totalPointsAllocated < maxPoints) {
            strengthPoints++;
            strengthInputText = std::to_string(strengthPoints);
            // REMOVED: ValidateAndUpdatePoints();
            totalPointsAllocated = strengthPoints + manaPoints + healthPoints; // Just recalculate
        }
    });

    strengthDownButton = Button(strengthDownButton.GetBounds(), "-", [this]() {
        if (strengthPoints > 0) {
            strengthPoints--;
            strengthInputText = std::to_string(strengthPoints);
            // REMOVED: ValidateAndUpdatePoints();
            totalPointsAllocated = strengthPoints + manaPoints + healthPoints; // Just recalculate
        }
    });

    manaUpButton = Button(manaUpButton.GetBounds(), "+", [this]() {
        if (totalPointsAllocated < maxPoints) {
            manaPoints++;
            manaInputText = std::to_string(manaPoints);
            // REMOVED: ValidateAndUpdatePoints();
            totalPointsAllocated = strengthPoints + manaPoints + healthPoints; // Just recalculate
        }
    });

    manaDownButton = Button(manaDownButton.GetBounds(), "-", [this]() {
        if (manaPoints > 0) {
            manaPoints--;
            manaInputText = std::to_string(manaPoints);
            // REMOVED: ValidateAndUpdatePoints();
            totalPointsAllocated = strengthPoints + manaPoints + healthPoints; // Just recalculate
        }
    });

    healthUpButton = Button(healthUpButton.GetBounds(), "+", [this]() {
        if (totalPointsAllocated < maxPoints) {
            healthPoints++;
            healthInputText = std::to_string(healthPoints);
            // REMOVED: ValidateAndUpdatePoints();
            totalPointsAllocated = strengthPoints + manaPoints + healthPoints; // Just recalculate
        }
    });

    healthDownButton = Button(healthDownButton.GetBounds(), "-", [this]() {
        if (healthPoints > 0) {
            healthPoints--;
            healthInputText = std::to_string(healthPoints);
            // REMOVED: ValidateAndUpdatePoints();
            totalPointsAllocated = strengthPoints + manaPoints + healthPoints; // Just recalculate
        }
    });

    confirmButton = Button(confirmButton.GetBounds(), "Confirm", [this]() {
        if (onConfirm) {
            onConfirm(strengthPoints, manaPoints, healthPoints);
        }
        Hide();
    });
}

// Sets the callback function to be invoked when the "Confirm" button is pressed.
void LevelUpPanel::SetOnConfirm(const std::function<void(int str, int mana, int health)> &callback) {
    onConfirm = callback;
}

// Sets the callback function to be invoked when the "Cancel" button is pressed (currently unused).
void LevelUpPanel::SetOnCancel(const std::function<void()> &callback) {
    onCancel = callback;
}

// Sets a reference to the Hero object whose stats are being leveled up.
void LevelUpPanel::SetHeroReference(Hero *hero) {
    heroRef = hero;
}

// Applies the allocated points to the referenced Hero object.
void LevelUpPanel::ApplyStatsToHero() const {
    if (!heroRef) return; // Only apply if a hero reference exists.

    heroRef->levelUp(strengthPoints, manaPoints, healthPoints); // Call hero's levelUp method.
}

// Shows the panel and associates it with a specific Hero for stat application.
void LevelUpPanel::ShowForHero(Hero *hero) {
    SetHeroReference(hero); // Set the hero reference.
    Show(); // Make the panel visible and reset its state.
}

// Returns the currently allocated strength points.
int LevelUpPanel::GetStrengthPoints() const {
    return strengthPoints;
}

// Returns the currently allocated mana points.
int LevelUpPanel::GetManaPoints() const {
    return manaPoints;
}

// Returns the currently allocated health points.
int LevelUpPanel::GetHealthPoints() const {
    return healthPoints;
}

// Returns the total number of points currently allocated across all stats.
int LevelUpPanel::GetTotalPointsAllocated() const {
    return totalPointsAllocated;
}

// Returns the number of points remaining to be allocated.
int LevelUpPanel::GetRemainingPoints() const {
    return maxPoints - totalPointsAllocated;
}

// Resets all allocated points and input field states to their initial values.
void LevelUpPanel::Reset() {
    strengthPoints = 0;
    manaPoints = 0;
    healthPoints = 0;
    totalPointsAllocated = 0;

    strengthInputText = "0";
    manaInputText = "0";
    healthInputText = "0";

    strengthInputActive = false;
    manaInputActive = false;
    healthInputActive = false;
}
