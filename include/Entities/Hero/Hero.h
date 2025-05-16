#ifndef Hero_h
#define Hero_h

#include "Inventory.h"
#include "C:/DandD/include/Utils/Position.h"
#include  "C:/DandD/include/Utils/Attack.h"
//#include "../../../include/Core/MapSystem.h"

enum class Race {
    Human,
    Mage,
    Warrior
};

class Hero : public Attack {
private:
    Race race{};
    double strength, mana, health, level;
    Inventory inventory;
    Position startingPosition;
    Position currentPosition;

public:
    explicit Hero(Race r);

    ~Hero() override = default;

    //bool move(int dx, int dy, const MapSystem& map);

    void setPosition(const Position &pos);

    [[nodiscard]] Position getCurrentPosition() const;

    [[nodiscard]] bool isDefeated() const;

    [[nodiscard]] double getStrength();

    [[nodiscard]] double getMana();

    [[nodiscard]] double getHealth();

    [[nodiscard]] double getLevel();

    [[nodiscard]] Inventory getInventory();

    double Attack::physicalAttack() override;

    double Attack::spellAttack() override;

    void takeDamage(int damage);

    void levelUp(int str, int mna, int hlth);
};

#endif
