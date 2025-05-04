#include "../../../include/Entities/Items/Spell.h"

Spell::Spell()
{
    this->name = "None";
    this->powerBonus = 0.0;
    this->requiredLevel = 0;
}

Spell::Spell(const std::string &n, double bonus, int level)
{
    this->name = n;
    this->powerBonus = bonus;
    this->requiredLevel = level;
}

Spell &Spell::operator=(const Spell &other)
{
    if (this == &other)
        return *this;

    this->name = other.name;
    this->powerBonus = other.powerBonus;
    this->requiredLevel = other.requiredLevel;

    return *this;
}

std::string Spell::getType() const { return "Spell"; }
