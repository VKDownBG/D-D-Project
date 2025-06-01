//
// Created by Lenovo on 6.5.2025 г.
//

#include "C:/DandD/include/Utils/ProbabilitySystem.h"

std::string ItemGenerator::chooseName(const std::vector<std::string> &names) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, names.size() - 1);

    return names[dist(gen)];
}

std::string ItemGenerator::getEquipmentType(const ItemType type) {
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

static std::unordered_map<std::string, std::vector<std::string> > nameCache;

std::string ItemGenerator::generateRandomName(const std::string &filePath, const ItemType type, const int level) {
    const std::string lookingFor = getEquipmentType(type) + "_LEVEL_" + std::to_string(level);
    if (nameCache.find(lookingFor) == nameCache.end()) {
        std::vector<std::string> names;

        std::ifstream file(filePath);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file: " + filePath);
        }

        std::string line;
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

        nameCache[lookingFor] = names;
    }

    return chooseName(nameCache.at(lookingFor));
}

double ItemGenerator::calculateBonus(const ItemType type, const int level) {
    static const std::unordered_map<ItemType, std::pair<double, double> > BONUS_RANGES = {
        {ItemType::ARMOR, {5.0, 10.0}},
        {ItemType::WEAPON, {20.0, 30.0}},
        {ItemType::SPELL, {20.0, 30.0}}
    };

    const auto &range = BONUS_RANGES.at(type);
    const double min = range.first + (level - 1) * (type == ItemType::ARMOR ? 4 : 15);
    const double max = range.second + (level - 1) * (type == ItemType::ARMOR ? 4 : 15);

    return RandomUtils::randomValue<double>(min, max);
}

Item *ItemGenerator::generateRandomItem(const int level) {
    const ItemType type = generateRandomType();

    const std::string name = generateRandomName("C:/DandD/assets/equipment/items.txt", type, level);

    const double bonus = calculateBonus(type, level);

    switch (type) {
        case ItemType::WEAPON: return new Weapon(name, bonus, level);
        case ItemType::ARMOR: return new Armor(name, bonus, level);
        case ItemType::SPELL: return new Spell(name, bonus, level);
        default: throw std::runtime_error("Invalid item type");
    }
}
