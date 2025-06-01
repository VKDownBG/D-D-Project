//
// Created by Lenovo on 5.5.2025 г.
//

#ifndef POSITION_H
#define POSITION_H

#include <iostream>

struct Position {
    int x, y;

    Position(int _x, int _y);

    Position(const Position &other);

    Position(Position &&other) noexcept;

    Position &operator=(const Position &other);

    bool operator==(const Position &other) const;

    bool operator!=(const Position &other) const;

    friend std::ostream &operator<<(std::ostream &os, const Position &pos);
};

#endif //POSITION_H
