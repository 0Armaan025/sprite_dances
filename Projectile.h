#pragma once
#include <SDL2/SDL.h>

class Projectile {
public:
  Projectile(int startX, int startY, float angle, float speed = 10.0f);

  void update();
  void render(SDL_Renderer *renderer);

  SDL_Rect getRect() const;
  float gravity; // pixels per frame^2
  float drag;

private:
  SDL_Rect rect;
  float posX, posY; // precise position for smooth movement
  float dx, dy;     // velocity components
                    // ir resistance
};
