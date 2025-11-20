#include "PowerUp.hpp"

void PowerUp::update(float dt) {
    // Move downward
    position.y += velocity * dt;

    // If very far off screen, deactivate
    if (position.y > 2000.f) active = false;
}
