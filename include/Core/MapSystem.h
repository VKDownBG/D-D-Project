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

    void setCell(const Position &pos, char c);

    size_t getWidth() const;

    size_t getHeight() const;

    const std::vector<Monster> &getMonstersConst() const;

    std::vector<Monster> &getMonsters();

    size_t GetMonsterCount() const;

    const std::vector<Treasure> &getTreasuresConst() const;

    std::vector<Treasure> &getTreasures();

    void removeTreasure(const Treasure &treasure);

    void removeMonster(const Monster &monster);

    size_t GetTreasureCount() const;

    int GetCurrentLevel() const;

private:
    std::vector<std::vector<char> > grid;
    size_t width, height;
    int currentLevel;

    Position startPos;
    std::vector<Monster> enemies;
    std::vector<Treasure> treasures;

private:
    void validateMap() const;

    static int parseLevelNumber(const std::string &line);

    void parseGridLine(const std::string &line, int rowIndex);
};

#endif //MAPSYSTEM_H
