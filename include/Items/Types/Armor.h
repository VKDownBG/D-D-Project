#ifndef Armor_h
#define Armor_h

#include "C:/DandD/include/Items/Item.h"

class Armor : public Item {
public:
    Armor(const std::string &n, double _bonus, int _level);
};

#endif //Armor_h
