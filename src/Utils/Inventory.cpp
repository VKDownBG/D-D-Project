#include <utility>

#include "../../../include/Utils/Inventory.h"

Inventory::Inventory(Armor a, Weapon w, Spell s) : armor(std::move(a)), weapon(std::move(w)), spell(std::move(s)) {
}

bool Inventory::newWeapon(const Weapon &replacement) {
    this->weapon = replacement;
    return true;
}

bool Inventory::newArmor(const Armor &replacement) {
    this->armor = replacement;
    return true;
}

bool Inventory::newSpell(const Spell &replacement) {
    this->spell = replacement;
    return true;
}

Armor const &Inventory::GetArmor() const {
    return armor;
}

Weapon const &Inventory::GetWeapon() const {
    return weapon;
}

Spell const &Inventory::GetSpell() const {
    return spell;
}
