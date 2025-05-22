//
// Created by Lenovo on 8.5.2025 г.
//

#ifndef ATTACK_H
#define ATTACK_H

#include "C:/DandD/include/Entities/Entity.h"
#include "C:/DandD/include/Entities/Hero.h"
#include "C:/DandD/include/Entities/Monster.h"

enum class AttackType {
    WEAPON,
    SPELL
};

class Attack {
public:
    Attack();

    ~Attack() = default;

    static double calculateWeaponAttack(const Entity &attacker, const Entity &defender);

    static double calculateSpellAttack(const Entity &attacker, const Entity &defender);

    static double performAttack(const Entity &attacker, Entity &defender, AttackType type);

    static bool isCriticalHit();

    static bool simulateBattle(Hero &player, Monster &monster);

    static void rewardExperience(Hero &player, const Monster &monster);

private:
    bool isPlayerTurn;

    static void displayBattleStartMessage(const Entity &player, const Entity &monster);

    static void displayTurnMessage(const Entity &entity, bool isPlayerTurn);

    static void displayBattleEndMessage(const Entity &winner, const Entity &loser);

    static void displayCriticalHitMessage(double bonusDamage, const Entity &target);
};

#endif //ATTACK_H
