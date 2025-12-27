#include "Init.h"
bool initSDL(SDL_Window *&window, SDL_Renderer *&renderer, int width,
             int height) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {

    cout << "error in sdl init: " << SDL_GetError() << endl;
    return false;
  }

  window = SDL_CreateWindow("sprite_dances", SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED, width, height,
                            SDL_WINDOW_RESIZABLE);

  if (!window) {
    cout << "error in creating window: " << SDL_GetError() << endl;
    return false;
  }

  renderer = SDL_CreateRenderer(
      window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

  if (!renderer) {

    cout << "error in creating renderer: " << SDL_GetError() << endl;
    return false;
  }

  // now time for maha yudh

  if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
    printf("SDL_image could not initialize! SDL_image Error: %s\n",
           IMG_GetError());
    return false;
  }

  return true;
}
