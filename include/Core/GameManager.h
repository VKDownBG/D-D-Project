//
// Created by Lenovo on 4.5.2025 г..
//

#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "C:/DandD/include/Core/MapSystem.h"
#include "C:/DandD/include/Entities/Hero.h"
#include "C:/DandD/include/Entities/Monster.h"
#include "C:/DandD/include/Entities/Treasure.h"
#include "C:/DandD/include/UI/screens/GameHUD.h"
#include "C:/DandD/include/Utils/Attack.h"
#include "C:/DandD/include/Utils/ProbabilitySystem.h"
#include "C:/DandD/include/UI/renderers/MapRenderer.h"
#include "C:/DandD/include/UI/screens/MainMenu.h"
#include <memory>
#include <vector>
#include <string>

enum class GameState {
    MAIN_MENU,
    PLAYING,
    BATTLE,
    LEVEL_COMPLETE,
    GAME_OVER,
    PAUSED
};

enum class MovementDirection {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

class GameManager {
public:
    GameManager(int screenWidth, int screenHeight);

    ~GameManager();

    void Initialize();

    void LoadResources();

    void Update(float deltaTime);

    void Draw();

    void HandleInput();

    bool shouldClose() const;

private:
    void StartNewGame();

    void LoadGame();

    void TransitionToBattle(Monster &monster);

    void HandleGameOver(bool playerWon);

    void Cleanup();

private:
    GameState currentState;

    MainMenu *mainMenu;
    Map *currentMap;
    Hero *player;
    GameHUD *gameHUD;
    MapRenderer *mapRenderer;

    int screenWidth, screenHeight;

    bool resourcesLoaded;
};

#endif //GAMEMANAGER_H
