#ifndef Item_h
#define Item_h

#include <iostream>

enum class ItemType {
    WEAPON,
    ARMOR,
    SPELL
};

class Item {
public:
    Item(const std::string &n, double _bonus, int _level, ItemType _type);

    virtual ~Item() = default;

    std::string GetName() const;

    void SetName(const std::string &name);

    double GetBonus() const;

    void SetBonus(float bonus);

    int GetLevel() const;

    void SetLevel(int level);

    std::string GetTypeStr() const;

    ItemType GetType() const;

protected:
    std::string name;
    double bonus = 0;
    int level = 1;

    ItemType type;
};

#endif //Item_h
