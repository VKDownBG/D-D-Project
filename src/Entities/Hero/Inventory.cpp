#include "../../../include/Entities/Items/Inventory.h"

Inventory::Inventory(const Weapon &w, const Spell &s) : weapon(w), spell(s) {
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

Weapon const &Inventory::getWeapon() const { return weapon; }

bool Inventory::hasArmor() const { return armor.has_value(); }

Armor const &Inventory::getArmor() const {
    if (hasArmor()) return *armor;

    return armor.value(); //при празно хвърля std::bad_optional_access
    //(необходимо за бъдеща логика
}

Spell const &Inventory::getSpell() const { return spell; }
