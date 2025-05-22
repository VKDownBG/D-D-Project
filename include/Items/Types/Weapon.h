#ifndef Weapon_h
#define Weapon_h

#include "C:/DandD/include/Items/Item.h"

class Weapon : public Item {
public:
    Weapon(const std::string &n, double _bonus, int _level);
};

#endif //Weapon_h
