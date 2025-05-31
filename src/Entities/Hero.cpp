#include <utility>

#include "../../include/Entities/Hero.h"

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

Hero::Hero(const std::string &raceName, std::string heroName)
    : race(stringToRace(raceName)),
      strength(0),
      mana(0),
      health(50),
      maxHealth(50),
      level(1),
      XP(0),
      inventory(
          Weapon("Basic sword", 20, 1),
          Spell("Fireball", 20, 1)),
      startingPosition{0, 0},
      currentPosition(startingPosition),
      name(std::move(heroName)) {
    switch (race) {
        case Race::Human:
            this->strength = 30;
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

    if (name.empty()) {
        name = GetRaceAsString() + " hero";
    }
}

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

void Hero::SetHealth(const float newHealth) {
    health = newHealth;
    if (health < 0) {
        health = 0;
    }
    if (health > maxHealth) {
        health = maxHealth;
    }
}

void Hero::takeDamage(const float damage) {
    health -= damage;
    if (health < 0) {
        health = 0;
    }
}

bool Hero::hasArmor() const {
    return inventory.hasArmor();
}

float Hero::GetWeaponBonus() const {
    return inventory.GetWeapon().GetBonus() / 100;
}

float Hero::GetSpellBonus() const {
    return inventory.GetSpell().GetBonus() / 100;
}

float Hero::GetArmorReduction() const {
    if (hasArmor()) {
        return inventory.GetArmor().GetBonus() / 100;
    }
    return 0.0f;
}

bool Hero::isDefeated() const {
    return health <= 0;
}

void Hero::setPosition(const Position &pos) {
    currentPosition = pos;
}

Position Hero::getCurrentPosition() const {
    return currentPosition;
}

Position Hero::getStartingPosition() const {
    return startingPosition;
}

void Hero::SetStrength(const int str) {
    strength = str;
}

void Hero::SetMana(const int mna) {
    mana = mna;
}

void Hero::SetMaxHealth(const float hlth) {
    maxHealth = hlth;
    if (health > maxHealth) {
        health = maxHealth;
    }
}

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

Race Hero::GetRace() const {
    return race;
}

std::string Hero::GetRaceAsString() const {
    switch (race) {
        case Race::Human:
            return "Human";
        case Race::Mage:
            return "Mage";
        case Race::Warrior:
            return "Warrior";
    }

    return "Unknown";
}

Inventory &Hero::GetInventory() {
    return inventory;
}

void Hero::levelUp(const int str, const int mna, const float hlth) {
    this->strength += str;
    this->mana += mna;
    this->health += hlth;
    this->maxHealth += hlth;
    this->level++;
}

void Hero::restoreHealthAfterBattle() {
    float halfMaxHealth = maxHealth / 2;

    if (health < halfMaxHealth) {
        health = halfMaxHealth;
        std::cout << GetName() << " recovered health to " << health << "." << std::endl;
    } else {
        std::cout << GetName() << " kept their current health of " << health << "." << std::endl;
    }
}
