#ifndef RENDERING_H
#define RENDERING_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "interpret.h"

void drawDebug(int x, int y, Chip8State *cpu, SDL_Renderer *renderer);


#endif
