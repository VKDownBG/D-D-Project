#ifndef BATTLESYSTEM_H
#define BATTLESYSTEM_H

#include "C:/DandD/include/Entities/Hero.h"
#include "C:/DandD/include/Entities/Monster.h"
#include "C:/DandD/include/Utils/Attack.h"
#include "C:/DandD/include/Utils/Position.h"
#include "C:/DandD/include/Core/MapSystem.h"
#include <functional>

enum class BattleResult {
    ONGOING,
    PLAYER_WON,
    PLAYER_LOST,
    PLAYER_FLED
};

class BattleSystem {
public:
    BattleSystem();

    ~BattleSystem();

    // Battle management
    bool CheckForBattle(Hero *player, const Position &newPosition);

    void StartBattle(Hero *player, Monster *monster);

    void EndBattle(BattleResult result);

    // Battle actions
    double PerformPlayerAttack(AttackType attackType);

    double PerformMonsterAttack();

    bool CanPlayerFlee() const;

    void PlayerFlee();

    // Battle state queries
    bool IsBattleActive() const;

    Hero *GetCurrentPlayer() const { return currentPlayer; }
    Monster *GetCurrentMonster() const { return currentMonster; }

    // Setup methods
    void SetMap(Map *map);

    void SetBattleEndCallback(const std::function<void(BattleResult)> &callback);

private:
    static constexpr double HEALTH_RESTORE_PERCENTAGE = 0.3;

    Attack *attackSystem;
    Map *gameMap;
    Hero *currentPlayer;
    Monster *currentMonster;
    double playerHealthBeforeBattle;
    bool battleActive;
    std::function<void(BattleResult)> onBattleEnd;

private:
    Monster *GetMonsterAtPosition(const Position &pos) const;

    bool DetermineFirstTurn() const;
};

#endif //BATTLESYSTEM_H
