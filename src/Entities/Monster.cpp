#include "../../include/Entities/Monster.h"

// Monster constructor: Scales stats based on level
Monster::Monster(const Position &pos, const int curlvl, const MonsterType _type)
    : level(curlvl), pos(pos), type(_type) {
    const int levelBonus = (level - 1) * 10; // Linear scaling per level

    health = BASE_HEALTH + levelBonus;
    maxHealth = health;
    strength = BASE_STRENGTH + levelBonus;
    mana = BASE_MANA + levelBonus;

    // Armor scaling caps at level 17
    scaleArmor = (level < 17) ? 15 + level * 5 : 95;

    name = "Level " + std::to_string(level) + " dragon";
}

bool Monster::operator==(const Monster &other) const {
    return pos == other.pos && type == other.type;
}

// Attribute accessors
std::string Monster::GetName() const { return name; }
int Monster::GetStrength() const { return strength; }
int Monster::GetMana() const { return mana; }
float Monster::GetHealth() const { return health; }
float Monster::GetMaxHealth() const { return maxHealth; }

// Health management with clamping
void Monster::SetHealth(const float newHealth) {
    health = newHealth;
    if (health < 0) health = 0;
    if (health > maxHealth) health = maxHealth;
}

void Monster::takeDamage(const float damage) {
    health -= damage;
    if (health < 0) health = 0; // Prevent negative health
}

// Equipment flags (monsters always have basic equipment)
bool Monster::hasWeapon() const { return true; }
bool Monster::hasSpell() const { return true; }

// Equipment bonuses (monsters use fixed values)
float Monster::GetWeaponBonus() const { return 0.0f; }
float Monster::GetSpellBonus() const { return 0.0f; }

// Armor damage reduction (scaled by level)
float Monster::GetArmorReduction() const {
    return static_cast<float>(scaleArmor) / 100; // Convert to percentage
}

bool Monster::isDefeated() const { return health <= 0; }

// Position management
void Monster::setPosition(const Position &pos) { this->pos = pos; }
Position Monster::GetPosition() const { return pos; }

// Special getters
int Monster::GetDamageResist() const { return scaleArmor; }
int Monster::GetLevel() const { return level; }
MonsterType Monster::GetType() const { return type; }
