//
// Created by Lenovo on 21.5.2025 г..
//

#include "C:/DandD/include/UI/renderers/MapRenderer.h"
#include <cmath>
#include <algorithm>

MapRenderer::MapRenderer(const int _screenWidth, const int _screenHeight)
    : map(nullptr),
      heroPosition(nullptr),
      screenWidth(_screenWidth),
      screenHeight(_screenHeight),
      cellSize(40.0f),
      resourcesLoaded(false) {
    backgroundColor = {20, 20, 35, 255};
    textColor = {220, 220, 255, 255};

    camera.visibleCellsX = 15;
    camera.visibleCellsY = 13;
    camera.x = 0;
    camera.y = 0;
    camera.offsetX = 0;
    camera.offsetY = 0;
}

MapRenderer::~MapRenderer() {
    Unload();
}

void MapRenderer::Initialize(Map *mapRef, Position *heroPositionRef) {
    map = mapRef;
    heroPosition = heroPositionRef;

    if (heroPosition) {
        camera.x = heroPosition->x - camera.visibleCellsX / 2;
        camera.y = heroPosition->y - camera.visibleCellsY / 2;
    }
}

void MapRenderer::LoadResources() {
    emptyTileTexture = LoadTexture("C:/DandD/assets/map/floor_tile.png");
    wallTileTexture = LoadTexture("C:/DandD/assets/map/wall_tile.png");
    heroTexture = LoadTexture("C:/DandD/assets/entities/hero_icon.png");
    monsterTexture = LoadTexture("C:/DandD/assets/entities/monster_icon.png");
    bossTexture = LoadTexture("C:/DandD/assets/entities/boss_icon.png");
    treasureTexture = LoadTexture("C:/DandD/assets/objects/treasure_icon.png");

    mapFont = LoadFont("C:/DandD/assets/fonts/.TTF");

    resourcesLoaded = true;
}

void MapRenderer::Unload() {
    if (resourcesLoaded) {
        UnloadTexture(emptyTileTexture);
        UnloadTexture(wallTileTexture);
        UnloadTexture(heroTexture);
        UnloadTexture(monsterTexture);
        UnloadTexture(bossTexture);
        UnloadTexture(treasureTexture);
        UnloadFont(mapFont);

        resourcesLoaded = false;
    }
}

void MapRenderer::Update(float deltaTime) {
    UpdateCameraPosition();

    const int newWidth = GetScreenWidth();
    const int newHeight = GetScreenHeight();

    if (newWidth != screenWidth || newHeight != screenHeight) {
        screenWidth = newWidth;
        screenHeight = newHeight;
    }
}

void MapRenderer::UpdateCameraPosition() {
    if (!heroPosition || !map) return;

    const float targetX = heroPosition->x - camera.visibleCellsX / 2.0f;
    const float targetY = heroPosition->y - camera.visibleCellsY / 2.0f;

    const float smoothFactor = 0.15f;
    camera.x = camera.x + (targetX - camera.x) * smoothFactor;
    camera.y = camera.y + (targetY - camera.y) * smoothFactor;

    int mapWidth = map->getWidth();
    int mapHeight = map->getHeight();

    camera.x = std::max(0.0f, std::min(camera.x, static_cast<float>(mapWidth - camera.visibleCellsX)));
    camera.y = std::max(0.0f, std::min(camera.y, static_cast<float>(mapHeight - camera.visibleCellsY)));

    camera.offsetX = (camera.x - floor(camera.x)) * cellSize;
    camera.offsetY = (camera.y - floor(camera.y)) * cellSize;
}

bool MapRenderer::IsCellVisible(int x, int y) const {
    int cameraLeft = static_cast<int>(camera.x);
    int cameraTop = static_cast<int>(camera.y);
    int cameraRight = cameraLeft + camera.visibleCellsX;
    int cameraBottom = cameraTop + camera.visibleCellsY;

    return x >= cameraLeft && x < cameraRight && y >= cameraTop && y < cameraBottom;
}

Vector2 MapRenderer::GetCellScreenPosition(int x, int y) const {
    float screenX = (x - camera.x) * cellSize + camera.offsetX;
    float screenY = (y - camera.y) * cellSize + camera.offsetY;

    float mapAreaX = (screenWidth - (camera.visibleCellsX * cellSize)) / 2.0f;
    float mapAreaY = 150;

    return {mapAreaX + screenX, mapAreaY + screenY};
}

Rectangle MapRenderer::GetVisibleMapArea() const {
    float mapAreaX = (screenWidth - (camera.visibleCellsX * cellSize)) / 2.0f;
    float mapAreaY = 150;
    float mapAreaWidth = camera.visibleCellsX * cellSize;
    float mapAreaHeight = camera.visibleCellsY * cellSize;

    return {mapAreaX, mapAreaY, mapAreaWidth, mapAreaHeight};
}

void MapRenderer::Draw() const {
    if (!map || !resourcesLoaded) return;

    Rectangle mapArea = GetVisibleMapArea();

    DrawRectangleRec(mapArea, backgroundColor);

    DrawMapBorder();

    DrawMapGrid();

    DrawEntities();

    DrawMinimap();
}

void MapRenderer::DrawMapBorder() const {
    Rectangle mapArea = GetVisibleMapArea();

    Color borderColor = {80, 80, 120, 255};
    Color glowColor = {100, 100, 180, 150};

    DrawRectangleLinesEx(mapArea, 3, borderColor);

    Rectangle innerBordere = {
        mapArea.x + 3,
        mapArea.y + 3,
        mapArea.width - 6,
        mapArea.height - 6
    };
    DrawRectangleLinesEx(innerBordere, 1, glowColor);
}

void MapRenderer::DrawMapGrid() const {
    if (!map) return;

    int mapWidth = map->getWidth();
    int mapHeight = map->getHeight();

    int startX = static_cast<int>(camera.x);
    int startY = static_cast<int>(camera.y);
    int endX = std::min(startX + camera.visibleCellsX + 1, mapWidth);
    int endY = std::min(startY + camera.visibleCellsY + 1, mapHeight);

    for (int y = startY; y < endY; y++) {
        for (int x = startX; x < endX; x++) {
            if (x < 0 || y < 0 || x > -mapWidth || y >= mapHeight) continue;

            Vector2 pos = GetCellScreenPosition(x, y);
            Rectangle destRect = {pos.x, pos.y, cellSize, cellSize};

            char cellType = map->getCell({x, y});

            if (cellType == '#') {
                DrawTexturePro(
                    wallTileTexture,
                    {0, 0, static_cast<float>(wallTileTexture.width), static_cast<float>(wallTileTexture.height)},
                    destRect,
                    {0, 0},
                    0.0f,
                    WHITE
                );
            } else if (cellType == '.') {
                DrawTexturePro(emptyTileTexture,
                               {
                                   0, 0, static_cast<float>(emptyTileTexture.width),
                                   static_cast<float>(emptyTileTexture.height)
                               },
                               destRect,
                               {0, 0},
                               0.0f,
                               WHITE
                );
            }
        }
    }
}

void MapRenderer::DrawEntities() const {
    if (!map || !heroPosition) return;

    int mapWidth = map->getWidth();
    int mapHeight = map->getHeight();

    for (const auto &treasure: map->getTreasures()) {
        if (IsCellVisible(treasure.getPosition().x, treasure.getPosition().y)) {
            Vector2 pos = GetCellScreenPosition(treasure.getPosition().x, treasure.getPosition().y);
            Rectangle destRect = {pos.x, pos.y, cellSize, cellSize};

            DrawTexturePro(
                treasureTexture,
                {0, 0, static_cast<float>(treasureTexture.width), static_cast<float>(treasureTexture.height)},
                destRect,
                {0, 0},
                0.0f,
                WHITE
            );
        }
    }

    for (const auto &monster: map->getMonsters()) {
        if (IsCellVisible(monster.GetPosition().x, monster.GetPosition().y)) {
            Vector2 pos = GetCellScreenPosition(monster.GetPosition().x, monster.GetPosition().y);
            Rectangle destRect = {pos.x, pos.y, cellSize, cellSize};

            Texture2D monsterTex = (monster.GetType() == MonsterType::BOSS) ? bossTexture : monsterTexture;

            DrawTexturePro(
                monsterTex,
                {0, 0, static_cast<float>(monsterTex.width), static_cast<float>(monsterTex.height)},
                destRect,
                {0, 0},
                0.0f,
                WHITE
            );
        }
    }

    if (IsCellVisible(heroPosition->x, heroPosition->y)) {
        Vector2 pos = GetCellScreenPosition(heroPosition->x, heroPosition->y);
        Rectangle destRect = {pos.x, pos.y, cellSize, cellSize};

        DrawTexturePro(
            heroTexture,
            {0, 0, static_cast<float>(heroTexture.width), static_cast<float>(heroTexture.height)},
            destRect,
            {0, 0},
            0.0f,
            WHITE
        );
    }
}

void MapRenderer::DrawMinimap() const {
    if (!map || !resourcesLoaded) return;

    const float minimapSize = 150.0f;
    const float minimapX = screenWidth - minimapSize - 20;
    const float minimapY = screenHeight - minimapSize - 20;
    const float minimapBorder = 2.0f;

    int mapWidth = map->getWidth();
    int mapHeight = map->getHeight();

    float miniCellSizeX = (minimapSize - 2 * minimapBorder) / mapWidth;
    float miniCellSizeY = (minimapSize - 2 * minimapBorder) / mapHeight;
    float miniCellSize = std::min(miniCellSizeX, miniCellSizeY);

    DrawRectangle(minimapX, minimapY, minimapSize, minimapSize, {30, 30, 50, 200});
    DrawRectangleLinesEx({minimapX, minimapY, minimapSize, minimapSize},
                         minimapBorder, {100, 100, 150, 200});

    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            char cellType = map->getCell({x, y});

            float cellX = minimapX + minimapBorder + x * miniCellSize;
            float cellY = minimapY + minimapBorder + y * miniCellSize;

            Color cellColor = {0, 0, 0, 0};

            if (cellType == '#') {
                cellColor = {100, 100, 130, 255};
            } else if (cellType == '.') {
                cellColor = {50, 50, 70, 255};
            }

            DrawRectangle(cellX, cellY, miniCellSize, miniCellSize, cellColor);
        }
    }

    for (const auto &treasure: map->getTreasures()) {
        float treasureX = minimapX + minimapBorder + treasure.getPosition().x * miniCellSize;
        float treasureY = minimapY + minimapBorder + treasure.getPosition().y * miniCellSize;

        DrawRectangle(treasureX, treasureY, miniCellSize, miniCellSize, {220, 180, 50, 255});
    }

    // Draw monsters on minimap
    for (const auto &monster: map->getMonsters()) {
        float monsterX = minimapX + minimapBorder + monster.GetPosition().x * miniCellSize;
        float monsterY = minimapY + minimapBorder + monster.GetPosition().y * miniCellSize;

        Color monsterColor = (monster.GetType() == MonsterType::BOSS)
                                 ? Color{200, 50, 50, 255}
                                 : Color{180, 80, 80, 255};

        DrawRectangle(monsterX, monsterY, miniCellSize, miniCellSize, monsterColor);
    }

    // Draw hero on minimap
    if (heroPosition) {
        float heroX = minimapX + minimapBorder + heroPosition->x * miniCellSize;
        float heroY = minimapY + minimapBorder + heroPosition->y * miniCellSize;

        DrawRectangle(heroX, heroY, miniCellSize, miniCellSize, {50, 150, 220, 255});
    }

    float viewX = minimapX + minimapBorder + static_cast<int>(camera.x) * miniCellSize;
    float viewY = minimapY + minimapBorder + static_cast<int>(camera.y) * miniCellSize;
    float viewWidth = camera.visibleCellsX * miniCellSize;
    float viewHeight = camera.visibleCellsY * miniCellSize;

    DrawRectangleLinesEx({viewX, viewY, viewWidth, viewHeight}, 1, {220, 220, 255, 200});
}

void MapRenderer::SetCellSize(float size) {
    cellSize = size;
}

void MapRenderer::SetVisibleRange(int cellsX, int cellsY) {
    camera.visibleCellsX = cellsX;
    camera.visibleCellsY = cellsY;

    if (heroPosition) {
        camera.x = heroPosition->x - camera.visibleCellsX / 2;
        camera.y = heroPosition->y - camera.visibleCellsY / 2;
    }
}
