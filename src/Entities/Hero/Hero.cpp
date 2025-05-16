#include "../../../include/Entities/Hero/Hero.h"

std::string raceToString(Race race) {
    switch (race) {
        case 0:
            return "Human";
            break;
        case 1:
            return "Mage";
            break;
        case 2:
            return "Warrior";
            break;
    }

    return "Error";
}

Hero::Hero(const Race r) : race(r), strength(0), mana(0), health(0), level(1), inventory(
                               Weapon("Basic sword", 20, 1),
                               Spell("Fireball", 20, 1)), startingPosition{0, 0}, currentPosition(startingPosition) {
    switch (r) {
        case Race::Human:
            this->strength = 30;
            this->mana = 20;
            this->health = 50;
            break;

        case Race::Mage:
            this->strength = 10;
            this->mana = 40;
            this->health = 50;
            break;

        case Race::Warrior:
            this->strength = 40;
            this->mana = 10;
            this->health = 50;
            break;
    }
}

void Hero::setPosition(const Position &pos) {
    this->currentPosition = pos;
}

Position Hero::getCurrentPosition() const {
    return currentPosition;
}

bool Hero::isDefeated() const { return this->health <= 0; }

double Hero::getStrength() {
    return strength;
}

double Hero::getMana() {
    return mana;
}

double Hero::getHealth() {
    return health;
}

double Hero::getLevel() {
    return level;
}

Inventory Hero::getInventory() {
    return inventory;
}

double Attack::physicalAttack() {
    return Hero::getStrength() * (1 + Hero::getInventory().getWeapon().getPowerBonus());
}

double Hero::spellAttack() {
    return Hero::getMana() * (1 + inventory.getSpell().getPowerBonus());
}

void Hero::takeDamage(const int damage) { this->health -= damage; }

void Hero::levelUp(const int str, const int mna, const int hlth) {
    this->strength += str;
    this->mana += mna;
    this->health += hlth;
    this->level++;
}
