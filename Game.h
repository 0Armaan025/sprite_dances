#pragma once
#include "Enemy.h"
#include "Gun.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>

enum GameState { PLAYING, GAME_OVER, PAUSED };

class Game {
public:
  Game(int width, int height, SDL_Renderer *renderer);
  ~Game();

  void handleInput();
  void update();
  void render(SDL_Renderer *renderer);
  void checkCollision();

  // using const here coz we , drawRectdon't wanna edit anything at all :D
  bool isRunning() const;
  SDL_Texture *playerTexture;
  SDL_Texture *shopTexture;
  SDL_Rect shopRect, drawRect;
  SDL_Rect player;
  Gun gun;
  SDL_Rect scoreRect;
  SDL_Texture *scoreTexture;
  bool displayShop = false;
  void spawnMoreEnemies();
  TTF_Font *font;
  void initializeFont(SDL_Renderer *renderer);
  // OUR SENKUUU HAS A GUN
  int screenW, screenH;
  int playerScore = 0;

  bool mouseHeld = false;
  SDL_Cursor *arrowCursor, *handCursor;

private:
  std::vector<Enemy> enemies;

  GameState state;
  int speed;
};
