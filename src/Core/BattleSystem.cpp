#include "C:/DandD/include/Core/BattleSystem.h"
#include "C:/DandD/include/Utils/ProbabilitySystem.h"
#include <algorithm>
#include <cmath>

BattleSystem::BattleSystem()
    : attackSystem(new Attack()),
      gameMap(nullptr),
      currentPlayer(nullptr),
      currentMonster(nullptr),
      playerHealthBeforeBattle(0.0),
      battleActive(false),
      onBattleEnd(nullptr) {
}

BattleSystem::~BattleSystem() {
    delete attackSystem;
}

bool BattleSystem::CheckForBattle(Hero *player, const Position &newPosition) {
    if (!player || !gameMap || battleActive) {
        return false;
    }

    Monster *monster = GetMonsterAtPosition(newPosition);
    if (monster && !monster->isDefeated()) {
        StartBattle(player, monster);
        return true;
    }

    return false;
}

void BattleSystem::StartBattle(Hero *player, Monster *monster) {
    if (!player || !monster || battleActive) {
        return;
    }

    currentPlayer = player;
    currentMonster = monster;
    playerHealthBeforeBattle = player->GetHealth();
    battleActive = true;
}

void BattleSystem::EndBattle(BattleResult result) {
    if (!battleActive) return;

    // Handle post-battle effects
    if (result == BattleResult::PLAYER_WON && currentPlayer) {
        // Restore partial health
        const double healthToRestore = playerHealthBeforeBattle * HEALTH_RESTORE_PERCENTAGE;
        double newHealth = std::min(
            static_cast<double>(currentPlayer->GetMaxHealth()),
            currentPlayer->GetHealth() + healthToRestore
        );
        currentPlayer->SetHealth(static_cast<int>(newHealth));

        // Award experience
        if (currentMonster) {
            // Assuming you have a method to award experience
            // currentPlayer->GainExperience(currentMonster->GetExperienceReward());
        }
    }

    // Execute callback before resetting state
    if (onBattleEnd) {
        onBattleEnd(result);
    }

    // Reset battle state
    battleActive = false;
    currentPlayer = nullptr;
    currentMonster = nullptr;
    playerHealthBeforeBattle = 0.0;
}

double BattleSystem::PerformPlayerAttack(AttackType attackType) {
    if (!battleActive || !currentPlayer || !currentMonster || !attackSystem) {
        return 0.0;
    }

    const double damage = attackSystem->performAttack(*currentPlayer, *currentMonster, attackType);

    // Check if monster is defeated
    if (currentMonster->isDefeated()) {
        EndBattle(BattleResult::PLAYER_WON);
    }

    return damage;
}

double BattleSystem::PerformMonsterAttack() {
    if (!battleActive || !currentPlayer || !currentMonster || !attackSystem) {
        return 0.0;
    }

    // Randomly choose monster attack type
    AttackType monsterAttackType = (RandomUtils::randomValue<int>(0, 1) == 0)
                                       ? AttackType::WEAPON
                                       : AttackType::SPELL;

    double damage = attackSystem->performAttack(*currentMonster, *currentPlayer, monsterAttackType);

    // Check if player is defeated
    if (currentPlayer->isDefeated()) {
        EndBattle(BattleResult::PLAYER_LOST);
    }

    return damage;
}

bool BattleSystem::CanPlayerFlee() const {
    // You can add logic here for flee success rate based on level differences, etc.
    return battleActive && currentPlayer && currentMonster;
}

void BattleSystem::PlayerFlee() {
    if (CanPlayerFlee()) {
        EndBattle(BattleResult::PLAYER_FLED);
    }
}

bool BattleSystem::IsBattleActive() const {
    return battleActive;
}

Monster *BattleSystem::GetMonsterAtPosition(const Position &pos) const {
    if (!gameMap) return nullptr;

    std::vector<Monster> &monsters = gameMap->getMonsters();
    for (auto &monster: monsters) {
        if (monster.GetPosition().x == pos.x && monster.GetPosition().y == pos.y) {
            return &monster;
        }
    }

    return nullptr;
}

void BattleSystem::SetMap(Map *map) {
    gameMap = map;
}

void BattleSystem::SetBattleEndCallback(const std::function<void(BattleResult)> &callback) {
    onBattleEnd = callback;
}

bool BattleSystem::DetermineFirstTurn() const {
    return RandomUtils::randomValue<int>(0, 1) == 0;
}
