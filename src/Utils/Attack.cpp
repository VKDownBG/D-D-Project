//
// Created by Lenovo on 20.5.2025 г.
//

#include "C:/DandD/include/Utils/Attack.h"
#include <random>
#include <iostream>

Attack::Attack() : isPlayerTurn(true) {
}


double Attack::calculateWeaponAttack(const Entity &attacker, const Entity &defender) {
    float baseAttack = attacker.GetStrength();

    float weaponBonus = 0.0f;
    //if (attacker.hasWeapon()) {
    weaponBonus = attacker.GetWeaponBonus();
    //}

    float totalAttack = baseAttack * (1.0f + weaponBonus);

    float armorReduction = 0.0f;
    if (defender.hasArmor()) {
        armorReduction = defender.GetArmorReduction();
    }

    float finalAttack = totalAttack * (1.0f - armorReduction);

    return finalAttack;
}

double Attack::calculateSpellAttack(const Entity &attacker, const Entity &defender) {
    float baseAttack = attacker.GetMana();

    float spellBonus = 0.0f;
    //if (attacker.hasSpell()) {
    spellBonus = attacker.GetSpellBonus();
    // }

    float totalAttack = baseAttack * (1.0f + spellBonus);

    float armorReduction = 0.0f;
    if (defender.hasArmor()) {
        armorReduction = defender.GetArmorReduction();
    }

    float finalAttack = totalAttack * (1.0f - armorReduction);

    return finalAttack;
}

double Attack::performAttack(const Entity &attacker, Entity &defender, const AttackType type) {
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
            std::cout << "Unknown typ of attack!" << std::endl;
            return 0;
    }

    if (criticalHit) {
        double bonusDamage = 0;
        bonusDamage = static_cast<int>(damage * 0.5f);
        damage += bonusDamage;
        displayCriticalHitMessage(bonusDamage, defender);
    }

    if (type == AttackType::WEAPON) {
        std::cout << attacker.GetName() << " attacks " << defender.GetName()
                << " with a weapon for " << damage << " damage!" << std::endl;
    } else {
        std::cout << attacker.GetName() << " used a spell on " << defender.GetName()
                << " for " << damage << " damage!" << std::endl;
    }

    defender.takeDamage(damage);

    std::cout << defender.GetName() << " has " << defender.GetHealth() << " health left." << std::endl;

    return damage;
}

bool Attack::isCriticalHit() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 100);
    int roll = dis(gen);

    return (roll <= 25);
}

bool Attack::simulateBattle(Hero &player, Monster &monster) {
    displayBattleStartMessage(player, monster);

    bool isPlayerTurn = true;

    while (!player.isDefeated() && !monster.isDefeated()) {
        displayTurnMessage(isPlayerTurn ? static_cast<Entity &>(player) : static_cast<Entity &>(monster), isPlayerTurn);
    }

    if (isPlayerTurn) {
        int choice = player.chooseAttackType();
        AttackType playerAttackType = (choice == 1) ? AttackType::WEAPON : AttackType::SPELL;
        performAttack(player, monster, playerAttackType);
    } else {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 1);
        AttackType monsterAttackType = (dis(gen) == 0) ? AttackType::WEAPON : AttackType::SPELL;
        performAttack(monster, player, monsterAttackType);
    }

    isPlayerTurn = !isPlayerTurn;

    std::cout << "-------------------" << std::endl;

    // Определяме победителя
    bool playerWon = !player.isDefeated();

    // Показваме съобщение за края на битката
    if
    (playerWon) {
        displayBattleEndMessage(player, monster);
        rewardExperience(player, monster);
        player.restoreHealthAfterBattle();
    } else {
        displayBattleEndMessage(monster, player);
    }

    return
            playerWon;
}

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
    std::cout << "CRITICAL HIT! +" << std::endl;
}
