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

    double performAttack(const Entity &attacker, Entity &defender, AttackType type);

    void rewardExperience(Hero &player, const Monster &monster);

    std::string getAttackName(const Entity &attacker, AttackType type);

    bool checkCriticalHit();

private:
    const int CRITICAL_HIT_CHANCE = 25;

    bool isPlayerTurn;

private:
    double calculateBaseAttack(float baseStat, float bonus, float armorReduction);

    double calculateWeaponAttack(const Entity &attacker, const Entity &defender);

    double calculateSpellAttack(const Entity &attacker, const Entity &defender);

    bool isCriticalHit() const;
};

#endif //ATTACK_H
