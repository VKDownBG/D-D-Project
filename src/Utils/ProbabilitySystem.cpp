//
// Created by Lenovo on 6.5.2025 г.
//

#include "C:/DandD/include/Utils/ProbabilitySystem.h"

#include "C:/DandD/include/Items/Types/Armor.h"
#include "C:/DandD/include/Items/Types/Spell.h"
#include "C:/DandD/include/Items/Types/Weapon.h"

std::string ItemGenerator::chooseName(const std::vector<std::string> &names) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, names.size() - 1);

    return names[dist(gen)];
}

std::string ItemGenerator::getEquipmentType(ItemType type) {
    switch (type) {
        case ItemType::WEAPON:
            return "WEAPON";

        case ItemType::ARMOR:
            return "ARMOR";

        case ItemType::SPELL:
            return "SPELL";
    }

    return "Unknown";
}

ItemType ItemGenerator::generateRandomType() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 2);
    return static_cast<ItemType>(dist(gen));
}

std::string ItemGenerator::generateRandomName(const std::string &filePath, ItemType type, int level) {
    std::vector<std::string> names;

    std::string tempItemTypeHolder = getEquipmentType(type);

    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filePath);
    }

    std::string line;
    std::string lookingFor = tempItemTypeHolder + "_LEVEL_" + std::to_string(level);
    bool readingNames = false;

    while (std::getline(file, line)) {
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);

        if (line.empty()) continue;

        if (line[0] == '[' && (line.find(lookingFor) != std::string::npos)) {
            readingNames = true;
            continue;
        }

        if (readingNames) {
            if (line[0] == ']') break;
            names.push_back(line);
        }
    }

    if (!names.empty()) {
        return chooseName(names);
    }

    return "Unknown";
}

double ItemGenerator::calculateBonus(ItemType type, int level) {
    double min = 0, max = 0;
    switch (type) {
        case ItemType::WEAPON:
            min = 20 + 15 * (level - 1);
            max = 30 + 15 * (level - 1);
            return RandomUtils::randomValue<double>(min, max);

        case ItemType::ARMOR: {
            min = 5 + 4 * (level - 1);
            max = 10 + 4 * (level - 1);
            return RandomUtils::randomValue<double>(min, max);
        }
        case ItemType::SPELL: {
            min = 20 + 10 * (level - 1);
            max = 30 + 10 * (level - 1);
            return RandomUtils::randomValue<double>(min, max);
        }
        default: return 0.0;
    };
}

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
