//
// Created by Lenovo on 21.5.2025 г..
//

#ifndef MAPRENDERER_H
#define MAPRENDERER_H

#include "C:/DandD/include/Core/MapSystem.h"
#include "raylib.h"
#include <vector>

class MapRenderer {
public:
    MapRenderer(int _screenWidth, int _screenHeight);

    ~MapRenderer();

    void Initialize(Map *mapRef, Position *heroPosition);

    void LoadResources();

    void Unload();

    void Update(float deltaTime);

    void Draw() const;

    void DrawPortal(const Position &position, float animationTime) const;

    float GetCellSize() const;

    void SetCellSize(float size);

    void SetVisibleRange(int cellsX, int cellsY);

    Vector2 WorldToScreen(int worldX, int worldY) const;

    void removeMonster(const Monster *monster);

private:
    Map *map;
    Position *heroPosition;

    int screenWidth, screenHeight;
    float cellSize;
    bool resourcesLoaded;

    struct Camera {
        Vector2 position;
        Vector2 target;
        int visibleCellsX;
        int visibleCellsY;
        float smoothFactor;

        Camera() : position{0, 0}, target{0, 0},
                   visibleCellsX{0}, visibleCellsY{0},
                   smoothFactor{0.05f} {
        }
    } camera;

    mutable struct VisibleArea {
        int startX, startY, endX, endY;
        bool needsUpdate;
        Vector2 lastCameraPos;

        VisibleArea() : startX(0), startY(0), endX(0), endY(0),
                        needsUpdate(true), lastCameraPos{-1, -1} {
        }
    } visibleArea;

    Texture2D floorTexture;
    Texture2D wallTexture;
    Texture2D heroTexture;
    Texture2D monsterTexture;
    Texture2D bossTexture;
    Texture2D treasureTexture;
    Texture2D portalTexture;

    Font gameFont;

    const Color BACKGROUND_COLOR = {20, 20, 35, 255};
    const Color BORDER_COLOR = {80, 80, 120, 255};
    const Color BORDER_GLOW = {100, 100, 180, 150};

private:
    void UpdateCamera();

    void UpdateVisibleArea() const;

    Rectangle GetMapArea() const;

    bool IsInVisibleArea(int x, int y) const;

    void DrawTiles() const;

    void DrawEntities() const;

    void DrawMapBorder() const;

    void DrawTexturedTile(const Texture2D &texture, const Vector2 &screenPos) const;

    void DrawMinimap() const;

    void DrawMinimapBackground(Vector2 pos, float size) const;

    void DrawMinimapTiles(Vector2 pos, float size, float cellSize) const;

    void DrawMinimapEntities(Vector2 pos, float size, float cellSize) const;

    void DrawMinimapViewport(Vector2 pos, float size, float cellSize) const;
};

#endif //MAPRENDERER_H
