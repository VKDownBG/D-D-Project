//
// Created by Lenovo on 5.5.2025 г.
//

#include "C:/DandD/include/Utils/Position.h"

Position::Position(int _x, int _y) : x(_x), y(_y) {
}

Position::Position(const Position &other) {
    this->x = other.x;
    this->y = other.y;
}


bool Position::operator==(const Position &other) const {
    return this->x == other.x && this->y == other.y;
}

bool Position::operator!=(const Position &other) const {
    return !(*this == other);
}

std::ostream &operator<<(std::ostream &os, const Position &pos) {
    os << "(" << pos.x << ", " << pos.y << ")";
    return os;
}
