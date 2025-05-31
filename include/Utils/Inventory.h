#ifndef Inventory_h
#define Inventory_h

#include "C:/DandD/include/Items/Types/Armor.h"
#include "C:/DandD/include/Items/Types/Weapon.h"
#include "C:/DandD/include/Items/Types/Spell.h"
#include <optional>

class Inventory {
public:
    Inventory(Weapon w, Spell s);

    bool newWeapon(const Weapon &replacement);

    bool newArmor(const Armor &replacement);

    bool newSpell(const Spell &replacement);

    const Armor &GetArmor() const;

    const Weapon &GetWeapon() const;

    const Spell &GetSpell() const;

    bool hasArmor() const;

private:
    Weapon weapon;
    std::optional<Armor> armor;
    Spell spell;
};

#endif
