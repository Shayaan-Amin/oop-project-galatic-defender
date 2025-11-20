#ifndef SLOWPOWERUP_HPP
#define SLOWPOWERUP_HPP

#include "powerUp.hpp"

// Slow reduces current enemy velocity (restored in Game after duration)
class SlowPowerUp : public PowerUp {
public:
    float slowDuration = 5.f;  // seconds to keep enemies slowed (managed by Game)
    float slowAmount = 0.6f;   // multiply enemy velocity by this
    SlowPowerUp() = default;
    SlowPowerUp(const sf::Texture &tx, const sf::Vector2f &pos);

    void applyEffect(Player &player, std::vector<std::unique_ptr<Enemy>> &enemies, float &enemyVelocity) override;
};

#endif // SLOWPOWERUP_HPP
