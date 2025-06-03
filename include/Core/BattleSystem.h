//
// Created by Lenovo on 4.5.2025 г.
//

#ifndef BATTLESYSTEM_H
#define BATTLESYSTEM_H

#include "C:/DandD/include/Entities/Hero.h"
#include "C:/DandD/include/Entities/Monster.h"
#include "C:/DandD/include/Utils/Attack.h"
#include "C:/DandD/include/UI/panels/BattlePanel.h"
#include "C:/DandD/include/Core/MapSystem.h"
#include "C:/DandD/include/Utils/Position.h"
#include <functional>

const double LEVEL_XP_MULTIPLIER = 1.5;
const int BASE_XP_REWARD = 100;
const double HEALTH_RESTORE_PERCENTAGE = 0.5;

class BattleSystem {
public:
    BattleSystem();

    ~BattleSystem();

    void Update();

    void Draw() const;

    bool CheckForBattle(Hero *player, const Position &newPosition);

    void StartBattle(Hero *player, Monster *monster);

    bool IsBattleActive() const;

    bool IsBattleFinished() const;

    BattleResult GetBattleResult() const;

    void SetFont(const Font &font);

    void SetMap(Map *map);

    void SetBattleEndCallback(const std::function<void(BattleResult)> &callback);

    bool determineFirstTurn() const;

private:
    void HandleBattleEnd();

    Monster *GetMonsterAtPosition(const Position &pos) const;

    BattlePanel *battlePanel;
    Attack *attackSystem;
    Map *gameMap;

    Hero *currentPlayer;
    Monster *currentMonster;
    double playerHealthBeforeBattle;

    bool battleActive;
    bool battleInitialized;

    std::function<void(BattleResult)> onBattleEnd;
};

#endif //BATTLESYSTEM_H
