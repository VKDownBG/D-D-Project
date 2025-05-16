//
// Created by Lenovo on 4.5.2025 г.
//

#ifndef BATTLESYSTEM_H
#define BATTLESYSTEM_H

#include "Entities/Hero/Hero.h";
#include "Entities/Monster.h";
#include "Utils/ProbabilitySystem.h";

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
private:
    Hero &m_hero;
    Monster &m_monster;

private:
    static Starts chooseStart() {
        return static_cast<Starts>(RandomUtils::randomValue<int>(0, 1));
    }

public:
    BattleSystem(Hero &hero, Monster &monster);

    void startBattle();

    void playerTurn(AttackType choise);

    void monsterTurn();

    bool isBattleOver() const;
};

#endif //BATTLESYSTEM_H
