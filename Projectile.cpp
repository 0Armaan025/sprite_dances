#include "Projectile.h"
#include <cmath>

Projectile::Projectile(int startX, int startY, float angle, float speed) {
  // starting position
  posX = static_cast<float>(startX);
  posY = static_cast<float>(startY);

  rect = {startX - 2, startY - 2, 20, 8};

  // velocity components
  dx = cos(angle) * speed;
  dy = sin(angle) * speed;

  // gravity (pixels per frame^2)
  gravity = 0.37f; // pixels per frame^2

  drag = 1.019f; // multiply dx and dy each frame
}

// man i hate myself, idk any physics wHY
void Projectile::update() {
  // update velocity with gravity
  dy += gravity;

  // update position
  posX += dx;
  posY += dy;

  // apply drag
  dx *= drag;
  dy *= drag;

  // update rect for rendering
  rect.x = static_cast<int>(posX);
  rect.y = static_cast<int>(posY);
}

void Projectile::render(SDL_Renderer *renderer) {
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // black bullet
  SDL_RenderFillRect(renderer, &rect);
}

SDL_Rect Projectile::getRect() const { return rect; }
