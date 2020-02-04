#ifndef SCREEN_H
#define SCREEN_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
int initScreen();
void drawScreen(uint8_t screen[][32]);
void cleanupSDL();
#endif
