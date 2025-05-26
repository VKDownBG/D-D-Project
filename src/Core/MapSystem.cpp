//
// Created by Lenovo on 4.5.2025 г.
//

#include "C:/DandD/include/Core/MapSystem.h"
#include <algorithm>
#include <fstream>
#include <stdexcept>

Map::Map() : width(0), height(0), currentLevel(1), startPos(-1, -1) {
}

void Map::validateMap() const {
    if (grid.empty()) {
        throw std::runtime_error("Map is empty!");
    }

    if (startPos.x < 0 || startPos.y < 0) {
        throw std::runtime_error("Missing start position (H)!");
    }
}

int Map::parseLevelNumber(const std::string &levelTag) {
    int level = 0;
    for (char c: levelTag) {
        if (isdigit(c)) {
            level = level * 10 + (c - '0');
        }
    }

    return (level == 0) ? 1 : level;
}

void Map::parseGridLine(const std::string &line, int rowIndex) {
    std::vector<char> row;

    for (size_t x = 0; x < line.size(); x++) {
        char c = line[x];
        if (c == ' ') continue;

        const size_t gridX = row.size();
        row.push_back(c);
        Position pos(static_cast<int>(gridX), rowIndex);

        if (c == 'H') {
            startPos = pos;

            row.back() = 'H';
        } else if (c == 'M' || c == 'B') {
            enemies.emplace_back(
                pos,
                (c == 'B' ? currentLevel + 1 : currentLevel),
                (c == 'B' ? MonsterType::BOSS : MonsterType::MONSTER)
            );

            row.back() = (c == 'B' ? 'B' : 'M');
        } else if (c == 'T') {
            treasures.emplace_back(pos);

            row.back() = 'T';
        }
    }

    if (!row.empty()) {
        grid.push_back(row);
    }
}

void Map::loadFromFile(const std::string &filePath, const std::string &levelTag) {
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

    while (std::getline(file, line)) {
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);

        if (line.empty()) continue;

        if (line[0] == '[' && (line.find(levelTag) != std::string::npos)) {
            readingLevel = true;
            continue;
        }

        if (readingLevel) {
            if (line == "DATA:") {
                readingData = true;
                continue;
            }

            if (readingData) {
                if (line[0] == '[') break;
                parseGridLine(line, static_cast<int>(grid.size()));
            }
        }
    }

    file.close();

    try {
        validateMap();
    } catch (const std::runtime_error &e) {
        std::cerr << "Map validation error: " << e.what() << std::endl;
        throw;
    }

    height = grid.size();
    width = grid[0].size();
}

Position Map::getStartPos() const {
    return startPos;
}

bool Map::isPassable(int x, int y) const {
    if (x < 0 || y < 0 || x >= width || y >= height) {
        return false;
    }

    return grid[y][x] == '.';
}

char Map::getCell(const Position &pos) const {
    if (pos.x < 0 || pos.y < 0 || pos.x >= width || pos.y >= height) {
        return '#';
    }

    return grid[pos.x][pos.y];
}

size_t Map::getWidth() const {
    return width;
}

size_t Map::getHeight() const {
    return height;
}

const std::vector<Monster> &Map::getMonsters() const {
    return enemies;
}

std::vector<Monster> &Map::getMonsters() {
    return enemies;
}

size_t Map::GetMonsterCount() const {
    return enemies.size();
}

const std::vector<Treasure> &Map::getTreasures() const {
    return treasures;
}

void Map::removeTreasure(const Treasure &treasure) {
    treasures.erase(std::remove(treasures.begin(), treasures.end(), treasure), treasures.end());
}

size_t Map::GetTreasureCount() const {
    return treasures.size();
}

int Map::GetCurrentLevel() const {
    return currentLevel;
}
