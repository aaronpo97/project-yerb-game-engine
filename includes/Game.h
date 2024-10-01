#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <iostream>

class Game {
private:
  SDL_Window   *window    = nullptr;
  SDL_Renderer *renderer  = nullptr;
  bool          isRunning = false;

public:
  Game();  // Constructor
  ~Game(); // Destructor

  void init();     // Initialize SDL and create window/renderer
  void run();      // Main game loop
  void sInput();   // Handle input events
  void sRender();  // Render the game scene
  void sCleanup(); // Cleanup and shutdown SDL
};

#endif // GAME_H