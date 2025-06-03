#include "C:/DandD/include/Items/Item.h"

// Item constructor
Item::Item(const std::string &n, const double _bonus, const int _level,
           const ItemType _type) : name(n), bonus(_bonus), level(_level), type(_type) {
}

// Accessors
std::string Item::GetName() const { return name; }
double Item::GetBonus() const { return bonus; }
int Item::GetLevel() const { return level; }
ItemType Item::GetType() const { return type; }

// Setters
void Item::SetName(const std::string &name) { this->name = name; }
void Item::SetBonus(const float bonus) { this->bonus = bonus; }
void Item::SetLevel(const int level) { this->level = level; }

// Converts item type to string
std::string Item::GetTypeStr() const {
    switch (type) {
        case ItemType::ARMOR: return "ARMOR";
        case ItemType::WEAPON: return "WEAPON";
        case ItemType::SPELL: return "SPELL";
        default: return "Unknown";
    };
}
