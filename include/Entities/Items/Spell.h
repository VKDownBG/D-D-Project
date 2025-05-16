#ifndef Spell_h
#define Spell_h

#include "Item.h"

class Spell : public Item {
public:
    Spell(const std::string &n, double bonus, int level);

    Spell &operator=(const Spell &other);
};

#endif
