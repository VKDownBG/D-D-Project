//
// Created by Lenovo on 21.5.2025 г..
//

#ifndef MAPRENDERER_H
#define MAPRENDERER_H

#include "C:/DandD/include/Core/MapSystem.h"
#include "raylib.h"
#include <string>
#include <vector>
#include <unordered_map>

class MapRenderer {
public:
    MapRenderer(int _screenWidth, int _screenHeight);

    ~MapRenderer();

    void Initialize(Map *mapRef, Position *heroPosition);

    void LoadResources();

    void Unload();

    void Update(float deltaTime);

    void Draw() const;

    void SetCellSize(float size);

    void SetVisibleRange(int cellsX, int cellsY);

private:
    struct CameraView {
        float x;
        float y;
        int visibleCellsX;
        int visibleCellsY;
        float offsetX;
        float offsetY;
    };

    Map *map;
    Position *heroPosition;
    int screenWidth, screenHeight;
    float cellSize;
    CameraView camera{};
    bool resourcesLoaded;

    std::vector<std::pair<int, int> > visibleCells;

    Texture2D emptyTileTexture{};
    Texture2D wallTileTexture{};
    Texture2D heroTexture{};
    Texture2D monsterTexture{};
    Texture2D bossTexture{};
    Texture2D treasureTexture{};

    Font mapFont{};

    Color backgroundColor{};
    Color textColor{};

    Vector2 GetCellScreenPosition(int x, int y) const;

    void UpdateCameraPosition();

    bool IsCellVisible(int x, int y) const;

    Rectangle GetVisibleMapArea() const;

    void DrawMapGrid() const;

    void DrawEntities() const;

    void DrawMapBorder() const;

    void DrawMinimap() const;
};

#endif //MAPRENDERER_H
