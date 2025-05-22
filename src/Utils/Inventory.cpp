#include <utility>

#include "../../../include/Utils/Inventory.h"

Inventory::Inventory(Weapon w, Spell s) : weapon(std::move(w)), spell(std::move(s)) {
}

bool Inventory::newWeapon(const Weapon &replacement, bool forceReplace) {
    if (!forceReplace) return false;
    this->weapon = replacement;
    return true;
}

bool Inventory::newArmor(const Armor &replacement, bool forceReplace) {
    if (!forceReplace) return false;
    this->armor = replacement;
    return true;
}

bool Inventory::newSpell(const Spell &replacement, bool forceReplace) {
    if (!forceReplace) return false;
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
