//
// Created by Lenovo on 8.5.2025 г.
//

#ifndef ATTACK_H
#define ATTACK_H

class Attack {
public:
    virtual ~Attack() = default;

    virtual double physicalAttack() = 0;

    virtual double spellAttack() = 0;
};

#endif //ATTACK_H
