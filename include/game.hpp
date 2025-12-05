#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>
#include "player.hpp"
#include "enemy.hpp"
#include "powerUp.hpp"
#include <vector>
#include <memory>

class Game {
public:
    Game();           // constructor loads resources and configures window
    void run();       // starts the main loop

private:
    // resource loading and helpers
    bool loadResources();
    void handleEvents();
    void update(float dt);
    void render();
    void handleInput(float dt);
    void spawnNextWave();
    void checkBulletEnemyCollisions();
    void checkPlayerEnemyCollisions();  // check if player collides with enemies
    void checkPowerUpCollection();
    void activateNukePowerUp();    // activate nuke when SPACE pressed
    void activateSlowPowerUp();    // activate slow when R pressed
    void reset();     // restart game after losing

    // Window and textures/sprites
    sf::RenderWindow window;
    sf::Texture texPlayer, texEnemy, texLaser, texBG, texNuke, texSlow;
    sf::Sprite background;

    // Game state
    Player player;
    std::vector<std::unique_ptr<Enemy>> enemies;
    std::vector<std::unique_ptr<PowerUp>> powerUps;

    float baseEnemyVelocity;
    float enemyVelocity;
    int level;
    int waveLength;
    int initialLives;
    sf::Font font;

    // Power-up activation cooldown (to prevent rapid activation)
    sf::Clock powerUpCooldown;
    float powerUpCooldownTime = 0.3f; // seconds between activations
};

#endif // GAME_HPP
