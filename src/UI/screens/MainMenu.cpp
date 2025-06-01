#include "C:/DandD/include/UI/screens/MainMenu.h"
#include <iostream>
#include <string>
#include <sstream>

MainMenu::MainMenu(const int screenWidth, const int screenHeight)
    : screenWidth(screenWidth), screenHeight(screenHeight),
      currentState(MAIN),
      quitSelected(false),
      startGameSelected(false),
      loadGameSelected(false) {
    frameColor = {40, 40, 70, 230};
    textColor = {220, 220, 250, 255};
    darkPurple = {30, 20, 50, 255};
    mysticBlue = {60, 80, 120, 255};
    deepRed = {150, 30, 50, 255};
    stoneGray = {80, 80, 90, 200};
    goldAccent = {220, 190, 100, 255};

    // Initialize textures as empty
    menuFont = GetFontDefault();
    menuFrameTexture = {0};
    backgroundTexture = {0};
    titleTexture = {0};

    CreateButtons();
}

MainMenu::~MainMenu() {
    Unload();
}

bool MainMenu::IsQuitSelected() const {
    return quitSelected;
}

bool MainMenu::IsStartGameSelected() const {
    return startGameSelected;
}

bool MainMenu::IsLoadGameSelected() const {
    return loadGameSelected;
}

void MainMenu::ResetSelections() {
    quitSelected = false;
    startGameSelected = false;
    loadGameSelected = false;
}

void MainMenu::Initialize() {
    ResetSelections();
    currentState = MAIN;
}

void MainMenu::LoadResources() {
    // CRITICAL FIX: Proper error handling and fallbacks for resource loading

    // Try to load custom font, fall back to default if it fails
    menuFont = LoadFont("C:/DandD/assets/fonts/menu.ttf");
    if (menuFont.texture.id == 0) {
        std::cout << "Warning: Could not load custom font, using default font" << std::endl;
        menuFont = GetFontDefault();
    }

    // Try to load textures with proper error handling
    menuFrameTexture = LoadTexture("C:/DandD/assets/border/testFrame.png");
    if (menuFrameTexture.id == 0) {
        std::cout << "Warning: Could not load menu frame texture" << std::endl;
    }

    backgroundTexture = LoadTexture("C:/DandD/assets/background/walls4.png");
    if (backgroundTexture.id == 0) {
        std::cout << "Warning: Could not load background texture" << std::endl;
    }

    titleTexture = LoadTexture("C:/DandD/assets/ui/title.png");
    if (titleTexture.id == 0) {
        std::cout << "Warning: Could not load title texture" << std::endl;
    }

    // Set font for all buttons
    startButton.SetFont(menuFont);
    loadButton.SetFont(menuFont);
    optionsButton.SetFont(menuFont);
    creditsButton.SetFont(menuFont);
    quitButton.SetFont(menuFont);
    backButton.SetFont(menuFont);

    std::cout << "MainMenu resources loaded" << std::endl;
}

void MainMenu::CreateButtons() {
    startButton = Button(CalculateButtonBounds(0, 5), "Start Game", [this]() { this->startGameSelected = true; });
    loadButton = Button(CalculateButtonBounds(1, 5), "Load Game", [this]() { this->loadGameSelected = true; });
    optionsButton = Button(CalculateButtonBounds(2, 5), "Options", [this]() { this->currentState = OPTIONS; });
    creditsButton = Button(CalculateButtonBounds(3, 5), "Credits", [this]() { this->currentState = CREDITS; });
    quitButton = Button(CalculateButtonBounds(4, 5), "Quit Game", [this]() { this->quitSelected = true; });

    backButton = Button({screenWidth / 2.0f - 100, screenHeight - 100.0f, 200, 50}, "Back",
                        [this]() { this->currentState = MAIN; });

    const Color buttonColor = {60, 60, 85, 255};
    const Color hoverColor = {75, 75, 110, 255};
    const Color pressColor = {90, 90, 130, 255};
    const Color borderColor = {110, 110, 170, 255};

    auto styleButton = [&](Button &btn) {
        btn.SetColors(buttonColor, hoverColor, pressColor, textColor);
        btn.SetBorder(borderColor, 2);
        btn.EnableHoverAnimation(true, 1.05f, 10.0f);
        btn.SetRoundedCorners(0.3f);
    };

    styleButton(startButton);
    styleButton(loadButton);
    styleButton(optionsButton);
    styleButton(creditsButton);
    styleButton(quitButton);
    styleButton(backButton);
}

void MainMenu::Update(float deltaTime) {
    int newWidth = GetScreenWidth();
    int newHeight = GetScreenHeight();
    if (newWidth != screenWidth || newHeight != screenHeight) {
        screenWidth = newWidth;
        screenHeight = newHeight;
        CreateButtons();
    }

    Vector2 mousePos = GetMousePosition();

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

void MainMenu::HandleMainMenuInput() {
}

void MainMenu::HandleOptionsMenuInput() {
}

void MainMenu::HandleCreditsMenuInput() {
}

void MainMenu::Draw() const {
    DrawBackground();
    DrawFrame();
    DrawTitle();

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

void MainMenu::DrawBackground() const {
    // CRITICAL FIX: Always draw something, even if texture fails to load
    if (backgroundTexture.id > 0) {
        const Rectangle backgroundRect = {0, 0, static_cast<float>(screenWidth), static_cast<float>(screenHeight)};
        DrawTexturePro(
            backgroundTexture,
            {0, 0, static_cast<float>(backgroundTexture.width), static_cast<float>(backgroundTexture.height)},
            backgroundRect,
            {0, 0},
            0.0f,
            WHITE
        );
    } else {
        // Fallback gradient background
        DrawRectangleGradientV(0, 0, screenWidth, screenHeight, darkPurple, ColorAlpha(mysticBlue, 0.6f));
    }
}

void MainMenu::DrawFrame() const {
    // CRITICAL FIX: Always draw frame elements for visual structure
    DrawRectangleLinesEx({0, 0, static_cast<float>(screenWidth), static_cast<float>(screenHeight - 2)}, 2, stoneGray);
    DrawRectangleLinesEx({0, 1, static_cast<float>(screenWidth), static_cast<float>(screenHeight - 1)}, 3,
                         ColorAlpha(mysticBlue, 0.6f));
    DrawRectangleLinesEx({0, 2, static_cast<float>(screenWidth), static_cast<float>(screenHeight)}, 4,
                         ColorAlpha(mysticBlue, 0.3f));

    // Title frame
    Rectangle titleFrameRect = {0, 0, static_cast<float>(screenWidth), 120};
    DrawRectangleRec(titleFrameRect, ColorAlpha(frameColor, 0.9f));

    DrawLine(0, 120, screenWidth, 120, stoneGray);
    DrawLineEx({0, 118}, {static_cast<float>(screenWidth), 118}, 2, ColorAlpha(mysticBlue, 0.6f));
    DrawLineEx({0, 119}, {static_cast<float>(screenWidth), 119}, 3, ColorAlpha(mysticBlue, 0.3f));

    // Content frame
    Rectangle contentFrameRect = {screenWidth / 2.0f - 300, screenHeight / 2.0f - 200, 600, 400};
    DrawRectangleRec(contentFrameRect, ColorAlpha(darkPurple, 0.8f));

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

void MainMenu::DrawTitle() const {
    const int fontSize = 50;
    const char *title = "Dungeons & Dragons";

    if (titleTexture.id > 0) {
        float titleWidth = static_cast<float>(titleTexture.width);
        float titleHeight = static_cast<float>(titleTexture.height);
        float scale = 0.5f;
        float scaledWidth = titleWidth * scale;
        float scaledHeight = titleHeight * scale;

        DrawTexturePro(
            titleTexture,
            {0, 0, titleWidth, titleHeight},
            {screenWidth / 2.0f - scaledWidth / 2.0f, 30, scaledWidth, scaledHeight},
            {0, 0},
            0.0f,
            WHITE
        );
    } else {
        // CRITICAL FIX: Always draw title text, even if texture fails
        Vector2 titleSize = MeasureTextEx(menuFont, title, fontSize, 1);
        float titleX = screenWidth / 2.0f - titleSize.x / 2.0f;

        DrawTextEx(menuFont, title, {titleX, 35}, fontSize, 1, goldAccent);
        DrawTextEx(menuFont, title, {titleX - 2, 37}, fontSize, 1, {goldAccent.r, goldAccent.g, goldAccent.b, 100});
    }
}

void MainMenu::DrawMainMenu() const {
    startButton.Draw();
    loadButton.Draw();
    optionsButton.Draw();
    creditsButton.Draw();
    quitButton.Draw();

    const char *version = "Version 1.0.0";
    Vector2 versionSize = MeasureTextEx(menuFont, version, 16, 1);
    DrawTextEx(menuFont, version, {screenWidth - versionSize.x - 10, screenHeight - versionSize.y - 10},
               16, 1, ColorAlpha(textColor, 0.7f));
}

void MainMenu::DrawOptionsMenu() const {
    backButton.Draw();
}

void MainMenu::DrawCreditsMenu() const {
    const int fontSize = 24;
    const int titleFontSize = 30;
    float startX = screenWidth / 2.0f - 250;
    float startY = screenHeight / 2.0f - 150;
    float lineSpacing = 30;

    DrawTextEx(menuFont, "CREDITS",
               {
                   screenWidth / 2.0f - MeasureTextEx(menuFont, "CREDITS", titleFontSize, 1).x / 2.0f,
                   startY - lineSpacing
               },
               titleFontSize, 1, goldAccent);

    DrawLineEx({startX, startY}, {startX + 500, startY}, 2, ColorAlpha(mysticBlue, 0.6f));

    const char *credits[] = {
        "Game Design:",
        "John Doe",
        "",
        "Programming:",
        "Jane Smith",
        "",
        "Art & Graphics:",
        "Bob Johnson",
        "",
        "Special Thanks:",
        "The Raylib Community"
    };

    for (int i = 0; i < 11; i++) {
        const Color color = i % 3 == 0 ? goldAccent : textColor;
        const int fontSz = i % 3 == 0 ? fontSize : fontSize - 4;
        const float indent = i % 3 == 0 ? 0 : 20;

        DrawTextEx(menuFont, credits[i],
                   {startX + indent, startY + lineSpacing * (i + 1)},
                   fontSz, 1, color);
    }

    backButton.Draw();
}

Rectangle MainMenu::CalculateButtonBounds(const int buttonIndex, int totalButtons) const {
    const float buttonWidth = 300;
    const float buttonHeight = 50;
    const float spacing = 20;

    Rectangle contentArea = {screenWidth / 2.0f - 300, screenHeight / 2.0f - 200, 600, 400};
    float totalHeight = buttonHeight * totalButtons + spacing * (totalButtons - 1);
    float startY = contentArea.y + (contentArea.height - totalHeight) / 2.0f;

    return {
        contentArea.x + (contentArea.width - buttonWidth) / 2.0f,
        startY + (buttonHeight + spacing) * buttonIndex,
        buttonWidth,
        buttonHeight
    };
}

void MainMenu::Unload() {
    // CRITICAL FIX: Only unload resources that were successfully loaded
    if (menuFont.texture.id != GetFontDefault().texture.id) {
        UnloadFont(menuFont);
    }
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
