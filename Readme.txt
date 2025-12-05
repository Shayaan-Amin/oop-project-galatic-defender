Galactic Defender SFML C++ OOP Game

Galactic Defender is a 2D space-shooter game built in C++ using SFML, designed to demonstrate Object-Oriented Programming concepts such as inheritance, polymorphism, abstraction, virtual functions, and smart pointers.  

The player controls a spaceship, shoots enemies, collects power-ups, and survives increasing waves of alien ships.



Game Overview

You play as the defender of the galaxy. Your objective is to:

- Destroy enemy ships  
- Survive as long as possible  
- Collect power-ups  
- Use nukes and slow-down abilities strategically  
- Progress through increasingly difficult waves  

Your score and level increase as you defeat enemies. The game ends when you lose all your lives.



Key Features

- Object-Oriented Design:
  - Entity base class  
  - Inheritance: Player, Enemy, Bullet, PowerUps  
  - Polymorphism with virtual functions  
  - Smart pointer memory management  

- Game Mechanics
  - Waves that increase in difficulty
  - Bullet–enemy collision detection
  - Player–enemy collision
  - Power-up system (Nuke + Slow)
  - Adjustable enemy speed
  - Game-over and restart system

- Power-ups
  - Nuke PowerUp: Removes all enemies  
  - Slow PowerUp: Slows enemy movement



Controls:

Key -> Action 

A -> Move Left |
D -> Move Right |
 W -> Shoot Bullet |
 N -> Activate Nuke PowerUp 
 Space -> Activate Slow PowerUp 
 R -> Restart after Game Over 
 Escape -> Quit Game 


How to Compile & Run (Windows + g++)

This game requires SFML 2.5.0 and g++ (MinGW).

Compile Command:
g++ main.cpp src/*.cpp -Iinclude -I"D:\Object oreinted programming\SFML-2.5.0-windows-gcc-7.3.0-mingw-64-bit\SFML-2.5.0\include" -L"D:\Object oreinted programming\SFML-2.5.0-windows-gcc-7.3.0-mingw-64-bit\SFML-2.5.0\lib" -lsfml-graphics -lsfml-window -lsfml-system -o game.exe


