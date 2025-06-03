#include "C:/DandD/include/Core/GameManager.h"

int main() {
    const int screenWidth = 1400;
    const int screenHeight = 800;

    GameManager gameManager(screenWidth, screenHeight);
    gameManager.RunGame();

    return 0;
}
