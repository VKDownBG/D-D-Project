//
// Created by Lenovo on 6.5.2025 г.
//

#ifndef PROBABILITYSYSTEM_H
#define PROBABILITYSYSTEM_H

#include "C:/DandD/include/Items/Item.h"
#include "C:/DandD/include/Items/Types/Armor.h"
#include "C:/DandD/include/Items/Types/Spell.h"
#include "C:/DandD/include/Items/Types/Weapon.h"
#include <random>
#include <algorithm>
#include <fstream>
#include <unordered_map>

namespace RandomUtils {
    static std::random_device rd;
    static std::mt19937 gen(rd());

    template<typename T>
    T randomValue(T min, T max) {
        if constexpr (std::is_integral_v<T>) {
            std::uniform_int_distribution<T> dist(min, max);
            return dist(gen);
        } else {
            std::uniform_real_distribution<T> dist(min, max);
            return dist(gen);
        }
    }
}

class ItemGenerator {
public:
    static Item *generateRandomItem(int level);

private:
    static std::string chooseName(const std::vector<std::string> &names);

    static std::string getEquipmentType(ItemType type);

    static ItemType generateRandomType();

    static std::string generateRandomName(const std::string &filePath, ItemType type, int level);

    static double calculateBonus(ItemType type, int level);
};


#endif //PROBABILITYSYSTEM_H
