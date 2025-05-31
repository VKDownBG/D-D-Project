//
// Created by Lenovo on 4.5.2025 г..
//

#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "C:/DandD/include/UI/managers/UIManager.h"
#include "C:/DandD/include/Entities/Hero.h"
#include "C:/DandD/include/Core/MapSystem.h"
#include "C:/DandD/include/Utils/Attack.h"
#include "C:/DandD/include/Utils/ProbabilitySystem.h"
#include <raylib.h>

class GameManager {
public:
    GameManager(int screenWidth, int screenHeight);

    ~GameManager();

    void RunGame();

private:
    void InitializeSystems();

    void ProcessInput();

    void Update(float deltaTime) const;

    void Render() const;

    void HandleMovement();

    void HandleCombatTrigger();

    void HandleTreasureCollection();

    void LoadCurrentLevel() const;

    void TransitionToNextLevel() const;

    void PositionHeroAtStart() const;

    const int screenWidth;
    const int screenHeight;
    bool isRunning;

    UIManager *uiManager;
    Hero *hero;
    Map *currentMap;
    Attack *attackSystem;
    Monster *currentMonster;
};

#endif
