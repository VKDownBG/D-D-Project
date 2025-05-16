#include "../../../include/Entities/Items/Spell.h"

Spell::Spell(const std::string &n, double bonus, int level) : Item(ItemType::SPELL, n, bonus, level) {
}

Spell &Spell::operator=(const Spell &other) {
    if (this == &other)
        return *this;

    this->name = other.name;
    this->powerBonus = other.powerBonus;
    this->requiredLevel = other.requiredLevel;

    return *this;
}
