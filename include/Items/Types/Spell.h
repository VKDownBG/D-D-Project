#ifndef Spell_h
#define Spell_h

#include "C:/DandD/include/Items/Item.h"

class Spell : public Item {
public:
    Spell(const std::string &n, double _bonus, int _level);
};

#endif //Spell_h
