#include "Game.h"
#include "Utils.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>
using namespace std;

Game::Game(int width, int height, SDL_Renderer *renderer)
    : screenW(width), screenH(height), speed(10), state(PLAYING),
      gun(renderer) {
  player = {100, 100, 128, 128};

  shopRect = {screenW / 2, 40, 140, 140};
  int ENEMY_LIMIT = 10;
  for (int i = 0; i < ENEMY_LIMIT; i++) {
    SDL_Rect r;
    bool valid = false;
    while (!valid) {
      r.x = getRandom(0, screenW - 50);
      r.y = getRandom(0, screenH - 50);
      r.w = 30;
      r.h = 30;

      if (!SDL_HasIntersection(&r, &player)) {
        valid = true;
      }
    }

    SDL_Color color = {255, 0, 0, 255};
    float dx = getRandom(-100, 100) / 100.0f;
    float dy = getRandom(-100, 100) / 100.0f;

    enemies.push_back({r, color, dx, dy});
  }
  playerTexture = IMG_LoadTexture(
      renderer, "/home/armaan025/Desktop/ARMAAN/Coding/"
                "projects/sdl2/sprite_dances/assets/senku2.png");
  shopTexture =
      IMG_LoadTexture(renderer, "/home/armaan025/Desktop/ARMAAN/Coding/"
                                "projects/sdl2/sprite_dances/assets/shop.png");

  initializeFont(renderer);
  arrowCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
  handCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
}

Game::~Game() {
  SDL_FreeCursor(arrowCursor);
  SDL_FreeCursor(handCursor);

  // IT IS A MUST BROTHERRRRR;
}

void Game::initializeFont(SDL_Renderer *renderer) {
  if (TTF_Init() < 0) {
    cout << "TTF ERROR: " << TTF_GetError() << endl;
    return;
  }

  font = TTF_OpenFont("minecraft-ten.ttf", 36);
}

void Game::spawnMoreEnemies() {
  for (int i = 0; i < 10; i++) {
    SDL_Rect r;
    bool valid = false;
    while (!valid) {
      r.x = getRandom(0, screenW - 50);
      r.y = getRandom(0, screenH - 50);
      r.w = 30;
      r.h = 30;

      if (!SDL_HasIntersection(&r, &player)) {
        valid = true;
      }
    }

    SDL_Color color = {255, 0, 0, 255};
    float dx = getRandom(-100, 100) / 100.0f;
    float dy = getRandom(-100, 100) / 100.0f;

    enemies.push_back({r, color, dx, dy});
  }
}

void Game::handleInput() {
  const Uint8 *keys = SDL_GetKeyboardState(NULL);
  float dx = 0, dy = 0;

  if (keys[SDL_SCANCODE_W])
    dy -= 1;
  if (keys[SDL_SCANCODE_S])
    dy += 1;
  if (keys[SDL_SCANCODE_A])
    dx -= 1;
  if (keys[SDL_SCANCODE_D])
    dx += 1;

  float length = sqrt(
      dx * dx + dy * dy); // I have no clue what htis math is , but apparnetly
                          // it helps with clamping the diagonal speeed

  if (length != 0) {
    // no idea either, math sucks
    dx /= length;
    dy /= length;
  }

  player.x += dx * speed;
  player.y += dy * speed;

  player.x = max(0, min(player.x, screenW - player.w));
  player.y = max(0, min(player.y, screenH - player.h));

  if (state == PAUSED) {
    if (keys[SDL_SCANCODE_ESCAPE]) {
      state = PLAYING;
    }

    return;
  }

  int mouseX, mouseY;
  float deltaTime = 0.016f;
  Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);
  gun.update(player, mouseX, mouseY, deltaTime);

  if (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT)) {
    gun.shoot();
  }

  bool hoveringShop = mouseX >= shopRect.x &&
                      mouseX <= shopRect.x + shopRect.w &&
                      mouseY >= shopRect.y && mouseY <= shopRect.y + shopRect.h;

  drawRect = shopRect;

  if (hoveringShop) {
    drawRect.x -= 10;
    drawRect.y -= 10;
    drawRect.w += 20;
    drawRect.h += 20;
  }

  // we could have used the same thing as in line 19, BUT (DEMO SAN), IT WILL
  // FIRE 60 TIMES / SECOND, THAT IS ONLY REQUIRED FOR A GUN, NOT A BTN

  bool leftDown = mouseState & SDL_BUTTON(SDL_BUTTON_LEFT);

  // checking if it a single click
  if (hoveringShop && leftDown && !mouseHeld) {
    state = PAUSED;
  }

  mouseHeld = leftDown;
}

void Game::update() {
  if (state != PLAYING)
    return;

  for (auto &e : enemies) {
    e.update(screenW, screenH);
  }

  for (size_t i = 0; i < gun.bullets.size(); i++) {
    SDL_Rect bulletRect = gun.bullets[i].getRect();

    if (enemies.size() < 3) {
      spawnMoreEnemies();
    }
    for (size_t j = 0; j < enemies.size(); j++) {
      if (SDL_HasIntersection(&bulletRect, &enemies[j].rect)) {
        enemies.erase(enemies.begin() + j);
        gun.bullets.erase(gun.bullets.begin() + i);
        playerScore++;
        i--;
        break;
      }
    }

    checkCollision();
  }
}

void Game::checkCollision() {
  for (auto &e : enemies) {
    if (SDL_HasIntersection(&player, &e.rect)) {
      state = GAME_OVER;
    }
  }
}

void Game::render(SDL_Renderer *renderer) {
  if (state == GAME_OVER) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderClear(renderer);
    return;
  }
  SDL_Color color = {0, 0, 0, 255};
  string s = to_string(playerScore);
  string joined_string = "Score: " + s;
  SDL_Surface *surface =
      TTF_RenderText_Solid(font, joined_string.c_str(), color);

  if (!surface) {
    cout << "error: " << SDL_GetError() << endl;
  }

  scoreTexture = SDL_CreateTextureFromSurface(renderer, surface);

  scoreRect.w = surface->w;
  scoreRect.h = surface->h;
  scoreRect.x = screenW - 200;

  scoreRect.y = 50;

  SDL_RenderCopy(renderer, scoreTexture, nullptr, &scoreRect);
  // instead of creating new textures, let's just overwrite this one

  string bullets_count = to_string(gun.noOfBullets).c_str();
  string starting_literal = "Bullets: " + bullets_count;
  surface = TTF_RenderText_Solid(font, starting_literal.c_str(), color);

  if (!surface) {
    cout << "error: " << SDL_GetError() << endl;
  }
  scoreTexture = SDL_CreateTextureFromSurface(renderer, surface);

  scoreRect.w = surface->w;
  scoreRect.h = surface->h;
  scoreRect.x = 30;

  scoreRect.y = 50;

  SDL_RenderCopy(renderer, scoreTexture, nullptr, &scoreRect);

  SDL_FreeSurface(surface);
  SDL_RenderCopy(renderer, playerTexture, nullptr, &player);

  if (gun.noOfBullets == 0) {
    SDL_RenderCopy(renderer, shopTexture, nullptr, &drawRect);
  }
  gun.render(renderer);
  for (auto &e : enemies) {
    e.render(renderer);
  }
  if (state == PAUSED) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    SDL_Rect overlay = {0, 0, screenW, screenH};
    SDL_RenderFillRect(renderer, &overlay);
  }

  SDL_DestroyTexture(scoreTexture);
}

// we are using const coz we do not want it to EDIT ANYTHING AT ALL
bool Game::isRunning() const { return state == PLAYING; }
