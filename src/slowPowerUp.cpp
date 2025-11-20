#include "SlowPowerUp.hpp"

SlowPowerUp::SlowPowerUp(const sf::Texture &tx, const sf::Vector2f &pos) {
    sprite.setTexture(tx);
    position = pos;
}

void SlowPowerUp::applyEffect(Player &player, std::vector<std::unique_ptr<Enemy>> &enemies, float &enemyVelocity) {
    // Reduce the current enemy velocity multiplier (Game will restore after timeout)
    enemyVelocity *= slowAmount;
}
