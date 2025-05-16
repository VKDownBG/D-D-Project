//
// Created by Lenovo on 5.5.2025 г.
//

#ifndef TREASURES_H
#define TREASURES_H

#include "Utils/Position.h"

class Treasure {
private:
    Position pos;
    bool collected = false;

public:
    explicit Treasure(const Position &pos);

    [[nodiscard]] Position getPosition() const;

    [[nodiscard]] bool isCollected() const;

    void markAsCollected();
};

#endif //TREASURES_H
