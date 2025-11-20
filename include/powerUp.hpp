#ifndef POWERUP_HPP
#define POWERUP_HPP

#include "entity.hpp"
#include "player.hpp"
#include "enemy.hpp"
#include <vector>

// Abstract base for collectible power-ups
class PowerUp : public Entity {
public:
    bool active = true; // removed when false
    float velocity = 80.f;// falling speed

    // Apply power-up effect derived classes implement behavior
    virtual void applyEffect(Player &player, std::vector<std::unique_ptr<Enemy>> &enemies, float &enemyVelocity) = 0;

    void update(float dt) override;
};

#endif // POWERUP_HPP
