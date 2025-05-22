//
// Created by Lenovo on 4.5.2025г.
//
#ifndef MAPSYSTEM_H
#define MAPSYSTEM_H

#include "C:/DandD/include/Utils/Position.h"
#include "C:/DandD/include/Entities/Monster.h"
#include "C:/DandD/include/Entities/Treasure.h"
#include <vector>
#include <string>

class Map {
public:
    Map();

    void loadFromFile(const std::string &filePath, const std::string &levelTag);

    Position getStartPos() const;

    bool isPassable(int x, int y) const;

    char getCell(const Position &pos) const;

    int getWidth() const;

    int getHeight() const;

    const std::vector<Monster> &getMonsters() const;

    int GetMonsterCount() const;

    const std::vector<Treasure> &getTreasures() const;

    void removeTreasure(const Treasure &treasure);

    int GetTreasureCount() const;

    int GetCurrentLevel() const;

private:
    std::vector<std::vector<char> > grid;
    size_t width, height;
    int currentLevel;

    Position startPos;
    std::vector<Monster> enemies;
    std::vector<Treasure> treasures;

    void validateMap() const;

    static int parseLevelNumber(const std::string &line);

    void parseGridLine(const std::string &line, int rowIndex);
};

#endif //MAPSYSTEM_H
