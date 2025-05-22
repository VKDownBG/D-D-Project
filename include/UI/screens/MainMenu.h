//
// Created by Lenovo on 15.5.2025 г..
//

#ifndef MAINMENU_H
#define MAINMENU_H

#include "raylib.h"
#include <vector>
#include <functional>
#include "C:/DandD/include/UI/widgets/Button.h"

class MainMenu {
public:
    enum MenuState {
        MAIN,
        OPTIONS,
        CREDITS
    };

    MainMenu(int _screenWidth, int _screenHeight);

    ~MainMenu();

    void Initialize();

    void LoadResources();

    void Update(float deltaTime);

    void Draw() const;

    void Unload();

    bool IsQuitSelected() const;

    bool IsStartGameSelected() const;

    bool IsLoadGameSelected() const;

    void ResetSelections();

private:
    int screenWidth;
    int screenHeight;

    Button startButton;
    Button loadButton;
    Button optionsButton;
    Button creditsButton;
    Button quitButton;
    Button backButton;

    MenuState currentState;

    bool quitSelected;
    bool startGameSelected;
    bool loadGameSelected;

    Font menuFont{};
    Texture2D backgroundTexture{};
    Texture2D titleTexture{};
    Texture2D menuFrameTexture{};

    Color frameColor{};
    Color textColor{};
    Color darkPurple{};
    Color mysticBlue{};
    Color deepRed{};
    Color stoneGray{};
    Color goldAccent{};

    void DrawBackground() const;

    void DrawTitle() const;

    void DrawMainMenu() const;

    void DrawOptionsMenu() const;

    void DrawCreditsMenu() const;

    void DrawFrame() const;

    Rectangle CalculateButtonBounds(int buttonIndex, int totalButtons) const;

    void CreateButtons();

    void HandleMainMenuInput();

    void HandleOptionsMenuInput();

    void HandleCreditsMenuInput();
};

#endif //MAINMENU_H
