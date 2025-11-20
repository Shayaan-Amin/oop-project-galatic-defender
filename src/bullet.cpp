#include "Bullet.hpp"

Bullet::Bullet(const sf::Texture &tx, const sf::Vector2f &pos) {
    sprite.setTexture(tx);
    position = pos;
}

void Bullet::update(float dt) {
    // Move up
    position.y -= speed * dt;
    // If offscreen above, mark inactive
    if (position.y + sprite.getGlobalBounds().height < 0) active = false;
}
