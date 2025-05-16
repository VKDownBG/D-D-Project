//
// Created by Lenovo on 4.5.2025г.
//
#ifndef MAPSYSTEM_H
#define MAPSYSTEM_H

#include "Utils/Position.h"
#include "Entities/Monster.h"
#include "Entities/Treasure.h"
#include <vector>
#include <string>

class Map {
private:
    std::vector<std::vector<char> > grid;

    Position startPos;

    std::vector<Monster> enemies;
    std::vector<Treasure> treasures;

    long width = 0, height = 0;

    int currentLevel = 1;

    void parseGridLine(const std::string &line, int rowIndex);

    void validateMap() const;

    static int parseLevelNumber(const std::string &line);

public:
    Map();

    void loadFromFile(const std::string &filePath, const std::string &levelTag);

    [[nodiscard]] Position getStartPos() const;

    [[nodiscard]] bool isPassable(int x, int y) const;

    [[nodiscard]] char getCell(const Position &pos) const;

    [[nodiscard]] int getWidth() const;

    [[nodiscard]] int getHeight() const;

    [[nodiscard]] const std::vector<Monster> &getMonsters() const;

    [[nodiscard]] const std::vector<Treasure> &getTreasures() const;
};

#endif //MAPSYSTEM_H
