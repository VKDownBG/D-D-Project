#include <utility>

#include "../../../include/Utils/Inventory.h"

Inventory::Inventory(Weapon w, Spell s) : weapon(std::move(w)), spell(std::move(s)) {
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
    return armor.value();
}

Weapon const &Inventory::GetWeapon() const {
    return weapon;
}

Spell const &Inventory::GetSpell() const {
    return spell;
}

bool Inventory::hasArmor() const {
    return armor.has_value();
}
