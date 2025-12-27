
#define IMG_GetError SDL_GetError
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#define IMG_GetError SDL_GetError

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "Game.h"
#include "Init.h"

using namespace std;

int main() {
  const int SCREEN_WIDTH = 1400;
  const int SCREEN_HEIGHT = 800;

  SDL_Window *window = nullptr;
  SDL_Renderer *renderer = nullptr;

  if (!initSDL(window, renderer, SCREEN_WIDTH, SCREEN_HEIGHT)) {
    return -1;
  }

  Game game(SCREEN_WIDTH, SCREEN_HEIGHT, renderer);

  bool running = true;
  SDL_Event event;

  while (running) {
    // -------- EVENTS --------
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        running = false;
      }
    }

    // -------- UPDATE --------
    game.handleInput();
    game.update();

    // -------- RENDER --------
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    game.render(renderer);

    SDL_RenderPresent(renderer);

    SDL_Delay(16); // ~60 FPS
  }

  // Cleanup (Game should destroy its own textures)
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  IMG_Quit();
  SDL_Quit();

  return 0;
}
