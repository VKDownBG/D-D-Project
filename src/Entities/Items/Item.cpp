#include "../../../include/Entities/Items/Item.h"

Item::Item(ItemType type, std::string name, double powerBonus, int requiredLevel) : name(std::move(name)), type(type),
    powerBonus(powerBonus), requiredLevel(requiredLevel) {
}

ItemType Item::getType() const {
    return type;
}


std::string Item::getName() const { return name; }

double Item::getPowerBonus() const { return powerBonus / 100; }

int Item::getRequiredLevel() const { return requiredLevel; }
