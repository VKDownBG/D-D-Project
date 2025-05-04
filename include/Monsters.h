#include <iostream>

#ifndef Monsters_h
#define Monsters_h

class Monsters
{
private:
    int strength, mana, health, level;
    double scaleArmor;

public:
    Monsters();

    void levelUp();

    bool isDefeated() const;
};

#endif
