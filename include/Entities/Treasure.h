//
// Created by Lenovo on 5.5.2025 г.
//

#ifndef TREASURES_H
#define TREASURES_H

#include "C:/DandD/include/Utils/Position.h"

class Treasure {
public:
    explicit Treasure(const Position &pos);

    Position getPosition() const;

    bool isCollected() const;

    void markAsCollected();

    bool operator==(const Treasure &other) const;

private:
    Position pos;
    bool collected = false;
};

#endif //TREASURES_H
