#include <utility>
#include "../../include/Entities/Hero.h"

// Converts string to Race enum
Race stringToRace(const std::string &r) {
    if (r == "Human" || r == "human")
        return Race::Human;
    else if (r == "Mage" || r == "mage")
        return Race::Mage;
    else if (r == "Warrior" || r == "warrior")
        return Race::Warrior;
    else
        throw std::runtime_error("Invalid race: " + r);
}

// Hero constructor: Initializes based on race
Hero::Hero(const std::string &raceName, std::string heroName)
    : race(stringToRace(raceName)), // Convert string to enum
      strength(0),
      mana(0),
      health(50),
      maxHealth(50),
      level(1),
      XP(0),
      inventory( // Default equipment
          Armor("Empty", 0, 0),
          Weapon("Basic sword", 20, 1),
          Spell("Fireball", 20, 1)),
      startingPosition{0, 0},
      currentPosition(startingPosition),
      name(std::move(heroName)) {
    // Race-specific attribute initialization
    switch (race) {
        case Race::Human:
            this->strength = 3000;
            this->mana = 20;
            break;

        case Race::Mage:
            this->strength = 10;
            this->mana = 40;
            break;

        case Race::Warrior:
            this->strength = 40;
            this->mana = 10;
            break;
    }

    // Default name if empty
    if (name.empty()) {
        name = GetRaceAsString() + " hero";
    }
}

// Attribute accessors
std::string Hero::GetName() const {
    return name;
}

int Hero::GetStrength() const {
    return strength;
}

int Hero::GetMana() const {
    return mana;
}

float Hero::GetHealth() const {
    return health;
}

float Hero::GetMaxHealth() const {
    return maxHealth;
}

// Health management with clamping
void Hero::SetHealth(const float newHealth) {
    health = newHealth;
    // Ensure health stays within bounds
    if (health < 0) {
        health = 0;
    }
    if (health > maxHealth) {
        health = maxHealth;
    }
}

// Damage application with death check
void Hero::takeDamage(const float damage) {
    health -= damage;
    if (health < 0) {
        health = 0; // Prevent negative health
    }
}

// Equipment bonus calculations (converted to decimal percentages)
float Hero::GetWeaponBonus() const {
    return inventory.GetWeapon().GetBonus() / 100;
}

float Hero::GetSpellBonus() const {
    return inventory.GetSpell().GetBonus() / 100;
}

float Hero::GetArmorReduction() const {
    return inventory.GetArmor().GetBonus() / 100; // Damage reduction percentage
}

bool Hero::isDefeated() const {
    return health <= 0; // Death check
}

// Position management
void Hero::setPosition(const Position &pos) {
    currentPosition = pos;
}

Position Hero::getCurrentPosition() const {
    return currentPosition;
}

Position Hero::getStartingPosition() const {
    return startingPosition;
}

// Attribute setters
void Hero::SetStrength(const int str) {
    strength = str;
}

void Hero::SetMana(const int mna) {
    mana = mna;
}

void Hero::SetMaxHealth(const float hlth) {
    maxHealth = hlth;
    // Adjust current health if over new max
    if (health > maxHealth) {
        health = maxHealth;
    }
}

// Experience system
float Hero::GetXP() const {
    return XP;
}

void Hero::SetXP(const float xp) {
    XP = xp;
}

void Hero::addXP(const float xp) {
    XP += xp;
}

int Hero::GetLevel() const {
    return level;
}

// Race information
Race Hero::GetRace() const {
    return race;
}

std::string Hero::GetRaceAsString() const {
    switch (race) {
        case Race::Human: return "Human";
        case Race::Mage: return "Mage";
        case Race::Warrior: return "Warrior";
        default: return "Unknown"; // Fallback
    }
}

// Inventory access
Inventory &Hero::GetInventory() {
    return inventory;
}

// Character progression
void Hero::levelUp(const int str, const int mna, const float hlth) {
    // Apply stat increases
    this->strength += str;
    this->mana += mna;
    this->health += hlth;
    this->maxHealth += hlth;
    this->level++; // Increment level
}

// Post-battle recovery
void Hero::restoreHealthAfterBattle() {
    float halfMaxHealth = maxHealth / 2;

    // Restore to 50% if below threshold
    if (health < halfMaxHealth) {
        health = halfMaxHealth;
        std::cout << GetName() << " recovered health to " << health << "." << std::endl;
    } else {
        // No change if already above 50%
        std::cout << GetName() << " kept their current health of " << health << "." << std::endl;
    }
}
