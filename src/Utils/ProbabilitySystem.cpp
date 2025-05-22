//
// Created by Lenovo on 6.5.2025 г.
//

#include "C:/DandD/include/Utils/ProbabilitySystem.h"

#include "C:/DandD/include/Items/Types/Armor.h"
#include "C:/DandD/include/Items/Types/Spell.h"
#include "C:/DandD/include/Items/Types/Weapon.h"

Item *ItemGenerator::generateRandomItem(int level) {
    const ItemType type = generateRandomType();

    std::string name = generateRandomName("C:/DandD/assets/equipment/items.txt", type, level);

    double bonus = calculateBonus(type, level);

    switch (type) {
        case ItemType::WEAPON: return new Weapon(name, bonus, level);
        case ItemType::ARMOR: return new Armor(name, bonus, level);
        case ItemType::SPELL: return new Spell(name, bonus, level);
        default: throw std::runtime_error("Invalid item type");
    }
}
