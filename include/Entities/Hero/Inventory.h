#ifndef Inventory_h
#define Inventory_h

#include "Entities/Items/Armor.h"
#include "Entities/Items/Spell.h"
#include <optional>
#include "Entities/Items/Weapon.h"

class Inventory {
private:
    Weapon weapon;
    std::optional<Armor> armor;
    Spell spell;

public:
    Inventory(const Weapon &w, const Spell &s);

    bool newWeapon(const Weapon &replacement, bool forceReplace = false);

    bool newArmor(const Armor &replacement, bool forceReplace = false);

    bool newSpell(const Spell &replacement, bool forceReplace = false);

    [[nodiscard]] const Weapon &getWeapon() const;

    [[nodiscard]] bool hasArmor() const;

    [[nodiscard]] const Armor &getArmor() const;

    [[nodiscard]] const Spell &getSpell() const;
};

#endif
