#include "C:/DandD/include/Core/GameManager.h"

GameManager::GameManager(int screenWidth, int screenHeight)
    : screenWidth(screenWidth), screenHeight(screenHeight),
      isRunning(false), uiManager(nullptr), hero(nullptr),
      currentMap(nullptr), attackSystem(nullptr), currentMonster(nullptr) {
    InitializeSystems();
}

GameManager::~GameManager() {
    delete attackSystem;
    delete currentMap;
    delete hero;
    delete uiManager;
}

void GameManager::InitializeSystems() {
    InitWindow(screenWidth, screenHeight, "Dungeons and Dragons");
    SetTargetFPS(60);

    hero = new Hero("Human", "Player");
    currentMap = new Map();
    attackSystem = new Attack();

    uiManager = new UIManager(screenWidth, screenHeight);
    uiManager->SetHero(hero);
    uiManager->SetCurrentMap(currentMap);
    uiManager->SetAttackSystem(attackSystem);
    uiManager->LoadResources();
    uiManager->Initialize();
}

void GameManager::RunGame() {
    isRunning = true;

    while (isRunning && !WindowShouldClose()) {
        const float deltaTime = GetFrameTime();
        ProcessInput();
        Update(deltaTime);
        Render();

        if (uiManager->ShouldQuit()) {
            isRunning = false;
        }
    }
    CloseWindow();
}

void GameManager::ProcessInput() {
    switch (uiManager->GetCurrentState()) {
        case UIState::MAIN_MENU:
            // Main menu input is handled in UIManager
            break;

        case UIState::GAMEPLAY:
            HandleMovement();
            HandleCombatTrigger();
            HandleTreasureCollection();
            break;

        case UIState::BATTLE:
            // Battle input is handled in BattlePanel
            break;

        case UIState::LEVEL_UP:
        case UIState::EQUIPMENT_SELECTION:
            // UI panels handle their own input
            break;
        default:
            break;
    }
}

void GameManager::HandleMovement() {
    if (!hero || !currentMap) return;

    Position newPos = hero->getCurrentPosition();
    bool moved = false;

    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
        newPos.x++;
        moved = true;
    } else if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
        newPos.x--;
        moved = true;
    } else if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        newPos.y--;
        moved = true;
    } else if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        newPos.y++;
        moved = true;
    }

    if (moved && currentMap->isPassable(newPos.x, newPos.y)) {
        hero->setPosition(newPos);
        uiManager->UpdateMapRenderer();
    }
}

void GameManager::HandleCombatTrigger() {
    if (!hero || !currentMap) return;

    Position heroPos = hero->getCurrentPosition();
    auto &monsters = currentMap->getMonsters();

    for (auto &monster: monsters) {
        if (!monster.isDefeated()) {
            Position mPos = monster.GetPosition();
            int distX = abs(heroPos.x - mPos.x);
            int distY = abs(heroPos.y - mPos.y);

            if (distX <= 1 && distY <= 1 && (distX != 0 || distY != 0)) {
                currentMonster = &monster;
                uiManager->StartBattle(hero, currentMonster);
                return;
            }
        }
    }
}

void GameManager::HandleTreasureCollection() {
    if (!hero || !currentMap || uiManager->GetCurrentState() != UIState::GAMEPLAY)
        return;

    const Position heroPos = hero->getCurrentPosition();
    const std::vector<Treasure> &treasures = currentMap->getTreasures();
    bool collectedAny = false;

    const std::vector<Treasure> treasuresCopy = treasures;

    for (const auto &treasure: treasuresCopy) {
        if (heroPos.x == treasure.getPosition().x &&
            heroPos.y == treasure.getPosition().y) {
            currentMap->removeTreasure(treasure);
            collectedAny = true;

            Item *newItem = ItemGenerator::generateRandomItem(currentMap->GetCurrentLevel());
            if (newItem) {
                uiManager->ShowEquipmentChoice(newItem);
            }
        }
    }

    if (collectedAny) {
        uiManager->UpdateHUDStats();
    }
}

void GameManager::Update(const float deltaTime) const {
    if (!uiManager) return;

    uiManager->Update(deltaTime);

    if (uiManager->IsLevelComplete() &&
        uiManager->GetCurrentState() == UIState::GAMEPLAY) {
        TransitionToNextLevel();
    }
}

void GameManager::Render() const {
    BeginDrawing();
    ClearBackground(BLACK);

    if (uiManager) {
        uiManager->Draw();
    }

    EndDrawing();
}

void GameManager::PositionHeroAtStart() const {
    if (!currentMap || !hero) return;

    Position startPos = currentMap->getStartPos();
    hero->setPosition(startPos);

    if (uiManager) {
        uiManager->UpdateMapRenderer();
    }
}

void GameManager::LoadCurrentLevel() const {
    if (!uiManager) return;

    uiManager->LoadLevel(uiManager->GetCurrentLevel());
    PositionHeroAtStart();
    uiManager->UpdateHUDStats();
}

void GameManager::TransitionToNextLevel() const {
    if (!uiManager) return;

    uiManager->GenerateNewLevel();
    LoadCurrentLevel();
}
