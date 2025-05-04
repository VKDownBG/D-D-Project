#include "Item.h"

#ifndef Armor_h
#define Armor_h

class Armor : public Item
{
public:
    Armor();

    Armor(const std::string& n, double bonus, int level);

    Armor &operator=(const Armor &other);

    std::string getType() const override;
};

#endif