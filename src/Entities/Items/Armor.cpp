#include "../../../include/Entities/Items/Armor.h"

Armor::Armor(const std::string &n, double bonus, int level): Item(ItemType::ARMOR, n, bonus, level) {
}

Armor &Armor::operator=(const Armor &other) {
    if (this == &other)
        return *this;

    this->name = other.name;
    this->powerBonus = other.powerBonus;
    this->requiredLevel = other.requiredLevel;

    return *this;
}
