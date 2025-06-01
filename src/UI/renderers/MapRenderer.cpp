//
// Created by Lenovo on 21.5.2025 г..
//

#include "C:/DandD/include/UI/renderers/MapRenderer.h"
#include <cmath>
#include <algorithm>

MapRenderer::MapRenderer(const int _screenWidth, const int _screenHeight)
    : map(nullptr), heroPosition(nullptr),
      screenWidth(_screenWidth), screenHeight(_screenHeight),
      cellSize(40.0f), resourcesLoaded(false) {
    camera.visibleCellsX = 10;
    camera.visibleCellsY = 10;
    camera.smoothFactor = 0.1f;
}

MapRenderer::~MapRenderer() {
    Unload();
}

void MapRenderer::Initialize(Map *mapRef, Position *heroPositionRef) {
    map = mapRef;
    heroPosition = heroPositionRef;

    if (heroPosition && map) {
        camera.position = {
            static_cast<float>(heroPosition->x) - camera.visibleCellsX * 0.5f,
            static_cast<float>(heroPosition->y) - camera.visibleCellsY * 0.5f
        };
        camera.target = camera.position;
        visibleArea.needsUpdate = true;
    }
}


void MapRenderer::LoadResources() {
    if (resourcesLoaded) return;

    floorTexture = LoadTexture("C:/DandD/assets/map/floor_tile.png");
    wallTexture = LoadTexture("C:/DandD/assets/map/wall_tile.png");
    heroTexture = LoadTexture("C:/DandD/assets/entities/hero_icon.png");
    monsterTexture = LoadTexture("C:/DandD/assets/entities/monster_icon.png");
    bossTexture = LoadTexture("C:/DandD/assets/entities/boss_icon.png");
    treasureTexture = LoadTexture("C:/DandD/assets/objects/treasure_icon.png");

    gameFont = LoadFont("C:/DandD/assets/fonts/.TTF");

    resourcesLoaded = true;
}

void MapRenderer::Unload() {
    if (!resourcesLoaded) return;

    UnloadTexture(floorTexture);
    UnloadTexture(wallTexture);
    UnloadTexture(heroTexture);
    UnloadTexture(monsterTexture);
    UnloadTexture(bossTexture);
    UnloadTexture(treasureTexture);
    UnloadFont(gameFont);

    resourcesLoaded = false;
}

void MapRenderer::Update(float deltaTime) {
    int newWidth = GetScreenWidth();
    int newHeight = GetScreenHeight();

    if (newWidth != screenWidth || newHeight != screenHeight) {
        screenWidth = newWidth;
        screenHeight = newHeight;
    }

    UpdateCamera();
}

void MapRenderer::UpdateCamera() {
    if (!heroPosition || !map) return;

    camera.target = {
        static_cast<float>(heroPosition->x) - camera.visibleCellsX * 0.5f,
        static_cast<float>(heroPosition->y) - camera.visibleCellsY * 0.5f
    };

    Vector2 diff = {
        camera.target.x - camera.position.x,
        camera.target.y - camera.position.y
    };

    camera.position.x += diff.x * camera.smoothFactor;
    camera.position.y += diff.y * camera.smoothFactor;

    float maxX = static_cast<float>(map->getWidth()) - camera.visibleCellsX;
    float maxY = static_cast<float>(map->getHeight()) - camera.visibleCellsY;

    camera.position.x = std::clamp(camera.position.x, 0.0f, std::max(0.0f, maxX));
    camera.position.y = std::clamp(camera.position.y, 0.0f, std::max(0.0f, maxY));

    Vector2 cameraDiff = {
        std::abs(camera.position.x - visibleArea.lastCameraPos.x),
        std::abs(camera.position.y - visibleArea.lastCameraPos.y)
    };

    if (cameraDiff.x > 0.01f || cameraDiff.y > 0.01f) {
        visibleArea.needsUpdate = true;
    }
}

void MapRenderer::UpdateVisibleArea() const {
    if (!visibleArea.needsUpdate) return;

    int cameraX = static_cast<int>(std::floor(camera.position.x));
    int cameraY = static_cast<int>(std::floor(camera.position.y));

    visibleArea.startX = std::max(0, cameraX - 1);
    visibleArea.startY = std::max(0, cameraY - 1);
    visibleArea.endX = std::min(static_cast<int>(map->getWidth()),
                                cameraX + camera.visibleCellsX + 2);
    visibleArea.endY = std::min(static_cast<int>(map->getHeight()),
                                cameraY + camera.visibleCellsY + 2);

    visibleArea.lastCameraPos = camera.position;
    visibleArea.needsUpdate = false;
}

Vector2 MapRenderer::WorldToScreen(const int worldX, const int worldY) const {
    const Rectangle mapArea = GetMapArea();

    const float screenX = (worldX - camera.position.x) * cellSize;
    const float screenY = (worldY - camera.position.y) * cellSize;

    return {mapArea.x + screenX, mapArea.y + screenY};
}

Rectangle MapRenderer::GetMapArea() const {
    const float mapWidth = camera.visibleCellsX * cellSize;
    const float mapHeight = camera.visibleCellsY * cellSize;
    const float mapX = (screenWidth - mapWidth) * 0.5f;
    const float mapY = 150.0f;

    return {mapX, mapY, mapWidth, mapHeight};
}

bool MapRenderer::IsInVisibleArea(int x, int y) const {
    UpdateVisibleArea();
    return x >= visibleArea.startX && x < visibleArea.endX &&
           y >= visibleArea.startY && y < visibleArea.endY;
}

void MapRenderer::Draw() const {
    if (!map || !resourcesLoaded) return;

    const Rectangle mapArea = GetMapArea();
    DrawRectangleRec(mapArea, BACKGROUND_COLOR);

    DrawMapBorder();
    DrawTiles();
    DrawEntities();
    DrawMinimap();
}

void MapRenderer::DrawMapBorder() const {
    const Rectangle mapArea = GetMapArea();

    // Main border
    DrawRectangleLinesEx(mapArea, 3, BORDER_COLOR);

    // Inner glow effect
    const Rectangle innerBorder = {
        mapArea.x + 3, mapArea.y + 3,
        mapArea.width - 6, mapArea.height - 6
    };
    DrawRectangleLinesEx(innerBorder, 1, BORDER_GLOW);
}

void MapRenderer::DrawTexturedTile(const Texture2D &texture, const Vector2 &screenPos) const {
    if (texture.id != 0) {
        DrawTexturePro(
            texture,
            {0, 0, static_cast<float>(texture.width), static_cast<float>(texture.height)},
            {screenPos.x, screenPos.y, cellSize, cellSize},
            {0, 0}, 0.0f, WHITE
        );
    }
}

void MapRenderer::DrawTiles() const {
    if (!map) return;
    UpdateVisibleArea();

    for (int y = visibleArea.startY; y < visibleArea.endY; y++) {
        for (int x = visibleArea.startX; x < visibleArea.endX; x++) {
            const char cellType = map->getCell({x, y});
            Vector2 screenPos = WorldToScreen(x, y);

            switch (cellType) {
                case '#': DrawTexturedTile(wallTexture, screenPos);
                    break;
                case '.':
                case 'H':
                case 'M':
                case 'B':
                case 'T':
                    DrawTexturedTile(floorTexture, screenPos);
                    break;
                default: ;
            }
        }
    }
}

void MapRenderer::DrawEntities() const {
    if (!map || !heroPosition) return;

    // Draw treasures
    for (const auto &treasure: map->getTreasures()) {
        if (IsInVisibleArea(treasure.getPosition().x, treasure.getPosition().y)) {
            const Vector2 screenPos = WorldToScreen(treasure.getPosition().x, treasure.getPosition().y);
            const Rectangle destRect = {screenPos.x, screenPos.y, cellSize, cellSize};

            if (treasureTexture.id != 0) {
                DrawTexturePro(
                    treasureTexture,
                    {0, 0, static_cast<float>(treasureTexture.width), static_cast<float>(treasureTexture.height)},
                    destRect, {0, 0}, 0.0f, WHITE
                );
            }
        }
    }

    // Draw monsters
    for (const auto &monster: map->getMonsters()) {
        if (IsInVisibleArea(monster.GetPosition().x, monster.GetPosition().y)) {
            const Vector2 screenPos = WorldToScreen(monster.GetPosition().x, monster.GetPosition().y);
            const Rectangle destRect = {screenPos.x, screenPos.y, cellSize, cellSize};

            const Texture *texture = (monster.GetType() == MonsterType::BOSS) ? &bossTexture : &monsterTexture;

            if (texture->id != 0) {
                DrawTexturePro(
                    *texture,
                    {0, 0, static_cast<float>(texture->width), static_cast<float>(texture->height)},
                    destRect, {0, 0}, 0.0f, WHITE
                );
            }
        }
    }

    // Draw hero (always on top)
    if (IsInVisibleArea(heroPosition->x, heroPosition->y)) {
        Vector2 screenPos = WorldToScreen(heroPosition->x, heroPosition->y);
        Rectangle destRect = {screenPos.x, screenPos.y, cellSize, cellSize};

        if (heroTexture.id != 0) {
            DrawTexturePro(
                heroTexture,
                {0, 0, static_cast<float>(heroTexture.width), static_cast<float>(heroTexture.height)},
                destRect, {0, 0}, 0.0f, WHITE
            );
        }
    }
}

void MapRenderer::DrawMinimap() const {
    if (!map) return;

    constexpr float MINIMAP_SIZE = 150.0f;
    constexpr float MARGIN = 20.0f;

    const Vector2 minimapPos = {
        screenWidth - MINIMAP_SIZE - MARGIN,
        screenHeight - MINIMAP_SIZE - MARGIN
    };

    const float cellSize = std::min(
        (MINIMAP_SIZE - 4) / static_cast<float>(map->getWidth()),
        (MINIMAP_SIZE - 4) / static_cast<float>(map->getHeight())
    );

    DrawMinimapBackground(minimapPos, MINIMAP_SIZE);
    DrawMinimapTiles(minimapPos, MINIMAP_SIZE, cellSize);
    DrawMinimapEntities(minimapPos, MINIMAP_SIZE, cellSize);
    DrawMinimapViewport(minimapPos, MINIMAP_SIZE, cellSize);
}

void MapRenderer::DrawMinimapBackground(Vector2 pos, float size) const {
    DrawRectangle(pos.x, pos.y, size, size, {30, 30, 50, 200});

    DrawRectangleLinesEx({pos.x, pos.y, size, size}, 2, {100, 100, 150, 200});
}

void MapRenderer::DrawMinimapTiles(Vector2 pos, float size, float cellSize) const {
    const Vector2 offset = {pos.x + 2, pos.y + 2};

    for (int y = 0; y < static_cast<int>(map->getHeight()); y++) {
        for (int x = 0; x < static_cast<int>(map->getWidth()); x++) {
            const char cellType = map->getCell({x, y});

            const float cellX = offset.x + x * cellSize;
            const float cellY = offset.y + y * cellSize;

            Color cellColor = {0, 0, 0, 0}; // Transparent by default

            switch (cellType) {
                case '#':
                    cellColor = {100, 100, 130, 255};
                    break;
                case '.':
                case 'H':
                case 'M':
                case 'B':
                case 'T':
                    cellColor = {50, 50, 70, 255};
                    break;
            }

            if (cellColor.a > 0) {
                DrawRectangle(cellX, cellY, cellSize, cellSize, cellColor);
            }
        }
    }
}

void MapRenderer::DrawMinimapEntities(Vector2 pos, float size, float cellSize) const {
    const Vector2 offset = {pos.x + 2, pos.y + 2};

    for (const auto &treasure: map->getTreasures()) {
        float x = offset.x + treasure.getPosition().x * cellSize;
        float y = offset.y + treasure.getPosition().y * cellSize;
        DrawRectangle(x, y, cellSize, cellSize, {220, 180, 50, 255});
    }

    // Draw monsters
    for (const auto &monster: map->getMonsters()) {
        const float x = offset.x + monster.GetPosition().x * cellSize;
        const float y = offset.y + monster.GetPosition().y * cellSize;

        const Color color = (monster.GetType() == MonsterType::BOSS)
                                ? Color{200, 50, 50, 255}
                                : Color{180, 80, 80, 255};

        DrawRectangle(x, y, cellSize, cellSize, color);
    }

    if (heroPosition) {
        const float x = offset.x + heroPosition->x * cellSize;
        const float y = offset.y + heroPosition->y * cellSize;
        DrawRectangle(x, y, cellSize, cellSize, {50, 150, 220, 255});
    }
}

void MapRenderer::DrawMinimapViewport(Vector2 pos, float size, float cellSize) const {
    const Vector2 offset = {pos.x + 2, pos.y + 2};

    const float viewX = offset.x + camera.position.x * cellSize;
    const float viewY = offset.y + camera.position.y * cellSize;
    const float viewWidth = camera.visibleCellsX * cellSize;
    const float viewHeight = camera.visibleCellsY * cellSize;

    DrawRectangleLinesEx(
        {viewX, viewY, viewWidth, viewHeight},
        1, {220, 220, 255, 200}
    );
}

void MapRenderer::SetCellSize(float size) {
    cellSize = std::max(10.0f, std::min(size, 100.0f)); // Clamp to reasonable range
    visibleArea.needsUpdate = true;
}

void MapRenderer::SetVisibleRange(int cellsX, int cellsY) {
    camera.visibleCellsX = std::max(5, cellsX);
    camera.visibleCellsY = std::max(5, cellsY);
    visibleArea.needsUpdate = true;

    // Recenter camera
    if (heroPosition) {
        camera.target = {
            static_cast<float>(heroPosition->x) - camera.visibleCellsX * 0.5f,
            static_cast<float>(heroPosition->y) - camera.visibleCellsY * 0.5f
        };
    }
}
