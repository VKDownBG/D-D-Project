#include "../../include/Entities/Monster.h"

Monster::Monster(const Position &pos, int curlvl, const MonsterType _type)
    : level(curlvl), pos(pos), type(_type) {
    const int levelBonus = (level - 1) * 10;

    health = BASE_HEALTH + levelBonus;
    maxHealth = health;
    strength = BASE_STRENGTH + levelBonus;
    mana = BASE_MANA + levelBonus;

    scaleArmor = (level < 17) ? 15 + level * 5 : 95;

    name.reserve(32);
    name = "Level " + std::to_string(level) + " dragon";
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

void Monster::takeDamage(const float damage) {
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
