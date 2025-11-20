#include "NukePowerUp.hpp"

NukePowerUp::NukePowerUp(const sf::Texture &tx, const sf::Vector2f &pos) {
    sprite.setTexture(tx);
    position = pos;
}

void NukePowerUp::applyEffect(Player &player, std::vector<std::unique_ptr<Enemy>> &enemies, float &enemyVelocity) {
    // Add number of enemies to player's score and clear the list
    player.score += static_cast<int>(enemies.size());
    enemies.clear();
}
