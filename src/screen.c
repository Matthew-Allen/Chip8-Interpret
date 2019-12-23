#include "screen.h"
#include "interpret.h"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;


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

  SDL_Window *renderWindow;
  SDL_Surface *surface;

  if(SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    printf("SDL Could not initialize: %s\n", SDL_GetError());
    return -1;
  }

  renderWindow = SDL_CreateWindow("Chip8 Interpreter", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  if(renderWindow == NULL)
  {
    printf("SDL Could not create window %s\n", SDL_GetError());
    return -1;
  }
  TTF_Init();

  surface = SDL_GetWindowSurface(renderWindow);

  SDL_Delay(50);

  SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 255, 255, 255));
  SDL_UpdateWindowSurface(renderWindow);

  return 0;
}

void quitScreen(SDL_Window *renderWindow)
{
  SDL_DestroyWindow(renderWindow);
  TTF_Quit();
  SDL_Quit();
}
