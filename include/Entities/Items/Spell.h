#include "Item.h"

#ifndef Spell_h
#define Spell_h

class Spell : public Item
{
public:
    Spell();

    Spell(const std::string& n, double bonus, int level);

    Spell &operator=(const Spell &other);

    std::string getType() const override;
};

#endif