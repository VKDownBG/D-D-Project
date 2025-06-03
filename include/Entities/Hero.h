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

    float GetHealth() const override;

    float GetMaxHealth() const override;

    void SetHealth(float newHealth) override;

    void takeDamage(float damage) override;

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

    void SetMaxHealth(float hlth);

    float GetXP() const;

    void SetXP(float xp);

    void addXP(float xp);

    int GetLevel() const;

    Race GetRace() const;

    std::string GetRaceAsString() const;

    Inventory &GetInventory();

    void levelUp(int str, int mna, float hlth);

    void restoreHealthAfterBattle();

private:
    Race race;

    int strength;
    int mana;
    float health;
    float maxHealth;
    int level;
    float XP;

    Inventory inventory;

    Position startingPosition;
    Position currentPosition;

    std::string name;
};

#endif //Hero_h
