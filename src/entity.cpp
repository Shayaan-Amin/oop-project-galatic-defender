#include "Entity.hpp"

void Entity::draw(sf::RenderWindow &win) {
    // Ensure sprite is at the entity's position before drawing
    sprite.setPosition(position);
    win.draw(sprite);
}

sf::FloatRect Entity::getBounds() const {
    // Sprite global bounds includes texture size; we align its top-left
    sf::FloatRect r = sprite.getGlobalBounds();
    r.left = position.x;
    r.top  = position.y;
    return r;
}
