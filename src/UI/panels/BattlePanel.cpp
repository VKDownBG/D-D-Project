//
// Created by Lenovo on 24.5.2025 г.
//

#include "C:/DandD/include/UI/panels/BattlePanel.h"
#include <algorithm>
#include <cmath>

BattlePanel::BattlePanel()
    : currentState(BattleState::HIDDEN),
      battleResult(BattleResult::ONGOING),
      animationProgress(0.0f),
      animationSpeed(3.0f),
      maxLogLines(8),
      logScrollOffset(0.0f),
      player(nullptr),
      currentMonster(nullptr),
      battleSystem(nullptr),
      panelColor({30, 30, 30, 240}),
      headerColor({50, 50, 50, 255}),
      logBackgroundColor({20, 20, 20, 255}),
      textColor(WHITE),
      uiFont(GetFontDefault()),
      isPlayerTurn(true),
      waitingForInput(false) {
    float screenWidth = GetScreenWidth();
    float screenHeight = GetScreenHeight();

    const float panelWidth = screenWidth * 0.8f;
    const float panelHeight = screenHeight * 0.8f;

    targetBounds = {
        (screenWidth - panelWidth) / 2,
        (screenHeight - panelHeight) / 2,
        panelWidth,
        panelHeight
    };

    hiddenBounds = {
        targetBounds.x,
        screenHeight,
        panelWidth,
        panelHeight
    };

    panelBounds = hiddenBounds;

    const float buttonWidth = 120;
    const float buttonHeight = 40;
    const float buttonY = targetBounds.y + targetBounds.height - 60;
    const float buttonSpacing = 20;
    const float startX = targetBounds.x + (targetBounds.width - (3 * buttonWidth + 2 * buttonSpacing)) / 2;

    weaponButton = Button(
        {startX, buttonY, buttonWidth, buttonHeight},
        "Weapon",
        [this]() { onWeaponAttack(); }
    );
    weaponButton.SetColors(DARKGREEN,GREEN, LIGHTGRAY,WHITE);
    weaponButton.SetRoundedCorners(5.0f);

    spellButton = Button(
        {startX + buttonWidth + buttonSpacing, buttonY, buttonWidth, buttonHeight},
        "Spell",
        [this]() { onSpellAttack(); }
    );
    spellButton.SetColors(DARKBLUE,BLUE, LIGHTGRAY,WHITE);
    spellButton.SetRoundedCorners(5.0f);

    fleeButton = Button(
        {startX + 2 * (buttonWidth + buttonSpacing), buttonY, buttonWidth, buttonHeight},
        "Flee",
        [this]() { onFlee(); }
    );

    fleeButton.SetColors(MAROON, RED, PINK, WHITE);
    fleeButton.SetRoundedCorners(5.0f);
}

void BattlePanel::StartBattle(Hero *_player, Monster *monster, Attack *attackSystem) {
    player = _player;
    currentMonster = monster;
    battleSystem = attackSystem;

    currentState = BattleState::ANIMATING_IN;
    battleResult = BattleResult::ONGOING;
    animationProgress = 0.0f;
    isPlayerTurn = true;
    waitingForInput = true;

    clearLog();
    addLogEntry("Battle begins!");
}

void BattlePanel::Update() {
    switch (currentState) {
        case BattleState::ANIMATING_IN:
            animationProgress += GetFrameTime() * animationSpeed;
            if (animationProgress >= 1.0f) {
                animationProgress = 1.0f;
                currentState = BattleState::ACTIVE;
            }
            panelBounds = lerpRectangle(hiddenBounds, targetBounds, easeOutCubic(animationProgress));
            break;

        case BattleState::ANIMATING_OUT:
            animationProgress -= GetFrameTime() * animationSpeed;
            if (animationProgress <= 0.0f) {
                animationProgress = 0.0f;
                currentState = BattleState::FINISHED;
            }
            panelBounds = lerpRectangle(hiddenBounds, targetBounds, easeOutCubic(animationProgress));
            break;

        case BattleState::ACTIVE:
            if (player && currentMonster) {
                if (player->isDefeated()) {
                    EndBattle(BattleResult::PLAYER_LOST);
                } else if (currentMonster->isDefeated()) {
                    EndBattle(BattleResult::PLAYER_WON);
                } else if (isPlayerTurn) {
                    if (waitingForInput) {
                        weaponButton.Update({0, 0});
                        spellButton.Update({0, 0});
                        fleeButton.Update({0, 0});
                    } else {
                        isPlayerTurn = false;
                    }
                } else if (!isPlayerTurn) {
                    HandleMonsterAttack();
                }
            }
            break;

        default:
            break;
    }
}

void BattlePanel::Draw() const {
    if (currentState == BattleState::HIDDEN || currentState == BattleState::FINISHED) {
        return;
    }

    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), {0, 0, 0, 150});

    drawPanel();
    drawHeader();
    drawHealthBars();
    drawBattleLog();

    if (currentState == BattleState::ACTIVE && isPlayerTurn && waitingForInput) {
        drawButtons();
    }
}

void BattlePanel::drawPanel() const {
    DrawRectangleRounded(panelBounds, 0.05f, 10, panelColor);
    DrawRectangleRoundedLines(panelBounds, 0.05f, 10, 2, WHITE);
}

void BattlePanel::drawHeader() const {
    if (!player || !currentMonster) return;

    const Rectangle headerRect = {
        panelBounds.x + 10,
        panelBounds.y + 10,
        panelBounds.width - 20,
        60
    };

    DrawRectangleRounded(headerRect, 0.1f, 10, headerColor);

    const std::string heroInfo = player->GetName() + " | Level: " + std::to_string(player->GetLevel());
    DrawTextEx(uiFont, heroInfo.c_str(),
               {headerRect.x + 10, headerRect.y + 10}, 20, 1, WHITE);

    std::string monsterInfo = currentMonster->GetName() + " | Level: " + std::to_string(currentMonster->GetLevel());
    Vector2 monsterTextSize = MeasureTextEx(uiFont, monsterInfo.c_str(), 20, 1);
    DrawTextEx(uiFont, monsterInfo.c_str(),
               {headerRect.x + headerRect.width - monsterTextSize.x - 10, headerRect.y + 10},
               20, 1, textColor);
}

void BattlePanel::drawHealthBars() const {
    if (!player || !currentMonster) return;

    const float barWidth = 200;
    const float barHeight = 20;
    const float barY = panelBounds.y + 80;

    const Rectangle heroHealthBg = {panelBounds.x + 20, barY, barWidth, barHeight};
    const Rectangle heroHealthFg = {
        heroHealthBg.x, heroHealthBg.y,
        barWidth * (player->GetHealth() / player->GetMaxHealth()),
        barHeight
    };

    DrawRectangleRec(heroHealthBg, DARKGRAY);
    DrawRectangleRec(heroHealthFg, GREEN);
    DrawRectangleLinesEx(heroHealthBg, 1, WHITE);

    const std::string heroHealth = std::to_string(static_cast<int>(player->GetHealth())) + "/" +
                                   std::to_string(player->GetMaxHealth());
    DrawTextEx(uiFont, heroHealth.c_str(),
               {heroHealthBg.x + 5, heroHealthBg.y + 2}, 16, 1, WHITE);

    const Rectangle monsterHealthBg = {
        panelBounds.x + panelBounds.width - barWidth - 20,
        barY, barWidth, barHeight
    };
    const Rectangle monsterHealthFg = {
        monsterHealthBg.x, monsterHealthBg.y,
        barWidth * (currentMonster->GetHealth() / currentMonster->GetMaxHealth()),
        barHeight
    };

    DrawRectangleRec(monsterHealthBg, DARKGRAY);
    DrawRectangleRec(monsterHealthFg, RED);
    DrawRectangleLinesEx(monsterHealthBg, 1, WHITE);

    const std::string monsterHealth = std::to_string(static_cast<int>(currentMonster->GetHealth())) + "/" +
                                      std::to_string(currentMonster->GetMaxHealth());
    Vector2 monsterHealthSize = MeasureTextEx(uiFont, monsterHealth.c_str(), 16, 1);
    DrawTextEx(uiFont, monsterHealth.c_str(),
               {monsterHealthBg.x + monsterHealthBg.width - monsterHealthSize.x - 5, monsterHealthBg.y + 2},
               16, 1,WHITE);
}

void BattlePanel::drawBattleLog() const {
    const Rectangle logRect = {
        panelBounds.x + 20,
        panelBounds.y + 120,
        panelBounds.width - 40,
        panelBounds.height - 220
    };

    DrawRectangleRounded(logRect, 0.05f, 10, logBackgroundColor);
    DrawRectangleRoundedLines(logRect, 0.05f, 10, 1, GRAY);

    DrawTextEx(uiFont, "Battle Log",
               {logRect.x + 10, logRect.y + 5}, 18, 1, LIGHTGRAY);

    float lineHeight = 25;
    float textY = logRect.y + 35;

    const int startIndex = std::max(0, static_cast<int>(battleLog.size()) - maxLogLines);
    for (int i = startIndex; i < battleLog.size(); i++) {
        DrawTextEx(uiFont, battleLog[i].c_str(),
                   {logRect.x + 10, textY}, 16, 1, textColor);
        textY += lineHeight;
    }
}

void BattlePanel::drawButtons() const {
    if (player) {
        const std::string weaponLabel = player->GetInventory().GetWeapon().GetName();
        const std::string spellLabel = player->GetInventory().GetSpell().GetName();

        weaponButton.Draw();
        spellButton.Draw();
        fleeButton.Draw();

        Rectangle weaponBounds = weaponButton.GetBounds();
        Rectangle spellBounds = spellButton.GetBounds();

        Vector2 weaponTextSize = MeasureTextEx(uiFont, weaponLabel.c_str(), 14, 1);
        Vector2 spellTextSize = MeasureTextEx(uiFont, spellLabel.c_str(), 14, 1);

        DrawTextEx(uiFont, weaponLabel.c_str(),
                   {
                       weaponBounds.x + (weaponBounds.width - weaponTextSize.x) / 2,
                       weaponBounds.y + (weaponBounds.height - weaponTextSize.y) / 2 + 15
                   },
                   14, 1, LIGHTGRAY
        );

        DrawTextEx(uiFont, spellLabel.c_str(),
                   {
                       spellBounds.x + (spellBounds.width - spellTextSize.x) / 2,
                       spellBounds.y + (spellBounds.height - spellTextSize.y) / 2 + 15
                   }, 14, 1, LIGHTGRAY);
    }
}

void BattlePanel::onWeaponAttack() {
    if (!waitingForInput || !isPlayerTurn) return;
    HandlePlayerAttack(AttackType::WEAPON);
}

void BattlePanel::onSpellAttack() {
    if (!waitingForInput || !isPlayerTurn) return;
    HandlePlayerAttack(AttackType::SPELL);
}

void BattlePanel::onFlee() {
    if (!waitingForInput || !isPlayerTurn) return;
    addLogEntry(player->GetName() + " fled from battle!");
    EndBattle(BattleResult::PLAYER_WON);
}

void BattlePanel::HandlePlayerAttack(AttackType attackType) {
    if (!player || !currentMonster || !battleSystem) return;

    waitingForInput = false;

    const std::string attackName = (attackType == AttackType::WEAPON)
                                       ? player->GetInventory().GetWeapon().GetName()
                                       : player->GetInventory().GetSpell().GetName();

    const double damage = battleSystem->performAttack(*player, *currentMonster, attackType);

    const std::string logEntry = player->GetName() + " used " + attackName + "! It dealt " +
                                 std::to_string(static_cast<int>(damage)) + " damage.";
    addLogEntry(logEntry);

    if (currentMonster->isDefeated()) {
        addLogEntry(currentMonster->GetName() + " is defeated!");
        EndBattle(BattleResult::PLAYER_WON);
    } else {
        isPlayerTurn = false;
    }
}

void BattlePanel::HandleMonsterAttack() {
    if (!player || !currentMonster || !battleSystem) return;

    AttackType monsterAttackType = (GetRandomValue(0, 1) == 0) ? AttackType::WEAPON : AttackType::SPELL;

    const std::string attackName = (monsterAttackType == AttackType::WEAPON) ? "Claw Attack" : "Fire Breath";

    const double damage = battleSystem->performAttack(*currentMonster, *player, monsterAttackType);

    const std::string logEntry = currentMonster->GetName() + " used " + attackName + "! You lost " +
                                 std::to_string(static_cast<int>(damage)) + " HP.";
    addLogEntry(logEntry);

    if (player->isDefeated()) {
        addLogEntry(player->GetName() + " is defeated!");
        EndBattle(BattleResult::PLAYER_LOST);
    } else {
        isPlayerTurn = true;
        waitingForInput = true;
    }
}

void BattlePanel::EndBattle(const BattleResult result) {
    battleResult = result;

    switch (result) {
        case BattleResult::PLAYER_WON:
            addLogEntry("Victory! " + player->GetName() + " wins!");
            if (battleSystem && player && currentMonster) {
                battleSystem->rewardExperience(*player, *currentMonster);
                player->restoreHealthAfterBattle();
            }
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

    currentState = BattleState::ANIMATING_OUT;
}

void BattlePanel::addLogEntry(const std::string &entry) {
    battleLog.push_back(entry);
    if (battleLog.size() > maxLogLines) {
        battleLog.erase(battleLog.begin());
    }
}

void BattlePanel::clearLog() {
    battleLog.clear();
}

bool BattlePanel::IsActive() const {
    return currentState == BattleState::ACTIVE || currentState == BattleState::ANIMATING_IN;
}

bool BattlePanel::isFinished() const {
    return currentState == BattleState::FINISHED;
}

BattleResult BattlePanel::GetResult() const {
    return battleResult;
}

void BattlePanel::SetFont(const Font &font) {
    uiFont = font;
}

void BattlePanel::SetColors(const Color panel, const Color header, const Color logBg, const Color text) {
    panelColor = panel;
    headerColor = header;
    logBackgroundColor = logBg;
    textColor = text;
}

void BattlePanel::SetAnimationSpeed(const float speed) {
    animationSpeed = speed;
}

float BattlePanel::easeOutCubic(const float t) {
    return 1.0f - std::pow(1.0f - t, 3.0f);
}

Rectangle BattlePanel::lerpRectangle(const Rectangle start, const Rectangle end, const float t) {
    return {
        start.x + (end.x - start.x) * t,
        start.y + (end.y - start.y) * t,
        start.width + (end.width - start.width) * t,
        start.height + (end.height - start.height) * t
    };
}
