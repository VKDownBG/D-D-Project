#include "../../include/Entities/Monster.h"

Monster::Monster(Position &pos, int curlvl, MonsterType _type) : strength(25), mana(25), health(50), level(curlvl),
                                                                 scaleArmor(15),
                                                                 pos(-1, -1),
                                                                 type(_type) {
    this->health += this->level * 10;
    this->mana += this->level * 10;
    this->strength += this->level * 10;

    if (level < 17)
        this->scaleArmor += this->level * 5;
    else
        this->scaleArmor = 95;
}

void Monster::setPosition(const Position &pos) {
    this->pos = pos;
}

Position Monster::getPosition() const {
    return pos;
}

int Monster::getStrength() const {
    return strength;
}

int Monster::getMana() const {
    return mana;
}

int Monster::getHealth() const {
    return health;
}

int Monster::getDamageResist() const {
    return scaleArmor;
}



MonsterType Monster::getType() const { return type; }

bool Monster::isDefeated() const { return health <= 0; }
