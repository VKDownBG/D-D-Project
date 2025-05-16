#ifndef Monsters_h
#define Monsters_h

#include "Utils/Position.h"
#include "Utils/ProbabilitySystem.h"

enum class MonsterType {
    BOSS,
    MONSTER
};

enum class attackChosen {
    PHYSICAL,
    SPELL
};

class Monster {
private:
    int strength, mana, health, level;
    int scaleArmor;
    Position pos;
    MonsterType type;

public:
    explicit Monster(Position &pos, int curlvl, MonsterType _type);

    void setPosition(const Position &pos);

    [[nodiscard]] Position getPosition() const;

    [[nodiscard]] int getStrength() const;

    [[nodiscard]] int getMana() const;

    [[nodiscard]] int getHealth() const;

    [[nodiscard]] int getDamageResist() const;

    [[nodiscard]] MonsterType getType() const;

    [[nodiscard]] bool isDefeated() const;
};

#endif
