#pragma once
#include <SDL2/SDL.h>

struct Enemy {
  SDL_Rect rect;
  SDL_Color color;
  float dx, dy;

  float posX, posY; // <-- precise float position for smooth movement

  void update(int screenW, int screenH);
  void render(SDL_Renderer *renderer);
};
