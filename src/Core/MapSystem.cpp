//
// Created by Lenovo on 4.5.2025 г.
//

#include "C:/DandD/include/Core/MapSystem.h"
#include <algorithm>
#include <fstream>
#include <stdexcept>

// Constructor: Initializes map state
Map::Map() : width(0), height(0), currentLevel(1), startPos(-1, -1) {
}

// Validates map integrity after loading
void Map::validateMap() const {
    if (grid.empty()) {
        throw std::runtime_error("Map is empty!");
    }

    if (startPos.x < 0 || startPos.y < 0) {
        throw std::runtime_error("Missing start position (H)!");
    }
}

// Extracts level number from section tag (e.g., "[LEVEL1]" → 1)
int Map::parseLevelNumber(const std::string &levelTag) {
    int level = 0;
    for (char c: levelTag) {
        if (isdigit(c)) {
            level = level * 10 + (c - '0'); // Build number from digits
        }
    }

    return (level == 0) ? 1 : level; // Default to level 1 if no number found
}

// Processes a single line of map data
void Map::parseGridLine(const std::string &line, const int rowIndex) {
    std::vector<char> row;

    for (size_t x = 0; x < line.size(); x++) {
        char c = line[x];
        if (c == ' ') continue; // Skip spaces

        const size_t gridX = row.size();
        row.push_back(c);
        Position pos(static_cast<int>(gridX), rowIndex);

        // Handle special map symbols
        if (c == 'H') {
            startPos = pos; // Player start position
            row.back() = 'H'; // Keep symbol in grid
        } else if (c == 'M' || c == 'B') {
            // Create monsters: Bosses are level+1, regular monsters at current level
            enemies.emplace_back(
                pos,
                (c == 'B' ? currentLevel + 1 : currentLevel),
                (c == 'B' ? MonsterType::BOSS : MonsterType::MONSTER)
            );
            row.back() = (c == 'B' ? 'B' : 'M'); // Keep symbol in grid
        } else if (c == 'T') {
            treasures.emplace_back(pos); // Treasure position
            row.back() = 'T'; // Keep symbol in grid
        }
    }

    // Add non-empty rows to grid
    if (!row.empty()) {
        grid.push_back(row);
    }
}

// Loads map data from file for specified level
void Map::loadFromFile(const std::string &filePath, const std::string &levelTag) {
    // Reset state
    currentLevel = parseLevelNumber(levelTag);
    grid.clear();
    enemies.clear();
    treasures.clear();
    startPos = Position(-1, -1);

    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filePath);
    }

    std::string line;
    bool readingLevel = false;
    bool readingData = false;

    // Parse file line-by-line
    while (std::getline(file, line)) {
        // Trim whitespace
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);

        if (line.empty()) continue;

        // Find level section
        if (line[0] == '[' && (line.find(levelTag) != std::string::npos)) {
            readingLevel = true;
            continue;
        }

        // Process level data
        if (readingLevel) {
            if (line == "DATA:") {
                readingData = true;
                continue;
            }

            if (readingData) {
                // Stop at next section
                if (line[0] == '[') break;
                parseGridLine(line, static_cast<int>(grid.size()));
            }
        }
    }

    file.close();

    // Validate loaded map
    try {
        validateMap();
    } catch (const std::runtime_error &e) {
        std::cerr << "Map validation error: " << e.what() << std::endl;
        throw;
    }

    // Set dimensions
    height = grid.size();
    width = (height > 0) ? grid[0].size() : 0;
}

// Getters for map properties and entities
Position Map::getStartPos() const {
    return startPos;
}

// Checks if position is walkable
bool Map::isPassable(const int x, const int y) const {
    // Boundary check
    if (x < 0 || y < 0 || x >= width || y >= height) {
        return false;
    }

    return grid[y][x] != '#'; // '#' is wall
}

// Gets cell character at position
char Map::getCell(const Position &pos) const {
    if (pos.x < 0 || pos.y < 0 || pos.x >= width || pos.y >= height) {
        return '#'; // Treat out-of-bounds as walls
    }

    return grid[pos.y][pos.x];
}

void Map::setCell(const Position &pos, const char c) {
    if (pos.x >= 0 && pos.x < width && pos.y >= 0 && pos.y < height) {
        grid[pos.y][pos.x] = c;
    }
}

size_t Map::getWidth() const {
    return width;
}

size_t Map::getHeight() const {
    return height;
}

// Monster accessors
const std::vector<Monster> &Map::getMonstersConst() const {
    return enemies;
}

std::vector<Monster> &Map::getMonsters() {
    return enemies;
}

size_t Map::GetMonsterCount() const {
    return enemies.size();
}

// Treasure accessors
const std::vector<Treasure> &Map::getTreasuresConst() const {
    return treasures;
}

std::vector<Treasure> &Map::getTreasures() {
    return treasures;
}

// Removes collected treasure
void Map::removeTreasure(const Treasure &treasure) {
    treasures.erase(std::remove(treasures.begin(), treasures.end(), treasure), treasures.end());
}

void Map::removeMonster(const Monster &monster) {
    enemies.erase(std::remove(enemies.begin(), enemies.end(), monster), enemies.end());
}

size_t Map::GetTreasureCount() const {
    return treasures.size();
}

int Map::GetCurrentLevel() const {
    return currentLevel;
}
