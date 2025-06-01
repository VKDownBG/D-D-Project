//
// Created by Lenovo on 15.5.2025 г.
//

#include "C:/DandD/include/UI/managers/UIManager.h"
#include <algorithm>
#include <cmath>
#include <queue>

UIManager::UIManager(const int _screenWidth, const int _screenHeight)
    : screenWidth(_screenWidth), screenHeight(_screenHeight),
      currentState(UIState::MAIN_MENU), previousState(UIState::MAIN_MENU),
      mainMenu(nullptr), gameHUD(nullptr), battlePanel(nullptr), levelUpPanel(nullptr),
      equipmentPanel(nullptr), mapRenderer(nullptr),
      hero(nullptr), currentMap(nullptr), attackSystem(nullptr), currentBattleMonster(nullptr),
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

    if (mainMenu) mainMenu->Initialize();

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

    delete battlePanel;
    battlePanel = nullptr;

    delete levelUpPanel;
    levelUpPanel = nullptr;

    delete equipmentPanel;
    equipmentPanel = nullptr;
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
    return mainMenu && mainMenu->IsQuitSelected();
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
    if (levelComplete || portalCreated) return;

    bool monstersDefeated = AreAllMonstersDefeated();
    bool bossesDefeated = AreAllBossesDefeated();

    if (monstersDefeated || bossesDefeated) {
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
    const Position portalPos = FindNearestWallPosition(playerPos);

    if (portalPos.x != -1 && portalPos.y != -1) {
        portals.emplace_back(portalPos);
        portalCreated = true;
    }
}

void UIManager::StartBattle(Hero *heroRef, Monster *monster) {
    if (!battlePanel || !heroRef || !monster || !attackSystem) return;

    currentBattleMonster = monster;
    battlePanel->StartBattle(heroRef, monster, attackSystem);
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
    return currentState == UIState::BATTLE && battlePanel && battlePanel->IsActive();
}

BattleResult UIManager::GetBattleResult() const {
    return battlePanel ? battlePanel->GetResult() : BattleResult::ONGOING;
}

void UIManager::ShowEquipmentChoice(const Item *newItem) {
    if (!equipmentPanel || !hero || !newItem) return;

    if (!equipmentPanel) {
        equipmentPanel = new EquipmentPanel(hero);
    }

    const Item *currentItem = nullptr;

    switch (newItem->GetType()) {
        case ItemType::ARMOR:
            if (hero->hasArmor()) {
                currentItem = &hero->GetInventory().GetArmor();
            }
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
    SetState(UIState::GAMEPLAY);
}

bool UIManager::IsEquipmentPanelVisible() const {
    return equipmentPanel && equipmentPanel->IsVisible();
}

void UIManager::ShowLevelUpPanel(int availablePoints) {
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

    CheckLevelCompletion();
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
        LoadLevel(currentLevel);

        int levelUpPoints = 30;
        ShowLevelUpPanel(levelUpPoints);

        transitionTimer = 0.0f;
        isTransitioning = false;
    }
}

void UIManager::DrawMainMenu() const {
    if (mainMenu) {
        mainMenu->Draw();
    }
}

void UIManager::DrawGameplay() const {
    if (mapRenderer) {
        mapRenderer->Draw();
    }
    if (gameHUD) {
        gameHUD->Draw();
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

    float alpha = std::sin(transitionTimer * 2.0f) * 127 + 128;
    DrawRectangle(0, 0, screenWidth, screenHeight, {0, 0, 0, static_cast<unsigned char>(alpha)});

    const std::string text = "Level Complete! Preparing next level...";
    const int textWidth = MeasureText(text.c_str(), 40);
    DrawText(text.c_str(), (screenWidth - textWidth) / 2, screenHeight / 2 - 20, 40, WHITE);
}

void UIManager::DrawPortals() const {
    if (!mapRenderer || portals.empty()) return;

    for (const auto &portal: portals) {
        if (portal.isActive) {
            const float pulseScale = 1.0f + std::sin(portal.animationTime * 4.0f) * 0.2f;
            const float alpha = (std::sin(portal.animationTime * 3.0f) + 1.0f) * 127;

            const Color portalColor = {100, 200, 255, static_cast<unsigned char>(alpha)};

            // Adjust coordinates based on your tile size (assuming 40x40 tiles)
            const Vector2 screenPos = {
                static_cast<float>(portal.position.x * 40),
                static_cast<float>(portal.position.y * 40)
            };
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
            abs(heroPos.x - portal.position.x) <= 1 &&
            abs(heroPos.y - portal.position.y) <= 1) {
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
    return std::all_of(monsters.begin(), monsters.end(),
                       [&](const Monster &monster) {
                           return monster.GetType() != MonsterType::MONSTER || monster.isDefeated();
                       });
}

bool UIManager::AreAllBossesDefeated() const {
    if (!currentMap) return false;

    const auto &monsters = currentMap->getMonsters();
    return std::all_of(monsters.begin(), monsters.end(),
                       [](const Monster &monster) {
                           return monster.GetType() != MonsterType::BOSS || monster.isDefeated();
                       });
}

Position UIManager::FindNearestWallPosition(const Position &playerPos) const {
    if (!currentMap) return Position(-1, -1);

    std::vector<Position> adjacentWalls = GetAdjacentWallPositions(playerPos);
    if (!adjacentWalls.empty()) {
        return adjacentWalls[0];
    }

    return GetClosestReachableWall(playerPos);
}

std::vector<Position> UIManager::GetAdjacentWallPositions(const Position &playerPos) const {
    std::vector<Position> wallPositions;

    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dx == 0 && dy == 0) continue;

            Position checkPos(playerPos.x + dx, playerPos.y + dy);
            if (IsPositionWall(checkPos) && IsValidPortalPosition(checkPos)) {
                wallPositions.push_back(checkPos);
            }
        }
    }

    return wallPositions;
}

Position UIManager::GetClosestReachableWall(const Position &playerPos) const {
    if (!currentMap) return Position(-1, -1);

    Position closestWall(-1, -1);
    float minDistance = std::numeric_limits<float>::max();

    for (int radius = 2; radius <= 10; radius++) {
        for (int dx = -radius; dx <= radius; dx++) {
            for (int dy = -radius; dy <= radius; dy++) {
                if (abs(dx) != radius && abs(dy) != radius) continue;

                Position checkPos(playerPos.x + dx, playerPos.y + dy);

                if (IsPositionWall(checkPos) &&
                    IsValidPortalPosition(checkPos) &&
                    IsPositionReachable(playerPos, checkPos)) {
                    float distance = sqrt(dx * dx + dy * dy);
                    if (distance < minDistance) {
                        minDistance = distance;
                        closestWall = checkPos;
                    }
                }
            }
        }
        if (closestWall.x != -1) break;
    }

    return closestWall;
}

bool UIManager::IsPositionWall(const Position &pos) const {
    if (!currentMap) return false;
    return currentMap->getCell(pos) == '#';
}

bool UIManager::IsValidPortalPosition(const Position &pos) const {
    if (!currentMap) return false;

    if (pos.x < 0 || pos.y < 0 ||
        pos.x >= static_cast<int>(currentMap->getWidth()) ||
        pos.y >= static_cast<int>(currentMap->getHeight())) {
        return false;
    }

    return IsPositionWall(pos);
}

bool UIManager::IsPositionReachable(const Position &from, const Position &to) const {
    if (!currentMap) return false;

    const int dx = abs(to.x - from.x);
    const int dy = abs(to.y - from.y);
    int x = from.x;
    int y = from.y;

    const int stepX = (from.x < to.x) ? 1 : -1;
    const int stepY = (from.y < to.y) ? 1 : -1;

    int err = dx - dy;

    while (true) {
        if (x == to.x && y == to.y) break;

        if ((x != to.x || y != to.y) && !currentMap->isPassable(x, y)) {
            return false;
        }

        const int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x += stepX;
        }
        if (e2 < dx) {
            err += dx;
            y += stepY;
        }
    }

    return true;
}

void UIManager::OnLevelUpConfirm(const int str, const int mana, const int health) {
    if (hero) {
        hero->levelUp(str, mana, health);
    }
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
    if (result == BattleResult::PLAYER_WON && currentBattleMonster && currentMap) {
        auto &monsters = currentMap->getMonsters();
        monsters.erase(
            std::remove_if(monsters.begin(), monsters.end(),
                           [this](const Monster &monster) {
                               return &monster == currentBattleMonster;
                           }),
            monsters.end()
        );
        UpdateHUDStats();
    }
}
