#ifndef Hero_h
#define Hero_h

#include "Entity.h"
#include "C:/DandD/include/Utils/Inventory.h"
#include "C:/DandD/include/Utils/Position.h"

enum class Race {
    Human,
    Mage,
    Warrior
};

Race stringToRace(const std::string &r);

class Hero : public Entity {
public:
    Hero(const std::string &raceName, std::string heroName);

    std::string GetName() const override;

    int GetStrength() const override;

    int GetMana() const override;

    double GetHealth() const override;

    int GetMaxHealth() const override;

    void SetHealth(int newHealth) override;

    void takeDamage(double damage) override;

    bool hasArmor() const override;

    float GetWeaponBonus() const override;

    float GetSpellBonus() const override;

    float GetArmorReduction() const override;

    bool isDefeated() const override;

    void setPosition(const Position &pos);

    Position *GetPosition() { return &currentPosition; }

    const Position *GetPosition() const { return &currentPosition; }

    Position getCurrentPosition() const;

    Position getStartingPosition() const;

    void SetStrength(int str);

    void SetMana(int mna);

    void SetMaxHealth(int hlth);

    int GetXP() const;

    void SetXP(int xp);

    void addXP(double xp);

    int GetLevel() const;

    Race GetRace() const;

    std::string GetRaceAsString() const;

    Inventory &GetInventory();

    void levelUp(int str, int mna, int hlth);

    void restoreHealthAfterBattle();

    int chooseAttackType() const;

private:
    Race race;
    int strength;
    int mana;
    double health;
    int level;
    int maxHealth;
    int XP;
    Inventory inventory;
    Position startingPosition;
    Position currentPosition;
    std::string name;
};

#endif //Hero_h
