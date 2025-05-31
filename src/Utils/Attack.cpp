//
// Created by Lenovo on 20.5.2025 г.
//

#include "C:/DandD/include/Utils/Attack.h"
#include <random>
#include <iostream>

Attack::Attack() : isPlayerTurn(true) {
}

double Attack::calculateBaseAttack(float baseStat, float bonus, float armorReduction) {
    float total = baseStat * (1.0f + bonus / 100);

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
        double bonusDamage = static_cast<int>(damage * 0.5f);
        damage += bonusDamage;
    }

    defender.takeDamage(damage);

    return damage;
}

bool Attack::isCriticalHit() const {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 100);
    int roll = dis(gen);

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

// bool Attack::simulateBattle(Hero &player, Monster &monster) {
//     displayBattleStartMessage(player, monster);
//
//     bool isPlayerTurn = true;
//
//     while (!player.isDefeated() && !monster.isDefeated()) {
//         displayTurnMessage(isPlayerTurn ? static_cast<Entity &>(player) : static_cast<Entity &>(monster), isPlayerTurn);
//     }
//
//     if (isPlayerTurn) {
//         const int choice = player.chooseAttackType();
//         const AttackType playerAttackType = (choice == 1) ? AttackType::WEAPON : AttackType::SPELL;
//         performAttack(player, monster, playerAttackType);
//     } else {
//         std::random_device rd;
//         std::mt19937 gen(rd());
//         std::uniform_int_distribution<> dis(0, 1);
//         AttackType monsterAttackType = (dis(gen) == 0) ? AttackType::WEAPON : AttackType::SPELL;
//         performAttack(monster, player, monsterAttackType);
//     }
//
//     isPlayerTurn = !isPlayerTurn;
//
//     std::cout << "-------------------" << std::endl;
//
//     const bool playerWon = !player.isDefeated();
//
//     if (playerWon) {
//         displayBattleEndMessage(player, monster);
//         rewardExperience(player, monster);
//         player.restoreHealthAfterBattle();
//     } else {
//         displayBattleEndMessage(monster, player);
//     }
//
//     return playerWon;
// }

void Attack::rewardExperience(Hero &player, const Monster &monster) {
    int expGained = static_cast<int>(monster.GetStrength() * 5 + monster.GetLevel() * 20);

    std::cout << player.GetName() << " gets " << expGained << " XP!" << std::endl;

    player.addXP(expGained);
}

void Attack::displayBattleStartMessage(const Entity &player, const Entity &monster) {
    std::cout << "\n====== BATTLE START ======" << std::endl;
    std::cout << player.GetName() << " [Health: " << player.GetHealth() << "] vs "
            << monster.GetName() << " [Health: " << monster.GetHealth() << "]" << std::endl;
    std::cout << "===============================" << std::endl;
}

void Attack::displayTurnMessage(const Entity &entity, const bool isPlayerTurn) {
    std::cout << "\n" << (isPlayerTurn ? "➤ Hero's turn" : "➤ Monster's turn") << std::endl;
    std::cout << entity.GetName() << "is preparing for an attack..." << std::endl;
}

void Attack::displayBattleEndMessage(const Entity &winner, const Entity &loser) {
    std::cout << "\n====== END OF BATTLE ======" << std::endl;
    std::cout << winner.GetName() << " defeats " << loser.GetName() << "!" << std::endl;
    std::cout << "============================" << std::endl;
}

void Attack::displayCriticalHitMessage(double bonusDamage, const Entity &target) {
    std::cout << "CRITICAL HIT! +" << bonusDamage << " bonus damage!" << std::endl;
}
