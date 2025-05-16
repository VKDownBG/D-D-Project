#ifndef Weapon_h
#define Weapon_h

#include "Item.h"

class Weapon final : public Item {
public:
    Weapon(const std::string &n, double bonus, int level);

    Weapon &operator=(const Weapon &other);
};

#endif
