//
// Created by Lenovo on 5.5.2025 г.
//

#include "C:/DandD/include/Entities/Treasure.h"

Treasure::Treasure(const Position &pos): pos(-1, -1) {
    this->pos = pos;
}

Position Treasure::getPosition() const {
    return pos;
}

bool Treasure::isCollected() const {
    return collected;
}

void Treasure::markAsCollected() {
    this->collected = true;
}

bool Treasure::operator==(const Treasure &other) const {
    return this->pos == other.pos;
}
