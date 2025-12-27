#pragma once

#include "Projectile.h"
#include <SDL2/SDL.h>
#include <vector>

class Gun {
public:
  Gun(SDL_Renderer *renderer);

  std::vector<Projectile> bullets;
  void update(const SDL_Rect &player, int mouseX, int mouseY, float deltaTime);

  void updatePosition(const SDL_Rect &playerRect);
  void shoot();

  void render(SDL_Renderer *renderer);
  float fireCooldown; // time remaining until next shot
  float fireRate;     // seconds per shot
  int noOfBullets = 20;
  bool outOfAmmo() const;

private:
  SDL_Rect body;
  SDL_Color bodyColor;
  SDL_Rect nozzle;
  SDL_Color nozzleColor;
  float angle, muzzleOffset;

  SDL_Texture *nozzleTexture;
};
