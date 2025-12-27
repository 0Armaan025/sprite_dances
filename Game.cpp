#include "Game.h"
#include "Projectile.h"
#include "Utils.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
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
      r.w = 60;
      r.h = 60;

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
  if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
    printf("SDL_mixer Error: %s\n", Mix_GetError());
  }

  bgMusic = Mix_LoadMUS("assets/music.mp3");

  if (!bgMusic) {
    cout << "error: " << Mix_GetError() << endl;
  }

  Mix_PlayMusic(bgMusic, -1);
  Mix_VolumeMusic(64);
}

Game::~Game() {
  SDL_FreeCursor(arrowCursor);
  SDL_FreeCursor(handCursor);
  if (scoreTexture)
    SDL_DestroyTexture(scoreTexture);
  if (playerTexture)
    SDL_DestroyTexture(playerTexture);
  if (shopTexture)
    SDL_DestroyTexture(shopTexture);
  if (font)
    TTF_CloseFont(font);
  TTF_Quit(); // Clean up the TTF subsystem

  Mix_FreeMusic(bgMusic);
  Mix_CloseAudio();
  // THIS IS THE MUST BROTHAA
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
      r.w = 60;
      r.h = 60;

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
  if (keys[SDL_SCANCODE_ESCAPE]) {
    if (!escHeld) { // Only trigger if it wasn't held last frame
      if (state == PLAYING)
        state = PAUSED;
      else if (state == PAUSED)
        state = PLAYING;
      escHeld = true;
    }
  } else {
    escHeld = false; // Reset when user lets go
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

  if (state == PAUSED) {
    if (keys[SDL_SCANCODE_X]) {
      if (!xHeld) {
        if (playerScore >= 8) {
          playerScore -= 8;

          gun.noOfBullets += 40;

          gun.fireRate = gun.fireRate - 0.05f;
        } else {
          showToast("NOT ENOUGH POINTS!", 60);
        }
        xHeld = true;
      }
      return;
    }
  } else {
    xHeld = false;
  }
  if (state == GAME_OVER && keys[SDL_SCANCODE_R]) {
    // Reset Variables
    playerScore = 0;
    gun.noOfBullets = 20; // or your starting amount
    player.x = 100;
    player.y = 100;
    gun.fireRate = 0.2f;
    enemies.clear();
    spawnMoreEnemies();
    state = PLAYING;
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
  }

  checkCollision();
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

    SDL_SetRenderDrawColor(renderer, 100, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_Color white = {255, 255, 255, 255};

    // "GAME OVER" Text
    SDL_Surface *surf = TTF_RenderText_Solid(font, "GAME OVER", white);
    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_Rect rect = {screenW / 2 - surf->w / 2, screenH / 2 - 50, surf->w,
                     surf->h};
    SDL_RenderCopy(renderer, tex, nullptr, &rect);
    SDL_FreeSurface(surf);
    SDL_DestroyTexture(tex);

    // "Press R to Restart"
    SDL_Surface *rSurf =
        TTF_RenderText_Solid(font, "Press R to Restart", white);
    SDL_Texture *rTex = SDL_CreateTextureFromSurface(renderer, rSurf);
    SDL_Rect rRect = {screenW / 2 - rSurf->w / 2, screenH / 2 + 50, rSurf->w,
                      rSurf->h};
    SDL_RenderCopy(renderer, rTex, nullptr, &rRect);
    SDL_FreeSurface(rSurf);
    SDL_DestroyTexture(rTex);
    return;
  }

  SDL_Color black = {0, 0, 0, 255};
  SDL_Color white = {255, 255, 255, 255};

  if (playerScore != lastScore) {
    if (scoreTexture)
      SDL_DestroyTexture(scoreTexture);

    string s = "Score: " + to_string(playerScore);
    SDL_Surface *surf = TTF_RenderText_Solid(font, s.c_str(), black);
    if (surf) {
      scoreTexture = SDL_CreateTextureFromSurface(renderer, surf);
      scoreRect = {screenW - surf->w - 30, 30, surf->w, surf->h};
      SDL_FreeSurface(surf);
    }
    lastScore = playerScore;
  }

  if (scoreTexture)
    SDL_RenderCopy(renderer, scoreTexture, nullptr, &scoreRect);

  // Bullet Count (rendering every frame for simplicity, but optimized with
  // local cleanup)
  string bulletStr = "Bullets: " + to_string(gun.noOfBullets);
  SDL_Surface *bSurf = TTF_RenderText_Solid(font, bulletStr.c_str(), black);
  if (bSurf) {
    SDL_Texture *bTex = SDL_CreateTextureFromSurface(renderer, bSurf);
    SDL_Rect bRect = {30, 30, bSurf->w, bSurf->h}; // Fixed: Added w and h
    SDL_RenderCopy(renderer, bTex, nullptr, &bRect);
    SDL_DestroyTexture(bTex);
    SDL_FreeSurface(bSurf);
  }

  SDL_RenderCopy(renderer, playerTexture, nullptr, &player);

  // Show shop icon only when out of ammo
  if (gun.noOfBullets == 0) {
    SDL_RenderCopy(renderer, shopTexture, nullptr, &drawRect);
  }

  gun.render(renderer);
  for (auto &e : enemies) {
    e.render(renderer);
  }

  if (toastTimer > 0) {
    SDL_Surface *tSurf =
        TTF_RenderText_Solid(font, toastMessage.c_str(), {255, 255, 0, 255});
    if (tSurf) {
      SDL_Texture *tText = SDL_CreateTextureFromSurface(renderer, tSurf);

      // USED GPT TO MAKE IT COOL LOOKING and dynamic positioning here coz i was
      // getting some random pos.
      SDL_Rect toastBg = {screenW - tSurf->w - 50, screenH - 80, tSurf->w + 20,
                          tSurf->h + 10};
      SDL_SetRenderDrawColor(renderer, 50, 50, 50,
                             200); // Dark grey transparent
      SDL_RenderFillRect(renderer, &toastBg);

      SDL_Rect tRect = {screenW - tSurf->w - 40, screenH - 75, tSurf->w,
                        tSurf->h};
      SDL_RenderCopy(renderer, tText, nullptr, &tRect);

      SDL_DestroyTexture(tText);
      SDL_FreeSurface(tSurf);
    }
    toastTimer--;
  }

  if (state == PAUSED) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
    SDL_Rect fullScreen = {0, 0, screenW, screenH};
    SDL_RenderFillRect(renderer, &fullScreen);

    // Draw Shop Container (Green Box)
    SDL_Rect shopBox = {screenW / 2 - 250, screenH / 2 - 150, 500, 300};
    SDL_SetRenderDrawColor(renderer, 34, 177, 76, 255); // Forest Green
    SDL_RenderFillRect(renderer, &shopBox);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White border
    SDL_RenderDrawRect(renderer, &shopBox);

    // "SHOP" Header
    SDL_Surface *headSurf = TTF_RenderText_Solid(font, "--- SHOP ---", white);
    if (headSurf) {
      SDL_Texture *headTex = SDL_CreateTextureFromSurface(renderer, headSurf);
      SDL_Rect headRect = {screenW / 2 - headSurf->w / 2, shopBox.y + 20,
                           headSurf->w, headSurf->h};
      SDL_RenderCopy(renderer, headTex, nullptr, &headRect);
      SDL_DestroyTexture(headTex);
      SDL_FreeSurface(headSurf);
    }

    string line1 = "Press X to buy 40 bullets";
    string line2 = "for 8 points, fire speed+";

    // TODO: LEARN MORE ABOUT THIS
    auto renderLine = [&](string text, int yOffset) {
      SDL_Surface *s = TTF_RenderText_Solid(font, text.c_str(), white);
      if (s) {
        SDL_Texture *t = SDL_CreateTextureFromSurface(renderer, s);
        SDL_Rect r = {screenW / 2 - s->w / 2, shopBox.y + 100 + yOffset, s->w,
                      s->h};
        SDL_RenderCopy(renderer, t, nullptr, &r);
        SDL_DestroyTexture(t);
        SDL_FreeSurface(s);
      }
    };

    renderLine(line1, 0);
    renderLine(line2, 50);
  }
}
void Game::showToast(string msg, int duration) {
  toastMessage = msg;
  toastTimer = duration;
}
// we are using const coz we do not want it to EDIT ANYTHING AT ALL
bool Game::isRunning() const { return state == PLAYING; }
