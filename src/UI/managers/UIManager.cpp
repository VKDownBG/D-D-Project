//
// Created by Lenovo on 15.5.2025 г.
//

#include "C:/DandD/include/UI/managers/UIManager.h"
#include <algorithm>
#include <cmath>
#include <map>
#include <queue>

// Constructor: Initializes UIManager with screen dimensions and sets initial UI states and flags.
UIManager::UIManager(const int _screenWidth, const int _screenHeight)
    : screenWidth(_screenWidth), screenHeight(_screenHeight),
      currentState(UIState::MAIN_MENU), previousState(UIState::MAIN_MENU),
      mainMenu(nullptr), gameHUD(nullptr), characterSelectionPanel(nullptr), battlePanel(nullptr),
      levelUpPanel(nullptr), equipmentPanel(nullptr),
      mapRenderer(nullptr), hero(nullptr),
      currentMap(nullptr), attackSystem(nullptr), currentBattleMonster(nullptr), battleSystem(nullptr),
      defeatPanel(nullptr), selectedRace(Race::Human),
      currentLevel(1), levelComplete(false), portalCreated(false),
      mapFilePath("C:/DandD/assets/maps/maps.txt"),
      transitionTimer(0.0f), isTransitioning(false) {
}

// Destructor: Cleans up dynamically allocated UI components.
UIManager::~UIManager() {
    Unload();
}

// Initializes various UI panels and sets up their callbacks.
void UIManager::Initialize() {
    // Create new instances of UI panels if they don't exist.
    if (!mainMenu) mainMenu = new MainMenu(screenWidth, screenHeight);
    if (!gameHUD) gameHUD = new GameHUD(screenWidth, screenHeight);
    if (!battlePanel) battlePanel = new BattlePanel();
    if (!levelUpPanel) levelUpPanel = new LevelUpPanel();
    if (!equipmentPanel) {
        equipmentPanel = new EquipmentPanel(hero); // Note: hero might be nullptr here initially

        equipmentPanel->SetOnEquipCallback([this]() {
            OnEquipmentEquip();
        });

        equipmentPanel->SetOnKeepCallback([this]() {
            OnEquipmentKeep();
        });
    }
    if (!mapRenderer) mapRenderer = new MapRenderer(screenWidth, screenHeight);
    if (!battleSystem) battleSystem = new BattleSystem();
    if (!characterSelectionPanel) characterSelectionPanel = new CharacterSelectionPanel(screenWidth, screenHeight);

    // Initialize the main menu.
    if (mainMenu) mainMenu->Initialize();

    // Set up callbacks for character selection panel.
    if (characterSelectionPanel) {
        characterSelectionPanel->Initialize();
        characterSelectionPanel->SetOnRaceSelected([this](const Race race) {
            // Lambda for race selection
            OnRaceSelected(race);
        });
        characterSelectionPanel->SetOnBack([this]() {
            // Lambda for back button
            OnCharacterSelectionBack();
        });
    }

    // Set up battle end callback for battle system.
    if (battleSystem) {
        battleSystem->SetBattleEndCallback([this](const BattleResult result) {
            // Lambda for battle end
            OnBattleEnd(result);
        });
    }

    // Set up callbacks for level up panel.
    if (levelUpPanel) {
        levelUpPanel->SetOnConfirm([this](const int str, const int mana, const int health) {
            // Lambda for level up confirm
            OnLevelUpConfirm(str, mana, health);
        });
        levelUpPanel->SetOnCancel([this]() {
            // Lambda for level up cancel
            OnLevelUpCancel();
        });
    }
}

// Loads resources required by various UI components.
void UIManager::LoadResources() const {
    if (mainMenu) mainMenu->LoadResources();
    if (gameHUD) gameHUD->LoadResources();
    if (mapRenderer) mapRenderer->LoadResources();
}

// Unloads resources and deletes dynamically allocated UI components to prevent memory leaks.
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

    delete characterSelectionPanel;
    characterSelectionPanel = nullptr;

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

// Updates the current UI state based on the game's progression.
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
        case UIState::CHARACTER_SELECTION:
            UpdateCharacterSelection(deltaTime);
            break;
    }

    // Update animation timers for all active portals.
    for (auto &portal: portals) {
        portal.animationTime += deltaTime;
    }
}

// Draws the UI components based on the current UI state.
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
        case UIState::CHARACTER_SELECTION:
            DrawCharacterSelection();
            break;
    }
}

// Sets the current UI state and handles transitions between states.
void UIManager::SetState(const UIState newState) {
    if (currentState != newState) {
        previousState = currentState; // Store the previous state
        currentState = newState; // Set the new state

        // Handle specific actions on state change.
        if (newState == UIState::GAMEPLAY && previousState == UIState::MAIN_MENU) {
            InitializeGameplay();
        } else if (newState == UIState::MAIN_MENU && previousState == UIState::GAMEPLAY) {
            CleanupGameplay();
        }
    }
}

// Returns the current UI state.
UIState UIManager::GetCurrentState() const {
    return currentState;
}

// Returns the current game level.
int UIManager::GetCurrentLevel() const {
    return currentLevel;
}

// Starts a new game, resetting level to 1 and loading the initial map.
void UIManager::StartNewGame() {
    currentLevel = 1; // Reset level to 1
    if (currentMap) {
        LoadLevel(currentLevel); // Load the first level

        if (hero) {
            const Position startPos = currentMap->getStartPos(); // Get starting position from map
            hero->setPosition(startPos); // Set hero's position
        }

        UpdateMapRenderer(); // Update map rendering
        UpdateHUDStats(); // Update HUD information
    }
    SetState(UIState::GAMEPLAY); // Transition to gameplay state
}

// Placeholder for loading a saved game (currently just starts a new game).
void UIManager::LoadGame() {
    StartNewGame();
}

// Checks if the game should quit based on main menu or defeat panel selections.
bool UIManager::ShouldQuit() const {
    return (mainMenu && mainMenu->IsQuitSelected()) ||
           (defeatPanel && defeatPanel->ShouldQuit());
}

// Loads a specific game level from a file.
void UIManager::LoadLevel(const int levelNumber) {
    if (!currentMap) return; // Ensure map object exists

    try {
        const std::string levelTag = "[LEVEL_" + std::to_string(levelNumber) + "]"; // Construct level tag
        currentMap->loadFromFile(mapFilePath, levelTag); // Load map from file
        currentLevel = levelNumber; // Update current level number
        ResetLevelState(); // Reset level-specific flags (e.g., portal status)

        if (mapRenderer && hero) {
            mapRenderer->Initialize(currentMap, hero->GetPosition()); // Re-initialize map renderer
        }

        UpdateHUDStats(); // Update HUD information
    } catch (const std::exception &e) {
        std::cerr << "Failed to load level: %s" << e.what(); // Log error if level loading fails
    }
}

// Generates and loads the next level.
void UIManager::GenerateNewLevel() {
    LoadLevel(currentLevel + 1);
}

// Checks if the current level is complete based on defeated monsters.
void UIManager::CheckLevelCompletion() {
    if (levelComplete || portalCreated || !currentMap) return; // Don't re-check if already complete or portal exists

    const auto monsters = currentMap->getMonstersConst(); // Get current monsters on the map

    if (monsters.empty()) {
        levelComplete = true; // If no monsters, level is complete
        CreatePortal(); // Create portal for next level
        return;
    }

    int aliveNormalMonsters = 0;
    int aliveBosses = 0;
    int totalNormalMonsters = 0;
    int totalBosses = 0;

    // Count alive and total monsters/bosses.
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

    const bool normalMonstersCleared = (totalNormalMonsters > 0 && aliveNormalMonsters == 0);
    const bool bossesCleared = (totalBosses > 0 && aliveBosses == 0);

    // Level is complete if all normal monsters or all bosses are cleared.
    if (normalMonstersCleared || bossesCleared) {
        levelComplete = true;
        CreatePortal();
    }
}

// Returns whether the current level is complete.
bool UIManager::IsLevelComplete() const {
    return levelComplete;
}

// Creates a portal to the next level near the hero's position.
void UIManager::CreatePortal() {
    if (portalCreated || !hero || !currentMap) return; // Don't create if already exists or no hero/map

    const Position playerPos = hero->getCurrentPosition(); // Get hero's current position

    // Define directions to check for a wall to place the portal.
    const Position directions[] = {
        {playerPos.x + 1, playerPos.y}, // Right
        {playerPos.x, playerPos.y + 1}, // Down
        {playerPos.x - 1, playerPos.y}, // Left
        {playerPos.x, playerPos.y - 1} // Up
    };

    // Iterate through directions to find a suitable wall cell.
    for (const auto &dir: directions) {
        if (currentMap->getCell(dir) == '#') {
            // If cell is a wall
            Portal portal;
            portal.position = dir;
            portal.isActive = true;
            portal.animationTime = 0.0f;
            portals.push_back(portal); // Add portal to list
            portalCreated = true; // Mark portal as created

            // Replace wall with floor in the map so hero can step on it.
            currentMap->setCell(dir, '.');

            // Update the map renderer to reflect the change.
            UpdateMapRenderer();
            break; // Exit loop once portal is created
        }
    }
}

// Updates the character selection panel.
void UIManager::UpdateCharacterSelection(const float deltaTime) {
    if (characterSelectionPanel) {
        characterSelectionPanel->Update();

        if (characterSelectionPanel->IsSelectionConfirmed()) {
            SetState(UIState::GAMEPLAY); // Transition to gameplay after selection
        }
    }
}

// Draws the character selection panel with a background.
void UIManager::DrawCharacterSelection() const {
    // Draw a dark background.
    DrawRectangle(0, 0, screenWidth, screenHeight, {20, 10, 5, 255});

    if (characterSelectionPanel) {
        characterSelectionPanel->Draw();
    }
}

// Shows the character selection panel and sets the UI state.
void UIManager::ShowCharacterSelection() {
    if (characterSelectionPanel) {
        characterSelectionPanel->Reset(); // Reset panel state
        characterSelectionPanel->Show(); // Make panel visible
        SetState(UIState::CHARACTER_SELECTION); // Change UI state
    }
}

// Sets the currently selected race.
void UIManager::SetSelectedRace(const Race race) {
    selectedRace = race;
}

// Callback when a race is selected in the character creation.
void UIManager::OnRaceSelected(const Race race) {
    selectedRace = race;

    if (onRaceSelectedCallback) {
        onRaceSelectedCallback(race);
    }
}

// Callback when the back button is pressed in character selection.
void UIManager::OnCharacterSelectionBack() {
    SetState(UIState::MAIN_MENU); // Return to main menu
}

// Initiates a battle between the hero and a monster.
void UIManager::StartBattle(Hero *heroRef, Monster *monster) {
    if (!battlePanel || !heroRef || !monster || !battleSystem) return;

    currentBattleMonster = monster; // Store reference to current monster

    battleSystem->StartBattle(heroRef, monster); // Start battle logic
    battlePanel->StartBattle(heroRef, monster, battleSystem); // Initialize battle panel

    SetState(UIState::BATTLE); // Transition to battle state
}

// Ends the current battle and transitions back to gameplay.
void UIManager::EndBattle() {
    if (battlePanel) {
        const BattleResult result = battlePanel->GetResult(); // Get battle outcome
        OnBattleEnd(result); // Handle battle end result
    }

    currentBattleMonster = nullptr; // Clear current monster reference
    SetState(UIState::GAMEPLAY); // Return to gameplay
}

// Checks if a battle is currently active.
bool UIManager::IsBattleActive() const {
    return battleSystem && battleSystem->IsBattleActive();
}

// Returns the result of the battle.
BattleResult UIManager::GetBattleResult() const {
    return battlePanel ? battlePanel->GetResult() : BattleResult::ONGOING;
}

// Shows the equipment choice panel for a new item.
void UIManager::ShowEquipmentChoice(const Item *newItem) {
    if (!equipmentPanel || !hero || !newItem) return;

    if (currentState == UIState::EQUIPMENT_SELECTION) {
        return; // Avoid showing multiple equipment panels
    }

    const Item *currentItem = nullptr;

    // Determine the current item of the same type in hero's inventory.
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

    equipmentPanel->Show(currentItem, newItem); // Display comparison
    SetState(UIState::EQUIPMENT_SELECTION); // Change UI state
}

// Hides the equipment panel and returns to gameplay.
void UIManager::HideEquipmentPanel() {
    if (equipmentPanel) {
        equipmentPanel->Hide();
    }

    if (currentState == UIState::EQUIPMENT_SELECTION) {
        SetState(UIState::GAMEPLAY);
    }
}

// Checks if the equipment panel is currently visible.
bool UIManager::IsEquipmentPanelVisible() const {
    return equipmentPanel && equipmentPanel->IsVisible();
}

// Shows the level up panel with available points.
void UIManager::ShowLevelUpPanel(const int availablePoints) {
    if (!levelUpPanel || !hero) return;

    levelUpPanel->SetHeroReference(hero); // Set hero for the panel
    levelUpPanel->SetMaxPoints(availablePoints); // Set points for allocation
    levelUpPanel->Show(); // Make panel visible
    SetState(UIState::LEVEL_UP); // Change UI state
}

// Hides the level up panel and returns to gameplay.
void UIManager::HideLevelUpPanel() {
    if (levelUpPanel) {
        levelUpPanel->Hide();
    }
    SetState(UIState::GAMEPLAY);
}

// Checks if the level up panel is currently visible.
bool UIManager::IsLevelUpPanelVisible() const {
    return levelUpPanel && levelUpPanel->IsVisible();
}

// Sets the hero reference for the UIManager and initializes GameHUD.
void UIManager::SetHero(Hero *heroRef) {
    hero = heroRef;
    if (gameHUD) {
        gameHUD->Initialize(hero);
    }
}

// Returns the hero reference.
Hero *UIManager::GetHero() const {
    return hero;
}

// Sets the current map and passes it to the BattleSystem.
void UIManager::SetCurrentMap(Map *map) {
    currentMap = map;
    if (battleSystem) {
        battleSystem->SetMap(map);
    }
}

// Returns the current map reference.
Map *UIManager::GetCurrentMap() const {
    return currentMap;
}

// Sets the attack system reference.
void UIManager::SetAttackSystem(Attack *attack) {
    attackSystem = attack;
}

// Updates the stats displayed in the GameHUD.
void UIManager::UpdateHUDStats() {
    if (!gameHUD || !currentMap) return;

    gameHUD->SetLevel(currentLevel); // Update current level
    gameHUD->SetMonstersRemaining(currentMap->GetMonsterCount()); // Update remaining monsters
    gameHUD->SetTreasuresRemaining(currentMap->GetTreasureCount()); // Update remaining treasures
}

// Updates the map renderer with the current map and hero position.
void UIManager::UpdateMapRenderer() {
    if (mapRenderer && currentMap && hero) {
        mapRenderer->Initialize(currentMap, hero->GetPosition());
    }
}

// Updates the main menu logic.
void UIManager::UpdateMainMenu(const float deltaTime) {
    if (mainMenu) {
        mainMenu->Update(deltaTime);
        HandleMainMenuInput(); // Handle main menu selections
    }
}

// Updates gameplay elements like HUD and map, and checks for portal interaction.
void UIManager::UpdateGameplay(const float deltaTime) {
    if (gameHUD) {
        gameHUD->Update(deltaTime);
    }
    if (mapRenderer) {
        mapRenderer->Update(deltaTime);
    }

    HandlePortalInteraction(); // Check if hero is on a portal
}

// Updates the battle panel logic.
void UIManager::UpdateBattle(const float deltaTime) {
    if (battlePanel) {
        battlePanel->Update();

        if (battlePanel->isFinished()) {
            EndBattle(); // End battle if finished
        }
    }
}

// Updates the level up panel logic.
void UIManager::UpdateLevelUp(float deltaTime) {
    if (levelUpPanel) {
        levelUpPanel->Update();
    }
}

// Updates the equipment selection panel logic.
void UIManager::UpdateEquipmentSelection(float deltaTime) {
    if (equipmentPanel) {
        equipmentPanel->Update();
    }
}

// Manages the level transition animation and state.
void UIManager::UpdateLevelTransition(const float deltaTime) {
    transitionTimer += deltaTime; // Increment transition timer

    // After a delay, show the level up panel and reset transition.
    if (transitionTimer >= 2.0f) {
        const int levelUpPoints = 30; // Hardcoded level up points
        ShowLevelUpPanel(levelUpPoints);

        transitionTimer = 0.0f;
        isTransitioning = false;
    }
}

// Updates the defeat panel logic.
void UIManager::UpdateDefeat(const float deltaTime) {
    if (defeatPanel) {
        defeatPanel->Update();
    }
}

// Draws the main menu.
void UIManager::DrawMainMenu() const {
    if (mainMenu) {
        mainMenu->Draw();
    }
}

// Draws gameplay elements: HUD, map, and portals.
void UIManager::DrawGameplay() const {
    if (gameHUD) {
        gameHUD->Draw();
    }
    if (mapRenderer) {
        mapRenderer->Draw();
    }

    DrawPortals(); // Draw active portals
}

// Draws battle elements on top of gameplay elements.
void UIManager::DrawBattle() const {
    DrawGameplay(); // Draw background gameplay

    if (battlePanel) {
        battlePanel->Draw();
    }
}

// Draws level up panel on top of gameplay elements.
void UIManager::DrawLevelUp() const {
    DrawGameplay(); // Draw background gameplay

    if (levelUpPanel) {
        levelUpPanel->Draw();
    }
}

// Draws equipment selection panel on top of gameplay elements.
void UIManager::DrawEquipmentSelection() const {
    DrawGameplay(); // Draw background gameplay

    if (equipmentPanel) {
        equipmentPanel->Draw();
    }
}

// Draws a visual transition effect when moving between levels.
void UIManager::DrawLevelTransition() const {
    DrawGameplay(); // Draw background gameplay

    // Calculate alpha for a fading effect.
    const float alpha = std::sin(transitionTimer * 2.0f) * 127 + 128;
    // Draw a semi-transparent black rectangle over the screen.
    DrawRectangle(0, 0, screenWidth, screenHeight, {0, 0, 0, static_cast<unsigned char>(alpha)});

    // Draw "Level Complete!" text.
    const std::string text = "Level Complete! Preparing next level...";
    const int textWidth = MeasureText(text.c_str(), 40);
    DrawText(text.c_str(), (screenWidth - textWidth) / 2, screenHeight / 2 - 20, 40, WHITE);
}

// Draws the defeat panel.
void UIManager::DrawDefeat() const {
    if (defeatPanel) {
        defeatPanel->Draw();
    }
}

// Draws active portals on the map.
void UIManager::DrawPortals() const {
    if (!mapRenderer || portals.empty()) return;

    for (const auto &portal: portals) {
        if (portal.isActive) {
            mapRenderer->DrawPortal(portal.position, portal.animationTime); // Draw each portal with its animation
        }
    }
}

// Handles input events for the main menu.
void UIManager::HandleMainMenuInput() {
    if (!mainMenu) return;

    if (mainMenu->IsStartGameSelected()) {
        ShowCharacterSelection(); // Transition to character selection
        mainMenu->ResetSelections(); // Reset main menu choices
    } else if (mainMenu->IsLoadGameSelected()) {
        mainMenu->ResetSelections(); // Reset main menu choices (Load Game not fully implemented yet)
    }
}

// Checks if the hero is interacting with an active portal.
void UIManager::HandlePortalInteraction() {
    if (!hero || portals.empty()) return;

    const Position &heroPos = hero->getCurrentPosition();
    for (const auto &portal: portals) {
        if (portal.isActive &&
            heroPos.x == portal.position.x &&
            heroPos.y == portal.position.y) {
            // If hero is on a portal
            isTransitioning = true; // Start level transition
            SetState(UIState::LEVEL_TRANSITION); // Change UI state to transition
            break;
        }
    }
}

// Initializes gameplay elements after a new game or load game.
void UIManager::InitializeGameplay() {
    if (gameHUD && hero) {
        gameHUD->Initialize(hero); // Initialize HUD with hero stats
    }

    UpdateMapRenderer(); // Ensure map is rendered correctly
    UpdateHUDStats(); // Update HUD stats
}

// Reinitializes GameHUD with proper item references after hero recreation
void UIManager::ReinitializeGameHUD() {
    if (!gameHUD || !hero) return;

    // Reinitialize the GameHUD with the new hero
    gameHUD->Initialize(hero);

    // Set the item references for the inventory buttons
    gameHUD->SetWeapon(&hero->GetInventory().GetWeapon());
    gameHUD->SetArmor(&hero->GetInventory().GetArmor());
    gameHUD->SetSpell(&hero->GetInventory().GetSpell());

    // Update HUD stats as well
    UpdateHUDStats();
}

// Cleans up gameplay-specific states (e.g., clear portals, reset flags).
void UIManager::CleanupGameplay() {
    portals.clear(); // Clear all portals
    portalCreated = false; // Reset portal creation flag
    levelComplete = false; // Reset level completion flag
    currentBattleMonster = nullptr; // Clear current battle monster
}

// Resets level-specific flags and timers.
void UIManager::ResetLevelState() {
    portals.clear();
    portalCreated = false;
    levelComplete = false;
    transitionTimer = 0.0f;
    isTransitioning = false;
}

// Checks if all normal monsters on the current map are defeated.
bool UIManager::AreAllMonstersDefeated() const {
    if (!currentMap) return false;

    const auto &monsters = currentMap->getMonstersConst();

    for (const Monster &monster: monsters) {
        if (monster.GetType() == MonsterType::MONSTER && !monster.isDefeated()) {
            return false; // Found an alive normal monster
        }
    }

    return true; // All normal monsters are defeated
}

// Checks if all bosses on the current map are defeated.
bool UIManager::AreAllBossesDefeated() const {
    if (!currentMap) return false;

    const auto &monsters = currentMap->getMonsters();

    for (const Monster &monster: monsters) {
        if (monster.GetType() == MonsterType::BOSS && !monster.isDefeated()) {
            return false; // Found an alive boss
        }
    }

    return true; // All bosses are defeated
}

// Checks if a given position on the map is a wall.
bool UIManager::IsPositionWall(const Position &pos) const {
    if (!currentMap) return false;
    return currentMap->getCell(pos) == '#';
}

// Callback when level up stats are confirmed.
void UIManager::OnLevelUpConfirm(const int str, const int mana, const float health) {
    if (hero) {
        hero->levelUp(str, mana, health); // Apply stat increases to hero
    }

    LoadLevel(currentLevel); // Reload current level (to refresh map if needed, e.g., monster positions)

    HideLevelUpPanel(); // Hide the panel
}

// Callback when level up is canceled.
void UIManager::OnLevelUpCancel() {
    HideLevelUpPanel(); // Hide the panel
}

// Callback for keeping current equipment (no change).
void UIManager::OnEquipmentKeep() {
    HideEquipmentPanel();
}

// Callback for equipping new item.
void UIManager::OnEquipmentEquip() {
    if (!hero || !equipmentPanel || !pendingItem) return;

    // Get non-const reference to inventory for modifications
    Inventory &inventory = hero->GetInventory(); // This needs to be non-const

    // Actually equip the item to the hero's inventory using the correct methods
    switch (pendingItem->GetType()) {
        case ItemType::ARMOR: {
            const Armor *newArmor = dynamic_cast<const Armor *>(pendingItem.get());
            if (newArmor) {
                inventory.newArmor(*newArmor);
            }
            break;
        }
        case ItemType::WEAPON: {
            const Weapon *newWeapon = dynamic_cast<const Weapon *>(pendingItem.get());
            if (newWeapon) {
                inventory.newWeapon(*newWeapon);
            }
            break;
        }
        case ItemType::SPELL: {
            const Spell *newSpell = dynamic_cast<const Spell *>(pendingItem.get());
            if (newSpell) {
                inventory.newSpell(*newSpell);
            }
            break;
        }
        default:
            break;
    }

    // Now update the GameHUD with the new item references
    if (gameHUD) {
        gameHUD->SetWeapon(&inventory.GetWeapon());
        gameHUD->SetArmor(&inventory.GetArmor());
        gameHUD->SetSpell(&inventory.GetSpell());
    }

    // Clear the pending item after successful equip
    pendingItem.reset();

    HideEquipmentPanel();
}

void UIManager::UpdateInventoryButtons() {
    if (gameHUD && hero) {
        const Inventory &inventory = hero->GetInventory();
        gameHUD->SetWeapon(&inventory.GetWeapon());
        gameHUD->SetArmor(&inventory.GetArmor());
        gameHUD->SetSpell(&inventory.GetSpell());
    }
}

// Callback for handling the end of a battle.
void UIManager::OnBattleEnd(const BattleResult result) {
    // Notify the battle panel about the end result.
    if (battlePanel) {
        battlePanel->OnBattleEnd(result);
    }

    // If player won, update map (remove defeated monster) and check for level completion.
    if (result == BattleResult::PLAYER_WON && currentBattleMonster && currentMap) {
        currentMap->setCell(*hero->GetPosition(), '.'); // Replace monster cell with floor
        UpdateMapRenderer(); // Refresh map rendering
        UpdateHUDStats(); // Update HUD stats (e.g., monster count)

        CheckLevelCompletion(); // Check if level is now complete
    }

    // If player lost, show defeat panel and change UI state.
    if (result == BattleResult::PLAYER_LOST) {
        if (!defeatPanel) {
            defeatPanel = new DefeatPanel(); // Create defeat panel if it doesn't exist
        }
        defeatPanel->Show(); // Show defeat panel
        SetState(UIState::DEFEAT); // Transition to defeat state
    }
}

// Checks if a battle should start at the new position.
bool UIManager::CheckForBattle(const Position &newPosition) {
    if (!hero || !battleSystem) return false;

    // Use battle system to determine if a battle should occur.
    if (battleSystem->CheckForBattle(hero, newPosition)) {
        SetState(UIState::BATTLE); // If battle starts, transition to battle state
        return true;
    }
    return false;
}

void UIManager::ShowEquipmentChoice(std::unique_ptr<Item> newItem) {
    if (!equipmentPanel || !hero || !newItem) return;

    if (currentState == UIState::EQUIPMENT_SELECTION) {
        return; // Avoid showing multiple equipment panels
    }

    // Store the item with ownership
    pendingItem = std::move(newItem);

    const Item *currentItem = nullptr;
    switch (pendingItem->GetType()) {
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

    equipmentPanel->Show(currentItem, pendingItem.get());
    SetState(UIState::EQUIPMENT_SELECTION);
}
