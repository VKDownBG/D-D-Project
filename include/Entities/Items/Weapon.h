#include "Item.h"

#ifndef Weapon_h
#define Weapon_h

class Weapon : public Item
{
public:
    Weapon();

    Weapon(const std::string& n, double bonus, int level);

    Weapon &operator=(const Weapon &other);

    std::string getType() const override;
};

#endif