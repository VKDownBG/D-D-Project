//
// Created by Lenovo on 18.5.2025 г.
//

#include "C:/DandD/include/Items/Item.h"

Item::Item(const std::string &n, const double _bonus, const int _level,
           const ItemType _type) : name(n), bonus(_bonus), level(_level), type(_type) {
}

std::string Item::GetName() const {
    return name;
}

void Item::SetName(const std::string &name) {
    this->name = name;
}

double Item::GetBonus() const {
    return bonus;
}

void Item::SetBonus(const float bonus) {
    this->bonus = bonus;
}

int Item::GetLevel() const {
    return level;
}

void Item::SetLevel(const int level) {
    this->level = level;
}

std::string Item::GetType() const {
    switch (type) {
        case ItemType::ARMOR:
            return "ARMOR";

        case ItemType::WEAPON:
            return "WEAPON";

        case ItemType::SPELL:
            return "SPELL";
    };

    return "Unknown";
}
