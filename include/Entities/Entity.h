//
// Created by Lenovo on 20.5.2025 г..
//

#ifndef ENTITY_H
#define ENTITY_H

#include <iostream>

class Entity {
public:
    virtual ~Entity() = default;

    virtual std::string GetName() const = 0;

    virtual int GetStrength() const = 0;

    virtual int GetMana() const = 0;

    virtual float GetHealth() const = 0;

    virtual float GetMaxHealth() const = 0;

    virtual void SetHealth(float newHealth) = 0;

    virtual void takeDamage(float damage) = 0;

    virtual bool hasArmor() const = 0;

    virtual float GetWeaponBonus() const = 0;

    virtual float GetSpellBonus() const = 0;

    virtual float GetArmorReduction() const = 0;

    virtual bool isDefeated() const = 0;
};

#endif //ENTITY_H
