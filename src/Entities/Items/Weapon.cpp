#include "../../../include/Entities/Items/Weapon.h"

Weapon::Weapon()
{
    this->name = "None";
    this->powerBonus = 0.0;
    this->requiredLevel = 0;
}

Weapon::Weapon(const std::string &n, double bonus, int level)
{
    this->name = n;
    this->powerBonus = bonus;
    this->requiredLevel = level;
}

Weapon &Weapon::operator=(const Weapon &other)
{
    if (this == &other)
        return *this;

    this->name = other.name;
    this->powerBonus = other.powerBonus;
    this->requiredLevel = other.requiredLevel;

    return *this;
}

std::string Weapon::getType() const { return "Weapon"; }