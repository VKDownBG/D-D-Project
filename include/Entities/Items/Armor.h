#ifndef Armor_h
#define Armor_h

#include "Item.h"

class Armor : public Item {
public:
    Armor(const std::string &n, double bonus, int level);

    Armor &operator=(const Armor &other);
};

#endif
