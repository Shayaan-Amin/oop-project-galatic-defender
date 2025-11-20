#ifndef SHIP_HPP
#define SHIP_HPP

#include "entity.hpp"

// Ship is a general movable object that has health and speed.
// Player and Enemy are both ships.
class Ship : public Entity {
public:
    int health = 1;
    float speed = 100.f;   // movement speed (px/sec)

    bool destroyed() const;    // returns true when health <= 0
    void takeDamage(int d);    // apply damage
};

#endif // SHIP_HPP
