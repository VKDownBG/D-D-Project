#include "C:/DandD/include/Core/GameManager.h"

// Constructor: Initializes game systems and window
GameManager::GameManager(int screenWidth, int screenHeight)
    : screenWidth(screenWidth), screenHeight(screenHeight),
      isRunning(false), uiManager(nullptr), hero(nullptr),
      currentMap(nullptr), attackSystem(nullptr), currentMonster(nullptr) {
    InitializeSystems();
}

// Destructor: Cleans up allocated resources
GameManager::~GameManager() {
    delete attackSystem;
    delete currentMap;
    delete hero;
    delete uiManager;
}

// Initializes game subsystems and resources
void GameManager::InitializeSystems() {
    InitWindow(screenWidth, screenHeight, "Dungeons and Dragons");
    SetTargetFPS(60);

    // Core game objects
    hero = new Hero("Human", "Player");
    currentMap = new Map();
    attackSystem = new Attack();

    // UI system setup
    uiManager = new UIManager(screenWidth, screenHeight);
    uiManager->SetHero(hero);
    uiManager->SetCurrentMap(currentMap);
    uiManager->SetAttackSystem(attackSystem);
    uiManager->Initialize();
    uiManager->LoadResources();

    LoadCurrentLevel();
}

// Main game loop
void GameManager::RunGame() {
    isRunning = true;

    while (isRunning && !WindowShouldClose()) {
        const float deltaTime = GetFrameTime();
        ProcessInput();
        Update(deltaTime);
        Render();

        // Check for quit request from UI
        if (uiManager->ShouldQuit()) {
            isRunning = false;
        }
    }
    CloseWindow();
}

// Routes input handling based on current game state
void GameManager::ProcessInput() {
    // Determine if UI panels are blocking movement
    const bool shouldBlockMovement = uiManager->IsEquipmentPanelVisible() ||
                                     uiManager->IsLevelUpPanelVisible() ||
                                     uiManager->GetCurrentState() != UIState::GAMEPLAY;

    // State-specific input handling
    switch (uiManager->GetCurrentState()) {
        case UIState::MAIN_MENU:
            // Handled internally by UIManager
            break;

        case UIState::GAMEPLAY:
            // Only process game actions if no blocking UI
            if (!shouldBlockMovement) {
                HandleMovement();
                HandleCombatTrigger();
                HandleTreasureCollection();
            }
            break;

        case UIState::BATTLE:
            // BattlePanel handles its own input
            break;

        case UIState::LEVEL_UP:
        case UIState::EQUIPMENT_SELECTION:
            // Panels handle their own input in UIManager
            break;

        default:
            break;
    }
}

// Handles player movement input
void GameManager::HandleMovement() {
    if (!hero || !currentMap) return;

    Position newPos = hero->getCurrentPosition();
    bool moved = false;

    // Process directional input
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

    // Validate and apply movement
    if (moved && currentMap->isPassable(newPos.x, newPos.y)) {
        hero->setPosition(newPos);
        uiManager->UpdateMapRenderer(); // Refresh map display
    }
}

// Checks for combat encounters at hero's position
void GameManager::HandleCombatTrigger() {
    if (!hero || !currentMap) return;

    const Position heroPos = hero->getCurrentPosition();
    auto &monsters = currentMap->getMonsters();

    // Check for undefeated monsters at hero's position
    for (auto &monster: monsters) {
        if (!monster.isDefeated()) {
            const Position mPos = monster.GetPosition();
            if (heroPos.x == mPos.x && heroPos.y == mPos.y) {
                currentMonster = &monster;
                uiManager->StartBattle(hero, currentMonster);

                // Wait for battle to actually finish
                return;
            }
        }
    }
}

// Handles treasure collection mechanics
void GameManager::HandleTreasureCollection() {
    // Validate preconditions
    if (!hero || !currentMap || uiManager->GetCurrentState() != UIState::GAMEPLAY)
        return;

    // Skip if equipment panel is open
    if (uiManager->IsEquipmentPanelVisible()) {
        return;
    }

    const Position heroPos = hero->getCurrentPosition();
    const std::vector<Treasure> &treasures = currentMap->getTreasures();
    const std::vector<Treasure> treasuresCopy = treasures; // Safe iteration copy

    // Check for treasure at hero's position
    for (const auto &treasure: treasuresCopy) {
        if (heroPos.x == treasure.getPosition().x &&
            heroPos.y == treasure.getPosition().y) {
            currentMap->removeTreasure(treasure);

            // Generate and show reward item
            Item *newItem = ItemGenerator::generateRandomItem(currentMap->GetCurrentLevel());
            if (newItem) {
                uiManager->ShowEquipmentChoice(newItem); // Show item selection UI
                uiManager->UpdateMapRenderer(); // Remove treasure from map
                return;
            }
        }
    }
}

// Updates game state and UI
void GameManager::Update(const float deltaTime) const {
    if (!uiManager) return;

    uiManager->Update(deltaTime);

    // Cleanup after equipment selection
    if (uiManager->GetCurrentState() == UIState::EQUIPMENT_SELECTION &&
        !uiManager->IsEquipmentPanelVisible()) {
        uiManager->SetState(UIState::GAMEPLAY);
        uiManager->UpdateMapRenderer();
        uiManager->UpdateHUDStats(); // Refresh HUD after equipment change
    }

    if (uiManager->GetCurrentState() == UIState::LEVEL_TRANSITION) {
        TransitionToNextLevel();
    }
}

// Main rendering pass
void GameManager::Render() const {
    BeginDrawing();
    uiManager->Draw(); // Delegate drawing to UI system
    EndDrawing();
}

// Places hero at level starting position
void GameManager::PositionHeroAtStart() const {
    if (!currentMap || !hero) return;

    const Position startPos = currentMap->getStartPos();
    hero->setPosition(startPos);

    if (uiManager) {
        uiManager->UpdateMapRenderer(); // Sync map display
    }
}

// Loads current level data
void GameManager::LoadCurrentLevel() const {
    if (!uiManager) return;

    // Ensure valid level number
    int levelToLoad = (uiManager->GetCurrentLevel() > 0) ? uiManager->GetCurrentLevel() : 1;
    uiManager->LoadLevel(uiManager->GetCurrentLevel());
    PositionHeroAtStart();
    uiManager->UpdateHUDStats(); // Refresh player stats display
}

// Handles level progression
void GameManager::TransitionToNextLevel() const {
    if (!uiManager) return;

    // Reset UI state before generating new level
    uiManager->SetState(UIState::GAMEPLAY);

    // Generate and load the new level
    uiManager->GenerateNewLevel();
    LoadCurrentLevel();
}
