#ifndef Item_h
#define Item_h

#include <iostream>

class Item
{
protected:
    std::string name;
    double powerBonus;
    int requiredLevel;

public:
    virtual std::string getType() const = 0;
    virtual ~Item() {}

    std::string getName() const;
    double getPowerBonus() const;
    int getRequiredLevel() const;
};

#endif