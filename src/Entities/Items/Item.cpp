#include "../../../include/Entities/Items/Item.h"

std::string Item::getName() const { return this->name; }

double Item::getPowerBonus() const { return this->powerBonus; }

int Item::getRequiredLevel() const { return this->requiredLevel; }