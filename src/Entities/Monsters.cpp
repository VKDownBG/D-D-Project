#include "Monsters.h"

Monsters::Monsters() : strength(25), mana(25), health(50), level(1), scaleArmor(15) {}

void Monsters::levelUp()
{
    this->level++;
    this->strength += 10;
    this->mana += 10;
    this->health += 10;
    this->scaleArmor += 5;
}

bool Monsters::isDefeated() const { return this->health == 0; }