#include "../../../include/Entities/Items/Inventory.h"

Inventory::Inventory(const Weapon &w, const Spell &s, const Armor &a)
{
    this->weapon = w;
    this->armor = a;
    this->spell = s;
}

void Inventory::newWeapon(const Weapon &replacement) { this->weapon = replacement; }

void Inventory::newArmor(const Armor &replacement) { this->armor = replacement; }

void Inventory::newSpell(const Spell &replacement) { this->spell = replacement; }

Weapon Inventory::getWeapon() const { return this->weapon; }

Armor Inventory::getArmor() const { return this->armor; }

Spell Inventory::getSpell() const { return this->spell; }