//
// Created by Lenovo on 21.5.2025 г.
//


#include "C:/DandD/include/UI/renderers/MapRenderer.h"
#include <cmath>
#include <algorithm>

// Constructor: Initializes MapRenderer with screen dimensions and default camera settings.
MapRenderer::MapRenderer(const int _screenWidth, const int _screenHeight)
    : map(nullptr), heroPosition(nullptr), // Pointers to map and hero position are initially null.
      screenWidth(_screenWidth), screenHeight(_screenHeight), // Store screen dimensions.
      cellSize(40.0f), resourcesLoaded(false) {
    // Default cell size and resource loading status.
    camera.visibleCellsX = 10; // Number of cells visible horizontally in the main map view.
    camera.visibleCellsY = 10; // Number of cells visible vertically in the main map view.
    camera.smoothFactor = 0.1f; // Factor for smooth camera movement.
}

// Destructor: Ensures all loaded resources are unloaded when the renderer is destroyed.
MapRenderer::~MapRenderer() {
    Unload();
}

// Initializes the renderer with references to the game map and hero's position.
void MapRenderer::Initialize(Map *mapRef, Position *heroPositionRef) {
    map = mapRef; // Assign the map reference.
    heroPosition = heroPositionRef; // Assign the hero's position reference.

    // If both map and hero position are valid, set initial camera position.
    if (heroPosition && map) {
        camera.position = {
            static_cast<float>(heroPosition->x) - camera.visibleCellsX * 0.5f, // Center camera on hero's X.
            static_cast<float>(heroPosition->y) - camera.visibleCellsY * 0.5f // Center camera on hero's Y.
        };
        camera.target = camera.position; // Set the camera target to its initial position.
        visibleArea.needsUpdate = true; // Mark the visible area as needing an update.
    }
}

// Loads all necessary textures and fonts for rendering the map and its entities.
void MapRenderer::LoadResources() {
    if (resourcesLoaded) return; // Prevent re-loading if already loaded.

    // Load various textures from specified paths.
    floorTexture = LoadTexture("C:/DandD/assets/map/floor_tile.png");
    wallTexture = LoadTexture("C:/DandD/assets/map/wall_tile.png");
    heroTexture = LoadTexture("C:/DandD/assets/entities/hero_icon.png");
    monsterTexture = LoadTexture("C:/DandD/assets/entities/monster_icon.png");
    bossTexture = LoadTexture("C:/DandD/assets/entities/boss_icon.png");
    treasureTexture = LoadTexture("C:/DandD/assets/objects/treasure_icon.png");
    portalTexture = LoadTexture("C:/DandD/assets/objects/portal_icon.png");

    gameFont = LoadFont("C:/DandD/assets/fonts/.TTF"); // Load game font.

    resourcesLoaded = true; // Mark resources as loaded.
}

// Unloads all previously loaded textures and fonts to free up memory.
void MapRenderer::Unload() {
    if (!resourcesLoaded) return; // Only unload if resources were loaded.

    // Unload all textures.
    UnloadTexture(floorTexture);
    UnloadTexture(wallTexture);
    UnloadTexture(heroTexture);
    UnloadTexture(monsterTexture);
    UnloadTexture(bossTexture);
    UnloadTexture(treasureTexture);
    UnloadTexture(portalTexture);
    UnloadFont(gameFont); // Unload the font.

    resourcesLoaded = false; // Mark resources as unloaded.
}

// Updates the renderer's state, including screen dimensions and camera position.
void MapRenderer::Update(float deltaTime) {
    const int newWidth = GetScreenWidth();
    const int newHeight = GetScreenHeight();

    // Update screen dimensions if the window size changes.
    if (newWidth != screenWidth || newHeight != screenHeight) {
        screenWidth = newWidth;
        screenHeight = newHeight;
    }

    UpdateCamera(); // Update the camera's position.
}

// Smoothly moves the camera towards the hero's position and clamps it within map bounds.
void MapRenderer::UpdateCamera() {
    if (!heroPosition || !map) return; // Requires hero position and map to update camera.

    // Set the camera's target position, centered on the hero.
    camera.target = {
        static_cast<float>(heroPosition->x) - camera.visibleCellsX * 0.5f,
        static_cast<float>(heroPosition->y) - camera.visibleCellsY * 0.5f
    };

    // Calculate the difference between current camera position and target.
    Vector2 diff = {
        camera.target.x - camera.position.x,
        camera.target.y - camera.position.y
    };

    // Smoothly interpolate the camera's position towards the target.
    camera.position.x += diff.x * camera.smoothFactor;
    camera.position.y += diff.y * camera.smoothFactor;

    // Calculate maximum camera coordinates to prevent it from going off the map.
    const float maxX = static_cast<float>(map->getWidth()) - camera.visibleCellsX;
    const float maxY = static_cast<float>(map->getHeight()) - camera.visibleCellsY;

    // Clamp the camera's position within valid map boundaries.
    camera.position.x = std::clamp(camera.position.x, 0.0f, std::max(0.0f, maxX));
    camera.position.y = std::clamp(camera.position.y, 0.0f, std::max(0.0f, maxY));

    // Calculate difference in camera position since last update to check if visible area needs recalculation.
    Vector2 cameraDiff = {
        std::abs(camera.position.x - visibleArea.lastCameraPos.x),
        std::abs(camera.position.y - visibleArea.lastCameraPos.y)
    };

    // If camera moved significantly, mark visible area for update.
    if (cameraDiff.x > 0.01f || cameraDiff.y > 0.01f) {
        visibleArea.needsUpdate = true;
    }
}

// Recalculates the rectangular area of map cells that are currently visible on screen.
void MapRenderer::UpdateVisibleArea() const {
    if (!visibleArea.needsUpdate) return; // Only update if necessary.

    // Get integer part of camera position.
    const int cameraX = static_cast<int>(std::floor(camera.position.x));
    const int cameraY = static_cast<int>(std::floor(camera.position.y));

    // Calculate the start and end coordinates of the visible area,
    // with a small buffer (+/- 1 or 2 cells) for smooth scrolling/rendering.
    visibleArea.startX = std::max(0, cameraX - 1);
    visibleArea.startY = std::max(0, cameraY - 1);
    visibleArea.endX = std::min(static_cast<int>(map->getWidth()),
                                cameraX + camera.visibleCellsX + 2);
    visibleArea.endY = std::min(static_cast<int>(map->getHeight()),
                                cameraY + camera.visibleCellsY + 2);

    visibleArea.lastCameraPos = camera.position; // Store current camera position.
    visibleArea.needsUpdate = false; // Mark update as complete.
}

// Converts world coordinates (map grid) to screen coordinates (pixels).
Vector2 MapRenderer::WorldToScreen(const int worldX, const int worldY) const {
    const Rectangle mapArea = GetMapArea(); // Get the rectangle defining the map's drawing area on screen.

    // Calculate screen coordinates relative to the map area and camera position.
    const float screenX = (worldX - camera.position.x) * cellSize;
    const float screenY = (worldY - camera.position.y) * cellSize;

    return {mapArea.x + screenX, mapArea.y + screenY};
}

// Returns the screen rectangle where the main game map should be drawn.
Rectangle MapRenderer::GetMapArea() const {
    const float mapWidth = camera.visibleCellsX * cellSize; // Calculate map width in pixels.
    const float mapHeight = camera.visibleCellsY * cellSize; // Calculate map height in pixels.
    const float mapX = (screenWidth - mapWidth) * 0.5f; // Center the map horizontally.
    const float mapY = 150.0f; // Fixed Y position for the top of the map.

    return {mapX, mapY, mapWidth, mapHeight};
}

// Checks if a given world coordinate (map cell) is currently within the visible screen area.
bool MapRenderer::IsInVisibleArea(const int x, const int y) const {
    UpdateVisibleArea(); // Ensure visible area is up-to-date.
    return x >= visibleArea.startX && x < visibleArea.endX && // Check X bounds.
           y >= visibleArea.startY && y < visibleArea.endY; // Check Y bounds.
}

// Draws the main game map, including borders, tiles, entities, and the minimap.
void MapRenderer::Draw() const {
    if (!map || !resourcesLoaded) return; // Can't draw without map data or loaded resources.

    const Rectangle mapArea = GetMapArea();
    // DrawRectangleRec(mapArea, BACKGROUND_COLOR); // Optional: Draw a background for the map area.

    DrawMapBorder(); // Draw the border around the main map.
    DrawTiles(); // Draw all visible map tiles (floor, walls).
    DrawEntities(); // Draw characters, monsters, and objects.
    DrawMinimap(); // Draw the small minimap in the corner.
}

// Draws the portal entity with a pulsing animation effect.
void MapRenderer::DrawPortal(const Position &position, float animationTime) const {
    if (!IsInVisibleArea(position.x, position.y) || portalTexture.id == 0) return;
    // Only draw if visible and texture loaded.

    Vector2 screenPos = WorldToScreen(position.x, position.y); // Convert world to screen coordinates.

    // Calculate pulsing scale based on animation time.
    const float pulseScale = 1.0f + std::sin(animationTime * 4.0f) * 0.1f;
    const float size = cellSize * pulseScale; // Calculate scaled size.

    // Adjust screen position to center the scaled portal.
    const float offset = (cellSize - size) * 0.5f;
    screenPos.x += offset;
    screenPos.y += offset;

    // Draw the portal texture with pulsing effect.
    DrawTexturePro(
        portalTexture,
        {0, 0, static_cast<float>(portalTexture.width), static_cast<float>(portalTexture.height)},
        // Source rectangle from texture.
        {screenPos.x, screenPos.y, size, size}, // Destination rectangle on screen with scaled size.
        {0, 0}, 0.0f, WHITE // Origin, rotation, tint.
    );
}

// Draws the border and an inner glow effect around the main map display area.
void MapRenderer::DrawMapBorder() const {
    const Rectangle mapArea = GetMapArea(); // Get the map's screen area.

    DrawRectangleLinesEx(mapArea, 3, BORDER_COLOR); // Draw the main thick border.

    // Inner glow effect: a thinner border slightly inside the main one.
    const Rectangle innerBorder = {
        mapArea.x + 3, mapArea.y + 3,
        mapArea.width - 6, mapArea.height - 6
    };
    DrawRectangleLinesEx(innerBorder, 1, BORDER_GLOW);
}

// Helper function to draw a single textured tile at a given screen position.
void MapRenderer::DrawTexturedTile(const Texture2D &texture, const Vector2 &screenPos) const {
    if (texture.id != 0) {
        // Only draw if the texture is valid.
        DrawTexturePro(
            texture,
            {0, 0, static_cast<float>(texture.width), static_cast<float>(texture.height)}, // Source rectangle.
            {screenPos.x, screenPos.y, cellSize, cellSize}, // Destination rectangle, scaled to cell size.
            {0, 0}, 0.0f, WHITE // Origin, rotation, tint.
        );
    }
}

// Iterates through visible map cells and draws their corresponding tiles (floor or wall).
void MapRenderer::DrawTiles() const {
    if (!map) return; // Requires map data.

    UpdateVisibleArea(); // Ensure the visible area is current.

    // Loop through all cells in the visible area.
    for (int y = visibleArea.startY; y < visibleArea.endY; y++) {
        for (int x = visibleArea.startX; x < visibleArea.endX; x++) {
            const char cellType = map->getCell({x, y}); // Get the character representing the cell type.
            Vector2 screenPos = WorldToScreen(x, y); // Convert world coordinates to screen coordinates.

            //screenPos.y = GetMapArea().y + GetMapArea().height - screenPos.y - cellSize; // This line appears to be commented out or potentially for an inverted Y-axis.

            // Draw different textures based on cell type.
            switch (cellType) {
                case '#':
                    DrawTexturedTile(wallTexture, screenPos); // Draw wall.
                    break;
                case '.':
                case 'H':
                case 'M':
                case 'B':
                case 'T':
                    DrawTexturedTile(floorTexture, screenPos); // Draw floor for empty spaces or where entities are.
                    break;
                default: ; // Do nothing for unknown cell types.
            }
        }
    }
}

// Draws all entities on the map (treasures, monsters, hero) if they are within the visible area.
void MapRenderer::DrawEntities() const {
    if (!map || !heroPosition) return; // Requires map and hero position.

    // Draw treasures.
    for (const auto &treasure: map->getTreasures()) {
        if (IsInVisibleArea(treasure.getPosition().x, treasure.getPosition().y)) {
            Vector2 screenPos = WorldToScreen(treasure.getPosition().x, treasure.getPosition().y);
            const Rectangle destRect = {screenPos.x, screenPos.y, cellSize, cellSize};

            //screenPos.y = GetMapArea().y + GetMapArea().height - screenPos.y - cellSize; // Similar commented line.

            if (treasureTexture.id != 0) {
                DrawTexturePro(
                    treasureTexture,
                    {0, 0, static_cast<float>(treasureTexture.width), static_cast<float>(treasureTexture.height)},
                    destRect, {0, 0}, 0.0f, WHITE
                );
            }
        }
    }

    // Draw monsters (only if not defeated).
    for (const auto &monster: map->getMonsters()) {
        if (!monster.isDefeated() && IsInVisibleArea(monster.GetPosition().x, monster.GetPosition().y)) {
            const Vector2 screenPos = WorldToScreen(monster.GetPosition().x, monster.GetPosition().y);
            const Rectangle destRect = {screenPos.x, screenPos.y, cellSize, cellSize};

            // Choose texture based on monster type (boss or regular).
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

    // Draw hero (always drawn on top of other entities).
    if (IsInVisibleArea(heroPosition->x, heroPosition->y)) {
        Vector2 screenPos = WorldToScreen(heroPosition->x, heroPosition->y);
        const Rectangle destRect = {screenPos.x, screenPos.y, cellSize, cellSize};

        if (heroTexture.id != 0) {
            DrawTexturePro(
                heroTexture,
                {0, 0, static_cast<float>(heroTexture.width), static_cast<float>(heroTexture.height)},
                destRect, {0, 0}, 0.0f, WHITE
            );
        }
    }
}

// Draws a small minimap in the corner of the screen, showing a portion of the map and entities.
void MapRenderer::DrawMinimap() const {
    if (!map || !heroPosition) return; // Requires map and hero position.

    constexpr float MINIMAP_SIZE = 200.0f; // Size of the minimap square.
    constexpr float MARGIN = 20.0f; // Margin from screen edges.
    constexpr int MINIMAP_TILES = 15; // Number of tiles (cells) shown on each side of the minimap.

    // Calculate minimap's top-left screen position.
    const Vector2 minimapPos = {
        screenWidth - MINIMAP_SIZE - MARGIN,
        screenHeight - MINIMAP_SIZE - MARGIN
    };

    // Calculate individual cell size for the minimap.
    const float cellSize = (MINIMAP_SIZE - 4) / MINIMAP_TILES; // Subtract 4 for a small internal border.

    DrawMinimapBackground(minimapPos, MINIMAP_SIZE); // Draw the minimap's background and border.
    DrawMinimapTiles(minimapPos, MINIMAP_SIZE, cellSize); // Draw the map tiles on the minimap.
    DrawMinimapEntities(minimapPos, MINIMAP_SIZE, cellSize);
    // Draw entities (hero, monsters, treasures) on the minimap.
    DrawMinimapViewport(minimapPos, MINIMAP_SIZE, cellSize); // Draw the main camera's viewport on the minimap.
}

// Draws the background rectangle and border for the minimap.
void MapRenderer::DrawMinimapBackground(const Vector2 pos, const float size) const {
    DrawRectangle(pos.x, pos.y, size, size, {30, 30, 50, 200}); // Dark semi-transparent background.

    DrawRectangleLinesEx({pos.x, pos.y, size, size}, 2, {100, 100, 150, 200}); // Blue-ish border.
}

// Draws the individual tiles (walls and floors) on the minimap.
void MapRenderer::DrawMinimapTiles(const Vector2 pos, float size, const float cellSize) const {
    const Vector2 offset = {pos.x + 2, pos.y + 2}; // Offset for internal drawing to account for border.
    constexpr int MINIMAP_TILES = 15;
    const int halfTiles = MINIMAP_TILES / 2;

    // Calculate the world coordinates for the minimap's top-left corner, centered around the hero.
    int minX = heroPosition->x - halfTiles;
    int minY = heroPosition->y - halfTiles;
    int maxX = minX + MINIMAP_TILES;
    int maxY = minY + MINIMAP_TILES;

    // Clamp the minimap's world boundaries to the actual map's boundaries.
    // If clamping occurs, shift the entire minimap view to keep the same number of tiles.
    if (minX < 0) {
        maxX += -minX; // Shift right if left edge is out of bounds.
        minX = 0;
    }
    if (minY < 0) {
        maxY += -minY; // Shift down if top edge is out of bounds.
        minY = 0;
    }
    if (maxX > static_cast<int>(map->getWidth())) {
        minX -= (maxX - static_cast<int>(map->getWidth())); // Shift left if right edge is out of bounds.
        maxX = static_cast<int>(map->getWidth());
    }
    if (maxY > static_cast<int>(map->getHeight())) {
        minY -= (maxY - static_cast<int>(map->getHeight())); // Shift up if bottom edge is out of bounds.
        maxY = static_cast<int>(map->getHeight());
    }

    // Ensure minX and minY don't go below 0 after any shifting.
    minX = std::max(0, minX);
    minY = std::max(0, minY);

    // Iterate through the calculated minimap world area and draw tiles.
    for (int worldY = minY; worldY < maxY; worldY++) {
        for (int worldX = minX; worldX < maxX; worldX++) {
            const char cellType = map->getCell({worldX, worldY}); // Get cell type from the map.

            // Convert world coordinates to minimap's local screen coordinates.
            const int minimapX = worldX - minX;
            const int minimapY = worldY - minY;

            const float cellX = offset.x + minimapX * cellSize;
            const float cellY = offset.y + minimapY * cellSize;

            Color cellColor = {0, 0, 0, 0}; // Default transparent color.

            // Assign colors based on cell type.
            switch (cellType) {
                case '#':
                    cellColor = {100, 100, 130, 255}; // Wall color.
                    break;
                case '.':
                case 'H':
                case 'M':
                case 'B':
                case 'T':
                    cellColor = {50, 50, 70, 255}; // Floor color.
                    break;
            }

            if (cellColor.a > 0) {
                // Only draw if color is not transparent.
                DrawRectangle(cellX, cellY, cellSize, cellSize, cellColor);
            }
        }
    }
}

// Draws entities (treasures, monsters, hero) as colored dots on the minimap.
void MapRenderer::DrawMinimapEntities(const Vector2 pos, float size, const float cellSize) const {
    const Vector2 offset = {pos.x + 2, pos.y + 2};
    constexpr int MINIMAP_TILES = 15;
    const int halfTiles = MINIMAP_TILES / 2;

    // Recalculate minimap world bounds (same logic as for tiles).
    int minX = heroPosition->x - halfTiles;
    int minY = heroPosition->y - halfTiles;
    int maxX = minX + MINIMAP_TILES;
    int maxY = minY + MINIMAP_TILES;

    // Apply the same clamping and shifting logic as in DrawMinimapTiles.
    if (minX < 0) {
        maxX += -minX;
        minX = 0;
    }
    if (minY < 0) {
        maxY += -minY;
        minY = 0;
    }
    if (maxX > static_cast<int>(map->getWidth())) {
        minX -= (maxX - static_cast<int>(map->getWidth()));
        maxX = static_cast<int>(map->getWidth());
    }
    if (maxY > static_cast<int>(map->getHeight())) {
        minY -= (maxY - static_cast<int>(map->getHeight()));
        maxY = static_cast<int>(map->getHeight());
    }

    minX = std::max(0, minX);
    minY = std::max(0, minY);

    // Draw treasures on the minimap if they are within its bounds.
    for (const auto &treasure: map->getTreasures()) {
        const int treasureX = treasure.getPosition().x;
        const int treasureY = treasure.getPosition().y;

        if (treasureX >= minX && treasureX < maxX && treasureY >= minY && treasureY < maxY) {
            const float x = offset.x + (treasureX - minX) * cellSize;
            const float y = offset.y + (treasureY - minY) * cellSize;
            DrawRectangle(x, y, cellSize, cellSize, {220, 180, 50, 255}); // Gold-like color for treasures.
        }
    }

    // Draw monsters on the minimap if they are within bounds and not defeated.
    for (const auto &monster: map->getMonsters()) {
        if (!monster.isDefeated()) {
            const int monsterX = monster.GetPosition().x;
            const int monsterY = monster.GetPosition().y;

            if (monsterX >= minX && monsterX < maxX && monsterY >= minY && monsterY < maxY) {
                const float x = offset.x + (monsterX - minX) * cellSize;
                const float y = offset.y + (monsterY - minY) * cellSize;

                // Different colors for boss monsters and regular monsters.
                const Color color = (monster.GetType() == MonsterType::BOSS)
                                        ? Color{200, 50, 50, 255} // Red for boss.
                                        : Color{180, 80, 80, 255}; // Darker red for regular monster.

                DrawRectangle(x, y, cellSize, cellSize, color);
            }
        }
    }

    // Draw hero on the minimap (always visible since the minimap is centered on the hero).
    const float heroX = offset.x + (heroPosition->x - minX) * cellSize;
    const float heroY = offset.y + (heroPosition->y - minY) * cellSize;
    DrawRectangle(heroX, heroY, cellSize, cellSize, {50, 150, 220, 255}); // Blue for hero.
}

// Draws a rectangle on the minimap representing the main camera's current viewport.
void MapRenderer::DrawMinimapViewport(const Vector2 pos, float size, const float cellSize) const {
    const Vector2 offset = {pos.x + 2, pos.y + 2};
    constexpr int MINIMAP_TILES = 15;
    const int halfTiles = MINIMAP_TILES / 2;

    // Recalculate minimap world bounds (same logic as for tiles).
    int minX = heroPosition->x - halfTiles;
    int minY = heroPosition->y - halfTiles;
    int maxX = minX + MINIMAP_TILES;
    int maxY = minY + MINIMAP_TILES;

    // Apply clamping and shifting.
    if (minX < 0) {
        maxX += -minX;
        minX = 0;
    }
    if (minY < 0) {
        maxY += -minY;
        minY = 0;
    }
    if (maxX > static_cast<int>(map->getWidth())) {
        minX -= (maxX - static_cast<int>(map->getWidth()));
        maxX = static_cast<int>(map->getWidth());
    }
    if (maxY > static_cast<int>(map->getHeight())) {
        minY -= (maxY - static_cast<int>(map->getHeight()));
        maxY = static_cast<int>(map->getHeight());
    }

    minX = std::max(0, minX);
    minY = std::max(0, minY);

    // Calculate viewport's screen position and size relative to the minimap.
    const float viewX = offset.x + (camera.position.x - minX) * cellSize;
    const float viewY = offset.y + (camera.position.y - minY) * cellSize;
    const float viewWidth = camera.visibleCellsX * cellSize;
    const float viewHeight = camera.visibleCellsY * cellSize;

    // Only draw the viewport rectangle if it's within the minimap's visible area.
    if (viewX + viewWidth > offset.x && viewX < offset.x + MINIMAP_TILES * cellSize &&
        viewY + viewHeight > offset.y && viewY < offset.y + MINIMAP_TILES * cellSize) {
        DrawRectangleLinesEx(
            {viewX, viewY, viewWidth, viewHeight},
            1, {220, 220, 255, 200} // Light blue-ish border for the viewport.
        );
    }
}

// Returns the current size (in pixels) of a single map cell.
float MapRenderer::GetCellSize() const {
    return cellSize;
}

// Sets the size (in pixels) of a single map cell, clamping it to a reasonable range.
// Also triggers a visible area update.
void MapRenderer::SetCellSize(const float size) {
    cellSize = std::max(10.0f, std::min(size, 100.0f)); // Clamp between 10 and 100.
    visibleArea.needsUpdate = true; // Mark visible area for recalculation.
}

// Sets the number of cells visible horizontally and vertically in the main map view.
// Also triggers a visible area update and recenters the camera.
void MapRenderer::SetVisibleRange(const int cellsX, const int cellsY) {
    camera.visibleCellsX = std::max(5, cellsX); // Ensure minimum 5 visible cells.
    camera.visibleCellsY = std::max(5, cellsY); // Ensure minimum 5 visible cells.
    visibleArea.needsUpdate = true; // Mark visible area for recalculation.

    // Recenter the camera immediately after changing the visible range.
    if (heroPosition) {
        camera.target = {
            static_cast<float>(heroPosition->x) - camera.visibleCellsX * 0.5f,
            static_cast<float>(heroPosition->y) - camera.visibleCellsY * 0.5f
        };
    }
}

// This method's purpose is to "remove" a monster by drawing a floor tile over its position.
// It's effectively redrawing the background at the monster's previous location,
// which is a common technique for clearing entities in tile-based rendering without true "removal"
// from a dynamic list in the renderer itself.
void MapRenderer::removeMonster(const Monster *monster) {
    const Vector2 screenPos = WorldToScreen(monster->GetPosition().x, monster->GetPosition().y);
    const Rectangle destRect = {screenPos.x, screenPos.y, cellSize, cellSize};

    const Texture *texture = &floorTexture; // Use the floor texture to cover the monster.

    if (texture->id != 0) {
        // Ensure the floor texture is loaded.
        DrawTexturePro(
            *texture,
            {0, 0, static_cast<float>(texture->width), static_cast<float>(texture->height)},
            destRect, {0, 0}, 0.0f, WHITE
        );
    }
}
