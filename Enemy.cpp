#include "Enemy.h"
#include <SDL2/SDL.h>

void Enemy::update(int screenW, int screenH) {
  float COOL_SPEED = 2.0f;

  // Initialize posX/posY if first frame
  if (posX == 0 && posY == 0) {
    posX = rect.x;
    posY = rect.y;
  }

  posX += dx * COOL_SPEED;
  posY += dy * COOL_SPEED;

  // LEFT / RIGHT WALL
  if (posX <= 0) {
    posX = 0;
    dx *= -1;
  } else if (posX + rect.w >= screenW) {
    posX = screenW - rect.w;
    dx *= -1;
  }

  // TOP / BOTTOM WALL
  if (posY <= 0) {
    posY = 0;
    dy *= -1;
  } else if (posY + rect.h >= screenH) {
    posY = screenH - rect.h;
    dy *= -1;
  }

  // Copy float positions to rect for rendering
  rect.x = static_cast<int>(posX);
  rect.y = static_cast<int>(posY);
}

void Enemy::render(SDL_Renderer *renderer) {
  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
  SDL_RenderFillRect(renderer, &rect);
}
