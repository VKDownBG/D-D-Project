#include "C:/DandD/include/UI/screens/MainMenu.h"
#include <iostream>
#include <string>
#include <sstream>

// Constructor for the MainMenu class, initializes screen dimensions and menu states.
MainMenu::MainMenu(const int screenWidth, const int screenHeight)
    : screenWidth(screenWidth), screenHeight(screenHeight),
      currentState(MAIN), // Sets the initial state of the menu to MAIN.
      quitSelected(false),
      startGameSelected(false),
      loadGameSelected(false) {
    // Define various colors used for the UI elements.
    frameColor = {40, 40, 70, 230};
    textColor = {220, 220, 250, 255};
    darkPurple = {30, 20, 50, 255};
    mysticBlue = {60, 80, 120, 255};
    deepRed = {150, 30, 50, 255};
    stoneGray = {80, 80, 90, 200};
    goldAccent = {220, 190, 100, 255};

    // Initialize textures with default empty values.
    menuFont = GetFontDefault();
    menuFrameTexture = {0};
    backgroundTexture = {0};
    titleTexture = {0};

    // Create the menu buttons.
    CreateButtons();
}

// Destructor for the MainMenu class, responsible for unloading resources.
MainMenu::~MainMenu() {
    Unload();
}

// Checks if the "Quit Game" option has been selected.
bool MainMenu::IsQuitSelected() const {
    return quitSelected;
}

// Checks if the "Start Game" option has been selected.
bool MainMenu::IsStartGameSelected() const {
    return startGameSelected;
}

// Checks if the "Load Game" option has been selected.
bool MainMenu::IsLoadGameSelected() const {
    return loadGameSelected;
}

// Resets all menu selections to false.
void MainMenu::ResetSelections() {
    quitSelected = false;
    startGameSelected = false;
    loadGameSelected = false;
}

// Initializes the MainMenu by resetting selections and setting the state to MAIN.
void MainMenu::Initialize() {
    ResetSelections();
    currentState = MAIN;
}

// Loads necessary resources like fonts and textures for the MainMenu.
void MainMenu::LoadResources() {
    // Attempt to load a custom font. If it fails, fall back to the default font.
    menuFont = LoadFont("C:/DandD/assets/fonts/menu.ttf");
    if (menuFont.texture.id == 0) {
        std::cout << "Warning: Could not load custom font, using default font" << std::endl;
        menuFont = GetFontDefault();
    }

    // Attempt to load menu frame texture with error handling.
    menuFrameTexture = LoadTexture("C:/DandD/assets/border/testFrame.png");
    if (menuFrameTexture.id == 0) {
        std::cout << "Warning: Could not load menu frame texture" << std::endl;
    }

    // Attempt to load background texture with error handling.
    backgroundTexture = LoadTexture("C:/DandD/assets/background/walls4.png");
    if (backgroundTexture.id == 0) {
        std::cout << "Warning: Could not load background texture" << std::endl;
    }

    // Attempt to load title texture with error handling.
    titleTexture = LoadTexture("C:/DandD/assets/ui/title.png");
    if (titleTexture.id == 0) {
        std::cout << "Warning: Could not load title texture" << std::endl;
    }

    // Set the loaded font for all interactive buttons.
    startButton.SetFont(menuFont);
    loadButton.SetFont(menuFont);
    optionsButton.SetFont(menuFont);
    creditsButton.SetFont(menuFont);
    quitButton.SetFont(menuFont);
    backButton.SetFont(menuFont);

    std::cout << "MainMenu resources loaded" << std::endl;
}

// Creates and configures all buttons for the main menu and sub-menus.
void MainMenu::CreateButtons() {
    // Initialize main menu buttons with their bounds, text, and click actions (lambdas).
    startButton = Button(CalculateButtonBounds(0, 5), "Start Game", [this]() { this->startGameSelected = true; });
    loadButton = Button(CalculateButtonBounds(1, 5), "Load Game", [this]() { this->loadGameSelected = true; });
    optionsButton = Button(CalculateButtonBounds(2, 5), "Options", [this]() { this->currentState = OPTIONS; });
    creditsButton = Button(CalculateButtonBounds(3, 5), "Credits", [this]() { this->currentState = CREDITS; });
    quitButton = Button(CalculateButtonBounds(4, 5), "Quit Game", [this]() { this->quitSelected = true; });

    // Initialize the back button for sub-menus.
    backButton = Button({screenWidth / 2.0f - 100, screenHeight - 100.0f, 200, 50}, "Back",
                        [this]() { this->currentState = MAIN; });

    // Define button colors and styling parameters.
    const Color buttonColor = {60, 60, 85, 255};
    const Color hoverColor = {75, 75, 110, 255};
    const Color pressColor = {90, 90, 130, 255};
    const Color borderColor = {110, 110, 170, 255};

    // Lambda function to apply a consistent style to buttons.
    auto styleButton = [&](Button &btn) {
        btn.SetColors(buttonColor, hoverColor, pressColor, textColor);
        btn.SetBorder(borderColor, 2);
        btn.EnableHoverAnimation(true, 1.05f, 10.0f);
        btn.SetRoundedCorners(0.3f);
    };

    // Apply the defined style to all buttons.
    styleButton(startButton);
    styleButton(loadButton);
    styleButton(optionsButton);
    styleButton(creditsButton);
    styleButton(quitButton);
    styleButton(backButton);
}

// Updates the state of the menu and its buttons based on delta time and mouse input.
void MainMenu::Update(float deltaTime) {
    // Check if the screen dimensions have changed and recreate buttons if necessary.
    const int newWidth = GetScreenWidth();
    const int newHeight = GetScreenHeight();
    if (newWidth != screenWidth || newHeight != screenHeight) {
        screenWidth = newWidth;
        screenHeight = newHeight;
        CreateButtons(); // Recreates buttons to adjust to new screen size.
    }

    Vector2 mousePos = GetMousePosition(); // Get current mouse position.

    // Update buttons based on the current menu state.
    switch (currentState) {
        case MAIN:
            startButton.Update(mousePos);
            loadButton.Update(mousePos);
            optionsButton.Update(mousePos);
            creditsButton.Update(mousePos);
            quitButton.Update(mousePos);
            break;

        case OPTIONS:
            backButton.Update(mousePos);
            break;

        case CREDITS:
            backButton.Update(mousePos);
            break;
    }
}

// Placeholder for handling input specific to the main menu.
void MainMenu::HandleMainMenuInput() {
}

// Placeholder for handling input specific to the options menu.
void MainMenu::HandleOptionsMenuInput() {
}

// Placeholder for handling input specific to the credits menu.
void MainMenu::HandleCreditsMenuInput() {
}

// Draws all elements of the current menu state.
void MainMenu::Draw() const {
    DrawBackground(); // Draw the background.
    DrawFrame(); // Draw the decorative frame.
    DrawTitle(); // Draw the game title.

    // Draw specific menu content based on the current state.
    switch (currentState) {
        case MAIN:
            DrawMainMenu();
            break;

        case OPTIONS:
            DrawOptionsMenu();
            break;

        case CREDITS:
            DrawCreditsMenu();
            break;
    }
}

// Draws the background of the main menu.
void MainMenu::DrawBackground() const {
    if (backgroundTexture.id > 0) {
        // Check if background texture was successfully loaded.
        const Rectangle backgroundRect = {0, 0, static_cast<float>(screenWidth), static_cast<float>(screenHeight)};
        DrawTexturePro(
            backgroundTexture,
            {0, 0, static_cast<float>(backgroundTexture.width), static_cast<float>(backgroundTexture.height)},
            // Source rectangle for the whole texture.
            backgroundRect, // Destination rectangle to cover the whole screen.
            {0, 0}, // Origin for texture drawing.
            0.0f, // Rotation angle.
            WHITE // Tint color.
        );
    } else {
        // Fallback to a gradient background if the texture failed to load.
        DrawRectangleGradientV(0, 0, screenWidth, screenHeight, darkPurple, ColorAlpha(mysticBlue, 0.6f));
    }
}

// Draws the decorative frames and borders of the main menu.
void MainMenu::DrawFrame() const {
    // Draw outer screen borders with varying thickness and opacity for a layered effect.
    DrawRectangleLinesEx({0, 0, static_cast<float>(screenWidth), static_cast<float>(screenHeight - 2)}, 2, stoneGray);
    DrawRectangleLinesEx({0, 1, static_cast<float>(screenWidth), static_cast<float>(screenHeight - 1)}, 3,
                         ColorAlpha(mysticBlue, 0.6f));
    DrawRectangleLinesEx({0, 2, static_cast<float>(screenWidth), static_cast<float>(screenHeight)}, 4,
                         ColorAlpha(mysticBlue, 0.3f));

    // Draw the frame behind the title area.
    const Rectangle titleFrameRect = {0, 0, static_cast<float>(screenWidth), 120};
    DrawRectangleRec(titleFrameRect, ColorAlpha(frameColor, 0.9f));

    // Draw separation lines below the title frame with layered effects.
    DrawLine(0, 120, screenWidth, 120, stoneGray);
    DrawLineEx({0, 118}, {static_cast<float>(screenWidth), 118}, 2, ColorAlpha(mysticBlue, 0.6f));
    DrawLineEx({0, 119}, {static_cast<float>(screenWidth), 119}, 3, ColorAlpha(mysticBlue, 0.3f));

    // Draw the main content frame in the center of the screen.
    const Rectangle contentFrameRect = {screenWidth / 2.0f - 300, screenHeight / 2.0f - 200, 600, 410};
    DrawRectangleRec(contentFrameRect, ColorAlpha(darkPurple, 0.8f));

    // Draw borders for the content frame with layered effects.
    DrawRectangleLinesEx(contentFrameRect, 1, stoneGray);
    DrawRectangleLinesEx({
                             contentFrameRect.x + 1, contentFrameRect.y + 1,
                             contentFrameRect.width - 2, contentFrameRect.height - 2
                         }, 1,
                         ColorAlpha(mysticBlue, 0.6f));
    DrawRectangleLinesEx({
                             contentFrameRect.x + 2, contentFrameRect.y + 2,
                             contentFrameRect.width - 4, contentFrameRect.height - 4
                         }, 1,
                         ColorAlpha(mysticBlue, 0.3f));
}

// Draws the game title at the top of the screen.
void MainMenu::DrawTitle() const {
    const int fontSize = 50;
    const std::string title = "Dungeons & Dragons";

    if (titleTexture.id > 0) {
        // If title texture loaded, draw it.
        const float titleWidth = static_cast<float>(titleTexture.width);
        const float titleHeight = static_cast<float>(titleTexture.height);
        const float scale = 0.5f; // Scale down the texture.
        const float scaledWidth = titleWidth * scale;
        const float scaledHeight = titleHeight * scale;

        DrawTexturePro(
            titleTexture,
            {0, 0, titleWidth, titleHeight},
            {screenWidth / 2.0f - scaledWidth / 2.0f, 30, scaledWidth, scaledHeight}, // Centered horizontally.
            {0, 0},
            0.0f,
            WHITE
        );
    } else {
        // Fallback to drawing text if the title texture failed to load.
        const Vector2 titleSize = MeasureTextEx(menuFont, title.c_str(), fontSize, 1);
        const float titleX = screenWidth / 2.0f - titleSize.x / 2.0f;

        // Draw the title text with a slight offset for a glow effect.
        DrawTextEx(menuFont, title.c_str(), {titleX, 35}, fontSize, 1, goldAccent);
        DrawTextEx(menuFont, title.c_str(), {titleX - 2, 37}, fontSize, 1,
                   {goldAccent.r, goldAccent.g, goldAccent.b, 100});
    }
}

// Draws the main menu buttons and version information.
void MainMenu::DrawMainMenu() const {
    startButton.Draw();
    loadButton.Draw();
    optionsButton.Draw();
    creditsButton.Draw();
    quitButton.Draw();

    // Draw the game version number at the bottom right.
    const char *version = "Version 1.0.0";
    Vector2 versionSize = MeasureTextEx(menuFont, version, 16, 1);
    DrawTextEx(menuFont, version, {screenWidth - versionSize.x - 10, screenHeight - versionSize.y - 10},
               16, 1, ColorAlpha(textColor, 0.7f));
}

// Draws the options menu content (currently only the back button).
void MainMenu::DrawOptionsMenu() const {
    backButton.Draw();
}

// Draws the credits menu content.
void MainMenu::DrawCreditsMenu() const {
    const int fontSize = 24;
    const int titleFontSize = 30;
    const float startX = screenWidth / 2.0f - 250; // Starting X for credits text.
    const float startY = screenHeight / 2.0f - 150; // Starting Y for credits text.
    const float lineSpacing = 30; // Vertical spacing between lines of text.

    // Draw the "CREDITS" title.
    DrawTextEx(menuFont, "CREDITS",
               {
                   screenWidth / 2.0f - MeasureTextEx(menuFont, "CREDITS", titleFontSize, 1).x / 2.0f,
                   startY - lineSpacing
               },
               titleFontSize, 1, goldAccent);

    // Draw a horizontal line below the credits title.
    DrawLineEx({startX, startY}, {startX + 500, startY}, 2, ColorAlpha(mysticBlue, 0.6f));

    // Array of credit lines.
    const char *credits[] = {
        "Game Design:",
        "John Doe",
        "", // Empty string for extra spacing.
        "Programming:",
        "Jane Smith",
        "",
        "Art & Graphics:",
        "Bob Johnson",
        "",
        "Special Thanks:",
        "The Raylib Community"
    };

    // Loop through credits array and draw each line, applying different styles.
    for (int i = 0; i < 11; i++) {
        // Alternate colors for categories and names.
        const Color color = i % 3 == 0 ? goldAccent : textColor;
        // Alternate font sizes for categories and names.
        const int fontSz = i % 3 == 0 ? fontSize : fontSize - 4;
        // Indent names slightly.
        const float indent = i % 3 == 0 ? 0 : 20;

        DrawTextEx(menuFont, credits[i],
                   {startX + indent, startY + lineSpacing * (i + 1)},
                   fontSz, 1, color);
    }

    backButton.Draw(); // Draw the back button.
}

// Calculates the bounding rectangle for a button based on its index and total number of buttons.
Rectangle MainMenu::CalculateButtonBounds(const int buttonIndex, const int totalButtons) const {
    const float buttonWidth = 300;
    const float buttonHeight = 50;
    const float spacing = 20;

    // Define the content area within which buttons will be placed.
    const Rectangle contentArea = {screenWidth / 2.0f - 300, screenHeight / 2.0f - 200, 600, 400};
    // Calculate the total height occupied by all buttons and their spacing.
    const float totalHeight = buttonHeight * totalButtons + spacing * (totalButtons - 1);
    // Calculate the starting Y position to center the block of buttons vertically within the content area.
    const float startY = contentArea.y + (contentArea.height - totalHeight) / 2.0f;

    return {
        contentArea.x + (contentArea.width - buttonWidth) / 2.0f, // Center button horizontally.
        startY + (buttonHeight + spacing) * buttonIndex, // Position button vertically based on its index.
        buttonWidth,
        buttonHeight
    };
}

// Unloads all loaded resources to prevent memory leaks.
void MainMenu::Unload() {
    // Unload font only if it's not the default font (which is not loaded/unloaded by the user).
    if (menuFont.texture.id != GetFontDefault().texture.id) {
        UnloadFont(menuFont);
    }
    // Unload textures only if their ID is greater than 0, indicating they were loaded.
    if (menuFrameTexture.id > 0) {
        UnloadTexture(menuFrameTexture);
    }
    if (backgroundTexture.id > 0) {
        UnloadTexture(backgroundTexture);
    }
    if (titleTexture.id > 0) {
        UnloadTexture(titleTexture);
    }
}
