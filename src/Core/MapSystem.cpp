//
// Created by Lenovo on 4.5.2025 г.
//

#include "Core/MapSystem.h"
#include <algorithm>
#include <fstream>
#include <stdexcept>

Map::Map() : startPos(-1, -1) {
}

void Map::validateMap() const {
    if (grid.empty()) {
        throw std::runtime_error("Map is empty!");
    }

    const size_t expectedWidth = grid[0].size();
    for (const auto &row: grid) {
        if (row.size() != expectedWidth) {
            throw std::runtime_error("Map is not rectangular!");
        }
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
    int x = 0;
    for (char c: line) {
        if (c == ' ') {
            x++;
            continue;
        }

        row.push_back(c);
        Position pos(x, grid.size());

        if (c == 'H') {
            startPos = pos;
        } else if (c == 'M' || c == 'B') {
            enemies.emplace_back(
                pos, (c == 'B' ? currentLevel + 1 : currentLevel),
                (c == 'B'
                     ? MonsterType::Boss
                     : MonsterType::Monster)
            );
        } else if (c == 'T') {
            treasures.emplace_back(pos);
        }
        x++;
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
        line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());

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
                parseGridLine(line, grid.size());
            }
        }

        validateMap();

        height = grid.size();
        width = grid[0].size();
    }
}

Position Map::getStartPos() const {
    return startPos;
}

bool Map::isPassable(int x, int y) const {
    return x >= 0 && y >= 0 && x < 100 && y < 100 && grid[x][y] == '.';
}

char Map::getCell(const Position &pos) const {
    return grid[pos.x][pos.y];
}

int Map::getWidth() const {
    return width;
}

int Map::getHeight() const {
    return height;
}

const std::vector<Monster> &Map::getMonsters() const {
    return enemies;
}

const std::vector<Treasure> &Map::getTreasures() const {
    return treasures;
}
