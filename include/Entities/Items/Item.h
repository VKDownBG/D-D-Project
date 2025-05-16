#ifndef Item_h
#define Item_h

#include <iostream>

enum class ItemType {
    WEAPON,
    ARMOR,
    SPELL
};

class Item {
protected:
    std::string name;
    ItemType type;
    double powerBonus = 0;
    int requiredLevel = 0;

public:
    Item(ItemType type, std::string name, double powerBonus, int requiredLevel);

    virtual ~Item() = default;

    [[nodiscard]] ItemType getType() const;

    [[nodiscard]] std::string getName() const;

    [[nodiscard]] double getPowerBonus() const;

    [[nodiscard]] int getRequiredLevel() const;
};

#endif
