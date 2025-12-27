#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
using namespace std;

bool initSDL(SDL_Window *&window, SDL_Renderer *&renderer, int width,
             int height);
