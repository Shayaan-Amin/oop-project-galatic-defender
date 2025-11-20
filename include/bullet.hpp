#ifndef BULLET_HPP
#define BULLET_HPP

#include "entity.hpp"

// Bullet moves upwards (player bullets). Very simple class.
class Bullet : public Entity {
public:
    float speed = 350.f; // pixels per second (upwards)
    bool active = true;  // set to false when it should be removed

    Bullet() = default;

    // Construct with texture and starting position
    Bullet(const sf::Texture &tx, const sf::Vector2f &pos);

    // move the bullet each frame
    void update(float dt) override;
};

#endif // BULLET_HPP
