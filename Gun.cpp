#include "Gun.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <cmath>

Gun::Gun(SDL_Renderer *renderer)
    : angle(0), muzzleOffset(20), fireRate(0.2f), fireCooldown(0) {
  body = {0, 0, 60, 10};
  nozzle = {0, 0, 60, 10};
  bodyColor = {0, 0, 0, 255};
  nozzleColor = {255, 0, 0, 255};

  // TODO: LEARN MORE ABOUT THIS
  nozzleTexture =
      SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                        SDL_TEXTUREACCESS_TARGET, nozzle.w, nozzle.h);

  SDL_SetRenderTarget(renderer, nozzleTexture);
  SDL_SetRenderDrawColor(renderer, nozzleColor.r, nozzleColor.g, nozzleColor.b,
                         nozzleColor.a);
  SDL_RenderClear(renderer);
  SDL_SetRenderTarget(renderer, nullptr);
}

void Gun::update(const SDL_Rect &player, int mouseX, int mouseY,
                 float deltaTime) {
  // attaching GUN TO MY SENKU <3
  //  NOW THE KINGDOM OF SCIENCE WILL WIN FOR SRUE, COZ SENKU HAS THIS, NOT XENO

  body.x = player.x + player.w / 2 - body.w / 2;
  body.y = player.y + player.h / 2 - body.h / 2;

  // I HAVE NO CLUE WHAT THIS PHYSICS IS, THANKS TO GPT
  // BUT I DO KNOW THAT DELTA IS CHANGING STUFFJj
  //  angle to mouse
  float deltaX = mouseX - (body.x + body.w / 2);
  float deltaY = mouseY - (body.y + body.h / 2);
  angle = atan2(deltaY, deltaX);

  nozzle.x = body.x + body.w / 2 + cos(angle) * muzzleOffset - nozzle.w / 2;
  nozzle.y = body.y + body.h / 2 + sin(angle) * muzzleOffset - nozzle.h / 2;
  for (auto &b : bullets) {
    b.update();
  }
  if (fireCooldown > 0)
    fireCooldown -= deltaTime;
}

bool Gun::outOfAmmo() const { return noOfBullets == 0; }

void Gun::shoot() {
  if (noOfBullets == 0) {
    return;
    // TODO: we will show a shop icon
  }
  if (fireCooldown <= 0) {
    bullets.push_back(Projectile(nozzle.x + nozzle.w / 2,
                                 nozzle.y + nozzle.h / 2, angle, 15.0f));
    fireCooldown = fireRate; // reset cooldown
    noOfBullets--;
  }
}
void Gun::render(SDL_Renderer *renderer) {
  // Draw body
  //  SDL_SetRenderDrawColor(renderer, bodyColor.r, bodyColor.g, bodyColor.b,
  //                         bodyColor.a);
  // SDL_RenderFillRect(renderer, &body);

  // Draw nozzle

  SDL_Point center = {nozzle.w / 2, nozzle.h / 2};
  SDL_RenderCopyEx(renderer, nozzleTexture, nullptr, &nozzle,
                   angle * 180 / M_PI, &center, SDL_FLIP_NONE);

  // Draw bullets
  for (auto &b : bullets) {
    b.render(renderer);
  }
}
