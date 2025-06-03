//
// Created by Lenovo on 3.6.2025 г..
//

#include "C:/DandD/include/UI/panels/DefeatPanel.h"

// Shows the defeat panel on the screen.
void DefeatPanel::Show() {
    isActive = true; // Set the panel to active, making it visible and interactive.
    quitSelected = false; // Reset the quit flag.

    const int screenWidth = GetScreenWidth();
    const int screenHeight = GetScreenHeight();

    // Initialize the "QUIT" button with its position, size, text, and callback.
    quitButton = Button(
        {static_cast<float>(screenWidth / 2 - 110), static_cast<float>(screenHeight / 2 + 120), 200, 70},
        "QUIT",
        // Lambda function as a callback: when the button is clicked, close the window.
        [this]() { CloseWindow(); });
    quitButton.Draw(); // Initial draw call for the button (might be redundant if Update/Draw loop is immediate).
    quitButton.SetColors({
                             85, 0, 0, 255 // Normal color (dark red)
                         }, {75, 0, 0, 255}, // Hover color (slightly darker red)
                         LIGHTGRAY, // Pressed color
                         WHITE); // Text color
    quitButton.SetRoundedCorners(4.0f); // Set rounded corners for the button.
    quitButton.SetActive(true); // Ensure the button is interactive.
}

// Updates the state of the defeat panel.
void DefeatPanel::Update() {
    if (!isActive) return; // If the panel is not active, do nothing.

    // Update the quit button's state based on mouse interaction.
    quitButton.Update(GetMousePosition());
}

// Draws the defeat panel on the screen.
void DefeatPanel::Draw() const {
    if (!isActive) return; // If the panel is not active, do not draw.

    // Draw a semi-transparent black overlay over the entire screen to dim the background.
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), {0, 0, 0, 100});

    // Define and draw the "DEFEAT" text.
    const char *text = "DEFEAT";
    const int fontSize = 250;
    const int textWidth = MeasureText(text, fontSize); // Calculate text width for centering.
    DrawTextEx(GetFontDefault(), text,
               {
                   static_cast<float>(GetScreenWidth() / 2 - 450), // X position to center the text.
                   static_cast<float>(GetScreenHeight() / 2 - 150), // Y position.
               }, fontSize, 2,
               {85, 0, 0, 255}); // Color of the defeat text (dark red).

    // Draw the quit button.
    quitButton.Draw();
}

// Checks if the player has selected to quit.
bool DefeatPanel::ShouldQuit() const {
    return quitSelected; // Returns true if the quit button was pressed.
}
