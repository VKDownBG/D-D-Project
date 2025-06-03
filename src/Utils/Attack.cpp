//
// Created by Lenovo on 20.5.2025 г.
//

#include "C:/DandD/include/Utils/Attack.h"
#include <random>

Attack::Attack() : isPlayerTurn(true) {
}

double Attack::calculateBaseAttack(const float baseStat, const float bonus, const float armorReduction) {
    const float total = baseStat * (1.0f + bonus / 100);

    return total * (1.0f - armorReduction / 100);
}


double Attack::calculateWeaponAttack(const Entity &attacker, const Entity &defender) {
    return calculateBaseAttack(attacker.GetStrength(), attacker.GetWeaponBonus(), defender.GetArmorReduction());
}

double Attack::calculateSpellAttack(const Entity &attacker, const Entity &defender) {
    return calculateBaseAttack(attacker.GetStrength(), attacker.GetSpellBonus(), defender.GetArmorReduction());
}

double Attack::performAttack(const Entity &attacker, Entity &defender, AttackType type) {
    double damage = 0.0f;
    const bool criticalHit = isCriticalHit();

    switch (type) {
        case AttackType::WEAPON:
            damage = calculateWeaponAttack(attacker, defender);
            break;

        case AttackType::SPELL:
            damage = calculateSpellAttack(attacker, defender);
            break;

        default:
            return 0;
    }

    if (criticalHit) {
        const double bonusDamage = static_cast<int>(damage * 0.5f);
        damage += bonusDamage;
    }

    defender.takeDamage(damage);

    return damage;
}

bool Attack::isCriticalHit() const {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 100);
    const int roll = dis(gen);

    return (roll <= CRITICAL_HIT_CHANCE);
}

bool Attack::checkCriticalHit() {
    return isCriticalHit();
}

std::string Attack::getAttackName(const Entity &attacker, const AttackType type) {
    if (type == AttackType::WEAPON) {
        return "Claw Attack";
    } else {
        return "Fire Breath";
    }
}

void Attack::rewardExperience(Hero &player, const Monster &monster) {
    const int expGained = static_cast<int>(monster.GetStrength() * 5 + monster.GetLevel() * 20);

    player.addXP(expGained);
}
