//
// Created by Lenovo on 24.5.2025 г..
//

#ifndef BATTLEPANEL_H
#define BATTLEPANEL_H

#include "raylib.h"
#include "C:/DandD/include/UI/widgets/Button.h"
#include "C:/DandD/include/Utils/Attack.h"
#include "C:/DandD/include/Entities/Hero.h"
#include "C:/DandD/include/Entities/Monster.h"
#include <string>
#include <vector>

enum class BattleState {
    HIDDEN,
    ANIMATING_IN,
    ACTIVE,
    ANIMATING_OUT,
    FINISHED
};

enum class BattleResult {
    ONGOING,
    PLAYER_WON,
    PLAYER_LOST,
    PLAYER_FLED
};

class BattlePanel {
public:
    BattlePanel();

    ~BattlePanel() = default;

    void StartBattle(Hero *_player, Monster *monster, Attack *attackSystem);

    void Update();

    void Draw() const;

    bool IsActive() const;

    bool isFinished() const;

    BattleResult GetResult() const;

    void SetFont(const Font &font);

    void SetColors(Color panel, Color header, Color logBg, Color text);

    void SetAnimationSpeed(float speed);

    void HandlePlayerAttack(AttackType attackType);

    void HandleMonsterAttack();

    void EndBattle(BattleResult result);

private:
    Rectangle panelBounds;
    Rectangle targetBounds;
    Rectangle hiddenBounds;

    BattleState currentState;
    BattleResult battleResult;

    float animationProgress;
    float animationSpeed;

    Button weaponButton;
    Button spellButton;
    Button fleeButton;

    std::vector<std::string> battleLog;
    int maxLogLines;
    float logScrollOffset;

    Hero *player;
    Monster *currentMonster;
    Attack *battleSystem;

    Color panelColor;
    Color headerColor;
    Color logBackgroundColor;
    Color textColor;
    Font uiFont;

    bool isPlayerTurn;
    bool waitingForInput;

private:
    float easeOutCubic(float t);

    Rectangle lerpRectangle(Rectangle start, Rectangle end, float t);

    void drawPanel() const;

    void drawHeader() const;

    void drawBattleLog() const;

    void drawButtons() const;

    void drawHealthBars() const;

    void addLogEntry(const std::string &entry);

    void clearLog();

    void onWeaponAttack();

    void onSpellAttack();

    void onFlee();
};

#endif //BATTLEPANEL_H
