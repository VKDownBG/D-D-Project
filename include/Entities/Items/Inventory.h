#include "Weapon.h"
#include "Armor.h"
#include "Spell.h"
#include <iostream>

#ifndef Inventory_h
#define Inventory_h

class Inventory
{
private:
    Weapon weapon;
    Armor armor;
    Spell spell;

public:
    Inventory() = default;

    Inventory(const Weapon &w, const Spell &s, const Armor &a);

    void newWeapon(const Weapon &replacement);
    void newArmor(const Armor &replacement);
    void newSpell(const Spell &replacement);

    Weapon getWeapon() const;
    Armor getArmor() const;
    Spell getSpell() const;
};

#endif