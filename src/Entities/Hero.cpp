#include "Hero.h"

Hero::Hero(const Race r) : race(r), level(1), currentPosition{0, 0}, inventory(Weapon("Basic sword", 20, 1),
                                                                               Spell("Fireball", 20, 1),
                                                                               Armor()) {
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

bool Hero::isDefeated() const { return this->health == 0; }

int main() {
    return 0;
}
