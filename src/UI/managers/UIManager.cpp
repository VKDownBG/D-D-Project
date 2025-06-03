//
// Created by Lenovo on 15.5.2025 г.
//

#include "C:/DandD/include/UI/managers/UIManager.h"
#include <algorithm>
#include <cmath>
#include <map>
#include <queue>

UIManager::UIManager(const int _screenWidth, const int _screenHeight)
    : screenWidth(_screenWidth), screenHeight(_screenHeight),
      currentState(UIState::MAIN_MENU), previousState(UIState::MAIN_MENU),
      mainMenu(nullptr), gameHUD(nullptr), battlePanel(nullptr), levelUpPanel(nullptr),
      equipmentPanel(nullptr), mapRenderer(nullptr),
      hero(nullptr), currentMap(nullptr), attackSystem(nullptr), currentBattleMonster(nullptr),
      battleSystem(nullptr), defeatPanel(nullptr),
      currentLevel(1), levelComplete(false), portalCreated(false),
      mapFilePath("C:/DandD/assets/maps/maps.txt"),
      transitionTimer(0.0f), isTransitioning(false) {
}

UIManager::~UIManager() {
    Unload();
}

void UIManager::Initialize() {
    if (!mainMenu) mainMenu = new MainMenu(screenWidth, screenHeight);
    if (!gameHUD) gameHUD = new GameHUD(screenWidth, screenHeight);
    if (!battlePanel) battlePanel = new BattlePanel();
    if (!levelUpPanel) levelUpPanel = new LevelUpPanel();
    if (!equipmentPanel) equipmentPanel = new EquipmentPanel(hero);
    if (!mapRenderer) mapRenderer = new MapRenderer(screenWidth, screenHeight);
    if (!battleSystem) battleSystem = new BattleSystem();

    if (mainMenu) mainMenu->Initialize();

    if (battleSystem) {
        battleSystem->SetBattleEndCallback([this](const BattleResult result) {
            OnBattleEnd(result);
        });
    }

    if (levelUpPanel) {
        levelUpPanel->SetOnConfirm([this](const int str, const int mana, const int health) {
            OnLevelUpConfirm(str, mana, health);
        });
        levelUpPanel->SetOnCancel([this]() {
            OnLevelUpCancel();
        });
    }
}

void UIManager::LoadResources() const {
    if (mainMenu) mainMenu->LoadResources();
    if (gameHUD) gameHUD->LoadResources();
    if (mapRenderer) mapRenderer->LoadResources();
}

void UIManager::Unload() {
    if (mainMenu) {
        mainMenu->Unload();
        delete mainMenu;
        mainMenu = nullptr;
    }
    if (gameHUD) {
        gameHUD->Unload();
        delete gameHUD;
        gameHUD = nullptr;
    }
    if (mapRenderer) {
        mapRenderer->Unload();
        delete mapRenderer;
        mapRenderer = nullptr;
    }

    delete battleSystem;
    battleSystem = nullptr;

    delete battlePanel;
    battlePanel = nullptr;

    delete levelUpPanel;
    levelUpPanel = nullptr;

    delete equipmentPanel;
    equipmentPanel = nullptr;

    delete defeatPanel;
    defeatPanel = nullptr;
}

void UIManager::Update(const float deltaTime) {
    switch (currentState) {
        case UIState::MAIN_MENU:
            UpdateMainMenu(deltaTime);
            break;
        case UIState::GAMEPLAY:
            UpdateGameplay(deltaTime);
            break;
        case UIState::BATTLE:
            UpdateBattle(deltaTime);
            break;
        case UIState::LEVEL_UP:
            UpdateLevelUp(deltaTime);
            break;
        case UIState::EQUIPMENT_SELECTION:
            UpdateEquipmentSelection(deltaTime);
            break;
        case UIState::LEVEL_TRANSITION:
            UpdateLevelTransition(deltaTime);
            break;
        case UIState::DEFEAT:
            UpdateDefeat(deltaTime);
            break;
    }

    for (auto &portal: portals) {
        portal.animationTime += deltaTime;
    }
}

void UIManager::Draw() const {
    switch (currentState) {
        case UIState::MAIN_MENU:
            DrawMainMenu();
            break;
        case UIState::GAMEPLAY:
            DrawGameplay();
            break;
        case UIState::BATTLE:
            DrawBattle();
            break;
        case UIState::LEVEL_UP:
            DrawLevelUp();
            break;
        case UIState::EQUIPMENT_SELECTION:
            DrawEquipmentSelection();
            break;
        case UIState::LEVEL_TRANSITION:
            DrawLevelTransition();
            break;
        case UIState::DEFEAT:
            DrawDefeat();
            break;
    }
}

void UIManager::SetState(const UIState newState) {
    if (currentState != newState) {
        previousState = currentState;
        currentState = newState;

        if (newState == UIState::GAMEPLAY && previousState == UIState::MAIN_MENU) {
            InitializeGameplay();
        } else if (newState == UIState::MAIN_MENU && previousState == UIState::GAMEPLAY) {
            CleanupGameplay();
        }
    }
}

UIState UIManager::GetCurrentState() const {
    return currentState;
}

int UIManager::GetCurrentLevel() const {
    return currentLevel;
}

void UIManager::StartNewGame() {
    currentLevel = 1;
    if (currentMap) {
        LoadLevel(currentLevel);

        if (hero) {
            const Position startPos = currentMap->getStartPos();
            hero->setPosition(startPos);
        }

        UpdateMapRenderer();
        UpdateHUDStats();
    }
    SetState(UIState::GAMEPLAY);
}

void UIManager::LoadGame() {
    StartNewGame();
}

bool UIManager::ShouldQuit() const {
    return (mainMenu && mainMenu->IsQuitSelected()) ||
           (defeatPanel && defeatPanel->ShouldQuit());
}

void UIManager::LoadLevel(const int levelNumber) {
    if (!currentMap) return;

    try {
        const std::string levelTag = "[LEVEL_" + std::to_string(levelNumber) + "]";
        currentMap->loadFromFile(mapFilePath, levelTag);
        currentLevel = levelNumber;
        ResetLevelState();

        if (mapRenderer && hero) {
            mapRenderer->Initialize(currentMap, hero->GetPosition());
        }

        UpdateHUDStats();
    } catch (const std::exception &e) {
        std::cerr << "Failed to load level: %s" << e.what();
    }
}

void UIManager::GenerateNewLevel() {
    LoadLevel(currentLevel + 1);
}

void UIManager::CheckLevelCompletion() {
    if (levelComplete || portalCreated || !currentMap) return;

    // Get a safe copy of the monsters vector
    const auto monsters = currentMap->getMonstersConst(); // Use the const version

    if (monsters.empty()) {
        levelComplete = true;
        CreatePortal();
        return;
    }

    // Count alive monsters by type
    int aliveNormalMonsters = 0;
    int aliveBosses = 0;
    int totalNormalMonsters = 0;
    int totalBosses = 0;

    for (const auto &monster: monsters) {
        if (monster.GetType() == MonsterType::BOSS) {
            totalBosses++;
            if (!monster.isDefeated()) {
                aliveBosses++;
            }
        } else {
            totalNormalMonsters++;
            if (!monster.isDefeated()) {
                aliveNormalMonsters++;
            }
        }
    }

    // Level complete if either all normal monsters are dead OR all bosses are dead
    const bool normalMonstersCleared = (totalNormalMonsters > 0 && aliveNormalMonsters == 0);
    const bool bossesCleared = (totalBosses > 0 && aliveBosses == 0);

    if (normalMonstersCleared || bossesCleared) {
        levelComplete = true;
        CreatePortal();
    }
}

bool UIManager::IsLevelComplete() const {
    return levelComplete;
}

void UIManager::CreatePortal() {
    if (portalCreated || !hero || !currentMap) return;

    const Position playerPos = hero->getCurrentPosition();

    if (playerPos.x + 1 == '#' || playerPos.y + 1 == '#' || playerPos.x - 1 == '#' || playerPos.y - 1 == '#') {
        DrawPortals();
    }
}

void UIManager::StartBattle(Hero *heroRef, Monster *monster) {
    if (!battlePanel || !heroRef || !monster || !battleSystem) return;

    currentBattleMonster = monster;

    // Start battle in BattleSystem
    battleSystem->StartBattle(heroRef, monster);

    // Start battle UI in BattlePanel
    battlePanel->StartBattle(heroRef, monster, battleSystem);

    SetState(UIState::BATTLE);
}

void UIManager::EndBattle() {
    if (battlePanel) {
        const BattleResult result = battlePanel->GetResult();
        OnBattleEnd(result);
    }

    currentBattleMonster = nullptr;
    SetState(UIState::GAMEPLAY);
}

bool UIManager::IsBattleActive() const {
    return battleSystem && battleSystem->IsBattleActive();
}

BattleResult UIManager::GetBattleResult() const {
    return battlePanel ? battlePanel->GetResult() : BattleResult::ONGOING;
}

void UIManager::ShowEquipmentChoice(const Item *newItem) {
    if (!equipmentPanel || !hero || !newItem) return;

    if (currentState == UIState::EQUIPMENT_SELECTION) {
        return;
    }

    const Item *currentItem = nullptr;

    switch (newItem->GetType()) {
        case ItemType::ARMOR:
            currentItem = &hero->GetInventory().GetArmor();
            break;
        case ItemType::WEAPON:
            currentItem = &hero->GetInventory().GetWeapon();
            break;
        case ItemType::SPELL:
            currentItem = &hero->GetInventory().GetSpell();
            break;
        default:
            break;
    }

    equipmentPanel->Show(currentItem, newItem);
    SetState(UIState::EQUIPMENT_SELECTION);
}

void UIManager::HideEquipmentPanel() {
    if (equipmentPanel) {
        equipmentPanel->Hide();
    }

    if (currentState == UIState::EQUIPMENT_SELECTION) {
        SetState(UIState::GAMEPLAY);
    }
}

bool UIManager::IsEquipmentPanelVisible() const {
    return equipmentPanel && equipmentPanel->IsVisible();
}

void UIManager::ShowLevelUpPanel(const int availablePoints) {
    if (!levelUpPanel || !hero) return;

    levelUpPanel->SetHeroReference(hero);
    levelUpPanel->SetMaxPoints(availablePoints);
    levelUpPanel->Show();
    SetState(UIState::LEVEL_UP);
}

void UIManager::HideLevelUpPanel() {
    if (levelUpPanel) {
        levelUpPanel->Hide();
    }
    SetState(UIState::GAMEPLAY);
}

bool UIManager::IsLevelUpPanelVisible() const {
    return levelUpPanel && levelUpPanel->IsVisible();
}

void UIManager::SetHero(Hero *heroRef) {
    hero = heroRef;
    if (gameHUD) {
        gameHUD->Initialize(hero);
    }
}

Hero *UIManager::GetHero() const {
    return hero;
}

void UIManager::SetCurrentMap(Map *map) {
    currentMap = map;
    if (battleSystem) {
        battleSystem->SetMap(map);
    }
}

Map *UIManager::GetCurrentMap() const {
    return currentMap;
}

void UIManager::SetAttackSystem(Attack *attack) {
    attackSystem = attack;
}

void UIManager::UpdateHUDStats() {
    if (!gameHUD || !currentMap) return;

    gameHUD->SetLevel(currentLevel);
    gameHUD->SetMonstersRemaining(currentMap->GetMonsterCount());
    gameHUD->SetTreasuresRemaining(currentMap->GetTreasureCount());
}

void UIManager::UpdateMapRenderer() {
    if (mapRenderer && currentMap && hero) {
        mapRenderer->Initialize(currentMap, hero->GetPosition());
    }
}

void UIManager::UpdateMainMenu(const float deltaTime) {
    if (mainMenu) {
        mainMenu->Update(deltaTime);
        HandleMainMenuInput();
    }
}

void UIManager::UpdateGameplay(const float deltaTime) {
    if (gameHUD) {
        gameHUD->Update(deltaTime);
    }
    if (mapRenderer) {
        mapRenderer->Update(deltaTime);
    }

    HandlePortalInteraction();
}

void UIManager::UpdateBattle(const float deltaTime) {
    if (battlePanel) {
        battlePanel->Update();

        if (battlePanel->isFinished()) {
            EndBattle();
        }
    }
}

void UIManager::UpdateLevelUp(float deltaTime) {
    if (levelUpPanel) {
        levelUpPanel->Update();
    }
}

void UIManager::UpdateEquipmentSelection(float deltaTime) {
    if (equipmentPanel) {
        equipmentPanel->Update();
    }
}

void UIManager::UpdateLevelTransition(const float deltaTime) {
    transitionTimer += deltaTime;

    if (transitionTimer >= 2.0f) {
        currentLevel++;

        const int levelUpPoints = 30;
        ShowLevelUpPanel(levelUpPoints);

        //LoadLevel(currentLevel);

        transitionTimer = 0.0f;
        isTransitioning = false;
    }
}

void UIManager::UpdateDefeat(const float deltaTime) {
    if (defeatPanel) {
        defeatPanel->Update();
    }
}

void UIManager::DrawMainMenu() const {
    if (mainMenu) {
        mainMenu->Draw();
    }
}

void UIManager::DrawGameplay() const {
    //DrawRectangle(0, 0, screenWidth, screenHeight, {20, 10, 5, 255});

    if (gameHUD) {
        gameHUD->Draw();
    }
    if (mapRenderer) {
        mapRenderer->Draw();
    }

    DrawPortals();
}

void UIManager::DrawBattle() const {
    DrawGameplay();

    if (battlePanel) {
        battlePanel->Draw();
    }
}

void UIManager::DrawLevelUp() const {
    DrawGameplay();

    if (levelUpPanel) {
        levelUpPanel->Draw();
    }
}

void UIManager::DrawEquipmentSelection() const {
    DrawGameplay();

    if (equipmentPanel) {
        equipmentPanel->Draw();
    }
}

void UIManager::DrawLevelTransition() const {
    DrawGameplay();

    const float alpha = std::sin(transitionTimer * 2.0f) * 127 + 128;
    DrawRectangle(0, 0, screenWidth, screenHeight, {0, 0, 0, static_cast<unsigned char>(alpha)});

    const std::string text = "Level Complete! Preparing next level...";
    const int textWidth = MeasureText(text.c_str(), 40);
    DrawText(text.c_str(), (screenWidth - textWidth) / 2, screenHeight / 2 - 20, 40, WHITE);
}

void UIManager::DrawDefeat() const {
    if (defeatPanel) {
        defeatPanel->Draw();
    }
}

void UIManager::DrawPortals() const {
    if (!mapRenderer || portals.empty()) return;

    const float cellSize = mapRenderer->GetCellSize();

    for (const auto &portal: portals) {
        if (portal.isActive) {
            // Convert world position to screen coordinates
            Vector2 screenPos = mapRenderer->WorldToScreen(portal.position.x, portal.position.y);

            // Adjust to center of cell
            screenPos.x += cellSize / 2;
            screenPos.y += cellSize / 2;

            const float pulseScale = 1.0f + std::sin(portal.animationTime * 4.0f) * 0.2f;
            const float alpha = (std::sin(portal.animationTime * 3.0f) + 1.0f) * 127;
            const Color portalColor = {100, 200, 255, static_cast<unsigned char>(alpha)};

            // Draw portal at corrected position
            DrawCircleV(screenPos, 20.0f * pulseScale, portalColor);
            DrawCircleLinesV(screenPos, 25.0f * pulseScale, {150, 255, 255, 200});
        }
    }
}

void UIManager::HandleMainMenuInput() {
    if (!mainMenu) return;

    if (mainMenu->IsStartGameSelected()) {
        StartNewGame();
        mainMenu->ResetSelections();
    } else if (mainMenu->IsLoadGameSelected()) {
        LoadGame();
        mainMenu->ResetSelections();
    }
}

void UIManager::HandlePortalInteraction() {
    if (!hero || portals.empty()) return;

    const Position &heroPos = hero->getCurrentPosition();
    for (const auto &portal: portals) {
        if (portal.isActive &&
            heroPos.x == portal.position.x &&
            heroPos.y == portal.position.y) {
            // Exact position match
            if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
                isTransitioning = true;
                SetState(UIState::LEVEL_TRANSITION);
                break;
            }
        }
    }
}

void UIManager::InitializeGameplay() {
    if (gameHUD && hero) {
        gameHUD->Initialize(hero);
    }

    const Weapon *weapon = &hero->GetInventory().GetWeapon();
    gameHUD->SetWeapon(weapon);

    const Armor *armor = &hero->GetInventory().GetArmor();
    gameHUD->SetArmor(armor);

    const Spell *spell = &hero->GetInventory().GetSpell();
    gameHUD->SetSpell(spell);

    UpdateMapRenderer();
    UpdateHUDStats();
}

void UIManager::CleanupGameplay() {
    portals.clear();
    portalCreated = false;
    levelComplete = false;
    currentBattleMonster = nullptr;
}

void UIManager::ResetLevelState() {
    portals.clear();
    portalCreated = false;
    levelComplete = false;
    transitionTimer = 0.0f;
    isTransitioning = false;
}

bool UIManager::AreAllMonstersDefeated() const {
    if (!currentMap) return false;

    const auto &monsters = currentMap->getMonstersConst();

    // Check if there are any regular monsters that are not defeated
    for (const Monster &monster: monsters) {
        if (monster.GetType() == MonsterType::MONSTER && !monster.isDefeated()) {
            return false;
        }
    }

    return true;
}

bool UIManager::AreAllBossesDefeated() const {
    if (!currentMap) return false;

    const auto &monsters = currentMap->getMonsters();

    // Check if there are any bosses that are not defeated
    for (const Monster &monster: monsters) {
        if (monster.GetType() == MonsterType::BOSS && !monster.isDefeated()) {
            return false;
        }
    }

    return true; // All bosses are either defeated or don't exist
}

bool UIManager::IsPositionWall(const Position &pos) const {
    if (!currentMap) return false;
    return currentMap->getCell(pos) == '#';
}

void UIManager::OnLevelUpConfirm(const int str, const int mana, const float health) {
    if (hero) {
        hero->levelUp(str, mana, health);
    }

    LoadLevel(currentLevel);

    HideLevelUpPanel();
}

void UIManager::OnLevelUpCancel() {
    HideLevelUpPanel();
}

void UIManager::OnEquipmentKeep() {
    HideEquipmentPanel();
}

void UIManager::OnEquipmentEquip() {
    HideEquipmentPanel();
}

void UIManager::OnBattleEnd(const BattleResult result) {
    // Notify the battle panel about the end result
    if (battlePanel) {
        battlePanel->OnBattleEnd(result);
    }

    if (result == BattleResult::PLAYER_WON && currentBattleMonster && currentMap) {
        mapRenderer->removeMonster(currentBattleMonster);
        UpdateMapRenderer();
        UpdateHUDStats();
    }

    if (result == BattleResult::PLAYER_LOST) {
        if (!defeatPanel) {
            defeatPanel = new DefeatPanel();
        }
        defeatPanel->Show();
        SetState(UIState::DEFEAT);
    }
}

bool UIManager::CheckForBattle(const Position &newPosition) {
    if (!hero || !battleSystem) return false;

    if (battleSystem->CheckForBattle(hero, newPosition)) {
        SetState(UIState::BATTLE);
        return true;
    }
    return false;
}
