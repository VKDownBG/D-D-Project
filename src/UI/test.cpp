// //
// // Created by Lenovo on 21.5.2025
// //
//
// #include "raylib.h"
// #include "../../include/UI/screens/MainMenu.h"
// #include "../../include/UI/screens/GameHUD.h"
// #include "../../include/UI/renderers/MapRenderer.h"  // Add MapRenderer
// #include "../../include/Entities/Hero.h"
// #include "../../include/Items/Types/Weapon.h"
// #include "../../include/Items/Types/Armor.h"
// #include "../../include/Items/Types/Spell.h"
// #include "../../include/Core/MapSystem.h"  // Add Map class
//
// enum GameState {
//     STATE_MENU,
//     STATE_GAME
// };
//
// int Clamp(int value, int min, int max) {
//     if (value < min) return min;
//     if (value > max) return max;
//     return value;
// }
//
// // Helper function to create a test map
// Map *CreateTestMap() {
//     Map *map = new Map(); // Assuming Map constructor takes width/height
//
//     // Create simple map layout
//     map->loadFromFile("C:/DandD/assets/maps/maps.txt", "[LEVEL_1]");
//
//     return map;
// }
//
// int main() {
//     const int screenWidth = 1400;
//     const int screenHeight = 800;
//
//     InitWindow(screenWidth, screenHeight, "Dungeons & Dragons");
//     SetWindowPosition(screenWidth / 2 - 200, screenHeight / 2 - 250);
//     SetTargetFPS(60);
//
//     // Create and initialize MainMenu
//     MainMenu mainMenu(screenWidth, screenHeight);
//     mainMenu.Initialize();
//     mainMenu.LoadResources();
//
//     // Game components
//     Hero *player = new Hero("Human", "Tester");
//     Map *gameMap = CreateTestMap(); // Create test map
//     MapRenderer mapRenderer(screenWidth, screenHeight); // Create map renderer
//
//     // Initialize map renderer with hero position
//     mapRenderer.Initialize(gameMap, player->GetPosition());
//     mapRenderer.LoadResources();
//
//     // Equipment setup
//     Armor *armor = new Armor("Aegis of Eternity", 12.5f, 4);
//     Weapon *weapon = new Weapon("Nightstalker Dagger", 25.0f, 4);
//     Spell *spell = new Spell("Thunderlord's Wrath", 35.0f, 4);
//
//     // HUD setup
//     GameHUD hud(screenWidth, screenHeight);
//     hud.Initialize(player);
//     hud.SetArmor(armor);
//     hud.SetWeapon(weapon);
//     hud.SetSpell(spell);
//     hud.LoadResources();
//
//     // Player stats
//     player->SetStrength(120000);
//     player->SetMana(120000);
//     player->SetXP(20);
//
//     GameState currentState = STATE_MENU;
//
//     while (!WindowShouldClose()) {
//         float deltaTime = GetFrameTime();
//
//         // Handle hero movement in game state
//         if (currentState == STATE_GAME) {
//             // Simple movement controls (WASD)
//             Position &pos = *player->GetPosition();
//             if (IsKeyDown(KEY_W)) pos.y--;
//             if (IsKeyDown(KEY_S)) pos.y++;
//             if (IsKeyDown(KEY_A)) pos.x--;
//             if (IsKeyDown(KEY_D)) pos.x++;
//
//             // Keep hero within map bounds
//             pos.x = Clamp(pos.x, 0, gameMap->getWidth() - 1);
//             pos.y = Clamp(pos.y, 0, gameMap->getHeight() - 1);
//         }
//
//         switch (currentState) {
//             case STATE_MENU:
//                 mainMenu.Update(deltaTime);
//                 if (mainMenu.IsStartGameSelected()) {
//                     currentState = STATE_GAME;
//                     mainMenu.ResetSelections();
//                 }
//                 if (mainMenu.IsQuitSelected()) {
//                     CloseWindow();
//                     return 0;
//                 }
//                 break;
//
//             case STATE_GAME:
//                 hud.Update(deltaTime);
//                 mapRenderer.Update(deltaTime); // Update map renderer
//                 break;
//         }
//
//         BeginDrawing();
//         ClearBackground({20, 20, 35, 255});
//
//         switch (currentState) {
//             case STATE_MENU:
//                 mainMenu.Draw();
//                 break;
//
//             case STATE_GAME:
//                 // Draw game map
//                 mapRenderer.Draw(); // Draw main map
//
//                 // Draw HUD on top
//                 hud.Draw();
//                 break;
//         }
//
//         // Debug info
//         DrawText(TextFormat("Hero Position: %d,%d",
//                             player->getCurrentPosition().x,
//                             player->getCurrentPosition().y),
//                  10, 10, 20, GREEN);
//
//         EndDrawing();
//     }
//
//     // Cleanup
//     delete player;
//     delete armor;
//     delete weapon;
//     delete spell;
//     delete gameMap; // Clean up map
//     mapRenderer.Unload(); // Unload map resources
//
//     CloseWindow();
//     return 0;
// }

#include <iomanip>

#include "raylib.h"
#include "C:/DandD/include/Entities/Hero.h"
#include "C:/DandD/include/Items/Types/Weapon.h"
#include "C:/DandD/include/UI/panels/EquipmentPanel.h"
#include <iostream>
#include <sstream>

enum class TestState {
    SHOW_CURRENT_EQUIPMENT,
    SHOW_CHOICE_MENU,
    SHOW_UPDATED_EQUIPMENT
};

void DrawEquipmentDisplay(Hero *hero, const std::string &title, int yOffset = 0) {
    const int screenWidth = GetScreenWidth();
    const int startY = 100 + yOffset;

    // Draw title
    const int titleWidth = MeasureText(title.c_str(), 30);
    DrawText(title.c_str(), (screenWidth - titleWidth) / 2, startY, 30, YELLOW);

    // Draw current weapon info
    const Inventory &inventory = hero->GetInventory();
    const Weapon &weapon = inventory.GetWeapon();

    int currentY = startY + 50;

    // Weapon name
    std::string weaponName = "Weapon: " + weapon.GetName();
    const int nameWidth = MeasureText(weaponName.c_str(), 20);
    DrawText(weaponName.c_str(), (screenWidth - nameWidth) / 2, currentY, 20, WHITE);
    currentY += 30;

    // Weapon level
    std::string levelText = "Level: " + std::to_string(weapon.GetLevel());
    const int levelWidth = MeasureText(levelText.c_str(), 18);
    DrawText(levelText.c_str(), (screenWidth - levelWidth) / 2, currentY, 18, LIGHTGRAY);
    currentY += 25;

    // Weapon bonus
    std::stringstream ss;
    ss << std::fixed << std::setprecision(1) << weapon.GetBonus();
    std::string bonusText = "Damage Bonus: " + ss.str() + "%";
    const int bonusWidth = MeasureText(bonusText.c_str(), 18);
    DrawText(bonusText.c_str(), (screenWidth - bonusWidth) / 2, currentY, 18, LIGHTGRAY);
}

int main() {
    const int screenWidth = 1200;
    const int screenHeight = 800;

    InitWindow(screenWidth, screenHeight, "Equipment Replacement Test");
    SetTargetFPS(60);

    // Create hero with initial weapon
    Hero hero("Warrior", "Test Hero");
    Weapon initialWeapon("Iron Sword", 1, 15.0);
    hero.GetInventory().newWeapon(initialWeapon, true);

    // Create new weapon to offer
    Weapon newWeapon("Steel Blade", 3, 25.0);

    // Create equipment panel
    EquipmentPanel equipmentPanel(&hero);

    TestState currentState = TestState::SHOW_CURRENT_EQUIPMENT;
    float stateTimer = 0.0f;
    bool choiceMade = false;

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();
        stateTimer += deltaTime;

        // Handle input and state transitions
        if (currentState == TestState::SHOW_CURRENT_EQUIPMENT) {
            if (IsKeyPressed(KEY_SPACE) || stateTimer > 3.0f) {
                currentState = TestState::SHOW_CHOICE_MENU;
                equipmentPanel.ShowComparison(&newWeapon);
                stateTimer = 0.0f;
            }
        } else if (currentState == TestState::SHOW_CHOICE_MENU) {
            equipmentPanel.Update();

            if (!equipmentPanel.IsVisible() && !choiceMade) {
                choiceMade = true;
                currentState = TestState::SHOW_UPDATED_EQUIPMENT;
                stateTimer = 0.0f;
            }
        } else if (currentState == TestState::SHOW_UPDATED_EQUIPMENT) {
            if (IsKeyPressed(KEY_R) || stateTimer > 5.0f) {
                // Reset test
                hero.GetInventory().newWeapon(initialWeapon, true);
                currentState = TestState::SHOW_CURRENT_EQUIPMENT;
                choiceMade = false;
                stateTimer = 0.0f;
            }
        }

        BeginDrawing();
        ClearBackground({20, 30, 60, 255});

        // Draw based on current state
        if (currentState == TestState::SHOW_CURRENT_EQUIPMENT) {
            DrawEquipmentDisplay(&hero, "CURRENT HERO EQUIPMENT");

            // Instructions
            const std::string instruction = "Press SPACE to find new weapon (or wait 3 seconds)";
            const int instrWidth = MeasureText(instruction.c_str(), 16);
            DrawText(instruction.c_str(), (screenWidth - instrWidth) / 2, 300, 16, GREEN);

            // Progress indicator
            int barWidth = 300;
            int barHeight = 10;
            int barX = (screenWidth - barWidth) / 2;
            int barY = 350;

            DrawRectangle(barX, barY, barWidth, barHeight, DARKGRAY);
            DrawRectangle(barX, barY, (int) (barWidth * (stateTimer / 3.0f)), barHeight, GREEN);
        } else if (currentState == TestState::SHOW_CHOICE_MENU) {
            equipmentPanel.Draw();
        } else if (currentState == TestState::SHOW_UPDATED_EQUIPMENT) {
            DrawEquipmentDisplay(&hero, "UPDATED HERO EQUIPMENT");

            // Show what happened
            const std::string resultText = choiceMade ? "Equipment choice completed!" : "No changes made";
            const int resultWidth = MeasureText(resultText.c_str(), 18);
            DrawText(resultText.c_str(), (screenWidth - resultWidth) / 2, 300, 18, GOLD);

            // Instructions
            const std::string instruction = "Press R to restart test (or wait 5 seconds)";
            const int instrWidth = MeasureText(instruction.c_str(), 16);
            DrawText(instruction.c_str(), (screenWidth - instrWidth) / 2, 330, 16, GREEN);

            // Progress indicator
            int barWidth = 300;
            int barHeight = 10;
            int barX = (screenWidth - barWidth) / 2;
            int barY = 360;

            DrawRectangle(barX, barY, barWidth, barHeight, DARKGRAY);
            DrawRectangle(barX, barY, (int) (barWidth * (stateTimer / 5.0f)), barHeight, GREEN);
        }

        // Draw state indicator in top-left
        std::string stateText;
        Color stateColor;
        switch (currentState) {
            case TestState::SHOW_CURRENT_EQUIPMENT:
                stateText = "State: Showing Current Equipment";
                stateColor = BLUE;
                break;
            case TestState::SHOW_CHOICE_MENU:
                stateText = "State: Equipment Choice Menu";
                stateColor = YELLOW;
                break;
            case TestState::SHOW_UPDATED_EQUIPMENT:
                stateText = "State: Showing Updated Equipment";
                stateColor = GREEN;
                break;
        }
        DrawText(stateText.c_str(), 10, 10, 16, stateColor);

        // Draw timer
        std::stringstream timerStream;
        timerStream << std::fixed << std::setprecision(1) << stateTimer;
        std::string timerText = "Timer: " + timerStream.str() + "s";
        DrawText(timerText.c_str(), 10, 30, 14, WHITE);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
