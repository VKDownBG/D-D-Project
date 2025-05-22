#include "C:/DandD/include/Items/Types/Spell.h"

Spell::Spell(const std::string &n, const double _bonus, const int _level) : Item(n, _bonus, _level, ItemType::ARMOR) {
}
