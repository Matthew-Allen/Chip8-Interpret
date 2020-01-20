#include "screen.h"
#include "interpret.h"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

static SDL_Window *displayWindow;
static SDL_Surface *windowSurface;
static SDL_Renderer *windowRenderer;

/*
void drawDebug(int x, int y, Chip8State cpu, SDL_Renderer *renderer)
{
  int i;
  uint8_t displayedInstructions[10];
  for(i = 0; i < 10 && (i + cpu.stackPointer) < 4096; i++) // Get opcodes at next ten memory addresses.
  {
    displayedInstructions[i] = cpu.memory[i+cpu.stackPointer]
  }
}
*/

/*
void renderScreen(bool showDebug)
{
  SDL_CreateRenderer(
*/

int initScreen()
{

  if(SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    printf("SDL Could not initialize: %s\n", SDL_GetError());
    return -1;
  }

  displayWindow = SDL_CreateWindow("Chip8 Interpreter", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  if(displayWindow == NULL)
  {
    printf("SDL Could not create window %s\n", SDL_GetError());
    return -1;
  }
  TTF_Init();

  windowSurface = SDL_GetWindowSurface(displayWindow);

  SDL_Delay(50);

  SDL_FillRect(windowSurface, NULL, SDL_MapRGB(windowSurface->format, 0, 0, 0));
  SDL_UpdateWindowSurface(displayWindow);

  windowRenderer = SDL_CreateRenderer(displayWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if(windowRenderer == NULL)
  {
    return -1;
  }

  return 0;
}

void drawPixel(int x, int y)
{
  SDL_Rect pixelRect;
  pixelRect.h = SCREEN_WIDTH / 32;
  pixelRect.w = SCREEN_HEIGHT / 64;
  pixelRect.x = x*pixelRect.w;
  pixelRect.y = y*pixelRect.h;

  SDL_RenderFillRect(windowRenderer, &pixelRect);
}

void drawScreen(Chip8State* cpu)
{
  
  SDL_SetRenderDrawColor(windowRenderer, 0,0,0,255);
  SDL_RenderClear(windowRenderer);
  for(int i = 0; i < 64; i++)
  {
    for(int j = 0; j < 32; j++)
    {
      if(cpu->screen[i][j] == 1)
      {
        SDL_SetRenderDrawColor(windowRenderer, 255, 255, 255, 255);
        drawPixel(i,j);
      }
    }
  }
  SDL_RenderPresent(windowRenderer);
}


void quitScreen()
{
  SDL_DestroyWindow(displayWindow);
  TTF_Quit();
  SDL_Quit();
}
