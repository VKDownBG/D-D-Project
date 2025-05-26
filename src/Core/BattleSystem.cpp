//
// Created by Lenovo on 7.5.2025 г.
//

#include "C:/DandD/include/Core/BattleSystem.h"
#include "C:/DandD/include/Utils/ProbabilitySystem.h"
#include <algorithm>
#include <cmath>

BattleSystem::BattleSystem()
    : battlePanel(new BattlePanel()),
      attackSystem(new Attack()),
      currentPlayer(nullptr),
      currentMonster(nullptr),
      playerHealthBeforeBattle(0.0),
      battleActive(false),
      battleInitialized(false),
      onBattleEnd(nullptr) {
}

BattleSystem::~BattleSystem() {
    delete battlePanel;
    delete attackSystem;
}

void BattleSystem::Update() {
    if (!battleActive && !battlePanel) {
        return;
    }

    battlePanel->Update();

    if (battlePanel->isFinished()) {
        HandleBattleEnd();
    }
}

void BattleSystem::Draw() const {
    if (battleActive && battlePanel) {
        battlePanel->Draw();
    }
}

bool BattleSystem::CheckForBattle(Hero *player, const Position &newPosition, Map *gameMap) {
    if (!player || !gameMap || battleActive) {
        return false;
    }

    Monster *monster = GetMonsterAtPosition(newPosition, gameMap);

    if (monster && !monster->isDefeated()) {
        StartBattle(player, monster);
        return true;
    }

    return false;
}

Monster *BattleSystem::GetMonsterAtPosition(const Position &pos, Map *gameMap) const {
    if (!gameMap) {
        return nullptr;
    }

    auto &monsters = gameMap->getMonsters();
    for (auto &monster: monsters) {
        if (monster.GetPosition().x == pos.x && monster.GetPosition().y == pos.y) {
            return &monster;
        }
    }

    return nullptr;
}

void BattleSystem::StartBattle(Hero *player, Monster *monster) {
    if (!player || !monster || battleActive) {
        return;
    }

    currentPlayer = player;
    currentMonster = monster;
    playerHealthBeforeBattle = player->GetHealth();

    battleActive = true;
    battleInitialized = false;

    battlePanel->StartBattle(player, monster, attackSystem);
}

bool BattleSystem::IsBattleActive() const {
    return battleActive;
}

bool BattleSystem::IsBattleFinished() const {
    return battlePanel->isFinished();
}

BattleResult BattleSystem::GetBattleResult() const {
    return battlePanel ? battlePanel->GetResult() : BattleResult::ONGOING;
}

void BattleSystem::HandleBattleEnd() {
    if (!battleActive || !currentPlayer) {
        return;
    }

    const BattleResult result = battlePanel->GetResult();

    if (result == BattleResult::PLAYER_WON) {
        const double healthToRestore = playerHealthBeforeBattle * HEALTH_RESTORE_PERCENTAGE;

        double newHealth = std::min(
            static_cast<double>(currentPlayer->GetMaxHealth()),
            currentPlayer->GetHealth() + healthToRestore
        );

        currentPlayer->SetHealth(static_cast<int>(newHealth));
    }

    if (onBattleEnd) {
        onBattleEnd(result);
    }

    battleActive = false;
    battleInitialized = false;
    currentPlayer = nullptr;
    currentMonster = nullptr;
    playerHealthBeforeBattle = 0.0;
}

bool BattleSystem::determineFirstTurn() const {
    return RandomUtils::randomValue<int>(0, 1) == 0;
}

void BattleSystem::SetFont(const Font &font) {
    if (battlePanel) {
        battlePanel->SetFont(font);
    }
}

void BattleSystem::SetBattleEndCallback(const std::function<void(BattleResult)> &callback) {
    onBattleEnd = callback;
}
