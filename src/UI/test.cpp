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

// main.cpp
#include "C:/DandD/include/Core/GameManager.h"

int main() {
    const int screenWidth = 1400;
    const int screenHeight = 800;

    GameManager gameManager(screenWidth, screenHeight);
    gameManager.RunGame();

    return 0;
}
