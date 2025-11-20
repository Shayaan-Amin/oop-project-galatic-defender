#include "Enemy.hpp"

Enemy::Enemy(const sf::Texture &tx, const sf::Vector2f &pos, float vel) {
    sprite.setTexture(tx);
    position = pos;
    velocity = vel;
    health = 1;
}

void Enemy::update(float dt) {
    position.y += velocity * dt;
}
