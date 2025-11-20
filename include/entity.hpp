#ifndef ENTITY_HPP
#define ENTITY_HPP

// Base abstract class for anything drawable / updatable in the game.
// All game objects (Player, Enemy, Bullet, PowerUp) derive from this.

#include <SFML/Graphics.hpp>

class Entity {
public:
    sf::Sprite sprite;            // sprite used to draw the texture
    sf::Vector2f position{0.f,0.f};

    virtual ~Entity() = default;

    // Update internal logic by delta time (seconds)
    virtual void update(float dt) = 0;

    // Draw this entity to the provided window
    virtual void draw(sf::RenderWindow &win);

    // Get axis-aligned bounding box (used for simple collision)
    virtual sf::FloatRect getBounds() const;
};

#endif // ENTITY_HPP
