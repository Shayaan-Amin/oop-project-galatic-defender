#include "game.hpp"
#include "nukePowerUp.hpp"
#include "slowPowerUp.hpp"

#include <iostream>
#include <random>
#include <algorithm>

// helper random number generator
static std::mt19937 rng((unsigned)std::random_device{}());
static int randint(int a, int b) { std::uniform_int_distribution<int> d(a,b); return d(rng); }
static constexpr int SCREEN_WIDTH  = 720;
static constexpr int SCREEN_HEIGHT = 450;

Game::Game()
: window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Galactic Defender"), baseEnemyVelocity(60.f),
  level(0), waveLength(3), initialLives(3)
{
    window.setFramerateLimit(60);
    enemyVelocity = baseEnemyVelocity;

    if (!loadResources()) {
        std::cout << "Warning: some resources failed to load. Check images/ folder.\n";
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

    // Try to load font from multiple locations
    bool fontLoaded = false;
    // Try images folder first
    if (font.loadFromFile("images/arial.ttf")) {
        fontLoaded = true;
    }
    // Try common Windows font paths
    else if (font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
        fontLoaded = true;
    }
    else if (font.loadFromFile("C:/Windows/Fonts/calibri.ttf")) {
        fontLoaded = true;
    }
    else if (font.loadFromFile("C:/Windows/Fonts/times.ttf")) {
        fontLoaded = true;
    }
    
    if (!fontLoaded) {
        std::cout << "Warning: Could not load font. HUD text will not display.\n";
        std::cout << "Please ensure a font file exists in images/arial.ttf or system fonts are available.\n";
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
        // Allow restart when game is over
        if (ev.type == sf::Event::KeyPressed && ev.key.code == sf::Keyboard::R && player.lives <= 0) {
            reset();
        }
    }
}

void Game::handleInput(float dt) {
    // Horizontal movement using A/D or Left/Right
    float moveX = 0.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        moveX -= player.speed * dt;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        moveX += player.speed * dt;

    // Clamp player x to window bounds
    auto bounds = player.sprite.getLocalBounds();
    player.position.x = std::max(0.f, std::min(player.position.x + moveX, SCREEN_WIDTH - bounds.width));


    // Shooting: W 
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) )
        player.shoot(texLaser);

    // Activate power-ups: N for boost (nuke), SPACE for decelerator (slow) (with cooldown to prevent rapid activation)
    if (powerUpCooldown.getElapsedTime().asSeconds() >= powerUpCooldownTime) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::N)) {
            activateNukePowerUp();
            powerUpCooldown.restart();
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            activateSlowPowerUp();
            powerUpCooldown.restart();
        }
    }
}

void Game::update(float dt) {
    // If player has no lives, halt game updates (game over)
    if (player.lives <= 0) {
        return; // Stop all game logic, only handle events and render
    }

    handleInput(dt);

    // Update player bullets and other player internal state
    player.update(dt);

    // Update enemies positions
    for (auto &e : enemies) if (e) e->update(dt);

    // Remove enemies that passed bottom and decrease lives
    enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
                [&](const std::unique_ptr<Enemy>& e){
                    if (!e) return true;
                    if (e->position.y > SCREEN_HEIGHT) {
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

    // Collisions: player vs enemies
    checkPlayerEnemyCollisions();

    // Player collects power-ups
    checkPowerUpCollection();

    // If no enemies remain spawn next wave
    if (enemies.empty()) {
        spawnNextWave();
    }
    
    // Ensure we never exceed 6 enemies (safety check)
    if (enemies.size() > 6) {
        enemies.erase(enemies.begin() + 6, enemies.end());
    }

    // Slow power-up is manually activated, no timer needed
}

void Game::spawnNextWave() {
    waveLength += 1;
    
    // Only increment level if player has lives 
    if (player.lives > 0) {
        level += 1;
    }
    
    // Update enemy velocity based on current level
    enemyVelocity = baseEnemyVelocity + (level-1)*12.f;

    // Limit wave length to ensure max 6 enemies
    int enemiesToSpawn = std::min(waveLength, 6);
    
    // Only spawn if we have less than 6 enemies
    int currentEnemyCount = static_cast<int>(enemies.size());
    int maxToSpawn = std::max(0, 6 - currentEnemyCount);
    enemiesToSpawn = std::min(enemiesToSpawn, maxToSpawn);

    for (int i = 0; i < enemiesToSpawn; ++i) {
        // Get enemy sprite bounds to ensure proper spawning
        sf::Sprite tempSprite;
        tempSprite.setTexture(texEnemy);
        auto enemyBounds = tempSprite.getLocalBounds();
        
        // Spawn enemies within screen bounds (not in borders)
        // Ensure enemy fits completely within screen width
        float minX = 10.f;
        float maxX = SCREEN_WIDTH - enemyBounds.width - 10.f;
        if (maxX < minX) maxX = minX; // safety check
        
        float ex = static_cast<float>(randint(static_cast<int>(minX), static_cast<int>(maxX)));
        float ey = static_cast<float>(-randint(20, 600));
        enemies.emplace_back(std::make_unique<Enemy>(texEnemy, sf::Vector2f(ex, ey), enemyVelocity));
    }

    // Spawn power-ups occasionally - use same bounds checking as enemies
    if (level % 2 == 0) {
        // Get nuke power-up sprite bounds to ensure proper spawning
        sf::Sprite tempNukeSprite;
        tempNukeSprite.setTexture(texNuke);
        auto nukeBounds = tempNukeSprite.getLocalBounds();
        
        // Spawn within screen bounds (not in borders) - same logic as enemies
        float minX = 10.f;
        float maxX = SCREEN_WIDTH - nukeBounds.width - 10.f;
        if (maxX < minX) maxX = minX; // safety check
        
        float nukeX = static_cast<float>(randint(static_cast<int>(minX), static_cast<int>(maxX)));
        float nukeY = static_cast<float>(-randint(20, 200));
        powerUps.emplace_back(std::make_unique<NukePowerUp>(
            texNuke, sf::Vector2f(nukeX, nukeY)));
    }
    if (level % 3 == 0) {
        // Get slow power-up sprite bounds to ensure proper spawning
        sf::Sprite tempSlowSprite;
        tempSlowSprite.setTexture(texSlow);
        auto slowBounds = tempSlowSprite.getLocalBounds();
        
        // Spawn within screen bounds (not in borders) - same logic as enemies
        float minX = 10.f;
        float maxX = SCREEN_WIDTH - slowBounds.width - 10.f;
        if (maxX < minX) maxX = minX; // safety check
        
        float slowX = static_cast<float>(randint(static_cast<int>(minX), static_cast<int>(maxX)));
        float slowY = static_cast<float>(-randint(20, 200));
        powerUps.emplace_back(std::make_unique<SlowPowerUp>(
            texSlow, sf::Vector2f(slowX, slowY)));
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
                std::cout << "Hitting an enemy" << std::endl;
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

void Game::checkPlayerEnemyCollisions() {
    // Check if player collides with any enemy
    for (auto it = enemies.begin(); it != enemies.end();) {
        if (!(*it)) { ++it; continue; }
        if (player.getBounds().intersects((*it)->getBounds())) {
            // Collision: remove enemy and decrement player life
            (*it)->active = false;
            it = enemies.erase(it);
            player.lives -= 1;
        } else {
            ++it;
        }
    }
}

void Game::checkPowerUpCollection() {
    for (auto it = powerUps.begin(); it != powerUps.end();) {
        if (!(*it)) { ++it; continue; }
        if (player.getBounds().intersects((*it)->getBounds())) {
            // Increment power-up counters instead of applying immediately
            if (dynamic_cast<NukePowerUp*>((*it).get())) {
                player.nukeCount += 1;
            } else if (dynamic_cast<SlowPowerUp*>((*it).get())) {
                player.slowCount += 1;
            }

            // Remove the powerup after collection
            it = powerUps.erase(it);
        } else ++it;
    }
}

void Game::activateNukePowerUp() {
    // Only activate if player has nuke power-ups
    if (player.nukeCount > 0) {
        // Add score for each enemy destroyed
        player.score += static_cast<int>(enemies.size());
        // Clear all enemies
        enemies.clear();
        // Decrement counter
        player.nukeCount -= 1;
    }
}

void Game::activateSlowPowerUp() {
    // Only activate if player has slow power-ups
    if (player.slowCount > 0) {
        // Reduce enemy velocity by a fixed amount 
        // Using 20 pixels/second as the reduction amount
        float reduction = 20.f;
        if (enemyVelocity > reduction) {
            enemyVelocity -= reduction;
        } else {
            enemyVelocity = std::max(10.f, enemyVelocity - reduction); // ensure minimum speed
        }
        // Decrement counter
        player.slowCount -= 1;
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

    // HUD (Lives, Score, Level, Power-ups) - display all game information
    // Check if font is loaded and valid
    bool fontValid = (font.getInfo().family != "");
    
    // Always display HUD if font is available
    if (fontValid) {
        sf::Text txt;
        txt.setFont(font);
        txt.setCharacterSize(20);
        txt.setFillColor(sf::Color::Green);

        // Lives (top left) - shows current number of lives
        txt.setString("Lives: " + std::to_string(player.lives));
        txt.setPosition(10.f, 10.f);
        window.draw(txt);

        // Boosts (below lives) - shows number of nuke power-ups collected
        txt.setString("Nuke(N): " + std::to_string(player.nukeCount));
        txt.setPosition(10.f, 38.f);
        window.draw(txt);

        // Decelerator (below boosts) - shows number of slow power-ups collected
        txt.setString("Decelerator(Space Bar): " + std::to_string(player.slowCount));
        txt.setPosition(10.f, 66.f);
        window.draw(txt);

        // Level (top right) - shows current level
        txt.setFillColor(sf::Color::White);
        txt.setString("Level: " + std::to_string(level));
        txt.setPosition(SCREEN_WIDTH - 130.f, 10.f);
        window.draw(txt);

        // Score (below level) - shows current score
        txt.setString("Score: " + std::to_string(player.score));
        txt.setPosition(SCREEN_WIDTH - 130.f, 38.f);
        window.draw(txt);
    }

    // Game-over overlay - appears when player has no lives
    if (player.lives <= 0) {
        // Dark overlay to dim the screen
        sf::RectangleShape overlay(sf::Vector2f((float)SCREEN_WIDTH, (float)SCREEN_HEIGHT));
        overlay.setFillColor(sf::Color(0,0,0,180));
        window.draw(overlay);

        // Game Over text in center of screen
        if (fontValid) {
            sf::Text gameOver;
            gameOver.setFont(font);
            gameOver.setString("GAME OVER");
            gameOver.setCharacterSize(50);
            gameOver.setFillColor(sf::Color::Red);
            gameOver.setStyle(sf::Text::Bold);
            
            // Center the text
            sf::FloatRect textBounds = gameOver.getLocalBounds();
            gameOver.setPosition(SCREEN_WIDTH/2.f - textBounds.width/2.f, SCREEN_HEIGHT/2.f - 60.f);
            window.draw(gameOver);

            // Restart instruction
            sf::Text restartText;
            restartText.setFont(font);
            restartText.setString("Press R to Restart");
            restartText.setCharacterSize(24);
            restartText.setFillColor(sf::Color::White);
            
            sf::FloatRect restartBounds = restartText.getLocalBounds();
            restartText.setPosition(SCREEN_WIDTH/2.f - restartBounds.width/2.f, SCREEN_HEIGHT/2.f + 10.f);
            window.draw(restartText);
        }
    }

    window.display();
}

void Game::reset() {
    enemies.clear();
    powerUps.clear();
    player.bullets.clear();
    player.lives = initialLives;
    player.score = 0;
    player.nukeCount = 0;
    player.slowCount = 0;
    level = 0;
    waveLength = 3;
    enemyVelocity = baseEnemyVelocity;
    powerUpCooldown.restart();
    spawnNextWave();
}
