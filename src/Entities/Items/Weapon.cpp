#include "../../../include/Entities/Items/Weapon.h"

Weapon::Weapon(const std::string &n, double bonus, int level) : Item(ItemType::WEAPON, n, bonus, level) {
}

Weapon &Weapon::operator=(const Weapon &other) {
    if (this == &other)
        return *this;

    this->name = other.name;
    this->powerBonus = other.powerBonus;
    this->requiredLevel = other.requiredLevel;

    return *this;
}
