#include "Game.hpp"
#include "NukePowerUp.hpp"
#include "SlowPowerUp.hpp"

#include <iostream>
#include <random>
#include <algorithm>

// helper random number generator
static std::mt19937 rng((unsigned)std::random_device{}());
static int randint(int a, int b) { std::uniform_int_distribution<int> d(a,b); return d(rng); }

Game::Game()
: window(sf::VideoMode(700,450), "Galactic Defender"), baseEnemyVelocity(60.f),
  level(0), waveLength(3), initialLives(3), slowRestoreTime(0.f)
{
    window.setFramerateLimit(60);
    enemyVelocity = baseEnemyVelocity;

    if (!loadResources()) {
        std::cerr << "Warning: some resources failed to load. Check images/ folder.\n";
    }

    // Place player near bottom-center
    player = Player(texPlayer, sf::Vector2f(350.f - texPlayer.getSize().x/2.f, 380.f));
    player.lives = initialLives;
}

bool Game::loadResources() {
    bool ok = true;

    // Load images from images/ directory (relative to working directory)
    if (!texPlayer.loadFromFile("images/player.png")) { std::cerr << "Failed to load images/player.png\n"; ok = false; }
    if (!texEnemy.loadFromFile("images/enemy.png"))   { std::cerr << "Failed to load images/enemy.png\n";  ok = false; }
    if (!texLaser.loadFromFile("images/laser.png"))   { std::cerr << "Failed to load images/laser.png\n";  ok = false; }
    if (!texBG.loadFromFile("images/background.png")) { std::cerr << "Failed to load images/background.png\n"; ok = false; }
    if (!texNuke.loadFromFile("images/boost.png"))    { std::cerr << "Failed to load images/boost.png\n";   ok = false; }
    if (!texSlow.loadFromFile("images/slow.png"))     { std::cerr << "Failed to load images/slow.png\n";   ok = false; }

    background.setTexture(texBG);

    // optional font (used for HUD). If missing the game still runs.
    if (!font.loadFromFile("images/arial.ttf")) {
        std::cout << "Optional: images/arial.ttf not found. HUD text may not display.\n";
    }

    return ok;
}

void Game::run() {
    sf::Clock clock;
    spawnNextWave();

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        handleEvents();
        update(dt);
        render();
    }
}

void Game::handleEvents() {
    sf::Event ev;
    while (window.pollEvent(ev)) {
        if (ev.type == sf::Event::Closed) window.close();
        if (ev.type == sf::Event::KeyPressed && ev.key.code == sf::Keyboard::Escape) window.close();
    }
}

void Game::handleInput(float dt) {
    // Horizontal movement using A/D or Left/Right
    float moveX = 0.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        moveX -= player.speed * dt;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        moveX += player.speed * dt;

    // Clamp player x to window bounds
    float maxX = window.getSize().x - player.sprite.getGlobalBounds().width;
    player.position.x = std::max(0.f, std::min(player.position.x + moveX, maxX));

    // Shooting: W or Up
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        player.shoot(texLaser);
}

void Game::update(float dt) {
    handleInput(dt);

    // Update player bullets and other player internal state
    player.update(dt);

    // Update enemies positions
    for (auto &e : enemies) if (e) e->update(dt);

    // Remove enemies that passed bottom and decrease lives
    enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
                [&](const std::unique_ptr<Enemy>& e){
                    if (!e) return true;
                    if (e->position.y > window.getSize().y) {
                        player.lives -= 1;
                        return true;
                    }
                    return false;
                }), enemies.end());

    // Update power-ups falling
    for (auto &p : powerUps) if (p) p->update(dt);

    // Remove inactive powerups
    powerUps.erase(std::remove_if(powerUps.begin(), powerUps.end(),
                [](const std::unique_ptr<PowerUp>& p){ return !p->active; }), powerUps.end());

    // Collisions: bullets vs enemies
    checkBulletEnemyCollisions();

    // Player collects power-ups
    checkPowerUpCollection();

    // If no enemies remain spawn next wave
    if (enemies.empty()) spawnNextWave();

    // If slow effect active, count down and restore speed when time's up
    if (slowRestoreTime > 0.f) {
        slowRestoreTime -= dt;
        if (slowRestoreTime <= 0.f) {
            enemyVelocity = baseEnemyVelocity + (level-1)*12.f;
        }
    }
}

void Game::spawnNextWave() {
    level += 1;
    waveLength += 1;
    enemyVelocity = baseEnemyVelocity + (level-1)*12.f;

    for (int i = 0; i < waveLength; ++i) {
        float ex = static_cast<float>(randint(10, static_cast<int>(window.getSize().x)-60));
        float ey = static_cast<float>(-randint(20, 600));
        enemies.emplace_back(std::make_unique<Enemy>(texEnemy, sf::Vector2f(ex, ey), enemyVelocity));
    }

    // Spawn power-ups occasionally
    if (level % 2 == 0)
        powerUps.emplace_back(std::make_unique<NukePowerUp>(texNuke, sf::Vector2f(static_cast<float>(randint(10, window.getSize().x - 40)), -randint(20,200))));
    if (level % 3 == 0) {
        powerUps.emplace_back(std::make_unique<SlowPowerUp>(texSlow, sf::Vector2f(static_cast<float>(randint(10, window.getSize().x - 40)), -randint(20,200))));
    }
}

void Game::checkBulletEnemyCollisions() {
    // Loop over each bullet and test intersection with enemies
    for (auto &bptr : player.bullets) {
        if (!bptr) continue;
        for (auto it = enemies.begin(); it != enemies.end();) {
            if (!(*it)) { ++it; continue; }
            if (bptr->getBounds().intersects((*it)->getBounds())) {
                // Hit: deactivate bullet, remove enemy, increase score
                bptr->active = false;
                (*it)->active = false;
                it = enemies.erase(it);
                player.score += 1;
                break; // bullet gone; continue with next bullet
            } else ++it;
        }
    }

    // Remove bullets marked inactive
    player.bullets.erase(std::remove_if(player.bullets.begin(), player.bullets.end(),
                [](const std::unique_ptr<Bullet>& b){ return !b->active; }), player.bullets.end());
}

void Game::checkPowerUpCollection() {
    for (auto it = powerUps.begin(); it != powerUps.end();) {
        if (!(*it)) { ++it; continue; }
        if (player.getBounds().intersects((*it)->getBounds())) {
            // Apply effect. Some effects modify enemyVelocity; slowRestoreTime handled in update().
            (*it)->applyEffect(player, enemies, enemyVelocity);

            // If slow powerup, set a timer to restore speed later
            if (dynamic_cast<SlowPowerUp*>((*it).get())) {
                slowRestoreTime = dynamic_cast<SlowPowerUp*>((*it).get())->slowDuration;
            }

            // Remove the powerup after collection
            it = powerUps.erase(it);
        } else ++it;
    }
}

void Game::render() {
    window.clear();

    // Background
    window.draw(background);

    // Draw enemies
    for (auto &e : enemies) if (e) e->draw(window);

    // Draw power-ups
    for (auto &p : powerUps) if (p) p->draw(window);

    // Draw bullets
    for (auto &b : player.bullets) if (b) b->draw(window);

    // Draw player
    player.draw(window);

    // HUD (Lives, Score, Level)
    if (font.getInfo().family != "") {
        sf::Text txt;
        txt.setFont(font);
        txt.setCharacterSize(18);
        txt.setFillColor(sf::Color::White);

        txt.setString("Lives: " + std::to_string(player.lives));
        txt.setPosition(10.f, 8.f);
        window.draw(txt);

        txt.setString("Score: " + std::to_string(player.score));
        txt.setPosition(window.getSize().x - 120.f, 8.f);
        window.draw(txt);

        txt.setString("Level: " + std::to_string(level));
        txt.setPosition(window.getSize().x/2.f - 40.f, 8.f);
        window.draw(txt);
    }

    // Game-over overlay
    if (player.lives <= 0) {
        sf::RectangleShape overlay(sf::Vector2f(static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y)));
        overlay.setFillColor(sf::Color(0,0,0,150));
        window.draw(overlay);

        if (font.getInfo().family != "") {
            sf::Text gameOver("You Lost! Press R to Restart", font, 28);
            gameOver.setFillColor(sf::Color::White);
            gameOver.setPosition(window.getSize().x/2.f - gameOver.getLocalBounds().width/2.f, window.getSize().y/2.f - 40.f);
            window.draw(gameOver);
        }
    }

    window.display();

    // If player lost and pressed R, reset the game
    if (player.lives <= 0 && sf::Keyboard::isKeyPressed(sf::Keyboard::R)) reset();
}

void Game::reset() {
    enemies.clear();
    powerUps.clear();
    player.bullets.clear();
    player.lives = initialLives;
    player.score = 0;
    level = 0;
    waveLength = 3;
    enemyVelocity = baseEnemyVelocity;
    spawnNextWave();
}
