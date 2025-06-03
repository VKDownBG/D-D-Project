//
// Created by Lenovo on 7.5.2025 г.
//

#include "C:/DandD/include/Core/BattleSystem.h"
#include "C:/DandD/include/Utils/ProbabilitySystem.h"
#include <algorithm>
#include <cmath>

// Constructor: Initializes battle components
BattleSystem::BattleSystem()
    : battlePanel(new BattlePanel()),
      attackSystem(new Attack()),
      gameMap(nullptr),
      currentPlayer(nullptr),
      currentMonster(nullptr),
      playerHealthBeforeBattle(0.0),
      battleActive(false),
      battleInitialized(false),
      onBattleEnd(nullptr) {
}

// Destructor: Cleans up allocated resources
BattleSystem::~BattleSystem() {
    delete battlePanel;
    delete attackSystem;
}

// Main update loop for battle system
void BattleSystem::Update() {
    // Early exit if no active battle or missing panel
    if (!battleActive && !battlePanel) {
        return;
    }

    if (!battlePanel) {
        throw std::runtime_error("BattlePanel is not initialized");
    }

    battlePanel->Update();

    // Handle battle conclusion when panel reports finished state
    if (battlePanel->isFinished()) {
        HandleBattleEnd();
    }
}

// Render battle interface
void BattleSystem::Draw() const {
    if (battleActive && battlePanel) {
        battlePanel->Draw();
    }
}

// Checks if player's movement triggers a battle
bool BattleSystem::CheckForBattle(Hero *player, const Position &newPosition) {
    // Validate preconditions
    if (!player || !gameMap || battleActive) {
        return false;
    }

    Monster *monster = GetMonsterAtPosition(newPosition);
    // Start battle if monster exists and is undefeated
    if (monster && !monster->isDefeated()) {
        StartBattle(player, monster);
        return true;
    }

    return false;
}

// Finds monster at given map position
Monster *BattleSystem::GetMonsterAtPosition(const Position &pos) const {
    if (!gameMap) return nullptr;

    std::vector<Monster> &monsters = gameMap->getMonsters();
    // Linear search through monster list
    for (auto &monster: monsters) {
        if (monster.GetPosition().x == pos.x && monster.GetPosition().y == pos.y) {
            return &monster;
        }
    }

    return nullptr;
}

// Initiates battle sequence
void BattleSystem::StartBattle(Hero *player, Monster *monster) {
    if (!player || !monster || battleActive) {
        return;
    }

    // Cache battle state
    currentPlayer = player;
    currentMonster = monster;
    playerHealthBeforeBattle = player->GetHealth();

    battleActive = true;
    battleInitialized = false;

    // Delegate to battle panel for UI flow
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

// Handles post-battle cleanup and rewards
void BattleSystem::HandleBattleEnd() {
    if (!battleActive || !currentPlayer) return;

    const BattleResult result = battlePanel->GetResult();

    // Player victory rewards
    if (result == BattleResult::PLAYER_WON && currentPlayer) {
        // Calculate partial health restoration
        const double healthToRestore = playerHealthBeforeBattle * HEALTH_RESTORE_PERCENTAGE;

        // Apply restoration without exceeding max health
        double newHealth = std::min(
            static_cast<double>(currentPlayer->GetMaxHealth()),
            currentPlayer->GetHealth() + healthToRestore
        );

        currentPlayer->SetHealth(static_cast<int>(newHealth));
    }

    // Execute external callback if registered
    if (onBattleEnd) {
        onBattleEnd(result);
    }

    // Reset battle state
    battleActive = false;
    battleInitialized = false;
    currentPlayer = nullptr;
    currentMonster = nullptr;
    playerHealthBeforeBattle = 0.0;
}

// Randomly determines battle turn order
bool BattleSystem::determineFirstTurn() const {
    return RandomUtils::randomValue<int>(0, 1) == 0;
}

// UI configuration method
void BattleSystem::SetFont(const Font &font) {
    if (battlePanel) {
        battlePanel->SetFont(font);
    }
}

// Dependency injection for game map
void BattleSystem::SetMap(Map *map) {
    gameMap = map;
}

// Registers battle end callback
void BattleSystem::SetBattleEndCallback(const std::function<void(BattleResult)> &callback) {
    onBattleEnd = callback;
}
