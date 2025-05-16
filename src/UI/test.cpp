//
// Created by Lenovo on 15.5.2025 г.
//


#include "raylib.h"
//#include "C:/DandD/include/UI/widgets/Button.h"
#include "C:/DandD/include/UI/widgets/ProgressBar.h"

int main() {
    // // Инициализация на прозореца
    // const int screenWidth = 800;
    // const int screenHeight = 600;
    // InitWindow(screenWidth, screenHeight, "Button Test");
    // SetTargetFPS(60);
    //
    // // Създаване на бутон
    // Button testButton(
    //     {100, 100, 200, 50},  // x, y, width, height
    //     "Click Me!",           // Текст
    //     [](){                  // Действие при клик
    //         TraceLog(LOG_INFO, "Button clicked!");
    //     }
    // );
    //
    // // Главен цикъл
    // while (!WindowShouldClose()) {
    //     // Обновяване
    //     testButton.Update();
    //
    //     // Рисуване
    //     BeginDrawing();
    //     ClearBackground(RAYWHITE);
    //     testButton.Draw();
    //     EndDrawing();
    // }
    //
    // CloseWindow();

    const int screenW = 400, screenH = 200;
    InitWindow(screenW, screenH, "ProgressBar Test");
    SetTargetFPS(60);

    ProgressBar bar({50, 80, 300, 30}, 100);
    bar.SetColors(DARKGRAY, LIME, BLACK);
    bar.SetTextPrefix("HP:");
    bar.SetFont(GetFontDefault(), 20, WHITE);
    bar.ShowText(true);

    float hp = 100;
    bool decreasing = true;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        // симулираме колебания между 0 и макс
        if (decreasing) {
            hp -= 50 * dt;
            if (hp <= 0) {
                hp = 0;
                decreasing = false;
            }
        } else {
            hp += 50 * dt;
            if (hp >= bar.GetMaxValue()) {
                hp = bar.GetMaxValue();
                decreasing = true;
            }
        }
        bar.Update(hp, dt);

        BeginDrawing();
        ClearBackground(RAYWHITE);
        bar.Draw();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
