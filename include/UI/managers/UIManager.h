//
// Created by Lenovo on 15.5.2025 г.
//

#ifndef UIMANAGER_H
#define UIMANAGER_H

#include "C:/DandD/include/UI/screens/MainMenu.h"
#include "C:/DandD/include/UI/screens/GameHUD.h"
#include "C:/DandD/include/UI/panels/BattlePanel.h"
#include "C:/DandD/include/UI/panels/LevelUpPanel.h"
#include "C:/DandD/include/UI/panels/EquipmentPanel.h"
#include "C:/DandD/include/UI/panels/DefeatPanel.h"
#include "C:/DandD/include/UI/renderers/MapRenderer.h"
#include "C:/DandD/include/Entities/Hero.h"
#include "C:/DandD/include/Entities/Monster.h"
#include "C:/DandD/include/Core/MapSystem.h"
#include "C:/DandD/include/Utils/Attack.h"
#include "C:/DandD/include/Items/Item.h"
#include <string>
#include <vector>
#include <memory>

enum class UIState {
    MAIN_MENU,
    GAMEPLAY,
    BATTLE,
    LEVEL_UP,
    EQUIPMENT_SELECTION,
    LEVEL_TRANSITION,
    DEFEAT
};

struct Portal {
    Position position;
    bool isActive;
    float animationTime;

    Portal(const Position &pos) : position(pos), isActive(true), animationTime(0.0f) {
    };
};

class UIManager {
public:
    UIManager(int _screenWidth, int _screenHeight);

    ~UIManager();

    void Initialize();

    void LoadResources() const;

    void Unload();

    void Update(float deltaTime);

    void Draw() const;

    void SetState(UIState newState);

    UIState GetCurrentState() const;

    int GetCurrentLevel() const;

    void StartNewGame();

    void LoadGame();

    bool ShouldQuit() const;

    void LoadLevel(int levelNumber);

    void GenerateNewLevel();

    void CheckLevelCompletion();

    bool IsLevelComplete() const;

    void CreatePortal();

    void StartBattle(Hero *hero, Monster *monster);

    void EndBattle();

    bool IsBattleActive() const;

    BattleResult GetBattleResult() const;

    void ShowEquipmentChoice(const Item *newItem);

    void HideEquipmentPanel();

    bool IsEquipmentPanelVisible() const;

    void ShowLevelUpPanel(int availablePoints);

    void HideLevelUpPanel();

    bool IsLevelUpPanelVisible() const;

    void SetHero(Hero *hero);

    Hero *GetHero() const;

    void SetCurrentMap(Map *map);

    Map *GetCurrentMap() const;

    void SetAttackSystem(Attack *attackSystem);

    void UpdateHUDStats();

    void UpdateMapRenderer();

    Position FindNearestWallPosition(const Position &playerPos) const;

    bool IsPositionWall(const Position &pos) const;

    bool IsPositionReachable(const Position &from, const Position &to) const;

private:
    int screenWidth;
    int screenHeight;

    UIState currentState;
    UIState previousState;

    MainMenu *mainMenu;
    GameHUD *gameHUD;
    BattlePanel *battlePanel;
    LevelUpPanel *levelUpPanel;
    EquipmentPanel *equipmentPanel;
    MapRenderer *mapRenderer;

    Hero *hero;
    Map *currentMap;
    Attack *attackSystem;
    Monster *currentBattleMonster;
    BattleSystem *battleSystem;
    DefeatPanel *defeatPanel;

    int currentLevel;
    std::vector<Portal> portals;
    bool levelComplete;
    bool portalCreated;

    std::string mapFilePath;

    float transitionTimer;
    bool isTransitioning;

    void UpdateMainMenu(float deltaTime);

    void UpdateGameplay(float deltaTime);

    void UpdateBattle(float deltaTime);

    void UpdateLevelUp(float deltaTime);

    void UpdateEquipmentSelection(float deltaTime);

    void UpdateLevelTransition(float deltaTime);

    void UpdateDefeat(float deltaTime);

    void DrawMainMenu() const;

    void DrawGameplay() const;

    void DrawBattle() const;

    void DrawLevelUp() const;

    void DrawEquipmentSelection() const;

    void DrawLevelTransition() const;

    void DrawDefeat() const;

    void DrawPortals() const;

    void HandleMainMenuInput();

    void HandlePortalInteraction();

    void InitializeGameplay();

    void CleanupGameplay();

    void ResetLevelState();

    bool AreAllMonstersDefeated() const;

    bool AreAllBossesDefeated() const;

    std::vector<Position> GetAdjacentWallPositions(const Position &playerPos) const;

    Position GetClosestReachableWall(const Position &playerPos) const;

    bool IsValidPortalPosition(const Position &pos) const;

    void OnLevelUpConfirm(int str, int mana, int health);

    void OnLevelUpCancel();

    void OnEquipmentKeep();

    void OnEquipmentEquip();

    void OnBattleEnd(BattleResult result);

    bool CheckForBattle(const Position &newPosition);
};

#endif //UIMANAGER_H
