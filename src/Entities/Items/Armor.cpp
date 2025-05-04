#include "../../../include/Entities/Items/Armor.h"

Armor::Armor()
{
    this->name = "None";
    this->powerBonus = 0.0;
    this->requiredLevel = 0;
}

Armor::Armor(const std::string &n, double bonus, int level)
{
    this->name = n;
    this->powerBonus = bonus;
    this->requiredLevel = level;
}

Armor &Armor::operator=(const Armor &other)
{
    if (this == &other)
        return *this;

    this->name = other.name;
    this->powerBonus = other.powerBonus;
    this->requiredLevel = other.requiredLevel;

    return *this;
}

std::string Armor::getType() const { return "Armor"; }