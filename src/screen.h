#ifndef SCREEN_H
#define SCREEN_H
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "settings.h"
#include "cimgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include <GL/gl3w.h>
#include <SDL2/SDL.h>
#include "interpret.h"
int initScreen();
void drawScreen(uint8_t screen[][32], ImVec4 color);
void cleanupSDL();
void renderFrame(Chip8State* state);
bool pollEvents();
#endif
