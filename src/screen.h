#ifndef SCREEN_H
#define SCREEN_H
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "tinyfiledialogs.h"
#include "cimgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include "interpret.h"
#include <GL/gl3w.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <libconfig.h>
int initScreen();
void drawScreen(uint8_t screen[][32], ImVec4 color);
void cleanupSDL();
void renderFrame(Chip8State* state, config_t* configData);
bool pollEvents(config_t* configData);
void playSquareWave();
void stopSound();
#endif
