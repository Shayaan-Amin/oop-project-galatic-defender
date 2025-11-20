#include "Game.hpp"
#include <iostream>


int main() {
    // Helpful debug: print working directory so you can confirm where "images/" should live.
    // std::cout << "Working directory: " << std::filesystem::current_path() << "\n";
    Game game;
    game.run();
    return 0;
}
