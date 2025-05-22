//
// Created by Lenovo on 4.5.2025 г.
//

#ifndef BATTLESYSTEM_H
#define BATTLESYSTEM_H

#include "../Entities/Hero.h"
#include "C:/DandD/include/Entities/Monster.h";
#include "C:/DandD/include/Utils/Attack.h";

enum class AttackType {
    WEAPON,
    SPELL,
    FLEE
};

enum class Starts {
    HERO,
    MONSTER
};

class BattleSystem {
public:
    void StartBattle(Hero *player, Monster *monster);

    void EndBattle();

    void ProcessPlayerTurn(AttackType playerChoise);

    void ProcessMonsterTurn();

    void CalculateDamage(bool isPlayerAtacking);

    bool IsBattleOver();

    bool isPlayerVictorious() const;

    Hero *GetCurrentPlayer() const;

    Monster *GetCurrentEnemy() const;

    int GetTurnNumber() const;

    void OnBattleEnded(bool isVictory);

private:
    Hero *player;
    Monster *enemy;
    int currentTurn;
    bool isPlayerTurn;

    void applyBonusesAndArmor(int &rawDamage, bool isPlayerAtacking);

    void handleVictory();

    void handleDefeat();
};

#endif //BATTLESYSTEM_H
