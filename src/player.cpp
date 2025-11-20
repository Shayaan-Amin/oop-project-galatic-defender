#include "Player.hpp"
#include <algorithm>

Player::Player(const sf::Texture &tx, const sf::Vector2f &pos) {
    sprite.setTexture(tx);
    position = pos;
    speed = 320.f;
    health = 1;
}

// Create a bullet centered above the player's ship
void Player::shoot(const sf::Texture &bulletTex) {
    if (shootClock.getElapsedTime().asSeconds() < shootCooldown) return;
    shootClock.restart();

    // Calculate bullet spawn position: horizontally centered on player
    float bx = position.x + sprite.getGlobalBounds().width/2.f - bulletTex.getSize().x/2.f;
    float by = position.y - bulletTex.getSize().y;
    bullets.emplace_back(std::make_unique<Bullet>(bulletTex, sf::Vector2f(bx, by)));
}

void Player::update(float dt) {
    // Update bullets
    for (auto &b : bullets) if (b) b->update(dt);

    // Remove inactive bullets
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
                [](const std::unique_ptr<Bullet>& b){ return !b->active; }),
                bullets.end());
}
