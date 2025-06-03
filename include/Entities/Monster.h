#ifndef Monsters_h
#define Monsters_h

#include "Entity.h"
#include "C:/DandD/include/Utils/Position.h"
#include <sstream>

enum class MonsterType {
    BOSS,
    MONSTER
};

const int BASE_HEALTH = 50;
const int BASE_MANA = 25;
const int BASE_STRENGTH = 25;

class Monster : public Entity {
public:
    Monster(const Position &pos, int curlvl, MonsterType _type);

    bool operator==(const Monster &other) const;

    std::string GetName() const override;

    int GetStrength() const override;

    int GetMana() const override;

    float GetHealth() const override;

    float GetMaxHealth() const override;

    void SetHealth(float newHealth) override;

    void takeDamage(float damage) override;

    bool hasWeapon() const;

    bool hasSpell() const;

    float GetWeaponBonus() const override;

    float GetSpellBonus() const override;

    float GetArmorReduction() const override;

    bool isDefeated() const override;

    void setPosition(const Position &pos);

    Position GetPosition() const;

    int GetDamageResist() const;

    int GetLevel() const;

    MonsterType GetType() const;

private:
    int strength;
    int mana;
    float health;
    float maxHealth;

    int level;
    int scaleArmor;

    Position pos;
    std::string name;
    MonsterType type;
};

#endif
