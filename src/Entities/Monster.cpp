#include "../../include/Entities/Monster.h"

Monster::Monster(const Position &pos, int curlvl, const MonsterType _type)
    : strength(25),
      mana(25),
      health(50),
      maxHealth(50),
      level(curlvl),
      scaleArmor(15),
      pos(pos),
      type(_type) {
    if (curlvl > 1) {
        this->health += this->level * 10;

        this->mana += this->level * 10;

        this->strength += this->level * 10;

        this->maxHealth = this->health;

        if (level < 17)
            this->scaleArmor += this->level * 5;
        else
            this->scaleArmor = 95;
    }


    std::stringstream ss;

    ss << "Level " << this->level << " dragon";

    this->name = ss.str();
}

std::string Monster::GetName() const {
    return name;
}

int Monster::GetStrength() const {
    return strength;
}

int Monster::GetMana() const {
    return mana;
}

float Monster::GetHealth() const {
    return health;
}

float Monster::GetMaxHealth() const {
    return maxHealth;
}

void Monster::SetHealth(const float newHealth) {
    health = newHealth;

    if (health < 0)
        health = 0;

    if (health > maxHealth)
        health = maxHealth;
}

void Monster::takeDamage(float damage) {
    health -= damage;

    if (health < 0)
        health = 0;
}

bool Monster::hasArmor() const {
    return true;
}

bool Monster::hasWeapon() const {
    return true;
}

bool Monster::hasSpell() const {
    return true;
}

float Monster::GetWeaponBonus() const {
    return 0.0f;
}

float Monster::GetSpellBonus() const {
    return 0.0f;
}

float Monster::GetArmorReduction() const {
    return static_cast<float>(scaleArmor) / 100;
}

bool Monster::isDefeated() const {
    return health <= 0;
}

void Monster::setPosition(const Position &pos) {
    this->pos = pos;
}

Position Monster::GetPosition() const {
    return pos;
}

int Monster::GetDamageResist() const {
    return scaleArmor;
}


int Monster::GetLevel() const {
    return level;
}

MonsterType Monster::GetType() const {
    return type;
}
