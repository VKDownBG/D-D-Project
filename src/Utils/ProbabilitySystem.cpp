//
// Created by Lenovo on 6.5.2025 г.
//

#include "Utils/ProbabilitySystem.h"

#include "Entities/Items/Armor.h"
#include "Entities/Items/Spell.h"
#include "Entities/Items/Weapon.h"

Item *ItemGenerator::generateRandomItem(int level) {
    const ItemType type = generateRandomType();

    std::string name = generateRandomName("C:\DandD\assets\equipment\items.txt", type, level);

    double bonus = calculateBonus(type, level);

    switch (type) {
        case ItemType::WEAPON: return new Weapon(name, bonus, level);
        case ItemType::ARMOR: return new Armor(name, bonus, level);
        case ItemType::SPELL: return new Spell(name, bonus, level);
        default: throw std::runtime_error("Invalid item type");
    }
}
