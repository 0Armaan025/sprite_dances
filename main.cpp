#include "Game.h"
#include "Init.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <emscripten.h>

const int SCREEN_WIDTH = 1400;
const int SCREEN_HEIGHT = 800;

SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;
Game *game = nullptr;

void mainLoop() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT) {
      emscripten_cancel_main_loop();
      return;
    }
  }

  game->handleInput();
  game->update();

  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderClear(renderer);

  game->render(renderer);

  SDL_RenderPresent(renderer);
}

int main() {
  if (!initSDL(window, renderer, SCREEN_WIDTH, SCREEN_HEIGHT)) {
    return -1; // <-- added closing brace
  }

  game = new Game(SCREEN_WIDTH, SCREEN_HEIGHT, renderer);

  emscripten_set_main_loop(mainLoop, 0, 1);
  return 0;
}
