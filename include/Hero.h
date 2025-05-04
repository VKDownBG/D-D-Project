#include "Entities/Items/Inventory.h"

#ifndef Hero_h
#define Hero_h

enum class Race {
    Human,
    Mage,
    Warrior
};

struct Position {
    int x, y;
};

class Hero {
private:
    Race race;
    int strength, mana, health, level;
    Inventory inventory = Inventory();
    Position currentPosition;

public:
    Hero(Race r);

    ~Hero() = default;

    [[nodiscard]] bool isDefeated() const;
};

#endif
