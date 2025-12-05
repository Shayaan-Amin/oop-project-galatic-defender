#ifndef ENEMY_HPP
#define ENEMY_HPP

#include "ship.hpp"

// Enemy descends downward. Simple autonomous behavior.
class Enemy : public Ship {
public:
    float velocity = 100.f; // downward px/sec
    bool active = true;

    Enemy() = default;

    // Construct with texture, start position, and velocity
    Enemy(const sf::Texture &tx, const sf::Vector2f &pos, float vel);

    // Move down each frame
    void update(float dt) override;
};

#endif // ENEMY_HPP
