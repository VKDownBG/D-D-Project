#include "C:/DandD/include/Entities/Treasure.h"

// Treasure constructor
Treasure::Treasure(const Position &pos): pos(-1, -1) {
    this->pos = pos;
    collected = false; // Starts uncollected
}

// Position accessor
Position Treasure::getPosition() const { return pos; }

// Collection status
bool Treasure::isCollected() const { return collected; }
void Treasure::markAsCollected() { collected = true; }

// Comparison operator (based on position)
bool Treasure::operator==(const Treasure &other) const {
    return this->pos == other.pos;
}
