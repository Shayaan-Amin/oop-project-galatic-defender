#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "ship.hpp"
#include "bullet.hpp"
#include <vector>
#include <memory>
#include <SFML/System.hpp>

// Player class manages bullets, lives, score, and shooting cooldown
class Player : public Ship {
public:
    int lives = 3;
    int score = 0;
    int nukeCount = 0;      // number of nuke power-ups collected
    int slowCount = 0;      // number of slow power-ups collected

    std::vector<std::unique_ptr<Bullet>> bullets; //  a list (vector) that will store bullets, and each bullet is stored using a unique_ptr so that it automatically deletes it when it's no longer needed.

    sf::Clock shootClock;      // to enforce cooldown
    float shootCooldown = 0.20f; // seconds between shots

    Player() = default;

    // Construct with player texture and initial position
    Player(const sf::Texture &tx, const sf::Vector2f &pos);

    // Attempt to shoot (creates a Bullet if cooldown elapsed)
    void shoot(const sf::Texture &bulletTex);

    // Update bullets (remove inactive ones)
    void update(float dt) override;
};

#endif // PLAYER_HPP
