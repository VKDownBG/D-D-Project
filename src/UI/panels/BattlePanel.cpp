#include "C:/DandD/include/UI/panels/BattlePanel.h"
#include <algorithm>
#include <cmath>
#include <iomanip>

// Constructor for BattlePanel. Initializes UI state, colors, fonts, and button properties.
BattlePanel::BattlePanel()
    : currentState(BattleState::HIDDEN), // Initial state is hidden
      battleResult(BattleResult::ONGOING), // Battle result is ongoing by default
      animationProgress(0.0f), // Animation progress for panel entry/exit
      animationSpeed(3.0f), // Speed of the panel animation
      maxLogLines(8), // Maximum number of lines to display in the battle log
      logScrollOffset(0.0f), // Not currently used for scrolling, but could be implemented
      player(nullptr), // Pointer to the hero (player character)
      currentMonster(nullptr), // Pointer to the current monster in battle
      battleSystem(nullptr), // Pointer to the BattleSystem for delegating game logic
      panelColor({30, 30, 30, 240}), // Color for the main battle panel background
      headerColor({50, 50, 50, 255}), // Color for the header section
      logBackgroundColor({20, 20, 20, 255}), // Color for the battle log background
      textColor(WHITE), // Default text color
      uiFont(GetFontDefault()), // Default font for UI text
      isPlayerTurn(true), // Flag to track whose turn it is
      waitingForInput(false) {
    // Flag to indicate if the panel is waiting for player input

    // Get screen dimensions for responsive UI layout.
    float screenWidth = GetScreenWidth();
    float screenHeight = GetScreenHeight();

    // Define the target size and position of the battle panel when active.
    const float panelWidth = screenWidth * 0.8f;
    const float panelHeight = screenHeight * 0.8f;

    targetBounds = {
        (screenWidth - panelWidth) / 2, // Center horizontally
        (screenHeight - panelHeight) / 2, // Center vertically
        panelWidth,
        panelHeight
    };

    // Define the initial hidden position of the panel (off-screen at the bottom).
    hiddenBounds = {
        targetBounds.x,
        screenHeight, // Panel starts below the screen
        panelWidth,
        panelHeight
    };

    // Current panel bounds, initialized to hidden.
    panelBounds = hiddenBounds;

    // Initialize buttons for player actions.
    const float buttonWidth = 120;
    const float buttonHeight = 40;
    const float buttonY = targetBounds.y + targetBounds.height - 60; // Position buttons near the bottom of the panel
    const float buttonSpacing = 20;
    // Calculate starting X position to center the group of buttons.
    const float startX = targetBounds.x + (targetBounds.width - (3 * buttonWidth + 2 * buttonSpacing)) / 2;

    // Weapon Attack Button
    weaponButton = Button(
        {startX, buttonY, buttonWidth, buttonHeight},
        "Weapon",
        [this]() { onWeaponAttack(); } // Callback for weapon attack
    );
    weaponButton.SetColors(DARKGREEN, GREEN, LIGHTGRAY, WHITE); // Set button colors
    weaponButton.SetRoundedCorners(5.0f); // Apply rounded corners

    // Spell Attack Button
    spellButton = Button(
        {startX + buttonWidth + buttonSpacing, buttonY, buttonWidth, buttonHeight},
        "Spell",
        [this]() { onSpellAttack(); } // Callback for spell attack
    );
    spellButton.SetColors(DARKBLUE, BLUE, LIGHTGRAY, WHITE);
    spellButton.SetRoundedCorners(5.0f);

    // Flee Button
    fleeButton = Button(
        {startX + 2 * (buttonWidth + buttonSpacing), buttonY, buttonWidth, buttonHeight},
        "Flee",
        [this]() { onFlee(); } // Callback for fleeing
    );
    fleeButton.SetColors(MAROON, RED, PINK, WHITE);
    fleeButton.SetRoundedCorners(5.0f);
}

// Starts a battle by setting up player, monster, and battle system references, then animating the panel in.
void BattlePanel::StartBattle(Hero *_player, Monster *monster, BattleSystem *battleSys) {
    if (!_player || !monster || !battleSys) return; // Ensure all references are valid

    player = _player;
    currentMonster = monster;
    battleSystem = battleSys;

    currentState = BattleState::ANIMATING_IN; // Begin animation to show the panel
    battleResult = BattleResult::ONGOING; // Reset battle result
    animationProgress = 0.0f; // Reset animation progress
    isPlayerTurn = true; // Player always starts first
    waitingForInput = true; // Wait for player's action

    clearLog(); // Clear previous battle log entries
    addLogEntry("Battle begins!"); // Add initial log message
}

// Updates the state of the battle panel, including animations and input handling.
void BattlePanel::Update() {
    switch (currentState) {
        case BattleState::ANIMATING_IN:
        case BattleState::ANIMATING_OUT:
            // Update animation progress based on direction (in or out) and frame time.
            animationProgress = std::clamp(animationProgress +
                                           GetFrameTime() * animationSpeed * (currentState == BattleState::ANIMATING_IN
                                                                                  ? 1
                                                                                  : -1),
                                           0.0f, 1.0f);

            // Transition to ACTIVE state if animation is complete (in).
            if (animationProgress == 1.0f) currentState = BattleState::ACTIVE;
            // Transition to FINISHED state if animation is complete (out).
            if (animationProgress == 0.0f) currentState = BattleState::FINISHED;

            // Interpolate panel bounds for smooth animation using easeOutCubic.
            panelBounds = lerpRectangle(hiddenBounds, targetBounds, easeOutCubic(animationProgress));
            break;

        case BattleState::ACTIVE:
            if (player && currentMonster && battleSystem) {
                // Check if the battle has ended according to the BattleSystem.
                if (!battleSystem->IsBattleActive()) {
                    // If battle ended, animate the panel out. The result is handled by OnBattleEnd callback.
                    currentState = BattleState::ANIMATING_OUT;
                } else if (isPlayerTurn && waitingForInput) {
                    // If it's player's turn and input is awaited, update button states.
                    weaponButton.Update(GetMousePosition());
                    spellButton.Update(GetMousePosition());
                    fleeButton.Update(GetMousePosition());
                }
            }
            break;

        default:
            break;
    }
}

// Draws the battle panel and its components.
void BattlePanel::Draw() const {
    // Don't draw if the panel is hidden or finished animating out.
    if (currentState == BattleState::HIDDEN || currentState == BattleState::FINISHED) {
        return;
    }

    // Draw a semi-transparent dark overlay over the entire screen.
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), {0, 0, 0, 150});

    // Draw the main panel background and border.
    drawPanel();
    // Draw the header with player and monster names/levels.
    drawHeader();
    // Draw health bars for player and monster.
    drawHealthBars();
    // Draw the battle log.
    drawBattleLog();

    // Only draw action buttons if it's the player's turn and input is awaited.
    if (currentState == BattleState::ACTIVE && isPlayerTurn && waitingForInput) {
        drawButtons();
    }
}

// Handles the player's weapon attack action.
void BattlePanel::onWeaponAttack() {
    // Only allow action if waiting for input, it's player's turn, and battle system is valid.
    if (!waitingForInput || !isPlayerTurn || !battleSystem) return;

    // Delegate the attack logic to the BattleSystem.
    const double damage = battleSystem->PerformPlayerAttack(AttackType::WEAPON);

    // Add a log entry describing the attack and damage.
    const std::string attackName = player->GetInventory().GetWeapon().GetName();
    const std::string logEntry = player->GetName() + " used " + attackName + "! It dealt " +
                                 std::to_string(static_cast<int>(damage)) + " damage.";
    addLogEntry(logEntry);

    waitingForInput = false; // Player has made a choice, no longer waiting for input

    // Check if the battle is still active after the player's attack.
    if (battleSystem->IsBattleActive()) {
        isPlayerTurn = false; // Switch to monster's turn
        HandleMonsterTurn(); // Execute monster's turn
    }
}

// Handles the player's spell attack action.
void BattlePanel::onSpellAttack() {
    if (!waitingForInput || !isPlayerTurn || !battleSystem) return;

    // Delegate the spell attack logic to the BattleSystem.
    double damage = battleSystem->PerformPlayerAttack(AttackType::SPELL);

    // Add a log entry describing the spell attack and damage.
    const std::string attackName = player->GetInventory().GetSpell().GetName();
    const std::string logEntry = player->GetName() + " used " + attackName + "! It dealt " +
                                 std::to_string(static_cast<int>(damage)) + " damage.";
    addLogEntry(logEntry);

    waitingForInput = false; // Player has made a choice

    // Check if the battle is still active after the player's attack.
    if (battleSystem->IsBattleActive()) {
        isPlayerTurn = false; // Switch to monster's turn
        HandleMonsterTurn(); // Execute monster's turn
    }
}

// Handles the player's attempt to flee from battle.
void BattlePanel::onFlee() {
    if (!waitingForInput || !isPlayerTurn || !battleSystem) return;

    // Delegate fleeing logic to the BattleSystem.
    if (battleSystem->CanPlayerFlee()) {
        addLogEntry(player->GetName() + " fled from battle!");
        battleSystem->PlayerFlee(); // Inform battle system of flee
    } else {
        addLogEntry("Cannot flee from this battle!");
        waitingForInput = true; // If flee failed, player can choose another action
    }
}

// Handles the monster's turn in battle.
void BattlePanel::HandleMonsterTurn() {
    if (!battleSystem || !battleSystem->IsBattleActive()) return;

    // Delegate monster attack logic to the BattleSystem.
    double damage = battleSystem->PerformMonsterAttack();

    // Add a log entry describing the monster's attack.
    const std::string logEntry = currentMonster->GetName() + " attacked! You lost " +
                                 std::to_string(static_cast<int>(damage)) + " HP.";
    addLogEntry(logEntry);

    // Check if the battle is still active after the monster's attack.
    if (battleSystem->IsBattleActive()) {
        isPlayerTurn = true; // Switch back to player's turn
        waitingForInput = true; // Wait for player's action
    }
}

// Callback method called by UIManager or BattleSystem when the battle concludes.
void BattlePanel::OnBattleEnd(const BattleResult result) {
    battleResult = result; // Store the battle outcome

    // Add a log entry based on the battle result.
    switch (result) {
        case BattleResult::PLAYER_WON:
            addLogEntry("Victory! " + player->GetName() + " wins!");
            break;
        case BattleResult::PLAYER_LOST:
            addLogEntry("Defeat! " + player->GetName() + " was defeated!");
            break;
        case BattleResult::PLAYER_FLED:
            addLogEntry("You escaped from battle!");
            break;
        default:
            break;
    }

    currentState = BattleState::ANIMATING_OUT; // Begin animation to hide the panel
}

// Draws the main rounded rectangle background of the battle panel.
void BattlePanel::drawPanel() const {
    DrawRectangleRounded(panelBounds, 0.05f, 10, panelColor); // Draw filled rounded rectangle
    DrawRectangleRoundedLines(panelBounds, 0.05f, 10, 2, WHITE); // Draw rounded border
}

// Draws the header section with player and monster information.
void BattlePanel::drawHeader() const {
    if (!player || !currentMonster) return;

    // Define the rectangle for the header.
    const Rectangle headerRect = {
        panelBounds.x + 10,
        panelBounds.y + 10,
        panelBounds.width - 20,
        60
    };

    DrawRectangleRounded(headerRect, 0.1f, 10, headerColor); // Draw rounded header background

    // Draw player information.
    const std::string heroInfo = player->GetName() + " | Level: " + std::to_string(player->GetLevel());
    DrawTextEx(uiFont, heroInfo.c_str(),
               {headerRect.x + 10, headerRect.y + 10}, 20, 1, WHITE);

    // Draw monster information (right-aligned).
    std::string monsterInfo = currentMonster->GetName() + " | Level: " + std::to_string(currentMonster->GetLevel());
    Vector2 monsterTextSize = MeasureTextEx(uiFont, monsterInfo.c_str(), 20, 1);
    DrawTextEx(uiFont, monsterInfo.c_str(),
               {headerRect.x + headerRect.width - monsterTextSize.x - 10, headerRect.y + 10},
               20, 1, textColor);
}

// Draws the health bars for the player and the current monster.
void BattlePanel::drawHealthBars() const {
    if (!player || !currentMonster) return;

    const float barWidth = 200;
    const float barHeight = 20;
    const float barY = panelBounds.y + 80;

    // Hero Health Bar
    const Rectangle heroHealthBg = {panelBounds.x + 20, barY, barWidth, barHeight};
    // Calculate the fill percentage for the health bar.
    const Rectangle heroHealthFg = {
        heroHealthBg.x, heroHealthBg.y,
        barWidth * (player->GetHealth() / player->GetMaxHealth()),
        barHeight
    };

    DrawRectangleRec(heroHealthBg, DARKGRAY); // Background for health bar
    DrawRectangleRec(heroHealthFg, GREEN); // Foreground (filled part)
    DrawRectangleLinesEx(heroHealthBg, 1, WHITE); // Border

    // Display hero's health text.
    std::ostringstream healthStreamHero;
    healthStreamHero << std::fixed << std::setprecision(1)
            << player->GetHealth() << "/"
            << player->GetMaxHealth();

    const std::string heroHealth = healthStreamHero.str();

    DrawTextEx(uiFont, heroHealth.c_str(),
               {heroHealthBg.x + 5, heroHealthBg.y + 2}, 16, 1, WHITE);

    // Monster Health Bar (right-aligned)
    const Rectangle monsterHealthBg = {
        panelBounds.x + panelBounds.width - barWidth - 20,
        barY, barWidth, barHeight
    };
    // Calculate the fill percentage for the monster's health bar.
    const Rectangle monsterHealthFg = {
        monsterHealthBg.x, monsterHealthBg.y,
        barWidth * (currentMonster->GetHealth() / currentMonster->GetMaxHealth()),
        barHeight
    };

    DrawRectangleRec(monsterHealthBg, DARKGRAY);
    DrawRectangleRec(monsterHealthFg, RED);
    DrawRectangleLinesEx(monsterHealthBg, 1, WHITE);

    // Display monster's health text.
    std::ostringstream healthStreamMonster;
    healthStreamMonster << std::fixed << std::setprecision(1)
            << currentMonster->GetHealth() << "/"
            << currentMonster->GetMaxHealth();

    const std::string monsterHealth = healthStreamMonster.str();

    Vector2 monsterHealthSize = MeasureTextEx(uiFont, monsterHealth.c_str(), 16, 1);
    DrawTextEx(uiFont, monsterHealth.c_str(),
               {monsterHealthBg.x + monsterHealthBg.width - monsterHealthSize.x - 5, monsterHealthBg.y + 2},
               16, 1, WHITE);
}

// Draws the battle log area.
void BattlePanel::drawBattleLog() const {
    // Define the rectangle for the battle log.
    const Rectangle logRect = {
        panelBounds.x + 20,
        panelBounds.y + 120,
        panelBounds.width - 40,
        panelBounds.height - 220
    };

    DrawRectangleRounded(logRect, 0.05f, 10, logBackgroundColor); // Draw log background
    DrawRectangleRoundedLines(logRect, 0.05f, 10, 1, GRAY); // Draw log border

    DrawTextEx(uiFont, "Battle Log",
               {logRect.x + 10, logRect.y + 5}, 18, 1, LIGHTGRAY); // Log title

    float lineHeight = 25; // Spacing between log lines
    float textY = logRect.y + 35; // Starting Y position for text

    // Determine the starting index to display only the most recent 'maxLogLines' entries.
    const int startIndex = std::max(0, static_cast<int>(battleLog.size()) - maxLogLines);
    for (int i = startIndex; i < battleLog.size(); i++) {
        DrawTextEx(uiFont, battleLog[i].c_str(),
                   {logRect.x + 10, textY}, 16, 1, textColor);
        textY += lineHeight; // Move to the next line
    }
}

// Draws the player action buttons.
void BattlePanel::drawButtons() const {
    if (!player) return;

    // Get names of equipped weapon and spell for button labels.
    const std::string weaponLabel = player->GetInventory().GetWeapon().GetName();
    const std::string spellLabel = player->GetInventory().GetSpell().GetName();

    weaponButton.Draw(); // Draw weapon button
    spellButton.Draw(); // Draw spell button
    fleeButton.Draw(); // Draw flee button

    // Get button bounds to position the text correctly.
    const Rectangle weaponBounds = weaponButton.GetBounds();
    const Rectangle spellBounds = spellButton.GetBounds();

    // Measure text size for centering.
    const Vector2 weaponTextSize = MeasureTextEx(uiFont, weaponLabel.c_str(), 14, 1);
    const Vector2 spellTextSize = MeasureTextEx(uiFont, spellLabel.c_str(), 14, 1);

    // Draw weapon name below the weapon button.
    DrawTextEx(uiFont, weaponLabel.c_str(),
               {
                   weaponBounds.x + (weaponBounds.width - weaponTextSize.x) / 2,
                   weaponBounds.y + (weaponBounds.height - weaponTextSize.y) / 2 + 15
               },
               14, 1, LIGHTGRAY
    );

    // Draw spell name below the spell button.
    DrawTextEx(uiFont, spellLabel.c_str(),
               {
                   spellBounds.x + (spellBounds.width - spellTextSize.x) / 2,
                   spellBounds.y + (spellBounds.height - spellTextSize.y) / 2 + 15
               }, 14, 1, LIGHTGRAY);
}

// Adds a new entry to the battle log, managing the maximum number of lines.
void BattlePanel::addLogEntry(const std::string &entry) {
    battleLog.push_back(entry); // Add entry to the end of the log
    if (battleLog.size() > maxLogLines) {
        battleLog.erase(battleLog.begin()); // Remove the oldest entry if log exceeds max lines
    }
}

// Clears all entries from the battle log.
void BattlePanel::clearLog() {
    battleLog.clear();
}

// Checks if the battle panel is currently active or animating in.
bool BattlePanel::IsActive() const {
    return currentState == BattleState::ACTIVE || currentState == BattleState::ANIMATING_IN;
}

// Checks if the battle panel has finished animating out.
bool BattlePanel::isFinished() const {
    return currentState == BattleState::FINISHED;
}

// Returns the final result of the battle.
BattleResult BattlePanel::GetResult() const {
    return battleResult;
}

// Sets the font to be used for UI text.
void BattlePanel::SetFont(const Font &font) {
    uiFont = font;
}

// Sets the colors for various parts of the battle panel.
void BattlePanel::SetColors(const Color panel, const Color header, const Color logBg, const Color text) {
    panelColor = panel;
    headerColor = header;
    logBackgroundColor = logBg;
    textColor = text;
}

// Sets the animation speed for panel entry and exit.
void BattlePanel::SetAnimationSpeed(const float speed) {
    animationSpeed = speed;
}

// Easing function for smooth animation (cubic ease-out).
float BattlePanel::easeOutCubic(const float t) {
    return 1.0f - std::pow(1.0f - t, 3.0f);
}

// Linear interpolation between two rectangles. Used for animating panel bounds.
Rectangle BattlePanel::lerpRectangle(const Rectangle start, const Rectangle end, const float t) {
    return {
        start.x + (end.x - start.x) * t,
        start.y + (end.y - start.y) * t,
        start.width + (end.width - start.width) * t,
        start.height + (end.height - start.height) * t
    };
}
