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

    std::vector<std::unique_ptr<Bullet>> bullets;

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
