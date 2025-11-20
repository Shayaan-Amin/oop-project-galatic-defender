#include "Ship.hpp"

bool Ship::destroyed() const {
    return health <= 0;
}

void Ship::takeDamage(int d) {
    health -= d;
}
