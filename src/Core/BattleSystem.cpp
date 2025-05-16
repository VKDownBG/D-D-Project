//
// Created by Lenovo on 7.5.2025 г.
//

#include "Core/BattleSystem.h"

BattleSystem::BattleSystem(Hero &hero, Monster &monster): m_hero(hero), m_monster(monster) {
}

void BattleSystem::startBattle() {
    Starts starter = chooseStart();

    switch (starter) {
        case Starts::HERO:
            playerTurn(); //button input тука
            break;

        case Starts::MONSTER:
            monsterTurn();
            break;
    };
}

bool BattleSystem::isBattleOver() const {
    return m_hero.isDefeated() || m_monster.isDefeated();
}
