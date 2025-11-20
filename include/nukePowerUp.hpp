#ifndef NUKEPOWERUP_HPP
#define NUKEPOWERUP_HPP

#include "powerUp.hpp"

// Nuke removes all enemies when collected
class NukePowerUp : public PowerUp {
public:
    NukePowerUp() = default;
    NukePowerUp(const sf::Texture &tx, const sf::Vector2f &pos);

    void applyEffect(Player &player, std::vector<std::unique_ptr<Enemy>> &enemies, float &enemyVelocity) override;
};

#endif // NUKEPOWERUP_HPP
